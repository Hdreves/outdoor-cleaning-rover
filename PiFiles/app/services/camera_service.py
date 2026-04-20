from io import BytesIO
from threading import Lock
import time
from picamera2 import Picamera2


class CameraService:
    def __init__(self) -> None:
        self.picam2 = None
        self.lock = Lock()
        self.running = False

    def start(self) -> None:
        if self.running:
            return

        self.picam2 = Picamera2()
        config = self.picam2.create_video_configuration(
            main={"size": (640, 480)}
        )
        self.picam2.configure(config)
        self.picam2.start()
        time.sleep(1.0)
        self.running = True

    def stop(self) -> None:
        if self.picam2 is not None:
            try:
                self.picam2.stop()
            except Exception:
                pass
            try:
                self.picam2.close()
            except Exception:
                pass
            self.picam2 = None

        self.running = False

    def is_running(self) -> bool:
        return self.running

    def get_jpeg_frame(self) -> bytes:
        if not self.running or self.picam2 is None:
            self.start()

        buf = BytesIO()

        with self.lock:
            self.picam2.capture_file(buf, format="jpeg")

        return buf.getvalue()

    def mjpeg_generator(self):
        if not self.running or self.picam2 is None:
            self.start()

        while True:
            try:
                frame = self.get_jpeg_frame()
                yield (
                    b"--frame\r\n"
                    b"Content-Type: image/jpeg\r\n\r\n" + frame + b"\r\n"
                )
                time.sleep(0.03)
            except Exception as e:
                print(f"Camera stream error: {e}")
                time.sleep(0.2)

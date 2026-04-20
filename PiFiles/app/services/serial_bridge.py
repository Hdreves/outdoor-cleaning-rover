import json
import threading
import time
import serial


class SerialBridge:
    def __init__(self, port: str = "/dev/ttyACM0", baudrate: int = 9600) -> None:
        self.port = port
        self.baudrate = baudrate
        self.ser: serial.Serial | None = None
        self.lock = threading.Lock()
        
    def connect(self) -> None:
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=0.5)
            time.sleep(1.5)  # give MCU time to reset after USB serial opens
            print(f"Connected to controller on {self.port}")
        except Exception as e:
            print(f"Controller not available on {self.port}: {e}")
            self.ser = None

    def close(self) -> None:
        if self.ser and self.ser.is_open:
            self.ser.close()

    def send_line(self, line: str) -> None:
        if not self.ser or not self.ser.is_open:
            raise RuntimeError("Serial bridge is not connected")
        with self.lock:
            self.ser.write((line.strip() + "\n").encode("utf-8"))
            self.ser.flush()

    def request_json(self, line: str) -> dict:
        if not self.ser or not self.ser.is_open:
            raise RuntimeError("Serial bridge is not connected")

        with self.lock:
            self.ser.reset_input_buffer()
            self.ser.write((line.strip() + "\n").encode("utf-8"))
            self.ser.flush()

            raw = self.ser.readline().decode("utf-8").strip()
            if not raw:
                raise RuntimeError("No response from controller")

            return json.loads(raw)

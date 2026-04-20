from app.schemas import RoverStatus, ControlCommand
from app.services.serial_bridge import SerialBridge


class RoverService:
    def __init__(self, bridge: SerialBridge) -> None:
        self.bridge = bridge
        self.cached_status = RoverStatus()

    def ping(self) -> bool:
        try:
            reply = self.bridge.request_json("PING")
            ok = bool(reply.get("ok", False))
            self.cached_status.connected = ok
            return ok
        except Exception:
            self.cached_status.connected = False
            return False

    def get_status(self) -> RoverStatus:
        try:
            reply = self.bridge.request_json("STATUS")

            current_camera = self.cached_status.camera
            self.cached_status = RoverStatus(**reply)
            self.cached_status.camera = current_camera

        except Exception:
            self.cached_status.connected = False

        return self.cached_status

    def handle_command(self, cmd: ControlCommand) -> RoverStatus:
        action = cmd.action.strip().lower()

        if action == "forward":
            speed = cmd.value if cmd.value is not None else 0.35
            self.bridge.send_line(f"DRIVE FORWARD {speed:.2f}")
            self.cached_status.drive = "forward"

        elif action == "reverse":
            speed = cmd.value if cmd.value is not None else 0.35
            self.bridge.send_line(f"DRIVE REVERSE {speed:.2f}")
            self.cached_status.drive = "reverse"

        elif action == "left":
            speed = cmd.value if cmd.value is not None else 0.30
            self.bridge.send_line(f"TURN LEFT {speed:.2f}")
            self.cached_status.drive = "left"

        elif action == "right":
            speed = cmd.value if cmd.value is not None else 0.30
            self.bridge.send_line(f"TURN RIGHT {speed:.2f}")
            self.cached_status.drive = "right"

        elif action == "stop":
            self.bridge.send_line("STOP")
            self.cached_status.drive = "stopped"

        elif action == "scoop_up":
            self.bridge.send_line("SCOOP UP")
            self.cached_status.scoop = "raising"

        elif action == "scoop_down":
            self.bridge.send_line("SCOOP DOWN")
            self.cached_status.scoop = "lowering"

        elif action == "dump":
            self.bridge.send_line("SCOOP DUMP")
            self.cached_status.scoop = "dumping"
            
        elif action == "scoop_stop":
            self.bridge.send_line("SCOOP STOP")
            self.cached_status.scoop = "idle"

        elif action == "emergency_stop":
            self.bridge.send_line("ESTOP")
            self.cached_status.estop = True
            self.cached_status.drive = "stopped"

        elif action == "clear_estop":
            self.bridge.send_line("CLEAR ESTOP")
            self.cached_status.estop = False

        elif action == "manual":
            self.bridge.send_line("MODE MANUAL")
            self.cached_status.mode = "manual"

        elif action == "test":
            self.bridge.send_line("MODE TEST")
            self.cached_status.mode = "test"

        elif action == "idle":
            self.bridge.send_line("MODE IDLE")
            self.cached_status.mode = "idle"
            self.cached_status.drive = "stopped"

        elif action == "camera_on":
            self.cached_status.camera = "online"
            self.cached_status.last_command = action
            return self.cached_status

        elif action == "camera_off":
            self.cached_status.camera = "offline"
            self.cached_status.last_command = action
            return self.cached_status

        else:
            raise ValueError(f"Unknown action: {action}")

        self.cached_status.last_command = action
        return self.get_status()

from pydantic import BaseModel
from typing import Optional


class ControlCommand(BaseModel):
    action: str
    value: Optional[float] = None


class RoverStatus(BaseModel):
    connected: bool = False
    drive: str = "stopped"
    scoop: str = "idle"
    mode: str = "manual"
    camera: str = "offline"
    estop: bool = False
    battery_v: Optional[float] = None
    left_ticks: int = 0
    right_ticks: int = 0
    tof_distance: Optional[int] = None
    last_command: str = "none"

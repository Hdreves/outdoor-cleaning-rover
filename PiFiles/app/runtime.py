from fastapi.templating import Jinja2Templates
from app.services.serial_bridge import SerialBridge
from app.services.rover_service import RoverService
from app.services.camera_service import CameraService

bridge = SerialBridge(port="/dev/ttyACM0", baudrate=9600)
rover = RoverService(bridge)
camera_service = CameraService()
templates = Jinja2Templates(directory="templates")

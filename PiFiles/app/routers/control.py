from fastapi import APIRouter, HTTPException
from app.runtime import rover, camera_service
from app.schemas import ControlCommand

router = APIRouter()

@router.post("/api/control")
def control_rover(command: ControlCommand):
    try:
        action = command.action.strip().lower()

        if action == "camera_on":
            camera_service.start()
            rover.cached_status.camera = "online"
            rover.cached_status.last_command = action
            return {"ok": True, "state": rover.cached_status.model_dump()}

        if action == "camera_off":
            camera_service.stop()
            rover.cached_status.camera = "offline"
            rover.cached_status.last_command = action
            return {"ok": True, "state": rover.cached_status.model_dump()}

        status = rover.handle_command(command)
        return {"ok": True, "state": status.model_dump()}
    except Exception as exc:
        raise HTTPException(status_code=400, detail=str(exc))

from fastapi import APIRouter, HTTPException
from fastapi.responses import StreamingResponse
from app.runtime import camera_service

router = APIRouter()

@router.get("/camera/stream")
def camera_stream():
    if not camera_service.is_running():
        raise HTTPException(status_code=503, detail="Camera is offline")

    return StreamingResponse(
        camera_service.mjpeg_generator(),
        media_type="multipart/x-mixed-replace; boundary=frame",
    )

from fastapi import APIRouter, Request
from app.runtime import rover, templates, camera_service

router = APIRouter()

@router.get("/")
def home(request: Request):
    return templates.TemplateResponse(request=request, name="index.html")

@router.get("/api/status")
def get_status():
    status = rover.get_status()
    status.camera = "online" if camera_service.is_running() else "offline"
    return status.model_dump()

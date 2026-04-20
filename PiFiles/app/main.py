from contextlib import asynccontextmanager
from fastapi import FastAPI
from fastapi.staticfiles import StaticFiles
from app.runtime import bridge, rover, camera_service
from app.routers.control import router as control_router
from app.routers.status import router as status_router
from app.routers.camera import router as camera_router

@asynccontextmanager
async def lifespan(app: FastAPI):
    bridge.connect()
    rover.ping()
    yield
    camera_service.stop()
    bridge.close()


app = FastAPI(title="Rover UI", lifespan=lifespan)

app.mount("/static", StaticFiles(directory="static"), name="static")

app.include_router(control_router)
app.include_router(status_router)
app.include_router(camera_router)

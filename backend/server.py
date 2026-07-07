import asyncio
import json
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from fastapi.middleware.cors import CORSMiddleware
from bridge import run_sort, run_path

app = FastAPI()
# allow the frontend to connect from file:// or localhost
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

# helpers
async def stream_race(ws: WebSocket, frames1: list, frames2: list, delay: float):
    """interleave frames from two algorithms and stream over websocket"""
    max_frames = max(len(frames1), len(frames2))

    for i in range(max_frames):
        if i < len(frames1):
            await ws.send_text(json.dumps({"player": 1, "frame": frames1[i]}))
        if i < len(frames2):
            await ws.send_text(json.dumps({"player": 2, "frame": frames2[i]}))
        await asyncio.sleep(delay)

    await ws.send_text(json.dumps({"event": "race_over"}))

# websocket endpoint 
@app.websocket("/ws")
async def websocket_endpoint(ws: WebSocket):
    await ws.accept()
    try:
        while True:
            raw  = await ws.receive_text()
            data = json.loads(raw)
            mode    = data.get("mode")          # "sort" or "path"
            algo1   = data.get("algo1")
            algo2   = data.get("algo2")
            speed   = float(data.get("speed", 1.0))   # frames per second
            delay   = 1.0 / speed
            if mode == "sort":
                array   = data["array"]
                frames1 = run_sort(algo1, array[:])
                frames2 = run_sort(algo2, array[:])
            elif mode == "path":
                grid    = data["grid"]
                rows    = data["rows"]
                cols    = data["cols"]
                start   = data["start"]
                goal    = data["goal"]
                frames1 = run_path(algo1, grid[:], rows, cols, start, goal)
                frames2 = run_path(algo2, grid[:], rows, cols, start, goal)
            else:
                await ws.send_text(json.dumps({"error": f"unknown mode: {mode}"}))
                continue
            await ws.send_text(json.dumps({
                "event":        "race_start",
                "algo1":        algo1,
                "algo2":        algo2,
                "total_frames": {
                    "algo1": len(frames1),
                    "algo2": len(frames2),
                }
            }))
            await stream_race(ws, frames1, frames2, delay)

    except WebSocketDisconnect:
        pass
    except Exception as e:
        await ws.send_text(json.dumps({"error": str(e)}))

FRONTEND_DIR = Path(__file__).resolve().parent.parent / "frontend"
app.mount("/", StaticFiles(directory=FRONTEND_DIR, html=True), name="frontend")

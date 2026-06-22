# AlgoScope
A real-time visualization tool where C implementations of sorting and pathfinding algorithms race side-by-side in the browser.

## Tech Stack

C — algorithm implementations
Python + FastAPI — backend and WebSocket streaming
Vanilla JS + HTML5 Canvas — browser animations


## Getting Started
1. Compile C algorithms
cd c && make

2. Install Python dependencies
pip install -r requirements.txt

3. Start the backend
uvicorn backend.server:app --reload

4. Open frontend/index.html in your browser

## Project Structure

algoscope/
├── c/              # C algorithm source files
├── backend/        # FastAPI server and ctypes bridge
└── frontend/       # HTML, JS, Canvas renderer

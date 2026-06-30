# AlgoScope

A real-time algorithm visualization tool where C implementations of sorting and pathfinding algorithms stream their step-by-step state to a Python FastAPI backend, which pushes live updates to an animated browser frontend via WebSockets.

Watch bubble sort, merge sort, A*, and Dijkstra race side-by-side — with play, pause, and speed controls.

# Features

- C algorithms emit step-by-step state via callbacks (no polling)
- Python bridge using ctypes to call compiled .so shared libraries
- FastAPI WebSocket server streams JSON step frames to the browser
- Side-by-side Canvas animations for sorting bars and pathfinding grids
- Play, pause, and speed controls in the UI
- Supports 4 algorithms out of the box — easily extensible


# Architecture

1. C Layer
Algorithms: bubble_sort, merge_sort, a_star, dijkstra. Each one emits step callbacks as it runs.
↓ (ctypes / subprocess pipe)

3. Python Backend
ctypes bridge calls into the compiled C library → FastAPI serializes each step into a JSON frame.
↓ (WebSocket)

5. Browser Frontend
WebSocket client receives frames → Canvas renderer animates them → Play / Pause / Speed controls let the user control playback.

# Project Structure
```
algoscope/
├── c/
│   ├── sort.c          # Bubble sort, merge sort
│   ├── pathfind.c      # A* and Dijkstra
│   └── Makefile        # Builds shared libraries (.so)
├── backend/
│   ├── server.py       # FastAPI app with WebSocket endpoints
│   └── bridge.py       # ctypes wrappers for C algorithms
├── frontend/
│   ├── index.html      # Main UI
│   ├── renderer.js     # Canvas drawing for bars and grids
│   └── controls.js     # Play / pause / speed / algorithm picker
├── requirements.txt
├── LICENSE
└── README.md
```

## Getting Started

# Prerequisites

- Python 3.10+
- GCC (for compiling C)
- A modern browser (Chrome, Firefox, Edge)


1. Clone the repo
'''
bashgit clone https://github.com/your-username/algoscope.git
cd algoscope
'''
2. Compile the C algorithms
'''
bashcd c
make
'''
# Produces sort.so and pathfind.so

3. Install Python dependencies
'''
bashpip install -r requirements.txt
'''
4. Start the backend
'''
bashcd backend
uvicorn server:app --reload --port 8000
'''
5. Open the frontend

Open frontend/index.html in your browser, or serve it locally:
'''
bashcd frontend
python -m http.server 3000
'''
Then visit http://localhost:3000

# Extending the Project

## To add a new algorithm:

- Implement it in c/sort.c or c/pathfind.c with a step callback
- Recompile with make
- Add a Python wrapper in backend/bridge.py
- Register a new WebSocket route in backend/server.py
- Add a renderer case in frontend/renderer.js

# Contributing

Pull requests are welcome! For major changes, please open an issue first to discuss what you'd like to change.

- Fork the repo
- Create your feature branch (git checkout -b feature/add-heapsort)
- Commit your changes (git commit -m 'Add heapsort with step callbacks')
- Push to the branch (git push origin feature/add-heapsort)
- Open a pull request

#License
This project is licensed under the GNU General Public License v3.0 — see LICENSE for details.

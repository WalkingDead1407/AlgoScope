(function () {
  "use strict";

  const WS_URL = "ws://localhost:8000/ws";

  const ALGOS = {
    sort: ["bubble", "merge"],
    path: ["dijkstra", "astar"],
  };

  const els = {
    modeSeg: document.getElementById("modeSeg"),
    selA: document.getElementById("selA"),
    selB: document.getElementById("selB"),
    sizeLabel: document.getElementById("sizeLabel"),
    sizeSlider: document.getElementById("sizeSlider"),
    sizeReadout: document.getElementById("sizeReadout"),
    wallsGroup: document.getElementById("wallsGroup"),
    wallsSlider: document.getElementById("wallsSlider"),
    wallsReadout: document.getElementById("wallsReadout"),
    speedSlider: document.getElementById("speedSlider"),
    speedReadout: document.getElementById("speedReadout"),
    playPauseBtn: document.getElementById("playPauseBtn"),
    startBtn: document.getElementById("startBtn"),
  };

  let mode = "sort";

  // mode + dropdown setup

  const ALGO_LABELS = {
    bubble: "bubble_sort()",
    merge: "merge_sort()",
    dijkstra: "dijkstra()",
    astar: "a_star()",
  };

  function populateAlgoSelects() {
    const options = ALGOS[mode];
    [els.selA, els.selB].forEach((sel, i) => {
      sel.innerHTML = "";
      options.forEach((name) => {
        const opt = document.createElement("option");
        opt.value = name;
        opt.textContent = ALGO_LABELS[name] || name;
        sel.appendChild(opt);
      });
      sel.selectedIndex = Math.min(i, options.length - 1);
    });
  }

  function setMode(next) {
    mode = next;
    els.modeSeg.querySelectorAll("button").forEach((b) => {
      b.classList.toggle("active", b.dataset.mode === mode);
    });
    populateAlgoSelects();

    if (mode === "sort") {
      els.sizeLabel.textContent = "Array size";
      els.sizeSlider.min = 5;
      els.sizeSlider.max = 60;
      els.sizeSlider.value = 14;
      els.wallsGroup.classList.add("hidden");
    } else {
      els.sizeLabel.textContent = "Grid size (n × n)";
      els.sizeSlider.min = 5;
      els.sizeSlider.max = 25;
      els.sizeSlider.value = 10;
      els.wallsGroup.classList.remove("hidden");
    }
    els.sizeReadout.textContent = els.sizeSlider.value;
  }

  els.modeSeg.addEventListener("click", (e) => {
    const btn = e.target.closest("button[data-mode]");
    if (btn) setMode(btn.dataset.mode);
  });

  els.sizeSlider.addEventListener("input", () => {
    els.sizeReadout.textContent = els.sizeSlider.value;
  });

  els.wallsSlider.addEventListener("input", () => {
    els.wallsReadout.textContent = els.wallsSlider.value + "%";
  });

  els.speedSlider.addEventListener("input", () => {
    const fps = Number(els.speedSlider.value);
    els.speedReadout.textContent = fps + " fps";
    window.AlgoScope.setSpeed(fps);
  });

  els.playPauseBtn.addEventListener("click", () => {
    window.AlgoScope.togglePlay();
  });

  //input generation

  function randomArray(size) {
    const arr = [];
    for (let i = 0; i < size; i++) arr.push(1 + Math.floor(Math.random() * 99));
    return arr;
  }

  // Builds a rows x cols grid (flat, row-major) with walls at the given
  // density, keeping start/goal clear, and retries until start can reach
  // goal (plain BFS over open cells) so races don't stall on a sealed maze.
  function randomGrid(n, wallPct) {
    const rows = n;
    const cols = n;
    const start = 0;
    const goal = rows * cols - 1;
    const density = wallPct / 100;

    for (let attempt = 0; attempt < 25; attempt++) {
      const grid = new Array(rows * cols).fill(0); // unvisited cells 
      for (let i = 0; i < grid.length; i++) {
        if (i === start || i === goal) continue;
        if (Math.random() < density) grid[i] = 4; // cell wall
      }
      if (isReachable(grid, rows, cols, start, goal)) {
        return { grid, rows, cols, start, goal };
      }
    }
    // fallback: no walls at all, guaranteed reachable
    return { grid: new Array(rows * cols).fill(0), rows, cols, start, goal };
  }

  function isReachable(grid, rows, cols, start, goal) {
    const seen = new Uint8Array(grid.length);
    const queue = [start];
    seen[start] = 1;
    while (queue.length) {
      const cur = queue.shift();
      if (cur === goal) return true;
      const row = Math.floor(cur / cols);
      const col = cur % cols;
      const neighbors = [
        [row - 1, col], [row + 1, col], [row, col - 1], [row, col + 1],
      ];
      for (const [r, c] of neighbors) {
        if (r < 0 || r >= rows || c < 0 || c >= cols) continue;
        const idx = r * cols + c;
        if (seen[idx] || grid[idx] === 4) continue;
        seen[idx] = 1;
        queue.push(idx);
      }
    }
    return false;
  }

  // race launch

  els.startBtn.addEventListener("click", () => {
    const speed = Number(els.speedSlider.value);
    const algo1 = els.selA.value;
    const algo2 = els.selB.value;

    let config;
    if (mode === "sort") {
      const size = Number(els.sizeSlider.value);
      config = {
        mode: "sort",
        algo1,
        algo2,
        array: randomArray(size),
        speed,
      };
    } else {
      const n = Number(els.sizeSlider.value);
      const wallPct = Number(els.wallsSlider.value);
      const { grid, rows, cols, start, goal } = randomGrid(n, wallPct);
      config = {
        mode: "path",
        algo1,
        algo2,
        grid,
        rows,
        cols,
        start,
        goal,
        speed,
      };
    }

    window.AlgoScope.setSpeed(speed);
    window.AlgoScope.startRace(config, WS_URL);
  });

  // init
  setMode("sort");
})();

/*
 * AlgoScope — controls.js
 *
 * Reads the control dock, builds a race config that matches the shape
 * the FastAPI server expects, generates the input data (random array or
 * random solvable-ish grid), and hands off to window.AlgoScope.startRace.
 * Nothing in here touches the WebSocket directly — that's renderer.js's job.
 */

(function () {
  "use strict";

  const els = {
    canvasA: document.getElementById("canvasA"),
    canvasB: document.getElementById("canvasB"),
    algoNameA: document.getElementById("algoNameA"),
    algoNameB: document.getElementById("algoNameB"),
    statsA: document.getElementById("statsA"),
    statsB: document.getElementById("statsB"),
    fillA: document.getElementById("fillA"),
    fillB: document.getElementById("fillB"),
    leaderMark: document.getElementById("leaderMark"),
    connDot: document.getElementById("connDot"),
    connLabel: document.getElementById("connLabel"),
    playPauseBtn: document.getElementById("playPauseBtn"),
  };

  const ctxA = els.canvasA.getContext("2d");
  const ctxB = els.canvasB.getContext("2d");

  // Resolve design tokens once from CSS so canvas fills stay in sync with the stylesheet.
  const rootStyle = getComputedStyle(document.documentElement);
  const COLOR = {
    panelRaised: rootStyle.getPropertyValue("--bg-panel-raised").trim(),
    hair: rootStyle.getPropertyValue("--line-hair").trim(),
    laneA: rootStyle.getPropertyValue("--lane-a").trim(),
    laneADim: rootStyle.getPropertyValue("--lane-a-dim").trim(),
    laneB: rootStyle.getPropertyValue("--lane-b").trim(),
    laneBDim: rootStyle.getPropertyValue("--lane-b-dim").trim(),
    alert: rootStyle.getPropertyValue("--alert").trim(),
    gold: rootStyle.getPropertyValue("--gold").trim(),
    ink: rootStyle.getPropertyValue("--ink-primary").trim(),
    faint: rootStyle.getPropertyValue("--ink-faint").trim(),
  };

  const ALGO_LABELS = {
    bubble: "bubble_sort()",
    merge: "merge_sort()",
    dijkstra: "dijkstra()",
    astar: "a_star()",
  };

  const state = {
    ws: null,
    mode: "sort",
    queueA: [],
    queueB: [],
    lastFrameA: null,
    lastFrameB: null,
    consumedA: 0,
    consumedB: 0,
    totalA: 0,
    totalB: 0,
    maxValue: 1, // sort bar scale, derived from the starting array
    playing: false,
    speed: 10, // fps, client-side only
    tickHandle: null,
    serverDone: false,
    finishedA: false,
    finishedB: false,
    winnerDecided: false,
  };

  // canvas sizing

  function fitCanvas(canvas) {
    const ratio = window.devicePixelRatio || 1;
    const rect = canvas.getBoundingClientRect();
    canvas.width = Math.max(1, Math.round(rect.width * ratio));
    canvas.height = Math.max(1, Math.round(rect.height * ratio));
    const ctx = canvas.getContext("2d");
    ctx.setTransform(ratio, 0, 0, ratio, 0, 0);
    return { width: rect.width, height: rect.height };
  }

  function redrawLane(which) {
    const frame = which === "A" ? state.lastFrameA : state.lastFrameB;
    if (!frame) return;
    drawFrame(which, frame);
  }

  window.addEventListener("resize", () => {
    fitCanvas(els.canvasA);
    fitCanvas(els.canvasB);
    redrawLane("A");
    redrawLane("B");
  });

  // drawing

  function drawFrame(which, frame) {
    const canvas = which === "A" ? els.canvasA : els.canvasB;
    const ctx = which === "A" ? ctxA : ctxB;
    const rect = canvas.getBoundingClientRect();
    const laneColor = which === "A" ? COLOR.laneA : COLOR.laneB;
    const laneColorDim = which === "A" ? COLOR.laneADim : COLOR.laneBDim;

    ctx.clearRect(0, 0, rect.width, rect.height);
    ctx.fillStyle = COLOR.panelRaised;
    ctx.fillRect(0, 0, rect.width, rect.height);

    if (frame.type === "sort") {
      drawSort(ctx, rect, frame, laneColor, laneColorDim);
    } else if (frame.type === "path") {
      drawPath(ctx, rect, frame, laneColor, laneColorDim);
    }
  }

  function drawSort(ctx, rect, frame, laneColor, laneColorDim) {
    const arr = frame.array;
    const n = arr.length;
    if (n === 0) return;
    const gap = 2;
    const barWidth = (rect.width - gap * (n + 1)) / n;
    const floorY = rect.height - 10;
    const usableHeight = rect.height - 20;

    for (let i = 0; i < n; i++) {
      const value = arr[i];
      const barHeight = Math.max(2, (value / state.maxValue) * usableHeight);
      const x = gap + i * (barWidth + gap);
      const y = floorY - barHeight;

      let fill = laneColorDim;
      let glow = null;
      if (i === frame.swap_a || i === frame.swap_b) {
        fill = COLOR.alert;
        glow = COLOR.alert;
      } else if (i === frame.compare_a || i === frame.compare_b) {
        fill = COLOR.gold;
        glow = COLOR.gold;
      }

      ctx.save();
      if (glow) {
        ctx.shadowColor = glow;
        ctx.shadowBlur = 10;
      }
      ctx.fillStyle = fill;
      ctx.fillRect(x, y, Math.max(1, barWidth), barHeight);
      ctx.restore();
    }

    // baseline, tinted with the lane's own color so the two panels read as distinct tracks
    ctx.strokeStyle = laneColor;
    ctx.globalAlpha = 0.5;
    ctx.beginPath();
    ctx.moveTo(0, floorY + 0.5);
    ctx.lineTo(rect.width, floorY + 0.5);
    ctx.stroke();
    ctx.globalAlpha = 1;
  }

  function drawPath(ctx, rect, frame, laneColor, laneColorDim) {
    const rows = frame.rows;
    const cols = frame.cols;
    const grid = frame.grid;
    if (!rows || !cols) return;

    const cellSize = Math.max(1, Math.min(rect.width / cols, rect.height / rows));
    const offsetX = (rect.width - cellSize * cols) / 2;
    const offsetY = (rect.height - cellSize * rows) / 2;

    for (let idx = 0; idx < grid.length; idx++) {
      const row = Math.floor(idx / cols);
      const col = idx % cols;
      const x = offsetX + col * cellSize;
      const y = offsetY + row * cellSize;
      const cellState = grid[idx];

      let fill = "rgba(255,255,255,0.03)"; // unvisited
      if (cellState === 4) fill = COLOR.faint; // wall
      else if (cellState === 1) fill = laneColorDim; // open
      else if (cellState === 2) fill = laneColor; // closed
      else if (cellState === 3) fill = COLOR.gold; // path

      ctx.fillStyle = fill;
      ctx.fillRect(x + 0.5, y + 0.5, cellSize - 1, cellSize - 1);

      if (idx === frame.current) {
        ctx.save();
        ctx.strokeStyle = COLOR.gold;
        ctx.shadowColor = COLOR.gold;
        ctx.shadowBlur = 8;
        ctx.lineWidth = 2;
        ctx.strokeRect(x + 1.5, y + 1.5, cellSize - 3, cellSize - 3);
        ctx.restore();
      }
    }
  }

  // stats + telemetry

  function updateStats(which, frame, consumed, total) {
    const el = which === "A" ? els.statsA : els.statsB;
    let body = `<span>frame <b>${consumed}${total ? " / " + total : ""}</b></span>`;
    if (frame.type === "sort") {
      body += `<span>comparisons <b>${frame.comparisons}</b></span>`;
      body += `<span>swaps <b>${frame.swaps}</b></span>`;
    } else if (frame.type === "path") {
      body += `<span>steps <b>${frame.steps}</b></span>`;
      if (frame.done) body += `<span>${frame.found ? "path found" : "no path"}</span>`;
    }
    el.innerHTML = body;
  }

  function updateTelemetry() {
    const pctA = state.totalA ? Math.min(100, (state.consumedA / state.totalA) * 100) : 0;
    const pctB = state.totalB ? Math.min(100, (state.consumedB / state.totalB) * 100) : 0;
    els.fillA.style.height = pctA + "%";
    els.fillB.style.height = pctB + "%";

    if (!state.winnerDecided) {
      if (pctA > pctB) {
        els.leaderMark.textContent = "A ▲";
        els.leaderMark.style.color = COLOR.laneA;
      } else if (pctB > pctA) {
        els.leaderMark.textContent = "B ▲";
        els.leaderMark.style.color = COLOR.laneB;
      } else {
        els.leaderMark.textContent = "—";
        els.leaderMark.style.color = "";
      }
    }
  }

  function decideWinnerIfNeeded() {
    if (state.winnerDecided) return;
    if (state.finishedA && state.finishedB) {
      state.winnerDecided = true;
      els.leaderMark.textContent = "TIE";
      els.leaderMark.style.color = COLOR.ink;
    } else if (state.finishedA) {
      state.winnerDecided = true;
      els.leaderMark.textContent = "A WINS";
      els.leaderMark.style.color = COLOR.laneA;
    } else if (state.finishedB) {
      state.winnerDecided = true;
      els.leaderMark.textContent = "B WINS";
      els.leaderMark.style.color = COLOR.laneB;
    }
  }

  // playback clock

  function tick() {
    if (state.queueA.length > 0) {
      const frame = state.queueA.shift();
      state.lastFrameA = frame;
      state.consumedA++;
      drawFrame("A", frame);
      updateStats("A", frame, state.consumedA, state.totalA);
      if (frame.done && !state.finishedA) {
        state.finishedA = true;
        decideWinnerIfNeeded();
      }
    }

    if (state.queueB.length > 0) {
      const frame = state.queueB.shift();
      state.lastFrameB = frame;
      state.consumedB++;
      drawFrame("B", frame);
      updateStats("B", frame, state.consumedB, state.totalB);
      if (frame.done && !state.finishedB) {
        state.finishedB = true;
        decideWinnerIfNeeded();
      }
    }

    updateTelemetry();

    const drained = state.queueA.length === 0 && state.queueB.length === 0;
    if (state.serverDone && drained) {
      stopClock();
      setConnStatus("idle", "race complete");
    }
  }

  function startClock() {
    stopClock();
    state.tickHandle = setInterval(tick, 1000 / state.speed);
  }

  function stopClock() {
    if (state.tickHandle) {
      clearInterval(state.tickHandle);
      state.tickHandle = null;
    }
    state.playing = false;
    els.playPauseBtn.textContent = "▶";
    els.playPauseBtn.setAttribute("aria-label", "Play");
  }

  //connection status
  function setConnStatus(kind, label) {
    els.connDot.className = "dot" + (kind ? " " + kind : "");
    els.connLabel.textContent = label;
  }

  // public api
  function resetLaneVisuals() {
    [ctxA, ctxB].forEach((ctx, i) => {
      const canvas = i === 0 ? els.canvasA : els.canvasB;
      const rect = canvas.getBoundingClientRect();
      ctx.clearRect(0, 0, rect.width, rect.height);
      ctx.fillStyle = COLOR.panelRaised;
      ctx.fillRect(0, 0, rect.width, rect.height);
    });
    els.statsA.innerHTML = "<span>frame <b>0</b></span>";
    els.statsB.innerHTML = "<span>frame <b>0</b></span>";
    els.fillA.style.height = "0%";
    els.fillB.style.height = "0%";
    els.leaderMark.textContent = "—";
    els.leaderMark.style.color = "";
  }

  function startRace(config, wsUrl) {
    if (state.ws) {
      try { state.ws.close(); } catch (e) { /* noop */ }
    }
    stopClock();

    Object.assign(state, {
      mode: config.mode,
      queueA: [],
      queueB: [],
      lastFrameA: null,
      lastFrameB: null,
      consumedA: 0,
      consumedB: 0,
      totalA: 0,
      totalB: 0,
      serverDone: false,
      finishedA: false,
      finishedB: false,
      winnerDecided: false,
    });

    state.maxValue = config.mode === "sort" ? Math.max(1, ...config.array) : 1;

    fitCanvas(els.canvasA);
    fitCanvas(els.canvasB);
    resetLaneVisuals();

    els.algoNameA.textContent = ALGO_LABELS[config.algo1] || config.algo1;
    els.algoNameB.textContent = ALGO_LABELS[config.algo2] || config.algo2;

    setConnStatus("connecting", "connecting…");

    const ws = new WebSocket(wsUrl);
    state.ws = ws;

    ws.onopen = () => {
      setConnStatus("live", "streaming");
      ws.send(JSON.stringify(config));
    };

    ws.onmessage = (ev) => {
      let data;
      try {
        data = JSON.parse(ev.data);
      } catch (e) {
        return;
      }

      if (data.event === "race_start") {
        state.totalA = (data.total_frames && data.total_frames.algo1) || 0;
        state.totalB = (data.total_frames && data.total_frames.algo2) || 0;
        return;
      }
      if (data.event === "race_over") {
        state.serverDone = true;
        return;
      }
      if (data.player === 1) {
        state.queueA.push(data.frame);
      } else if (data.player === 2) {
        state.queueB.push(data.frame);
      }
    };

    ws.onerror = () => {
      setConnStatus("error", "connection error");
    };

    ws.onclose = () => {
      if (!state.serverDone) setConnStatus("error", "disconnected");
    };

    // Start paused-off (playing) by default so the race is visible immediately.
    state.playing = true;
    els.playPauseBtn.textContent = "⏸";
    els.playPauseBtn.setAttribute("aria-label", "Pause");
    startClock();
  }

  function togglePlay() {
    if (!state.ws) return;
    if (state.playing) {
      stopClock();
    } else {
      state.playing = true;
      els.playPauseBtn.textContent = "⏸";
      els.playPauseBtn.setAttribute("aria-label", "Pause");
      startClock();
    }
  }

  function setSpeed(fps) {
    state.speed = Math.max(1, fps);
    if (state.playing) startClock(); // re-arm interval at the new rate
  }

  // initial paint so empty lanes aren't a flash of unstyled canvas
  fitCanvas(els.canvasA);
  fitCanvas(els.canvasB);
  resetLaneVisuals();

  window.AlgoScope = {
    startRace,
    togglePlay,
    setSpeed,
    isPlaying: () => state.playing,
  };
})();


/*
 * AlgoScope — renderer.js
 *
 * Owns: the WebSocket connection, per-lane frame buffers, the playback
 * clock, and all canvas drawing. controls.js drives this module through
 * the small public API attached to `window.AlgoScope`.
 *
 * Playback model: the server streams frames as fast as it likes. This
 * module pushes every incoming frame onto a per-lane queue immediately
 * (so nothing is ever dropped), and a single local interval — paced by
 * the client-side speed slider — pulls one buffered frame per lane per
 * tick and draws it. Pausing simply stops the interval from draining the
 * queues; frames keep arriving and stacking up underneath. Resuming
 * continues consuming from exactly where it left off.
 */

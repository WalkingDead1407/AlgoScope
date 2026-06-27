import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))

from bridge import run_sort, run_path

#test sorting
def test_sort():
    array = [5, 3, 8, 1, 9, 2, 7, 4, 6]
    for algo in ["bubble", "merge"]:
        frames= run_sort(algo, array)
        done= frames[-1]
        print(f"[{algo}] frames: {len(frames)} | "
              f"comparisons: {done['comparisons']} | "
              f"swaps: {done['swaps']} | "
              f"done: {done['done']} | "
              f"final: {done['array']}")

        assert done["done"] == 1,f"{algo}: done flag not set"
        assert done["array"]== sorted(array),f"{algo}: array not sorted"
    print("sort tests passed\n")

#test pathfinding
def test_path():
    rows, cols = 4, 5
    grid= [0] * (rows * cols)   # all passable
    start, goal = 0, rows * cols - 1

    for algo in ["dijkstra", "astar"]:
        frames= run_path(algo, grid[:], rows, cols, start, goal)
        done= frames[-1]

        print(f"[{algo}] frames: {len(frames)} | "
              f"steps: {done['steps']} | "
              f"found: {done['found']} | "
              f"done: {done['done']}")
        assert done["done"]  == 1,  f"{algo}: done flag not set"
        assert done["found"] == 1,  f"{algo}: path not found on open grid"
    print("path tests passed\n")

if __name__ == "__main__":
    test_sort()
    test_path()

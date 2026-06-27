import ctypes
import os
from pathlib import Path

#path
C_DIR = Path(__file__).parent.parent / "c"


#load shared libraries
libbubble= ctypes.CDLL(str(C_DIR / "libbubble.so"))
libmerge= ctypes.CDLL(str(C_DIR / "libmerge.so"))
libastar= ctypes.CDLL(str(C_DIR / "libastar.so"))
libdijkstra= ctypes.CDLL(str(C_DIR / "libdijkstra.so"))

#copy SortFrame from algo_interface.h
class SortFrame(ctypes.Structure):
    _fields_ = [
        ("array",ctypes.POINTER(ctypes.c_int)),
        ("size",ctypes.c_size_t),
        ("compare_a",ctypes.c_int),
        ("compare_b",ctypes.c_int),
        ("swap_a",ctypes.c_int),
        ("swap_b",ctypes.c_int),
        ("comparisons",ctypes.c_uint64),
        ("swaps", ctypes.c_uint64),
        ("done",ctypes.c_int),
    ]

#copy PathFrame from algo_interface.h
class PathFrame(ctypes.Structure):
    _fields_ = [
        ("grid",ctypes.POINTER(ctypes.c_int)),
        ("rows",ctypes.c_int),
        ("cols",ctypes.c_int),
        ("current",ctypes.c_int),
        ("done",ctypes.c_int),
        ("found",ctypes.c_int),
        ("steps",ctypes.c_uint64),
    ]

#callback types
SortCallbackType = ctypes.CFUNCTYPE(None, ctypes.POINTER(SortFrame), ctypes.c_void_p)
PathCallbackType = ctypes.CFUNCTYPE(None, ctypes.POINTER(PathFrame), ctypes.c_void_p)

#declare C function signatures
libbubble.bubble_sort.argtypes= [ctypes.POINTER(ctypes.c_int), ctypes.c_size_t, SortCallbackType, ctypes.c_void_p]
libbubble.bubble_sort.restype= None
libmerge.merge_sort.argtypes = [ctypes.POINTER(ctypes.c_int), ctypes.c_size_t,SortCallbackType, ctypes.c_void_p]
libmerge.merge_sort.restype =None 
libastar.a_star.argtypes= [ctypes.POINTER(ctypes.c_int), ctypes.c_int,ctypes.c_int, ctypes.c_int, ctypes.c_int,PathCallbackType, ctypes.c_void_p]
libastar.a_star.restype=None 
libdijkstra.dijkstra.argtypes= [ctypes.POINTER(ctypes.c_int), ctypes.c_int,ctypes.c_int, ctypes.c_int, ctypes.c_int,PathCallbackType, ctypes.c_void_p]
libdijkstra.dijkstra.restype=None

#sorting
def run_sort(algo: str, array: list[int]) -> list[dict]:
    size= len(array)
    c_array = (ctypes.c_int * size)(*array)
    frames= []

    def callback(frame_ptr, _):
        f = frame_ptr.contents
        frames.append({
            "type":"sort",
            "algo":algo,
            "array":list(f.array[:f.size]),
            "compare_a":f.compare_a,
            "compare_b":f.compare_b,
            "swap_a":f.swap_a,
            "swap_b":f.swap_b,
            "comparisons":f.comparisons,
            "swaps":f.swaps,
            "done":f.done,
        })

    cb = SortCallbackType(callback)

    if algo == "bubble":
        libbubble.bubble_sort(c_array, ctypes.c_size_t(size), cb, None)
    elif algo == "merge":
        libmerge.merge_sort(c_array, ctypes.c_size_t(size), cb, None)
    else:
        raise ValueError(f"unknown sort algo: {algo}")

    return frames

#pathfinding
def run_path(algo: str, grid: list[int], rows: int, cols: int,
             start: int, goal: int) -> list[dict]:

    c_grid = (ctypes.c_int * len(grid))(*grid)
    frames = []

    def callback(frame_ptr, _):
        f = frame_ptr.contents
        frames.append({
            "type":"path",
            "algo":algo,
            "grid":list(f.grid[:rows * cols]),
            "rows":f.rows,
            "cols":f.cols,
            "current":f.current,
            "steps":f.steps,
            "done":f.done,
            "found":f.found,
        })

    cb = PathCallbackType(callback)
    if algo == "astar":
        libastar.a_star(c_grid, rows, cols, start, goal, cb, None)
    elif algo == "dijkstra":
        libdijkstra.dijkstra(c_grid, rows, cols, start, goal, cb, None)
    else:
        raise ValueError(f"unknown path algo: {algo}")

    return frames

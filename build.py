import subprocess
import sys
from pathlib import Path

def build():
    c_dir = Path(__file__).parent / "c"
    result = subprocess.run(["make", "-C", str(c_dir)], check=False)
    if result.returncode != 0:
        print("ERROR: C build failed", file=sys.stderr)
        sys.exit(1)
    print("C shared libraries built successfully.")

if __name__ == "__main__":
    build()

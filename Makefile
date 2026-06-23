.PHONY: all build-c setup run clean dev

# Build C shared libraries
build-c:
	$(MAKE) -C c

# Install Python deps via uv
setup:
	cd backend && uv sync

# Build C libs + install Python deps
all: build-c setup

# Run the dev server
run:
	cd backend && uv run uvicorn server:app --reload

# Full dev bootstrap: build everything then start server
dev: all run

# Clean C build artifacts
clean:
	$(MAKE) -C c clean

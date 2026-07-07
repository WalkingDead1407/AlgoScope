FROM python:3.12-slim

# build-essential gives us gcc + make for the C layer
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# uv, for the Python side
COPY --from=ghcr.io/astral-sh/uv:latest /uv /usr/local/bin/uv

WORKDIR /app
COPY . .

# Build the four .so libraries (uses your root Makefile's build-c target)
RUN make -C c

# Install backend dependencies
RUN cd backend && uv sync --frozen

EXPOSE 8000

# Shell form so ${PORT} (injected by Railway) actually expands
CMD uv --directory backend run uvicorn server:app --host 0.0.0.0 --port ${PORT:-8000}

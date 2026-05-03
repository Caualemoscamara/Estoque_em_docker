FROM gcc:latest

WORKDIR /app

# Copy everything
COPY . .

# Build all binaries
RUN make

# Default (can be overridden by docker-compose)
CMD ["./server"]
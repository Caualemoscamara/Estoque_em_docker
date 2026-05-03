FROM gcc:latest

WORKDIR /app

# Copy everything
COPY . .

# Build all binaries
RUN make

# Ensure executables have permission (just in case)
RUN chmod +x server clientDocker

EXPOSE 8080

# Default (can be overridden by docker-compose)
CMD ["./server"]
#!/bin/bash

# Build the Docker image if it doesn't exist
if ! docker image inspect rt11-assembler >/dev/null 2>&1; then
    echo "Building Docker image..."
    docker build -t rt11-assembler builder/
fi

# Run the container with the current directory mounted to /app/data
docker run -it --rm \
    -v "$(pwd):/app/data" \
    rt11-assembler ./build.sh

echo
echo FIG FORTH standalone binary built:
ls -l forth.bin

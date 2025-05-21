#!/bin/bash

set -e

# Build the Docker image if it doesn't exist
if ! docker image inspect rt11-assembler >/dev/null 2>&1; then
    echo "Building Docker image..."
    docker build -t rt11-assembler builder/
fi

# Fix line endings of listing file when exiting
trap "perl -pi -e 's/\r//g' FORTH.LST" 0

# Run the container with the current directory mounted to /app/data
if docker run -it --rm \
    -v "$(pwd):/app/data" \
    rt11-assembler ./build.sh
then
  echo
  echo FIG FORTH standalone binary built:
  ls -l forth.bin
else
  echo build failed
fi

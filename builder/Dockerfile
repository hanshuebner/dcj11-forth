# Use Debian as base image
FROM debian:bullseye-slim

# Prevent interactive prompts during package installation
ENV DEBIAN_FRONTEND=noninteractive

# Install system dependencies
RUN apt-get update && apt-get install -y \
    python3 \
    python3-pip \
    curl \
    build-essential \
    git \
    pkg-config \
    libssl-dev \
    gcc \
    && rm -rf /var/lib/apt/lists/*

# Install Rust and Cargo
RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
ENV PATH="/root/.cargo/bin:${PATH}"

# Clone and build pdpfs
RUN git clone https://github.com/caldwell/pdpfs.git /tmp/pdpfs \
    && cd /tmp/pdpfs \
    && cargo build --release \
    && mv target/release/pdpfs /usr/local/bin/ \
    && cd / \
    && rm -rf /tmp/pdpfs

# Install SIMH
RUN apt-get update && apt-get install -y \
    simh \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy requirements.txt and install Python dependencies
COPY requirements.txt .
RUN pip3 install -r requirements.txt

# Compile lda2bin with C99 standard
RUN gcc -std=c99 -o lda2bin lda2bin.c

# Copy the rest of the application
COPY . .

# Set the default command
CMD ["/bin/bash"]

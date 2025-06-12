FROM ubuntu:22.04

# Install system dependencies
RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    python3 \
    python3-pip \
    gcc-11 \
    g++-11 \
    make \
    git \
    lcov \
    gcovr \
    && rm -rf /var/lib/apt/lists/*

# Set GCC 11 as default
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-11 110 \
    --slave /usr/bin/g++ g++ /usr/bin/g++-11 \
    --slave /usr/bin/gcov gcov /usr/bin/gcov-11

# Install PlatformIO
RUN pip3 install platformio

# Set working directory
WORKDIR /workspace

# Copy project files
COPY platformio.ini /workspace/platformio.ini

# Install project dependencies
RUN pio pkg update && pio pkg install

# Set entrypoint
ENTRYPOINT ["make"]

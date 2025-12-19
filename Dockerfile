# Multi-stage build for z03mmc firmware using Debian
FROM debian:bookworm AS builder

# Prevent interactive prompts
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    git \
    gcc \
    g++ \
    make \
    build-essential \
    binutils \
    python3 \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Set build environment variables
ENV BUILD_TYPE=Release \
    TOOLCHAIN_DIR=/workspace/tc32 \
    SDK_DIR=/workspace/telink_zigbee_sdk \
    SDK_PREFIX=/workspace/telink_zigbee_sdk/tl_zigbee_sdk

WORKDIR /workspace

# Clone the tc32 toolchain
RUN git clone https://github.com/devbis/tc32.git -b linux --depth 1 ${TOOLCHAIN_DIR}

# Clone the Telink Zigbee SDK
RUN git clone https://github.com/telink-semi/telink_zigbee_sdk.git -b V3.7.1.0 --depth 1 ${SDK_DIR}

# Copy project source
COPY . /workspace/project/

WORKDIR /workspace/project

# Build the firmware
RUN cmake -B /workspace/build \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DSDK_PREFIX=${SDK_PREFIX} \
    -DTOOLCHAIN_PREFIX=${TOOLCHAIN_DIR} && \
    cmake --build /workspace/build --config ${BUILD_TYPE} --target=z03mmc.zigbee

# Export stage - for direct artifact extraction
FROM scratch AS export

ARG BUILD_TYPE=Release

# Copy firmware artifacts (only the key binaries)
COPY --from=builder /workspace/build/src/z03mmc.bin /${BUILD_TYPE}/z03mmc.bin
COPY --from=builder /workspace/build/src/*.zigbee /${BUILD_TYPE}/
COPY --from=builder /workspace/build/src/z03mmc /${BUILD_TYPE}/z03mmc

# Final stage - minimal runtime image (optional, for docker run)
FROM debian:bookworm-slim

ARG BUILD_TYPE=Release

WORKDIR /output

COPY --from=builder /workspace/build/src/ ./${BUILD_TYPE}/

CMD ["/bin/sh"]

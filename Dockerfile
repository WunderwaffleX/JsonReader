FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt update && apt install -y \
    build-essential \
    cmake \
    qt6-base-dev \
    qt6-base-dev-tools \
    qt6-tools-dev \
    qt6-tools-dev-tools \
    libx11-dev \
    libxcb1-dev \
    libx11-xcb-dev \
    libxcb-randr0-dev \
    libxcb-xtest0-dev \
    libxcb-xinerama0-dev \
    libxcb-shape0-dev \
    libxcb-xkb-dev \
    libxcb-cursor-dev \
    libwayland-dev \
    wayland-protocols \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libfontconfig1-dev \
    libfreetype6-dev \
    libxrender-dev \
    libxext-dev \
    libxkbcommon-x11-dev \
    && apt clean && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN mkdir -p build && cd build && cmake .. && cmake --build .

CMD ["./build/JsonReader"]

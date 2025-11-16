#!/bin/bash

# Build script for Reencor C++ port

set -e

echo "Building Reencor C++ version..."

# Check if build directory exists
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

cd build

# Run CMake
echo "Running CMake configuration..."
cmake ..

# Build
echo "Building project..."
make -j$(nproc)

echo ""
echo "Build complete! Executable: build/Reencor"
echo ""
echo "To run the game, make sure you have the Assets folder with game data."
echo "Then run: ./build/Reencor"

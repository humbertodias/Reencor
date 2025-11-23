#!/bin/bash
# Build script for Reencor SDL2 version

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "=========================================="
echo "Reencor SDL2 Build Script"
echo "=========================================="
echo ""

# Check for required tools
echo "Checking for required build tools..."
command -v cmake >/dev/null 2>&1 || { echo -e "${RED}Error: cmake is not installed${NC}"; exit 1; }
command -v g++ >/dev/null 2>&1 || { echo -e "${RED}Error: g++ is not installed${NC}"; exit 1; }
command -v make >/dev/null 2>&1 || { echo -e "${RED}Error: make is not installed${NC}"; exit 1; }
echo -e "${GREEN}✓ Build tools found${NC}"
echo ""

# Check for SDL2 libraries
echo "Checking for SDL2 libraries..."
if ! pkg-config --exists sdl2; then
    echo -e "${YELLOW}Warning: SDL2 not found${NC}"
    echo "Please install SDL2 development libraries:"
    echo "  Ubuntu/Debian: sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libgl1-mesa-dev"
    echo "  macOS: brew install sdl2 sdl2_image sdl2_mixer"
    exit 1
fi
echo -e "${GREEN}✓ SDL2 libraries found${NC}"
echo ""

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Run CMake
echo "Running CMake..."
if ! cmake ..; then
    echo -e "${RED}CMake configuration failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ CMake configuration successful${NC}"
echo ""

# Build
echo "Building project..."
if ! make; then
    echo -e "${RED}Build failed${NC}"
    exit 1
fi
echo -e "${GREEN}✓ Build successful${NC}"
echo ""

echo "=========================================="
echo -e "${GREEN}Build completed successfully!${NC}"
echo "Run the game with: ./build/reencor"
echo "=========================================="

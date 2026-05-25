#!/bin/sh
# Build and run the standalone HDR-ProStudio engine test harness.
# Run from anywhere; paths are resolved relative to the repository root.
set -e

ROOT=$(cd "$(dirname "$0")/.." && pwd)
cd "$ROOT"

g++ -O2 -pipe -fPIC -std=c++11 -pthread \
    -I cpp \
    test/harness.cpp cpp/*.cpp \
    -o test/harness

./test/harness

#!/bin/bash

# Stop at first failure
set -euo pipefail

BUILD_DIR="build"

cmake -S . -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" -j"$(nproc)"

if [ "$#" -gt 0 ]; then
	"./$BUILD_DIR/bin/tests" --gtest_filter="$1.*"
else 
	ctest --test-dir "$BUILD_DIR" --output-on-failure
fi

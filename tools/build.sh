#!/bin/bash
set -e

PLATFORM=${1:-windows}
ROOT_DIR=$(realpath "$(dirname "$0")/..")
DOCKER_BUILD_DIR="/workspace"
BUILD_DIR="$ROOT_DIR/build"

echo "Building for $PLATFORM..."

rsync -a --delete --exclude 'build' --exclude '.git', --exclude '.CMakeFiles' "$ROOT_DIR/" "$DOCKER_BUILD_DIR/"

cmake --preset "$PLATFORM" -S "$DOCKER_BUILD_DIR" -B "$DOCKER_BUILD_DIR/.CMakeFiles"
cmake --build "$DOCKER_BUILD_DIR/.CMakeFiles"

rsync -a --delete "$DOCKER_BUILD_DIR/build/" "$BUILD_DIR/"

echo "Build finished."
#!/bin/bash

cd "$(dirname "$0")"

mkdir -p ./build
rm -rf ./build/*

cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel $(nproc)

cd build
cpack

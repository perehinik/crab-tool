#!/bin/bash

cd "$(dirname "$0")"

mkdir -p ./build
rm -rf ./build/*
cd build

cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --parallel $(nproc)

mkdir -p ../output
cp ./CrabTool-* ../output

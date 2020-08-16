#!/bin/bash

git submodule init
git submodule update
mkdir -p ./build
cmake -B ./build .
cmake --build ./build

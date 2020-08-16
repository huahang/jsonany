#!/bin/bash

git submodule init
git submodule update
mkdir -p ./build
cmake -S . -B ./build
cmake --build ./build

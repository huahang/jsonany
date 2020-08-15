#!/bin/bash

git submodule init
git submodule update
mkdir -p build
pushd build
cmake ..
make
popd

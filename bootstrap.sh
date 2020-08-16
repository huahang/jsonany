#!/bin/bash

git submodule init
git submodule update
cmake .
cmake --build .

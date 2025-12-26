#!/bin/zsh
cmake --build build -j
cd ./build && ./tinyrenderer
cd ..

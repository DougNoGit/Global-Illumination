#!/bin/sh
rm -rf build &&
mkdir build && 
cd build &&
mkdir frames &&
cmake .. &&
make -j12 &&
./P3

#!/bin/bash
set -e
bash ./build.sh -j4
cd build
ctest --output-on-failure $1

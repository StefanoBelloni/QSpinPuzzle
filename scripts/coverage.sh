#!/bin/bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd ${SCRIPT_DIR}/..

rm -rf build_coverage
mkdir -p build_coverage
cd ./build_coverage

# cmake --build . --target clean
# cmake -D RUN_COVERAGE=TRUE -D EXCLUDE='gtest,usr/include/'..

cmake -DCMAKE_BUILD_TYPE=Debug -DRUN_COVERAGE=TRUE -DUSE_QT=no ..
cmake --build .

# ctest --progress
# ========================================== #
# lcov -c -d . -o main_coverage.info
# genhtml main_coverage.info --output-directory ../out
# ========================================== #
# lcov --remove main_coverage.info '/usr/include/*' '*gtest*' -o filtered_coverage.info
# genhtml filtered_coverage.info --output-directory ../out

make t_puzzle_html

# make t_puzzle_lcov


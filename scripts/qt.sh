#!/bin/bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd ${SCRIPT_DIR}/..

rm -rf build_qt
mkdir -p build_qt
cd ./build_qt

cmake -D USE_QT=yes ..
cmake --build .


#!/bin/bash

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

cd ${SCRIPT_DIR}

# ./compile.sh android
./compile.sh coverage
./compile.sh qt
./compile.sh build
./compile.sh format

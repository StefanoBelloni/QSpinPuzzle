#!/bin/bash

set -e

python3 -m venv venv
source venv/bin/activate
for f in `ls packages/ | grep -v 'README'`; do
    pip install packages/$f
done

python tests/test_marbles.py
python tests/test_puzzle_side.py

python -m qspyn_puzzle
#!/bin/bash
set -e

for f in $(find ./tests/ -type f -name "*.lua"); do
  echo "Running js test: $f"
  luajit $f
done

#!/bin/bash
set -e

# waiting for rabbitmq
sleep 10

for f in $(find ./tests/ -type f -name "*.lua"); do
  echo "Running js test: $f"
  luajit $f
done

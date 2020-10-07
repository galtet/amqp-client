#!/bin/bash
set -e

# waiting for rabbitmq
timeout 20 sh -c 'until nc -z $0 $1; do sleep 1; done' rabbitmq 5672

for f in $(find ./tests/ -type f -name "*.lua"); do
  echo "Running js test: $f"
  valgrind --undef-value-errors=no --error-exitcode=1 luajit $f
done

#!/bin/bash
# File: sample_run.sh
# Description:
#   Simple test script to compile and run the Producer-Consumer program automatically.
#   Demonstrates correct compilation, blocking behavior, and synchronization.
#   Example usage:
#       ./sample_run.sh 3 2 10
#   Runs the program with 3 producers, 2 consumers, and a buffer size of 10.

#!/bin/bash

# Simple build and test script for the mini project.

set -e

echo "Compiling producer_consumer..."

gcc -Wall -Wextra -std=c11 \
    -Iinclude -Isrc \
    src/main.c \
    src/buffer.c \
    src/producer.c \
    src/consumer.c \
    src/producer_consumer.c \
    -pthread \
    -o producer_consumer

echo ""
echo "Build complete."
echo ""

echo "=== Test 1: Small buffer (stress case) ==="
echo "./producer_consumer 4 4 2"
./producer_consumer 4 4 2

echo ""
echo "=== Test 2: Larger buffer ==="
echo "./producer_consumer 4 4 32"
./producer_consumer 4 4 32

echo ""
echo "Both runs finished. Use the printed latency and throughput values"
echo "to fill the comparison table in the report."

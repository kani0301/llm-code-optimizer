#!/bin/bash
# Run perf and summarize results
TARGET="./build/sample_target"

if [ ! -f "$TARGET" ]; then
    echo "Target binary not found. Build first using CMake."
    exit 1
fi

mkdir -p results
perf stat -e cycles,instructions,branches,cache-misses -o results/perf_raw.txt "$TARGET"

grep -E "cycles|instructions|branches|cache-misses" results/perf_raw.txt | awk '{print $1","$2}' > results/summary.csv
echo "Perf results saved to results/summary.csv"

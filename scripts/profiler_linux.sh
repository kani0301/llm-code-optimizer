#!/bin/bash
# Simple Linux profiler using perf
# Usage: ./profiler_linux.sh ./analyzer/sample_target

TARGET=$1
OUTFILE=${2:-perf_stats.json}

if [ -z "$TARGET" ]; then
  echo "Usage: $0 <binary> [output.json]"
  exit 1
fi

echo "Running perf for $TARGET ..."
perf stat -x, -o temp_perf.txt -e cycles,instructions,branches,cache-misses "$TARGET" >/dev/null 2>&1

awk -F, '{
  if ($1 ~ /^[0-9]/) {
    gsub(/ /, "", $2);
    print "\"" $2 "\": " $1 ",";
  }
}' temp_perf.txt | sed '$ s/,$//' | awk 'BEGIN {print "{"} {print} END {print "}"}' > "$OUTFILE"

echo "Results saved to $OUTFILE"

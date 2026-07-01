#!/bin/bash

# Path to the compiled executable
EXECUTABLE="../build/zoo"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Binary not found. Did you run 'make' in the build directory?"
    exit 1
fi

echo "=========================================================="
echo " Gathering Hardware Telemetry via perf stat"
echo "=========================================================="
echo "Press Ctrl+C to stop the application and view the results."
echo ""

# -d: Detailed mode (includes L1 and LLC cache metrics)
# -e: Explicitly track context switches, page faults, and branch metrics
sudo perf stat -d -e context-switches,cpu-migrations,page-faults,branches,branch-misses $EXECUTABLE
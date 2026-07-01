#!/bin/bash

EXECUTABLE="../build/zoo"
# We expect the FlameGraph repo to be cloned next to the PerformanceZoo folder
FLAMEGRAPH_DIR="../../FlameGraph" 

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Binary not found. Did you run 'make' in the build directory?"
    exit 1
fi

echo "=========================================================="
echo " Recording Call Graph via perf record"
echo "=========================================================="
echo "Press Ctrl+C to stop recording."
echo ""

# Record the data
sudo perf record -g --call-graph fp -F 99 $EXECUTABLE

echo ""
echo "=========================================================="
echo " 📊 Terminal Analysis Summary"
echo "=========================================================="
# --hierarchy organizes the text output into a clean parent-child tree
# We pipe to head to keep the terminal from being flooded
sudo perf report --stdio --hierarchy --max-stack 5 | head -n 40
echo "..."

echo ""
echo "=========================================================="
echo " 🔥 Flame Graph Generation"
echo "=========================================================="

if [ ! -f "$FLAMEGRAPH_DIR/flamegraph.pl" ]; then
    echo "FlameGraph tools not installed at $FLAMEGRAPH_DIR."
    echo "Moving ahead without generating an SVG."
    echo "💡 Tip: To enable, run: git clone https://github.com/brendangregg/FlameGraph $FLAMEGRAPH_DIR"
else
    echo "Processing perf.data into a Flame Graph..."
    
    # Extract the trace data
    sudo perf script > out.perf
    
    # Collapse the stack traces and generate the SVG
    $FLAMEGRAPH_DIR/stackcollapse-perf.pl out.perf > out.folded
    $FLAMEGRAPH_DIR/flamegraph.pl out.folded > perf-flamegraph.svg
    
    # Cleanup intermediate files
    rm out.perf out.folded
    
    echo "✅ Flame graph saved as perf-flamegraph.svg! Open it in any web browser."
fi
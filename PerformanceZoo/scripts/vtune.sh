#!/bin/bash

EXECUTABLE="../build/zoo"
RESULT_DIR="./vtune_results"

if [ ! -f "$EXECUTABLE" ]; then
    echo "Error: Binary not found. Did you run 'make' in the build directory?"
    exit 1
fi

if ! command -v vtune &> /dev/null; then
    echo "Error: 'vtune' command not found."
    echo "Make sure Intel VTune is installed and you have sourced the environment variables."
    exit 1
fi

echo "=========================================================="
echo " Running Intel VTune Hotspots Analysis"
echo "=========================================================="
echo "Press Ctrl+C to stop the application and generate the report."
echo ""

# Run the collection
vtune -collect hotspots -result-dir $RESULT_DIR -- $EXECUTABLE

echo ""
echo "=========================================================="
echo " 📊 VTune Terminal Summary"
echo "=========================================================="
# Generate a clean text-based report right in the console
vtune -report summary -result-dir $RESULT_DIR -format text

echo ""
echo "✅ Collection complete! Open the '$RESULT_DIR' folder in the VTune GUI for the interactive Flame Graph."
#!/bin/bash

# Ensure we catch errors but allow clean step execution
set +e

EXECUTABLE="../build/zoo"
WORKLOAD_NAME=$(basename "${EXECUTABLE}")
FLAMEGRAPH_DIR="$HOME/FlameGraph"

DURATION=30
COOLDOWN=30

WORKLOADS=(
cpu
memory
io
lock
false
branch
pc
alloc
atomic
cache
busy
)

TIMESTAMP=$(date +%Y%m%d_%H%M%S)
BASE_DIR="../reports/${TIMESTAMP}"
mkdir -p "${BASE_DIR}"

SUMMARY="${BASE_DIR}/observation_summary.txt"
CSV="${BASE_DIR}/observation_summary.csv"
DASHBOARD="${BASE_DIR}/observation_dashboard.md"

# Initializing logs
echo "==================================================" > "${SUMMARY}"
echo "PerformanceZoo Bulletproof Continuous Summary" >> "${SUMMARY}"
echo "==================================================" >> "${SUMMARY}"
echo "Start Time : $(date)" >> "${SUMMARY}"
echo "" >> "${SUMMARY}"

# Collect Hardware Baseline Metadata
echo "========== CPU & CACHE METADATA ==========" >> "${SUMMARY}"
lscpu >> "${SUMMARY}"
echo "" >> "${SUMMARY}"

echo "========== KERNEL & MEMORY ==========" >> "${SUMMARY}"
uname -a >> "${SUMMARY}"
free -h >> "${SUMMARY}"
grep Huge /proc/meminfo >> "${SUMMARY}"
cat /sys/kernel/mm/transparent_hugepage/enabled 2>/dev/null >> "${SUMMARY}"
echo "" >> "${SUMMARY}"

echo "Workload,Status,Folder" > "${CSV}"
echo "# PerformanceZoo Continuous Observatory" > "${DASHBOARD}"
echo "" >> "${DASHBOARD}"
echo "|Workload|Status|Duration|Cooldown|" >> "${DASHBOARD}"
echo "|---|---|---|---|" >> "${DASHBOARD}"

echo "=================================================="
echo "LAUNCHING GLOBAL ASYNCHRONOUS PROFILING"
echo "=================================================="

# Launch ONE system-wide background perf record session
perf record \
    -a \
    -g \
    --call-graph fp \
    -F 99 \
    -o "${BASE_DIR}/global_perf.data" \
    2>/dev/null &
PERF_RECORD_PID=$!

# Launch system-wide global scheduler tracing
perf sched record -o "${BASE_DIR}/global_sched.data" 2>/dev/null &
PERF_SCHED_PID=$!

sleep 2

echo "Profilers successfully engaged (PIDs: Record=$PERF_RECORD_PID, Sched=$PERF_SCHED_PID)."
echo "Commencing workload execution timeline..."

# Main Timeline Loop
for W in "${WORKLOADS[@]}"
do
    echo ""
    echo "--------------------------------------------------"
    echo "Timeline Event: Spawning Workload -> [ ${W} ]"
    echo "--------------------------------------------------"
    
    # 1. Spawn the workload asynchronously in the background
    ${EXECUTABLE} ${W} &
    ZOO_PID=$!
    
    sleep 0.5
    
    # 2. Attach perf stat directly to the running PID
    perf stat \
        -p ${ZOO_PID} \
        -d -d -d \
        -e cycles,instructions,branches,branch-misses,cache-references,cache-misses,context-switches,cpu-migrations,page-faults \
        -o "${BASE_DIR}/${W}_zoo_stat.txt" \
        2>/dev/null &
    PERF_STAT_PID=$!
    
    # 3. Native Bash Timer (No blocking external wrappers)
    sleep ${DURATION}
    
    # 4. Graceful Teardown Sequence
    kill -INT ${PERF_STAT_PID} 2>/dev/null
    sleep 1
    kill -9 ${ZOO_PID} 2>/dev/null
    pkill -9 -f "${EXECUTABLE} ${W}" 2>/dev/null || true
    
    echo "${W} execution window concluded. Entering ${COOLDOWN}s cooling block."
    
    echo "${W} SUCCESS ${BASE_DIR}" >> "${SUMMARY}"
    echo "${W},SUCCESS,${BASE_DIR}" >> "${CSV}"
    echo "|${W}|SUCCESS|${DURATION}s|${COOLDOWN}s|" >> "${DASHBOARD}"
    
    sleep ${COOLDOWN}
done

echo ""
echo "=================================================="
echo "TEARING DOWN BACKGROUND PROFILING SESSIONS"
echo "=================================================="

# Safe Teardown Function (Anti-Zombie/Deadlock)
safe_teardown() {
    local target_pid=$1
    local name=$2
    
    if kill -0 ${target_pid} 2>/dev/null; then
        echo "Stopping ${name} (PID: ${target_pid})..."
        kill -INT ${target_pid} 2>/dev/null
        
        # Give it 15 seconds to flush ring buffers cleanly
        for i in {1..15}; do
            if ! kill -0 ${target_pid} 2>/dev/null; then
                echo "-> ${name} flushed and exited cleanly."
                return 0
            fi
            sleep 1
        done
        
        # Escalation: Force Kill
        echo "WARNING: ${name} (PID: ${target_pid}) did not exit gracefully. Sending SIGKILL..."
        kill -9 ${target_pid} 2>/dev/null
        
        # Give the kernel 2 seconds to reap the process
        sleep 2
        
        # Final Check: If it's still alive, it's trapped in Kernel D-state. Abandon it.
        if kill -0 ${target_pid} 2>/dev/null; then
            echo "CRITICAL: ${name} (PID: ${target_pid}) is trapped in Kernel D-state. Abandoning process to allow script completion."
        else
            echo "-> ${name} force-killed successfully."
        fi
    fi
}

safe_teardown ${PERF_RECORD_PID} "global perf record"
safe_teardown ${PERF_SCHED_PID} "global perf sched"

echo "All profiling streams processed. Moving to report generation..."

echo ""
echo "=================================================="
echo "GENERATING ZOO-ISOLATED POST-PROCESSING METRICS"
echo "=================================================="

# Process Zoo-Specific Hotspots
if [ -s "${BASE_DIR}/global_perf.data" ]; then
    echo "Compiling Zoo Hotspot Report..."
    perf report \
        -i "${BASE_DIR}/global_perf.data" \
        --comm "${WORKLOAD_NAME}" \
        --stdio \
        --hierarchy \
        --max-stack 20 \
        > "${BASE_DIR}/zoo_hotspots.txt" 2>/dev/null
else
    echo "WARNING: global_perf.data is empty or missing. Skipping hotspots."
fi

# Process Zoo-Specific Scheduler Latency
if [ -s "${BASE_DIR}/global_sched.data" ]; then
    echo "Compiling Zoo Scheduler Latency Report..."
    perf sched latency \
        -i "${BASE_DIR}/global_sched.data" 2>/dev/null | \
        grep -E "Task|----|${WORKLOAD_NAME}" \
        > "${BASE_DIR}/zoo_sched_latency.txt"
fi

# Generate Zoo-Isolated FlameGraph
if [ -f "${FLAMEGRAPH_DIR}/flamegraph.pl" ] && [ -s "${BASE_DIR}/global_perf.data" ]; then
    echo "Synthesizing Interactive Zoo FlameGraph..."
    
    perf script -i "${BASE_DIR}/global_perf.data" --comm "${WORKLOAD_NAME}" > "${BASE_DIR}/zoo_out.perf" 2>/dev/null
    
    if [ -s "${BASE_DIR}/zoo_out.perf" ]; then
        ${FLAMEGRAPH_DIR}/stackcollapse-perf.pl "${BASE_DIR}/zoo_out.perf" > "${BASE_DIR}/zoo_out.folded" 2>/dev/null
        ${FLAMEGRAPH_DIR}/flamegraph.pl "${BASE_DIR}/zoo_out.folded" > "${BASE_DIR}/zoo-isolated-flamegraph.svg" 2>/dev/null
        
        rm -f "${BASE_DIR}/zoo_out.perf"
        rm -f "${BASE_DIR}/zoo_out.folded"
    fi
fi

echo "" >> "${SUMMARY}"
echo "End Time : $(date)" >> "${SUMMARY}"
echo "" >> "${SUMMARY}"
echo "Observation Timeline Completed Successfully" >> "${SUMMARY}"

echo "" >> "${DASHBOARD}"
echo "Generated : $(date)" >> "${DASHBOARD}"

echo ""
echo "=================================================="
echo "Processing Completed Cleanly"
echo "=================================================="
echo "Output Directory : ${BASE_DIR}"
echo "Global Summary   : ${SUMMARY}"
echo ""

# 🦍 PerformanceZoo

A curated suite of C++ architectural bottlenecks, concurrency traps, and hardware-level performance anti-patterns.

This project is designed to act as a localized testbed for low-level systems profiling using Linux `perf` and Intel VTune. It isolates specific inefficiencies—from L1 cache thrashing and false sharing to CPU branch mispredictions—making it an ideal sandbox for optimizing low-latency applications (e.g., DPDK, eBPF, HFT) and analyzing deep hardware-to-OS interactions.

## 📂 The Zoo Enclosures (Bottlenecks)

Each file in `src/` is designed to artificially trigger a specific hardware or OS-level penalty:

* **`allocation_pressure.cpp`**: Global allocator lock contention and OS-level page faulting via heavy heap churn.
* **`atomic_contention.cpp`**: Hardware-level lock prefix overhead and memory bus saturation.
* **`branch_prediction.cpp`**: Pipeline flushing and speculative execution failures via randomized branching.
* **`busy_wait.cpp`**: CPU starvation, power inefficiency, and hyper-thread resource stealing via naive spin-polling.
* **`cache_patterns.cpp`**: L1/L2 cache misses and hardware prefetcher defeat via massive strided memory access.
* **`cpu_hotspot.cpp`**: Pure ALU/FPU saturation yielding maximum IPC but 100% core pinning.
* **`false_sharing.cpp`**: Hidden MESI protocol invalidation traffic caused by independent threads modifying the same 64-byte cache line.
* **`io_hotspot.cpp`**: System call overhead and context switching penalties from synchronous, unbuffered disk I/O.
* **`lock_contention.cpp`**: Thread serialization, futex calls, and OS scheduler overhead via coarse-grained mutexing.
* **`memory_hotspot.cpp`**: Memory bandwidth saturation and TLB thrashing.
* **`producer_consumer.cpp`**: Condition variable wakeup latency and unbounded queue memory growth.

## 🛠  Prerequisites

To build and profile this project, you will need a Linux environment (Ubuntu 20.04 recommended) with the following installed:

* `g++` (GCC 11+ recommended) with C++17 support
* `cmake` (>= 3.10)
* `linux-tools-common` and `linux-tools-generic` (for `perf`)
* Intel VTune

---

## 📊 Observation Framework Overview

The **PerformanceZoo Observation Script** (`observe.sh`) is an automated benchmarking and profiling harness. It executes a series of defined workloads, collects system-level hardware information, and captures deep-level performance metrics using Linux `perf` and FlameGraphs.

This framework is designed to run workloads for a specified duration, capture detailed hardware events, and allow the system to cool down between runs to prevent thermal throttling skew.

### Output Structure

Upon completion, the framework generates a timestamped directory containing a comprehensive suite of reports, summaries, and workload-specific profiles.

```text
reports/
└── {TIMESTAMP}/
    ├── observation_summary.txt     # Global system state and execution summary
    ├── observation_summary.csv     # Machine-readable execution status
    ├── observation_dashboard.md    # Quick-glance markdown dashboard
    ├── cpu/                        # Workload-specific directory (e.g., 'cpu')
    │   ├── report_perf.txt         # Consolidated perf report for the workload
    │   ├── perf_stat.txt           # PMU counters (cycles, instructions, misses)
    │   ├── perf_hotspots.txt       # Top 200 stack trace hotspots
    │   ├── perf.data               # Raw perf record binary
    │   ├── perf-flamegraph.svg     # Interactive CPU FlameGraph
    │   ├── perf_c2c.txt            # Cache-2-Cache (False Sharing) report
    │   ├── perf_mem.txt            # Memory access profiling
    │   ├── perf_sched.txt          # Scheduler latency metrics
    │   └── perf_lock.txt           # Lock contention report
    ├── memory/                     # Reports for 'memory' workload
    ├── io/                         # Reports for 'io' workload
    └── ...                         # (Other workloads)

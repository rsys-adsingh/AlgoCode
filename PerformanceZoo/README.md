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

## 🛠 Prerequisites

To build and profile this project, you will need a Linux environment (Ubuntu recommended) with the following installed:

* `g++` (or Clang) with C++17 support
* `cmake` (>= 3.10)
* `linux-tools-common` and `linux-tools-generic` (for `perf`)
* Intel VTune
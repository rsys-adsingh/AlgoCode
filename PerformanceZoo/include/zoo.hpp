#pragma once

/**
 * @file zoo.hpp
 * @brief Declarations for various performance anti-patterns and bottlenecks.
 * * Each function in this suite isolates a specific hardware, OS, or architectural 
 * inefficiency. These are designed to be profiled (e.g., using perf or VTune) 
 * to observe the resulting hardware counters, cache behaviors, and system calls.
 */

namespace perf_zoo {

    /**
     * @brief Demonstrates a pure CPU-bound bottleneck.
     * * Simulates heavy arithmetic or algorithmic intensity without memory stalls.
     * Useful for observing high instruction-per-cycle (IPC) rates but 100% core utilization.
     */
    void cpu_hotspot();

    /**
     * @brief Demonstrates poor memory locality and high cache miss rates.
     * * Typically implemented via random pointer chasing or strided memory access.
     * Highlights the latency cost of fetching data from Main Memory vs L1/L2 caches.
     */
    void memory_hotspot();

    /**
     * @brief Demonstrates throughput degradation due to blocking I/O.
     * * Highlights the overhead of frequent system calls and context switching 
     * (e.g., reading/writing small chunks to disk or network synchronously).
     */
    void io_hotspot();

    /**
     * @brief Demonstrates thread serialization via a single synchronization primitive.
     * * Multiple threads fighting for a single std::mutex. Highlights context switch 
     * overhead, priority inversion risks, and the cost of kernel-level arbitration.
     */
    void lock_contention();

    /**
     * @brief Demonstrates hidden synchronization costs at the hardware cache-line level.
     * * Occurs when multiple threads modify independent variables that happen to reside 
     * on the same 64-byte cache line, causing constant cache invalidation traffic 
     * (MESI protocol bouncing) across cores.
     */
    void false_sharing();

    /**
     * @brief Demonstrates the cost of CPU pipeline stalls.
     * * Typically shown by processing an unsorted array vs a sorted array. 
     * Highlights how modern superscalar processors suffer when the branch predictor 
     * fails and has to flush its instruction pipeline.
     */
    void branch_prediction();

    /**
     * @brief Demonstrates queue bottlenecks and condition variable overhead.
     * * Highlights the complexities of safely handing off data between threads. 
     * Useful for analyzing wait-states and wake-up latencies in message passing.
     */
    void producer_consumer();

    /**
     * @brief Demonstrates heap fragmentation and memory allocator overhead.
     * * Heavy reliance on dynamic allocation (new/delete) in hot paths. Highlights 
     * why system-level programming often relies on arena allocators or object pools 
     * to avoid the global lock inside standard malloc/free.
     */
    void allocation_pressure();

    /**
     * @brief Demonstrates hardware-level contention without OS intervention.
     * * High-frequency increments on a shared std::atomic. Even though it avoids 
     * kernel locks, the atomic instructions (e.g., LOCK CMPXCHG) saturate the 
     * memory bus and degrade multi-core scaling.
     */
    void atomic_contention();

    /**
     * @brief Demonstrates hardware prefetcher friendly vs unfriendly access.
     * * Examples include traversing a 2D matrix in Row-Major (cache-friendly) 
     * vs Column-Major (cache-thrashing) order.
     */
    void cache_patterns();

    /**
     * @brief Demonstrates thermal and power inefficiencies of spin-polling.
     * * A thread looping continuously waiting for a state change without yielding.
     * Highlights how this wastes CPU cycles, increases CPU temperature, and steals 
     * resources from hyper-threads on the same physical core.
     */
    void busy_wait();

} // namespace perf_zoo
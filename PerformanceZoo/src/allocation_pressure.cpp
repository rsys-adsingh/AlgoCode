#include <iostream>

namespace perf_zoo {

    /**
     * @brief Demonstrates the overhead of frequent heap allocations.
     * * Why it's a bottleneck:
     * 1. Global/Arena Locks: Standard `new`/`malloc` often requires acquiring a lock 
     * in the memory allocator (like glibc's ptmalloc or jemalloc) to find free space.
     * 2. OS Overhead: Large allocations trigger the `mmap` syscall instead of `brk`, 
     * forcing kernel context switches and heavy page faulting upon first access.
     * 3. TLB Thrashing: Churning large contiguous blocks evicts translation lookaside 
     * buffer (TLB) entries, making memory address translation slower globally.
     */
    void allocation_pressure() 
    {
        while (true) 
        {
            // Allocating ~4MB of memory continuously
            auto p = new int[1000000];
            
            // Immediately releasing it causes high allocator churn
            delete[] p;
        }
    }

} // namespace perf_zoo
#include <vector>

namespace perf_zoo {

    /**
     * @brief Demonstrates memory bandwidth saturation and TLB thrashing.
     * * Why it's a bottleneck:
     * By incrementing the index by 64, we are moving forward by 256 bytes (64 * sizeof(int)).
     * Because a hardware cache line is typically 64 bytes, this guarantees that *every single read* * misses the L1 cache. Furthermore, iterating over a massive 2GB vector (500M * 4 bytes) 
     * rapidly evicts pages from the Translation Lookaside Buffer (TLB), causing page walks.
     * You will see the CPU essentially waiting on RAM latency for every instruction.
     */
    void memory_hotspot() 
    {
        // ~2GB allocation
        std::vector<int> v(500000000);

        while (true) 
        {
            // Strided access pattern explicitly designed to defeat caching
            for (std::size_t i = 0; i < v.size(); i += 64) 
            {
                v[i]++;
            }
        }
    }

} // namespace perf_zoo
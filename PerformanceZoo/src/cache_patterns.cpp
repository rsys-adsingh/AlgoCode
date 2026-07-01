#include <vector>

namespace perf_zoo {

    /**
     * @brief Demonstrates the catastrophic penalty of cache-unfriendly memory access.
     * * Why it's a bottleneck:
     * Memory is loaded into the CPU L1 cache in 64-byte chunks (cache lines). 
     * C++ arrays are row-major. By iterating column-by-column (inner loop increments the row), 
     * you access one integer (4 bytes), and then jump 10,000 integers away for the next read. 
     * This defeats the hardware prefetcher. The CPU has to fetch a new cache line from main 
     * memory for almost every single iteration, resulting in a ~100% L1/L2 cache miss rate.
     */
    void cache_patterns() 
    {
        // Allocating a flattened 10k x 10k matrix
        std::vector<int> m(10000 * 10000);

        while (true) 
        {
            // Column-major traversal (Anti-pattern in C++)
            for (int j = 0; j < 10000; j++) 
            {
                for (int i = 0; i < 10000; i++) 
                {
                    // Accessing elements with a massive stride
                    m[i * 10000 + j]++;
                }
            }
        }
    }

} // namespace perf_zoo
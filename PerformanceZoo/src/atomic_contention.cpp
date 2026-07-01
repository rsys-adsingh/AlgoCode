#include <atomic>
#include <thread>
#include <vector>

namespace perf_zoo {

    // Shared across all threads. Sits in a single 64-byte cache line.
    std::atomic<int> counter = 0;

    /**
     * @brief Worker function that saturates the memory bus.
     * * Why it's a bottleneck:
     * While atomics avoid kernel-level OS context switches (unlike mutexes), they rely 
     * on hardware-level locking (e.g., the `LOCK` prefix on x86). When multiple cores 
     * hammer the same atomic variable, the cache line containing `counter` constantly 
     * bounces between the L1 caches of the different cores via the MESI protocol. 
     * This "cache line ping-pong" completely stalls the CPU pipeline.
     */
    void atomic_worker() 
    {
        while (true) 
        {
            // Translates to a locked read-modify-write instruction
            counter++; 
        }
    }

    void atomic_contention() 
    {
        std::thread t1(atomic_worker);
        std::thread t2(atomic_worker);
        std::thread t3(atomic_worker);

        t1.join();
        t2.join();
        t3.join();
    }

} // namespace perf_zoo
#include <mutex>
#include <thread>

namespace perf_zoo {

    std::mutex lock_mutex;

    /**
     * @brief Demonstrates coarse-grained locking and thread serialization.
     * * Why it's a bottleneck:
     * The critical section (the work done inside the lock) is massive. 
     * When Thread 1 holds the lock, Thread 2 is put to sleep by the OS (using a futex on Linux).
     * This destroys concurrency. The system spends more time managing thread queues and 
     * context switching than it does executing actual application logic.
     * * Fix: Reduce lock granularity, use lock-free data structures, or apply Thread-Local 
     * Storage (TLS) to do work locally and only lock briefly to merge results.
     */
    void lock_worker() 
    {
        while (true) 
        {
            // Acquiring standard OS-backed mutex
            std::lock_guard<std::mutex> lg(lock_mutex);

            // Massive critical section blocks all other threads
            for (int i = 0; i < 1000000; i++) 
            {
                // simulated work...
            }
        }
    }

    void lock_contention() 
    {
        std::thread t1(lock_worker);
        std::thread t2(lock_worker);

        t1.join();
        t2.join();
    }

} // namespace perf_zoo
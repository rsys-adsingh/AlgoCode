#include <queue>
#include <mutex>
#include <thread>
#include <condition_variable>

namespace perf_zoo {

    std::queue<int> q;
    std::mutex queue_mutex;
    std::condition_variable cv;

    /**
     * @brief Demonstrates unbounded queue growth and heavy CV overhead.
     * * Why it's a bottleneck:
     * 1. Unbounded Queue: The producer generates data faster than the consumer processes it, 
     * leading to out-of-memory (OOM) eventualities. 
     * 2. Heavy Wakeups: Calling `notify_one()` on every single push forces the OS kernel 
     * to wake up the consumer thread continuously, which is highly inefficient.
     * * Real-world systems use ring-buffers (like DPDK rte_ring) or batching to mitigate this.
     */
    void producer() 
    {
        while (true) 
        {
            // Scope limit the lock so it releases before notify_one
            {
                std::unique_lock<std::mutex> lock(queue_mutex);
                q.push(1);
            }
            cv.notify_one(); 
        }
    }

    void consumer() 
    {
        while (true) 
        {
            std::unique_lock<std::mutex> lock(queue_mutex);
            
            // Handle spurious wakeups
            while (q.empty()) 
            {
                cv.wait(lock);
            }
            
            q.pop();
        }
    }

    void producer_consumer() 
    {
        std::thread t1(producer);
        std::thread t2(consumer);

        t1.join();
        t2.join();
    }

} // namespace perf_zoo
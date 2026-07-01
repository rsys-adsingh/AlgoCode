#include <thread>

namespace perf_zoo {

    /**
     * @brief Demonstrates the hidden penalty of false sharing.
     * * Why it's a bottleneck:
     * CPU caches operate on 64-byte chunks called "cache lines". 
     * `Data.a` and `Data.b` are adjacent in memory, meaning they sit on the exact same cache line.
     * When Thread 1 modifies `a` and Thread 2 modifies `b`, they aren't actually sharing data, 
     * but the hardware doesn't know that. Every write forces the CPU to invalidate the entire 
     * 64-byte cache line across all cores via the MESI protocol. 
     * This "cache ping-pong" cripples multi-core scaling.
     * * Fix: Force them onto separate cache lines using `alignas(64) int a; alignas(64) int b;`
     */
    struct Data 
    {
        int a = 0;
        int b = 0;
    };

    Data d;

    void w1() 
    {
        while (true) 
        {
            d.a++;
        }
    }

    void w2() 
    {
        while (true) 
        {
            d.b++;
        }
    }

    void false_sharing() 
    {
        std::thread t1(w1);
        std::thread t2(w2);

        t1.join();
        t2.join();
    }

} // namespace perf_zoo
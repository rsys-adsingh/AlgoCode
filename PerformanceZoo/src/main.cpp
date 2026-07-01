#include "../include/zoo.hpp"
#include <thread>
#include <iostream>

/**
 * @brief Entry point for the Performance Zoo.
 * Note: Spawning all 11 performance bottlenecks simultaneously will completely 
 * overload your system and make targeted profiling nearly impossible, as the 
 * OS scheduler will thrash. 
 * * For profiling with `perf` or `vtune`, it's best to run one of these at a time.
 */
int main() 
{
    std::cout << "Starting Performance Zoo (WARNING: High system load expected)...\n";

    // Since we put everything in the perf_zoo namespace in zoo.hpp, 
    // we prefix the calls here.
    std::thread t1(perf_zoo::cpu_hotspot);
    std::thread t2(perf_zoo::memory_hotspot);
    std::thread t3(perf_zoo::io_hotspot);
    std::thread t4(perf_zoo::lock_contention);
    std::thread t5(perf_zoo::false_sharing);
    std::thread t6(perf_zoo::branch_prediction);
    std::thread t7(perf_zoo::producer_consumer);
    std::thread t8(perf_zoo::allocation_pressure);
    std::thread t9(perf_zoo::atomic_contention);
    std::thread t10(perf_zoo::cache_patterns);
    std::thread t11(perf_zoo::busy_wait);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    t11.join();

    return 0;
}
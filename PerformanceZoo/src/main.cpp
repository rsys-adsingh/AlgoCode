#include "../include/zoo.hpp"

#include <iostream>
#include <string>

void usage()
{
    std::cout << "\nPerformanceZoo\n\n";

    std::cout << "Usage:\n";
    std::cout << "./zoo <workload>\n\n";

    std::cout << "Available workloads:\n";
    std::cout << "  cpu\n";
    std::cout << "  memory\n";
    std::cout << "  io\n";
    std::cout << "  lock\n";
    std::cout << "  false\n";
    std::cout << "  branch\n";
    std::cout << "  pc\n";
    std::cout << "  alloc\n";
    std::cout << "  atomic\n";
    std::cout << "  cache\n";
    std::cout << "  busy\n\n";
}

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        usage();
        return 1;
    }

    std::string mode(argv[1]);

    std::cout << "Launching workload : "
              << mode
              << std::endl;

    if (mode == "cpu")
        perf_zoo::cpu_hotspot();

    else if (mode == "memory")
        perf_zoo::memory_hotspot();

    else if (mode == "io")
        perf_zoo::io_hotspot();

    else if (mode == "lock")
        perf_zoo::lock_contention();

    else if (mode == "false")
        perf_zoo::false_sharing();

    else if (mode == "branch")
        perf_zoo::branch_prediction();

    else if (mode == "pc")
        perf_zoo::producer_consumer();

    else if (mode == "alloc")
        perf_zoo::allocation_pressure();

    else if (mode == "atomic")
        perf_zoo::atomic_contention();

    else if (mode == "cache")
        perf_zoo::cache_patterns();

    else if (mode == "busy")
        perf_zoo::busy_wait();

    else
    {
        usage();
        return 1;
    }

    return 0;
}

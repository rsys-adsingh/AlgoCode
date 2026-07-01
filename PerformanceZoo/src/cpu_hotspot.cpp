#include <cmath>

namespace perf_zoo {

    /**
     * @brief Demonstrates a heavily compute-bound workload.
     * * Why it's a bottleneck:
     * Unlike memory or lock bottlenecks, this maximizes the CPU's Arithmetic Logic Unit (ALU) 
     * and Floating Point Units (FPU). It will show excellent IPC (Instructions Per Cycle) 
     * in profilers like VTune, but will pin the core to 100% utilization.
     * * The `volatile` keyword is critical here: without it, modern compilers like GCC or Clang 
     * will notice that `x` is never read, and optimize away the entire loop during compilation.
     */
    void cpu_hotspot() 
    {
        volatile double x = 0;

        while (true) 
        {
            for (long i = 0; i < 100000000; i++) 
            {
                // Expensive math operation forcing CPU pipeline saturation
                x += sqrt(i);
            }
        }
    }

} // namespace perf_zoo
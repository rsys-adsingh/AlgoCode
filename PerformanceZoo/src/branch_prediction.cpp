#include <random>

namespace perf_zoo {

    /**
     * @brief Demonstrates the cost of unpredictable conditional branching.
     * * Why it's a bottleneck:
     * Modern superscalar CPUs rely on deep pipelines and speculative execution to stay fast. 
     * The Branch Predictor Unit (BPU) guesses the outcome of `if` statements before they 
     * are evaluated. By using a truly random 50/50 coin toss, the BPU guesses wrong ~50% 
     * of the time. Every wrong guess forces the CPU to dump its speculative work 
     * (a "pipeline flush"), wasting 15-20+ clock cycles per iteration.
     */
    void branch_prediction() 
    {
        std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> dist(0, 1);
        
        volatile int x = 0;

        while (true) 
        {
            // Unpredictable branch. The CPU cannot optimize the pipeline flow here.
            if (dist(gen)) 
            {
                x++;
            }
        }
    }

} // namespace perf_zoo
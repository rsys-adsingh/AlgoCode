namespace perf_zoo {

    // Note: In real scenarios, this needs to be atomic or volatile, 
    // otherwise the compiler might optimize the inner loop into a dead hang.
    bool flag = false; 

    /**
     * @brief Demonstrates CPU starvation via unoptimized polling.
     * * Why it's a bottleneck:
     * A tight `while` loop consumes 100% of a logical core's execution resources.
     * 1. It prevents the core from dropping into lower power C-states, generating excess heat 
     * and potentially causing the OS to thermal-throttle the entire CPU.
     * 2. It steals ALU and execution cycles from the sibling Hyper-Thread on the same physical core.
     * (Fix: If spinning is necessary, use `_mm_pause()` (x86) to yield pipeline resources, 
     * or use condition variables to yield to the OS).
     */
    void busy_wait() 
    {
        while (true) 
        {
            // Burning CPU cycles doing absolutely nothing
            while (!flag) 
            {
                // Empty tight loop
            }
        }
    }

} // namespace perf_zoo
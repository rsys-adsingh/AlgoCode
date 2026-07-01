#include <fstream>
#include <thread>
#include <chrono>

namespace perf_zoo {

    /**
     * @brief Demonstrates the overhead of unbuffered, high-frequency disk I/O.
     * * Why it's a bottleneck:
     * 1. System Call Overhead: Opening and closing a file descriptors forces a transition 
     * from user space to kernel space. 
     * 2. Lock Contention in the VFS: The OS Virtual File System has to serialize writes 
     * to the inode.
     * In high-performance systems, this is why we use memory-mapped files (mmap), 
     * user-space network stacks (like DPDK for packets), or asynchronous I/O (io_uring) 
     * to avoid these constant kernel boundary crossings.
     */
    void io_hotspot() 
    {
        while (true) 
        {
            // Anti-pattern: Opening/closing inside a tight loop
            std::ofstream file("data.txt", std::ios::app);
            file << "AMD\n";
            file.close();

            // Putting the thread to sleep forces the OS scheduler to step in,
            // adding further context-switching latency.
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

} // namespace perf_zoo
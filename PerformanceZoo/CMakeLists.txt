# Require at least CMake 3.10 for modern target-based commands
cmake_minimum_required(VERSION 3.10)

project(PerformanceZoo)

# Force the C++17 standard for modern concurrency and language features
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =========================================================================
# CRITICAL PROFILING FLAGS
# =========================================================================
# -O3: Maximum optimizations. You must profile optimized code, otherwise 
#      you are just measuring debug overhead.
# -g: Includes debug symbols so profilers can map addresses to source code.
# -fno-omit-frame-pointer: Crucial for `perf` and `vtune`. It keeps the 
#      RBP/EBP register pointing to the stack frame, allowing profilers 
#      to generate accurate CPU call graphs and stack traces.
# =========================================================================
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g -fno-omit-frame-pointer")

# Define the final executable binary ('zoo') and list all source files.
add_executable(zoo
    src/main.cpp
    src/cpu_hotspot.cpp
    src/memory_hotspot.cpp
    src/io_hotspot.cpp
    src/lock_contention.cpp
    src/false_sharing.cpp
    src/branch_prediction.cpp
    src/producer_consumer.cpp
    src/allocation_pressure.cpp
    src/atomic_contention.cpp
    src/cache_patterns.cpp
    src/busy_wait.cpp
)

# Tell the compiler where to find the header files (.hpp).
# We use target_include_directories (Modern CMake) instead of global 
# include_directories so the include path is safely scoped only to 'zoo'.
target_include_directories(zoo PRIVATE include)

# Link against the OS threading library.
# Under the hood on Ubuntu/Linux, C++ primitives like std::thread, std::mutex, 
# and std::condition_variable are wrappers around POSIX threads. 
# The linker needs 'pthread' to resolve these system-level definitions.
target_link_libraries(zoo pthread)
#include <chrono>
#include <thread>
//TODO: Make git submodules
#define psl_sleep(ms) std::this_thread::sleep_for(std::chrono::milliseconds(ms))
#define pls_getArrayLength(array) sizeof(array)/sizeof(array[0])
// ---------------------------------------------------------------------------
// AdaChess – Multiprocessor utilities (implementation)
// Translated from: acmultiprocessor.ads (wrapper around System.Multiprocessors)
// ---------------------------------------------------------------------------
#include "util/multiprocessor.hpp"

namespace util::multiprocessor {

unsigned int number_of_cpus() {
    auto n = std::thread::hardware_concurrency();
    return (n > 0) ? n : 1;
}

void assign_to_cpu(unsigned int /*cpu*/, std::thread::native_handle_type /*handle*/) {
    // Platform-specific CPU affinity is deferred to a later milestone.
    // On Linux: use pthread_setaffinity_np
    // On Windows: use SetThreadAffinityMask
    // For now, this is a no-op stub.
}

} // namespace util::multiprocessor

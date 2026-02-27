// ---------------------------------------------------------------------------
// AdaChess – Multiprocessor utilities
// Translated from: acmultiprocessor.ads
// ---------------------------------------------------------------------------
#pragma once

#include <thread>

namespace util::multiprocessor {

// -----------------------------------------------------------------------
// Ada: subtype CPU_Range is System.Multiprocessors.CPU_Range;
//      Not_A_Specific_CPU : constant CPU_Range := ...;
//      function Number_Of_CPUs return CPU_Range ...;
//      procedure Assign_To_CPU (CPU, Task_Id) ...;
// -----------------------------------------------------------------------

using CpuRange = unsigned int;

/// Special value indicating no specific CPU affinity.
inline constexpr CpuRange Not_A_Specific_CPU = 0;

/// Detect the number of CPUs available on the system.
[[nodiscard]] unsigned int number_of_cpus();

/// Set thread affinity to a specific CPU.
/// This is a stub in this milestone; full implementation uses
/// platform-specific APIs (pthread_setaffinity_np, SetThreadAffinityMask).
void assign_to_cpu(unsigned int cpu, std::thread::native_handle_type handle);

} // namespace util::multiprocessor

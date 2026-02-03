/**
 * AdaChess - Smart Chess Engine
 *
 * Timer types and interface.
 * C++ port of the original Ada implementation (libs/timers_lib/timers.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_LIBS_TIMERS_TIMERS_HPP
#define ADACHESS_LIBS_TIMERS_TIMERS_HPP

#include <cstdint>

namespace chess::timers {

/**
 * Timer status enumeration.
 * Represents the current state of a timer.
 * 
 * This matches the Ada Timer_Status_Type:
 *   type Timer_Status_Type is
 *     (Not_Yet_Started, Started, Stopped)
 */
enum class TimerStatus : std::uint8_t {
    NotYetStarted = 0,  ///< Timer has not been started yet (default)
    Started       = 1,  ///< Timer is currently running
    Stopped       = 2   ///< Timer was started and has been stopped
};

/**
 * Check if a timer status indicates the timer has not started.
 * @param status The timer status to check.
 * @return True if the timer has not been started.
 */
[[nodiscard]] constexpr bool is_not_started(TimerStatus status) noexcept {
    return status == TimerStatus::NotYetStarted;
}

/**
 * Check if a timer status indicates the timer is running.
 * @param status The timer status to check.
 * @return True if the timer is currently running.
 */
[[nodiscard]] constexpr bool is_running(TimerStatus status) noexcept {
    return status == TimerStatus::Started;
}

/**
 * Check if a timer status indicates the timer is stopped.
 * @param status The timer status to check.
 * @return True if the timer has been stopped.
 */
[[nodiscard]] constexpr bool is_stopped(TimerStatus status) noexcept {
    return status == TimerStatus::Stopped;
}

} // namespace chess::timers

#endif // ADACHESS_LIBS_TIMERS_TIMERS_HPP

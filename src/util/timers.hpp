// ---------------------------------------------------------------------------
// AdaChess – Timer utilities (countdown timer)
// Translated from: timers.ads + timers-countdown.ads/.adb
// ---------------------------------------------------------------------------
#pragma once

#include <chrono>

namespace util::timers {

// -----------------------------------------------------------------------
// Timer status
// Ada: type Timer_Status_Type is (Not_Yet_Started, Started, Stopped)
// -----------------------------------------------------------------------
enum class TimerStatus { Not_Yet_Started, Started, Stopped };

// -----------------------------------------------------------------------
// CountdownTimer — a simple start/stop timer measuring elapsed time.
// Translated from Ada's generic Timers.Countdown package.
// In Ada this was a generic package instantiated once; in C++ it's a class.
// -----------------------------------------------------------------------
class CountdownTimer {
public:
    CountdownTimer() = default;

    /// Start the timer.
    void start();

    /// Stop the timer.
    void stop();

    /// Return the elapsed time in seconds (as a double).
    /// - Not_Yet_Started: returns 0.0
    /// - Started: returns time since start
    /// - Stopped: returns time between start and stop
    [[nodiscard]] double elapsed_time() const;

private:
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;

    TimerStatus status_ = TimerStatus::Not_Yet_Started;
    TimePoint   start_time_{};
    TimePoint   stop_time_{};
};

} // namespace util::timers

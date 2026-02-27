// ---------------------------------------------------------------------------
// AdaChess – Timer utilities (implementation)
// Translated from: timers-countdown.adb
// ---------------------------------------------------------------------------
#include "util/timers.hpp"

namespace util::timers {

void CountdownTimer::start() {
    status_ = TimerStatus::Started;
    start_time_ = Clock::now();
}

void CountdownTimer::stop() {
    stop_time_ = Clock::now();
    status_ = TimerStatus::Stopped;
}

double CountdownTimer::elapsed_time() const {
    switch (status_) {
        case TimerStatus::Not_Yet_Started:
            return 0.0;
        case TimerStatus::Started: {
            auto diff = Clock::now() - start_time_;
            return std::chrono::duration<double>(diff).count();
        }
        case TimerStatus::Stopped: {
            auto diff = stop_time_ - start_time_;
            return std::chrono::duration<double>(diff).count();
        }
    }
    return 0.0; // unreachable
}

} // namespace util::timers

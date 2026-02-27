// ---------------------------------------------------------------------------
// AdaChess – Pin type definitions
// Translated from: chess-pins.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>

namespace chess {

// -----------------------------------------------------------------------
// Pin_Type: classification of pin status for a piece.
// Ada: type Pin_Type is (No_Pin, Relative, Cross, Absolute)
//        with Default_Value => No_Pin, Size => 2;
// -----------------------------------------------------------------------
enum class Pin : std::uint8_t {
    No_Pin   = 0,
    Relative = 1,
    Cross    = 2,
    Absolute = 3
};

} // namespace chess

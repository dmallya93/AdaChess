// ---------------------------------------------------------------------------
// AdaChess – Color type definitions
// Translated from: chess-colors.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>

namespace chess {

/// Color_Type: White or Black.
/// Ada: type Color_Type is (White, Black) with Size => 2;
enum class Color : std::uint8_t {
    White = 0,
    Black = 1
};

/// Swap colors — corresponds to Ada's "not" operator on Color_Type.
[[nodiscard]] constexpr Color operator~(Color c) noexcept {
    return (c == Color::White) ? Color::Black : Color::White;
}

} // namespace chess

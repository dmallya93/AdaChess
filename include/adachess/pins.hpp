/**
 * AdaChess - Smart Chess Engine
 *
 * Pin type definitions for detecting pinned pieces.
 * C++ port of the original Ada implementation (chess-pins.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_PINS_HPP
#define ADACHESS_PINS_HPP

#include <cstdint>

namespace chess {

/**
 * Pin type enumeration representing different types of pins.
 * Corresponds to Ada's Pin_Type (with Size => 2, Default_Value => No_Pin).
 *
 * - NoPin: The piece is not pinned
 * - Relative: The piece is relatively pinned (pinned to a piece other than king)
 * - Cross: The piece is cross-pinned (pinned along two different lines)
 * - Absolute: The piece is absolutely pinned (cannot move without exposing king)
 */
enum class Pin : std::uint8_t {
    NoPin    = 0,
    Relative = 1,
    Cross    = 2,
    Absolute = 3
};

/**
 * Check if a pin type indicates the piece is not pinned.
 *
 * @param pin The pin type to check.
 * @return True if the piece is not pinned.
 */
[[nodiscard]] constexpr bool is_not_pinned(Pin pin) noexcept {
    return pin == Pin::NoPin;
}

/**
 * Check if a pin type indicates the piece is relatively pinned.
 *
 * @param pin The pin type to check.
 * @return True if the piece is relatively pinned.
 */
[[nodiscard]] constexpr bool is_relatively_pinned(Pin pin) noexcept {
    return pin == Pin::Relative;
}

/**
 * Check if a pin type indicates the piece is cross-pinned.
 *
 * @param pin The pin type to check.
 * @return True if the piece is cross-pinned.
 */
[[nodiscard]] constexpr bool is_cross_pinned(Pin pin) noexcept {
    return pin == Pin::Cross;
}

/**
 * Check if a pin type indicates the piece is absolutely pinned.
 *
 * @param pin The pin type to check.
 * @return True if the piece is absolutely pinned.
 */
[[nodiscard]] constexpr bool is_absolutely_pinned(Pin pin) noexcept {
    return pin == Pin::Absolute;
}

/**
 * Check if the piece has any kind of pin.
 *
 * @param pin The pin type to check.
 * @return True if the piece is pinned in any way.
 */
[[nodiscard]] constexpr bool is_pinned(Pin pin) noexcept {
    return pin != Pin::NoPin;
}

} // namespace chess

#endif // ADACHESS_PINS_HPP

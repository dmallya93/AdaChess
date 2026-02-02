/**
 * AdaChess - Smart Chess Engine
 *
 * Color type definitions for white and black pieces/players.
 * C++ port of the original Ada implementation (chess-colors.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_COLORS_HPP
#define ADACHESS_COLORS_HPP

#include <cstdint>

namespace chess {

/**
 * Color enumeration representing the two sides in chess.
 * Uses explicit underlying type for memory efficiency (matches Ada's Size => 2).
 */
enum class Color : std::uint8_t {
    White = 0,
    Black = 1
};

/**
 * Flip (toggle) the color from White to Black or vice versa.
 * Corresponds to Ada's overloaded "not" operator.
 *
 * @param color The color to flip.
 * @return The opposite color.
 */
[[nodiscard]] constexpr Color flip(Color color) noexcept {
    return (color == Color::White) ? Color::Black : Color::White;
}

/**
 * Operator overload for color negation (alias for flip).
 * Provides a convenient syntax: ~Color::White returns Color::Black
 *
 * @param color The color to negate.
 * @return The opposite color.
 */
[[nodiscard]] constexpr Color operator~(Color color) noexcept {
    return flip(color);
}

/**
 * Convert color to array index (0 for White, 1 for Black).
 * Useful for indexing into color-specific arrays.
 *
 * @param color The color to convert.
 * @return The index (0 or 1).
 */
[[nodiscard]] constexpr std::size_t to_index(Color color) noexcept {
    return static_cast<std::size_t>(color);
}

/**
 * Number of colors (for array sizing).
 */
inline constexpr std::size_t kNumColors = 2;

} // namespace chess

#endif // ADACHESS_COLORS_HPP

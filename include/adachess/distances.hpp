/**
 * AdaChess - Smart Chess Engine
 *
 * Distance type definitions for piece movement calculations.
 * C++ port of the original Ada implementation (chess-distances.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_DISTANCES_HPP
#define ADACHESS_DISTANCES_HPP

#include <cstdint>

namespace chess {

/**
 * Distance type - represents the number of moves a piece requires
 * to reach a certain square from its current position.
 *
 * Range: 0 to 9 (matches Ada's Distance_Type).
 *
 * If a piece cannot reach a square (e.g., a bishop on white squares
 * cannot reach black squares, or a pawn cannot reach squares not in
 * its trajectory), the special value Unreachable (9) is used.
 */
using Distance = std::uint8_t;

/**
 * Maximum valid distance value.
 */
inline constexpr Distance kMaxDistance = 9;

/**
 * No distance - piece is already at the destination.
 * Corresponds to Ada's No_Distance constant.
 */
inline constexpr Distance kNoDistance = 0;

/**
 * Unreachable - piece cannot reach the target square.
 * Corresponds to Ada's Unreachable constant (Distance_Type'Last).
 *
 * Used when:
 * - A bishop on light squares cannot reach dark squares
 * - A pawn cannot reach squares outside its trajectory
 * - Any piece blocked without a valid path
 */
inline constexpr Distance kUnreachable = kMaxDistance;

/**
 * Check if a distance value indicates the square is unreachable.
 *
 * @param distance The distance value to check.
 * @return True if the distance indicates unreachability.
 */
[[nodiscard]] constexpr bool is_unreachable(Distance distance) noexcept {
    return distance == kUnreachable;
}

/**
 * Check if a distance value indicates the piece is already at destination.
 *
 * @param distance The distance value to check.
 * @return True if the distance is zero (already at destination).
 */
[[nodiscard]] constexpr bool is_at_destination(Distance distance) noexcept {
    return distance == kNoDistance;
}

} // namespace chess

#endif // ADACHESS_DISTANCES_HPP

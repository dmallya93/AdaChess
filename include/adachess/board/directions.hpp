/**
 * AdaChess - Smart Chess Engine
 *
 * Direction constants and piece movement offsets for the 10x12 mailbox board.
 * C++ port of the original Ada implementation (chess-board-directions.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_BOARD_DIRECTIONS_HPP
#define ADACHESS_BOARD_DIRECTIONS_HPP

#include <array>
#include <cstdint>

namespace chess::board {

// ============================================================================
// Direction Type
// ============================================================================

/**
 * Direction type representing square offsets for piece movement.
 * In the 10x12 mailbox, moving north subtracts 10, moving east adds 1, etc.
 * Range: -21 to +21 (matching Ada's Direction_Type).
 */
using Direction = std::int8_t;

// ============================================================================
// Basic Direction Constants
// ============================================================================

// Cardinal directions
inline constexpr Direction kNorth = -10;
inline constexpr Direction kSouth = +10;
inline constexpr Direction kEast  = +1;
inline constexpr Direction kWest  = -1;

// Diagonal directions
inline constexpr Direction kNorthEast = -9;   // North + East = -10 + 1
inline constexpr Direction kNorthWest = -11;  // North + West = -10 - 1
inline constexpr Direction kSouthEast = +11;  // South + East = +10 + 1
inline constexpr Direction kSouthWest = +9;   // South + West = +10 - 1

// Knight move directions (L-shaped)
inline constexpr Direction kNorthNorthEast = -19;  // 2*North + East = -20 + 1
inline constexpr Direction kNorthEastEast  = -8;   // North + 2*East = -10 + 2
inline constexpr Direction kSouthEastEast  = +12;  // South + 2*East = +10 + 2
inline constexpr Direction kSouthSouthEast = +21;  // 2*South + East = +20 + 1
inline constexpr Direction kSouthSouthWest = +19;  // 2*South + West = +20 - 1
inline constexpr Direction kSouthWestWest  = +8;   // South + 2*West = +10 - 2
inline constexpr Direction kNorthWestWest  = -12;  // North + 2*West = -10 - 2
inline constexpr Direction kNorthNorthWest = -21;  // 2*North + West = -20 - 1

// No direction (stationary)
inline constexpr Direction kNoDirection = 0;

// ============================================================================
// Piece Movement Offset Arrays
// ============================================================================

/**
 * Number of knight move directions (8 L-shaped moves).
 */
inline constexpr std::size_t kNumKnightOffsets = 8;

/**
 * Number of bishop/rook diagonal/orthogonal directions (4 each).
 */
inline constexpr std::size_t kNumShortOffsets = 4;

/**
 * Number of queen/king directions (8: all orthogonal and diagonal).
 */
inline constexpr std::size_t kNumLongOffsets = 8;

/**
 * Knight movement offsets (8 L-shaped moves).
 * Matches Ada's Knight_Offsets constant.
 */
inline constexpr std::array<Direction, kNumKnightOffsets> kKnightOffsets = {{
    kNorthNorthEast, kNorthEastEast,
    kSouthEastEast,  kSouthSouthEast,
    kSouthSouthWest, kSouthWestWest,
    kNorthWestWest,  kNorthNorthWest
}};

/**
 * Bishop movement offsets (4 diagonal directions).
 * Matches Ada's Bishop_Offsets constant.
 */
inline constexpr std::array<Direction, kNumShortOffsets> kBishopOffsets = {{
    kNorthWest, kNorthEast, kSouthEast, kSouthWest
}};

/**
 * Rook movement offsets (4 orthogonal directions).
 * Matches Ada's Rook_Offsets constant.
 */
inline constexpr std::array<Direction, kNumShortOffsets> kRookOffsets = {{
    kNorth, kEast, kSouth, kWest
}};

/**
 * Queen movement offsets (8 directions: all orthogonal and diagonal).
 * Matches Ada's Queen_Offsets constant.
 */
inline constexpr std::array<Direction, kNumLongOffsets> kQueenOffsets = {{
    kNorth, kNorthEast, kEast, kSouthEast,
    kSouth, kSouthWest, kWest, kNorthWest
}};

/**
 * King movement offsets (8 directions: same as queen but only one step).
 * Matches Ada's King_Offsets constant.
 */
inline constexpr std::array<Direction, kNumLongOffsets> kKingOffsets = {{
    kNorth, kNorthEast, kEast, kSouthEast,
    kSouth, kSouthWest, kWest, kNorthWest
}};

// ============================================================================
// Direction Utility Functions
// ============================================================================

/**
 * Check if a direction is diagonal.
 * @param dir The direction to check.
 * @return true if the direction is diagonal (NE, NW, SE, SW).
 */
[[nodiscard]] constexpr bool is_diagonal(Direction dir) noexcept {
    return dir == kNorthEast || dir == kNorthWest ||
           dir == kSouthEast || dir == kSouthWest;
}

/**
 * Check if a direction is orthogonal (horizontal or vertical).
 * @param dir The direction to check.
 * @return true if the direction is orthogonal (N, S, E, W).
 */
[[nodiscard]] constexpr bool is_orthogonal(Direction dir) noexcept {
    return dir == kNorth || dir == kSouth ||
           dir == kEast || dir == kWest;
}

/**
 * Get the opposite direction.
 * @param dir The direction.
 * @return The opposite direction (-dir).
 */
[[nodiscard]] constexpr Direction opposite(Direction dir) noexcept {
    return static_cast<Direction>(-dir);
}

} // namespace chess::board

#endif // ADACHESS_BOARD_DIRECTIONS_HPP

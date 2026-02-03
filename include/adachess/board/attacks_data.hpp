/**
 * AdaChess - Smart Chess Engine
 *
 * Attack data infrastructure including direction tables and attack detection functions.
 * C++ port of the original Ada implementation (chess-board-attacks_data.ads and chess-engine.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_BOARD_ATTACKS_DATA_HPP
#define ADACHESS_BOARD_ATTACKS_DATA_HPP

#include <array>
#include <cstdint>

#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/board/board.hpp"
#include "adachess/board/directions.hpp"

namespace chess::board {

// ============================================================================
// Attack Data Structures
// ============================================================================

/**
 * Attack_Type structure holding information about a single attacker.
 * Corresponds to Ada's Attack_Type record.
 */
struct AttackType {
    Square origin;  ///< Square where the attack originates
    Piece piece;    ///< The attacking piece
};

/**
 * Maximum number of attackers that can attack a single square.
 */
inline constexpr std::size_t kMaxAttackers = 8;

/**
 * Attack_Array_Type for storing multiple attackers.
 * Corresponds to Ada's Attack_Array_Type.
 */
using AttackArray = std::array<AttackType, kMaxAttackers>;

/**
 * Attack_Collection_Type structure for collecting all attackers to a square.
 * Corresponds to Ada's Attack_Collection_Type record.
 */
struct AttackCollection {
    AttackArray attackers;          ///< Array of attackers
    std::size_t number_of_attackers; ///< Count of attackers found
};

// ============================================================================
// Direction Table
// ============================================================================

/**
 * Direction table type: 120x120 array mapping square pairs to connecting direction.
 * Corresponds to Ada's Direction_Table_Type.
 */
using DirectionTable = std::array<std::array<Direction, kBoardSize>, kBoardSize>;

/**
 * Global direction table for fast lookup of sliding direction between two squares.
 * This table is populated at startup by preload_sliding_direction().
 */
extern DirectionTable g_directions_table;

/**
 * Find the sliding direction connecting two squares.
 * The two squares are connected (from a sliding perspective) if they are on:
 * - the same file (North/South)
 * - the same rank (East/West)
 * - the same diagonal (NorthEast/SouthWest)
 * - the same anti-diagonal (NorthWest/SouthEast)
 *
 * @param origin The starting square.
 * @param destination The ending square.
 * @return The direction connecting the squares, or kNoDirection if not connected.
 * @pre origin != destination
 */
[[nodiscard]] inline Direction find_sliding_direction(Square origin, Square destination) noexcept {
    return g_directions_table[origin.to_index()][destination.to_index()];
}

/**
 * Preload the sliding direction table.
 * Must be called once at program startup before any chess operations.
 * Populates g_directions_table with the correct sliding directions.
 */
void preload_sliding_direction();

// ============================================================================
// Attack Detection Function Type
// ============================================================================

/**
 * Type alias for attack detection functions.
 * All attack detection functions take (Board, Side, Square) and return bool.
 * Corresponds to Ada's Attacks_Direction_Access type.
 */
using AttackFunction = bool (*)(const BoardArray& board, Color side, Square square);

// ============================================================================
// Attack Detection Functions - Sliding Pieces (8 directions)
// ============================================================================

/**
 * Detect if a rook or queen attacks from the north direction.
 * Scans northward from the square looking for rook/queen of the given side.
 *
 * @param board The board array.
 * @param side The side whose pieces we're checking for attacks.
 * @param square The square to check for attacks to.
 * @return true if a rook or queen of the given side attacks along this direction.
 */
[[nodiscard]] bool attacks_from_north(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a rook or queen attacks from the south direction.
 */
[[nodiscard]] bool attacks_from_south(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a rook or queen attacks from the east direction.
 */
[[nodiscard]] bool attacks_from_east(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a rook or queen attacks from the west direction.
 */
[[nodiscard]] bool attacks_from_west(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a bishop or queen attacks from the north-east direction.
 */
[[nodiscard]] bool attacks_from_north_east(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a bishop or queen attacks from the north-west direction.
 */
[[nodiscard]] bool attacks_from_north_west(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a bishop or queen attacks from the south-east direction.
 */
[[nodiscard]] bool attacks_from_south_east(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a bishop or queen attacks from the south-west direction.
 */
[[nodiscard]] bool attacks_from_south_west(const BoardArray& board, Color side, Square square) noexcept;

// ============================================================================
// Attack Detection Functions - Knight Directions (8 directions)
// ============================================================================

/**
 * Detect if a knight attacks from the north-north-east direction (2 up, 1 right).
 *
 * @param board The board array.
 * @param side The side whose pieces we're checking for attacks.
 * @param square The square to check for knight presence.
 * @return true if a knight of the given side is at this square.
 */
[[nodiscard]] bool attacks_from_north_north_east(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a knight attacks from the north-east-east direction (1 up, 2 right).
 */
[[nodiscard]] bool attacks_from_north_east_east(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a knight attacks from the south-east-east direction (1 down, 2 right).
 */
[[nodiscard]] bool attacks_from_south_east_east(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a knight attacks from the south-south-east direction (2 down, 1 right).
 */
[[nodiscard]] bool attacks_from_south_south_east(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a knight attacks from the south-south-west direction (2 down, 1 left).
 */
[[nodiscard]] bool attacks_from_south_south_west(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a knight attacks from the south-west-west direction (1 down, 2 left).
 */
[[nodiscard]] bool attacks_from_south_west_west(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a knight attacks from the north-west-west direction (1 up, 2 left).
 */
[[nodiscard]] bool attacks_from_north_west_west(const BoardArray& board, Color side, Square square) noexcept;

/**
 * Detect if a knight attacks from the north-north-west direction (2 up, 1 left).
 */
[[nodiscard]] bool attacks_from_north_north_west(const BoardArray& board, Color side, Square square) noexcept;

// ============================================================================
// Placeholder Function
// ============================================================================

/**
 * Placeholder function for unused direction slots in the dispatch table.
 * Always returns false.
 *
 * @param board Unused.
 * @param side Unused.
 * @param square Unused.
 * @return Always false.
 */
[[nodiscard]] bool attacks_from_placeholder(const BoardArray& board, Color side, Square square) noexcept;

// ============================================================================
// Attack Dispatch Table
// ============================================================================

/**
 * Direction range for dispatch table indexing.
 * Direction values range from -21 to +21, so we offset by 21 to get 0-42.
 */
inline constexpr std::size_t kDirectionOffset = 21;
inline constexpr std::size_t kDirectionTableSize = 43;  // -21 to +21 inclusive

/**
 * Attack dispatch table type.
 * Maps direction values to attack detection functions.
 */
using AttackDispatchTable = std::array<AttackFunction, kDirectionTableSize>;

/**
 * Global attack dispatch table.
 * Initialized with placeholder functions, then populated with real functions
 * during initialization.
 *
 * Usage: g_attacks_to[direction + kDirectionOffset](board, side, square)
 */
extern AttackDispatchTable g_attacks_to;

/**
 * Initialize the attack dispatch table.
 * Maps each valid direction to its corresponding attack function.
 * Called during program initialization.
 */
void initialize_attacks_dispatch_table();

/**
 * Helper function to lookup attack function by direction.
 *
 * @param direction The direction to look up.
 * @return Pointer to the attack function for this direction.
 */
[[nodiscard]] inline AttackFunction get_attack_function(Direction direction) noexcept {
    return g_attacks_to[static_cast<std::size_t>(direction + static_cast<int>(kDirectionOffset))];
}

} // namespace chess::board

#endif // ADACHESS_BOARD_ATTACKS_DATA_HPP

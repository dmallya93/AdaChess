/**
 * AdaChess - Smart Chess Engine
 *
 * Player type definitions for identifying human vs engine players.
 * C++ port of the original Ada implementation (chess-players.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_PLAYERS_HPP
#define ADACHESS_PLAYERS_HPP

#include <cstdint>

namespace chess::players {

// ============================================================================
// Player Type
// ============================================================================

/**
 * Player type enumeration to distinguish between human and engine players.
 *
 * This type is used by:
 * - Protocol handlers to distinguish between human and engine players
 * - Game management to determine who is to move
 * - Analysis features for annotating games
 *
 * Corresponds to Ada's Player_Type enumeration with:
 * - Size => 2 (2 bits)
 * - Default_Value => Human
 *
 * The underlying type std::uint8_t is the smallest standard integer type
 * that can hold 2 bits.
 */
enum class PlayerType : std::uint8_t {
    Human = 0,   // Human player
    Engine = 1   // Computer/engine player
};

/**
 * Default player type (Human), matching Ada's Default_Value specification.
 */
inline constexpr PlayerType kDefaultPlayer = PlayerType::Human;

// ============================================================================
// Player Type Utility Functions
// ============================================================================

/**
 * Check if a player is a human.
 * @param player The player type to check.
 * @return true if the player is human.
 */
[[nodiscard]] constexpr bool is_human(PlayerType player) noexcept {
    return player == PlayerType::Human;
}

/**
 * Check if a player is an engine.
 * @param player The player type to check.
 * @return true if the player is an engine.
 */
[[nodiscard]] constexpr bool is_engine(PlayerType player) noexcept {
    return player == PlayerType::Engine;
}

/**
 * Convert a PlayerType to its string representation.
 * @param player The player type.
 * @return A human-readable string describing the player type.
 */
[[nodiscard]] inline const char* player_to_string(PlayerType player) noexcept {
    switch (player) {
        case PlayerType::Human:
            return "Human";
        case PlayerType::Engine:
            return "Engine";
    }
    return "Unknown";
}

} // namespace chess::players

#endif // ADACHESS_PLAYERS_HPP

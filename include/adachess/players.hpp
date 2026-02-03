/**
 * AdaChess - Smart Chess Engine
 *
 * Player type definitions.
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

namespace chess {
namespace players {

/**
 * Player type enumeration.
 *
 * Distinguishes between human and engine players, which affects
 * how the chess engine interacts during play (e.g., time management,
 * protocol handling).
 *
 * Uses explicit underlying type std::uint8_t to minimize storage,
 * matching Ada's Size => 2 specification.
 *
 * Default value is Human, matching Ada's Default_Value => Human.
 */
enum class PlayerType : std::uint8_t {
    Human = 0,   ///< Human player (default)
    Engine = 1   ///< Computer engine player
};

/**
 * Default player type (Human).
 * Matches Ada's Default_Value => Human specification.
 */
inline constexpr PlayerType kDefaultPlayerType = PlayerType::Human;

} // namespace players
} // namespace chess

#endif // ADACHESS_PLAYERS_HPP

// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/square.hpp"
#include <array>
#include <cstdint>

namespace adachess {

// Direction type for piece movement offsets
// Range: -21 .. +21 (corresponds to movement on a 10x12 mailbox board)
using Direction = int8_t;

// Direction constants for moving pieces on 10x12 mailbox
// The board is indexed row-by-row with 10 squares per row
// Moving North (towards rank 8) means -10, South means +10
constexpr Direction NO_DIRECTION = 0;
constexpr Direction NORTH = -10;
constexpr Direction NORTH_NORTH_EAST = -19;
constexpr Direction NORTH_EAST = -9;
constexpr Direction NORTH_EAST_EAST = -8;
constexpr Direction EAST = +1;
constexpr Direction SOUTH_EAST_EAST = +12;
constexpr Direction SOUTH_EAST = +11;
constexpr Direction SOUTH_SOUTH_EAST = +21;
constexpr Direction SOUTH = +10;
constexpr Direction SOUTH_SOUTH_WEST = +19;
constexpr Direction SOUTH_WEST = +9;
constexpr Direction SOUTH_WEST_WEST = +8;
constexpr Direction WEST = -1;
constexpr Direction NORTH_WEST_WEST = -12;
constexpr Direction NORTH_WEST = -11;
constexpr Direction NORTH_NORTH_WEST = -21;

// Knight move offsets (8 L-shaped moves)
constexpr std::array<Direction, 8> KNIGHT_OFFSETS = {
    NORTH_NORTH_EAST, NORTH_EAST_EAST,
    SOUTH_EAST_EAST, SOUTH_SOUTH_EAST,
    SOUTH_SOUTH_WEST, SOUTH_WEST_WEST,
    NORTH_WEST_WEST, NORTH_NORTH_WEST
};

// Bishop move offsets (4 diagonal directions)
constexpr std::array<Direction, 4> BISHOP_OFFSETS = {
    NORTH_WEST, NORTH_EAST, SOUTH_EAST, SOUTH_WEST
};

// Rook move offsets (4 orthogonal directions)
constexpr std::array<Direction, 4> ROOK_OFFSETS = {
    NORTH, EAST, SOUTH, WEST
};

// Queen move offsets (8 directions: diagonals + orthogonals)
constexpr std::array<Direction, 8> QUEEN_OFFSETS = {
    NORTH, NORTH_EAST, EAST, SOUTH_EAST,
    SOUTH, SOUTH_WEST, WEST, NORTH_WEST
};

// King move offsets (8 directions, same as queen)
constexpr std::array<Direction, 8> KING_OFFSETS = {
    NORTH, NORTH_EAST, EAST, SOUTH_EAST,
    SOUTH, SOUTH_WEST, WEST, NORTH_WEST
};

} // namespace adachess

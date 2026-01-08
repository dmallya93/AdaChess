//
// AdaChess - Smart Chess Engine
//
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// Email: adachess@gmail.com - Web Page: https://github.com/adachess/AdaChess
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ADACHESS_BOARD_DIRECTION_HPP
#define ADACHESS_BOARD_DIRECTION_HPP

#include "adachess/core/types.hpp"
#include <array>
#include <cstdint>

namespace adachess::board {

// ====== Direction Type ======
using Direction = int8_t;

// ====== Cardinal Directions ======
constexpr Direction North = -10;
constexpr Direction South = 10;
constexpr Direction East = 1;
constexpr Direction West = -1;

// ====== Diagonal Directions ======
constexpr Direction North_East = -9;
constexpr Direction North_West = -11;
constexpr Direction South_East = 11;
constexpr Direction South_West = 9;

// ====== Knight Directions ======
constexpr Direction North_North_East = -19;
constexpr Direction North_East_East = -8;
constexpr Direction South_East_East = 12;
constexpr Direction South_South_East = 21;
constexpr Direction South_South_West = 19;
constexpr Direction South_West_West = 8;
constexpr Direction North_West_West = -12;
constexpr Direction North_North_West = -21;

// ====== Direction Offset Arrays ======
constexpr std::array<Direction, 8> knight_offsets = {
    North_North_East, North_East_East, South_East_East, South_South_East,
    South_South_West, South_West_West, North_West_West, North_North_West
};

constexpr std::array<Direction, 4> bishop_offsets = {
    North_East, North_West, South_East, South_West
};

constexpr std::array<Direction, 4> rook_offsets = {
    North, South, East, West
};

constexpr std::array<Direction, 8> queen_offsets = {
    North, South, East, West,
    North_East, North_West, South_East, South_West
};

constexpr std::array<Direction, 8> king_offsets = {
    North, South, East, West,
    North_East, North_West, South_East, South_West
};

// ====== Pawn Direction Helpers ======
[[nodiscard]] constexpr inline Direction pawn_push(adachess::core::Color color) noexcept {
    return color == adachess::core::Color::White ? North : South;
}

[[nodiscard]] constexpr inline Direction pawn_double_push(adachess::core::Color color) noexcept {
    return color == adachess::core::Color::White ? (North + North) : (South + South);
}

}  // namespace adachess::board

#endif  // ADACHESS_BOARD_DIRECTION_HPP

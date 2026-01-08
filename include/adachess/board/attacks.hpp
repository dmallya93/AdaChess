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

#ifndef ADACHESS_BOARD_ATTACKS_HPP
#define ADACHESS_BOARD_ATTACKS_HPP

#include "adachess/core/types.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

namespace adachess::board {

// Forward declarations for types we need
using Square = uint8_t;
using Direction = int8_t;
using BoardArray = std::array<adachess::core::Piece, 120>;

// ====== Attack Type Definitions ======

struct Attack {
    Square origin;  // Square where the attack begins
    adachess::core::Piece piece;
};

struct AttackCollection {
    std::array<Attack, 8> attackers;
    uint8_t number_of_attackers;
};

// ====== Direction Constants ======

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
constexpr Direction NO_DIRECTION = 0;

// ====== Attack Detection Functions ======

// Sliding piece attacks (rook, queen on ranks/files)
[[nodiscard]] inline bool attacks_from_north(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_south(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_east(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_west(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

// Sliding piece attacks (bishop, queen on diagonals)
[[nodiscard]] inline bool attacks_from_north_east(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_north_west(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_south_east(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_south_west(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

// Knight attacks
[[nodiscard]] inline bool attacks_from_north_north_east(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_north_east_east(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_south_east_east(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_south_south_east(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_south_south_west(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_south_west_west(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_north_west_west(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

[[nodiscard]] inline bool attacks_from_north_north_west(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

// Placeholder function for unused directions
[[nodiscard]] inline bool attacks_from_placeholder(
    const BoardArray& board,
    adachess::core::Color side,
    Square square) noexcept;

// ====== Function Pointer Type ======

using AttackFunction = bool (*)(const BoardArray&, adachess::core::Color, Square) noexcept;

// ====== Attack Function Pointer Array ======

// Array mapping direction (-21..+21) to attack detection function
// This allows O(1) lookup: attacks_to[direction](board, side, square)
constexpr size_t DIRECTION_ARRAY_SIZE = 43;  // Range: -21 to +21 (inclusive)
constexpr size_t DIRECTION_OFFSET = 21;      // Offset to convert direction to array index

extern std::array<AttackFunction, DIRECTION_ARRAY_SIZE> attacks_to;

// Helper function to initialize the attacks_to array
void initialize_attacks_to();

}  // namespace adachess::board

// ====== Inline Implementations ======

namespace adachess::board {

using adachess::core::Color;
using adachess::core::Piece;
using adachess::core::White_Rook;
using adachess::core::White_Queen;
using adachess::core::Black_Rook;
using adachess::core::Black_Queen;
using adachess::core::White_Bishop;
using adachess::core::Black_Bishop;
using adachess::core::White_Knight;
using adachess::core::Black_Knight;

// Orthogonal attacks (rook/queen)

inline bool attacks_from_north(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    Square target = square + NORTH;
    while (board[target] == Piece::Empty) {
        target += NORTH;
    }
    const Piece piece = board[target];
    if (side == Color::White) {
        return piece == White_Rook || piece == White_Queen;
    }
    return piece == Black_Rook || piece == Black_Queen;
}

inline bool attacks_from_south(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    Square target = square + SOUTH;
    while (board[target] == Piece::Empty) {
        target += SOUTH;
    }
    const Piece piece = board[target];
    if (side == Color::White) {
        return piece == White_Rook || piece == White_Queen;
    }
    return piece == Black_Rook || piece == Black_Queen;
}

inline bool attacks_from_east(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    Square target = square + EAST;
    while (board[target] == Piece::Empty) {
        target += EAST;
    }
    const Piece piece = board[target];
    if (side == Color::White) {
        return piece == White_Rook || piece == White_Queen;
    }
    return piece == Black_Rook || piece == Black_Queen;
}

inline bool attacks_from_west(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    Square target = square + WEST;
    while (board[target] == Piece::Empty) {
        target += WEST;
    }
    const Piece piece = board[target];
    if (side == Color::White) {
        return piece == White_Rook || piece == White_Queen;
    }
    return piece == Black_Rook || piece == Black_Queen;
}

// Diagonal attacks (bishop/queen)

inline bool attacks_from_north_east(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    Square target = square + NORTH_EAST;
    while (board[target] == Piece::Empty) {
        target += NORTH_EAST;
    }
    const Piece piece = board[target];
    if (side == Color::White) {
        return piece == White_Bishop || piece == White_Queen;
    }
    return piece == Black_Bishop || piece == Black_Queen;
}

inline bool attacks_from_north_west(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    Square target = square + NORTH_WEST;
    while (board[target] == Piece::Empty) {
        target += NORTH_WEST;
    }
    const Piece piece = board[target];
    if (side == Color::White) {
        return piece == White_Bishop || piece == White_Queen;
    }
    return piece == Black_Bishop || piece == Black_Queen;
}

inline bool attacks_from_south_east(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    Square target = square + SOUTH_EAST;
    while (board[target] == Piece::Empty) {
        target += SOUTH_EAST;
    }
    const Piece piece = board[target];
    if (side == Color::White) {
        return piece == White_Bishop || piece == White_Queen;
    }
    return piece == Black_Bishop || piece == Black_Queen;
}

inline bool attacks_from_south_west(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    Square target = square + SOUTH_WEST;
    while (board[target] == Piece::Empty) {
        target += SOUTH_WEST;
    }
    const Piece piece = board[target];
    if (side == Color::White) {
        return piece == White_Bishop || piece == White_Queen;
    }
    return piece == Black_Bishop || piece == Black_Queen;
}

// Knight attacks (non-sliding, check single square)

inline bool attacks_from_north_north_east(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    const Piece piece = board[square];
    if (side == Color::White) {
        return piece == White_Knight;
    }
    return piece == Black_Knight;
}

inline bool attacks_from_north_east_east(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    const Piece piece = board[square];
    if (side == Color::White) {
        return piece == White_Knight;
    }
    return piece == Black_Knight;
}

inline bool attacks_from_south_east_east(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    const Piece piece = board[square];
    if (side == Color::White) {
        return piece == White_Knight;
    }
    return piece == Black_Knight;
}

inline bool attacks_from_south_south_east(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    const Piece piece = board[square];
    if (side == Color::White) {
        return piece == White_Knight;
    }
    return piece == Black_Knight;
}

inline bool attacks_from_south_south_west(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    const Piece piece = board[square];
    if (side == Color::White) {
        return piece == White_Knight;
    }
    return piece == Black_Knight;
}

inline bool attacks_from_south_west_west(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    const Piece piece = board[square];
    if (side == Color::White) {
        return piece == White_Knight;
    }
    return piece == Black_Knight;
}

inline bool attacks_from_north_west_west(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    const Piece piece = board[square];
    if (side == Color::White) {
        return piece == White_Knight;
    }
    return piece == Black_Knight;
}

inline bool attacks_from_north_north_west(
    const BoardArray& board,
    Color side,
    Square square) noexcept {
    const Piece piece = board[square];
    if (side == Color::White) {
        return piece == White_Knight;
    }
    return piece == Black_Knight;
}

// Placeholder for unused directions
inline bool attacks_from_placeholder(
    const BoardArray& /* board */,
    Color /* side */,
    Square /* square */) noexcept {
    return false;
}

}  // namespace adachess::board

#endif  // ADACHESS_BOARD_ATTACKS_HPP

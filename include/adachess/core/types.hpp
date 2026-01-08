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

#ifndef ADACHESS_CORE_TYPES_HPP
#define ADACHESS_CORE_TYPES_HPP

#include <cstdint>

namespace adachess::core {

// ====== Color Types ======
enum class Color : uint8_t {
    White = 0,
    Black = 1
};

// ====== Piece Types ======
enum class Piece : uint8_t {
    Empty = 0,
    Frame = 1,

    White_Pawn = 2,
    White_Knight = 3,
    White_Bishop = 4,
    White_Rook = 5,
    White_Queen = 6,
    White_King = 7,

    Black_Pawn = 8,
    Black_Knight = 9,
    Black_Bishop = 10,
    Black_Rook = 11,
    Black_Queen = 12,
    Black_King = 13
};

// Helper functions for piece queries
[[nodiscard]] constexpr inline bool is_white_piece(Piece piece) noexcept {
    return piece >= Piece::White_Pawn && piece <= Piece::White_King;
}

[[nodiscard]] constexpr inline bool is_black_piece(Piece piece) noexcept {
    return piece >= Piece::Black_Pawn && piece <= Piece::Black_King;
}

[[nodiscard]] constexpr inline bool is_empty(Piece piece) noexcept {
    return piece == Piece::Empty;
}

[[nodiscard]] constexpr inline bool is_frame(Piece piece) noexcept {
    return piece == Piece::Frame;
}

[[nodiscard]] constexpr inline Color piece_color(Piece piece) noexcept {
    return is_white_piece(piece) ? Color::White : Color::Black;
}

// ====== Subtypes for specific pieces ======
constexpr Piece White_Pawn = Piece::White_Pawn;
constexpr Piece White_Knight = Piece::White_Knight;
constexpr Piece White_Bishop = Piece::White_Bishop;
constexpr Piece White_Rook = Piece::White_Rook;
constexpr Piece White_Queen = Piece::White_Queen;
constexpr Piece White_King = Piece::White_King;

constexpr Piece Black_Pawn = Piece::Black_Pawn;
constexpr Piece Black_Knight = Piece::Black_Knight;
constexpr Piece Black_Bishop = Piece::Black_Bishop;
constexpr Piece Black_Rook = Piece::Black_Rook;
constexpr Piece Black_Queen = Piece::Black_Queen;
constexpr Piece Black_King = Piece::Black_King;

// ====== Other Basic Types ======
using Depth = uint8_t;
using History = uint16_t;
using Hash = uint64_t;
using Score = int16_t;
using NodeCount = uint64_t;

}  // namespace adachess::core

#endif  // ADACHESS_CORE_TYPES_HPP

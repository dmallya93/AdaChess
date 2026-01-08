// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>

namespace adachess {

// Color enumeration
enum class Color : uint8_t {
    White = 0,
    Black = 1
};

// Color swap operator
constexpr Color operator!(Color c) {
    return (c == Color::White) ? Color::Black : Color::White;
}

// Piece type enumeration
// Preserves Ada representation values for compatibility
enum class Piece : uint8_t {
    Frame = 0,     // Board frame (off-board marker)
    Empty = 1,     // Empty square

    // White pieces (2-7)
    White_Pawn = 2,
    White_Knight = 3,
    White_Bishop = 4,
    White_Rook = 5,
    White_Queen = 6,
    White_King = 7,

    // Black pieces (8-13)
    Black_Pawn = 8,
    Black_Knight = 9,
    Black_Bishop = 10,
    Black_Rook = 11,
    Black_Queen = 12,
    Black_King = 13
};

// Board_Piece_Type represents pieces that can appear on the board (excludes Frame)
// In Ada: subtype Board_Piece_Type is Piece_Type range Empty .. Black_King;
// We use Piece for both, but document the constraint
using Board_Piece_Type = Piece;

// Piece predicates
constexpr bool is_empty(Piece p) {
    return p == Piece::Empty;
}

constexpr bool is_white_piece(Piece p) {
    return p >= Piece::White_Pawn && p <= Piece::White_King;
}

constexpr bool is_black_piece(Piece p) {
    return p >= Piece::Black_Pawn && p <= Piece::Black_King;
}

constexpr bool is_piece(Piece p) {
    return is_white_piece(p) || is_black_piece(p);
}

constexpr Color piece_color(Piece p) {
    // Assumes p is a valid piece (not Empty or Frame)
    return static_cast<uint8_t>(p) <= static_cast<uint8_t>(Piece::White_King)
        ? Color::White
        : Color::Black;
}

} // namespace adachess

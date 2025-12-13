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
//

#pragma once

#include <cstdint>
#include <array>

namespace adachess {

/// Piece type enumeration representing all chess pieces plus Frame and Empty
/// Uses 4 bits to match Ada's representation
enum class Piece : std::uint8_t {
  Frame        = 0,
  Empty        = 1,
  WhitePawn    = 2,
  WhiteKnight  = 3,
  WhiteBishop  = 4,
  WhiteRook    = 5,
  WhiteQueen   = 6,
  WhiteKing    = 7,
  BlackPawn    = 8,
  BlackKnight  = 9,
  BlackBishop  = 10,
  BlackRook    = 11,
  BlackQueen   = 12,
  BlackKing    = 13
};

/// Returns true if the piece is Empty (Ada's "not" operator)
[[nodiscard]] constexpr bool is_empty(Piece piece) noexcept {
  return piece == Piece::Empty;
}

/// Returns true if the piece is a real chess piece (not Empty or Frame)
[[nodiscard]] constexpr bool is_piece(Piece piece) noexcept {
  return piece != Piece::Empty && piece != Piece::Frame;
}

/// Type predicates for piece classification

[[nodiscard]] constexpr bool is_board_piece(Piece piece) noexcept {
  return piece >= Piece::Empty && piece <= Piece::BlackKing;
}

[[nodiscard]] constexpr bool is_chess_piece(Piece piece) noexcept {
  return piece >= Piece::WhitePawn && piece <= Piece::BlackKing;
}

[[nodiscard]] constexpr bool is_white_piece(Piece piece) noexcept {
  return piece >= Piece::WhitePawn && piece <= Piece::WhiteKing;
}

[[nodiscard]] constexpr bool is_black_piece(Piece piece) noexcept {
  return piece >= Piece::BlackPawn && piece <= Piece::BlackKing;
}

[[nodiscard]] constexpr bool is_white_promotion(Piece piece) noexcept {
  return piece >= Piece::WhiteKnight && piece <= Piece::WhiteQueen;
}

[[nodiscard]] constexpr bool is_black_promotion(Piece piece) noexcept {
  return piece >= Piece::BlackKnight && piece <= Piece::BlackQueen;
}

[[nodiscard]] constexpr bool is_white_non_pawn(Piece piece) noexcept {
  return piece >= Piece::WhiteKnight && piece <= Piece::WhiteQueen;
}

[[nodiscard]] constexpr bool is_black_non_pawn(Piece piece) noexcept {
  return piece >= Piece::BlackKnight && piece <= Piece::BlackQueen;
}

[[nodiscard]] constexpr bool is_pawn(Piece piece) noexcept {
  return piece == Piece::WhitePawn || piece == Piece::BlackPawn;
}

[[nodiscard]] constexpr bool is_knight(Piece piece) noexcept {
  return piece == Piece::WhiteKnight || piece == Piece::BlackKnight;
}

[[nodiscard]] constexpr bool is_bishop(Piece piece) noexcept {
  return piece == Piece::WhiteBishop || piece == Piece::BlackBishop;
}

[[nodiscard]] constexpr bool is_rook(Piece piece) noexcept {
  return piece == Piece::WhiteRook || piece == Piece::BlackRook;
}

[[nodiscard]] constexpr bool is_queen(Piece piece) noexcept {
  return piece == Piece::WhiteQueen || piece == Piece::BlackQueen;
}

[[nodiscard]] constexpr bool is_king(Piece piece) noexcept {
  return piece == Piece::WhiteKing || piece == Piece::BlackKing;
}

[[nodiscard]] constexpr bool is_sliding_piece(Piece piece) noexcept {
  return is_bishop(piece) || is_rook(piece) || is_queen(piece);
}

/// Piece symbols for display (matches Ada's Symbols array)
inline constexpr std::array<char, 14> piece_symbols = {
  ' ',  // Frame
  ' ',  // Empty
  'P',  // WhitePawn
  'N',  // WhiteKnight
  'B',  // WhiteBishop
  'R',  // WhiteRook
  'Q',  // WhiteQueen
  'K',  // WhiteKing
  'p',  // BlackPawn
  'n',  // BlackKnight
  'b',  // BlackBishop
  'r',  // BlackRook
  'q',  // BlackQueen
  'k'   // BlackKing
};

/// Get the symbol for a piece
[[nodiscard]] constexpr char piece_to_symbol(Piece piece) noexcept {
  auto index = static_cast<std::uint8_t>(piece);
  if (index >= piece_symbols.size()) {
    return '?';  // Invalid piece
  }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  return piece_symbols[index];
}

/// Piece counter types (matches Ada's counter ranges)
using PieceCounter = std::uint8_t;  // 0..16
using PawnCounter = std::uint8_t;   // 0..8

constexpr PieceCounter max_pieces_per_side = 16;
constexpr PawnCounter max_pawns_per_side = 8;

}  // namespace adachess

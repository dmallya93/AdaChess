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

#include <adachess/pieces.hpp>
#include <adachess/colors.hpp>
#include <array>
#include <cstdint>

namespace adachess {

/// Square type representing positions on the 10x12 mailbox board
/// Valid squares are 21-98 (the 8x8 playable area)
/// Squares 0-20, 29-30, 39-40, ..., 99-119 are Frame (out of bounds)
using Square = std::uint8_t;

constexpr Square min_square = 0;
constexpr Square max_square = 119;

/// Board type is a 10x12 mailbox representation
/// The frame allows detecting moves out of bounds
/// Layout:
/// x x x x x x x x x x   (0-9)
/// x x x x x x x x x x   (10-19)
/// x r n b q k b n r x   (20-29, playable: 21-28 = rank 8)
/// x p p p p p p p p x   (30-39, playable: 31-38 = rank 7)
/// x . . . . . . . . x   (40-49, playable: 41-48 = rank 6)
/// x . . . . . . . . x   (50-59, playable: 51-58 = rank 5)
/// x . . . . . . . . x   (60-69, playable: 61-68 = rank 4)
/// x . . . . . . . . x   (70-79, playable: 71-78 = rank 3)
/// x P P P P P P P P x   (80-89, playable: 81-88 = rank 2)
/// x R N B Q K B N R x   (90-99, playable: 91-98 = rank 1)
/// x x x x x x x x x x   (100-109)
/// x x x x x x x x x x   (110-119)
using BoardArray = std::array<Piece, 120>;

/// Square constants (a1-h8)
namespace squares {
  constexpr Square a8 = 21;
  constexpr Square b8 = 22;
  constexpr Square c8 = 23;
  constexpr Square d8 = 24;
  constexpr Square e8 = 25;
  constexpr Square f8 = 26;
  constexpr Square g8 = 27;
  constexpr Square h8 = 28;

  constexpr Square a7 = 31;
  constexpr Square b7 = 32;
  constexpr Square c7 = 33;
  constexpr Square d7 = 34;
  constexpr Square e7 = 35;
  constexpr Square f7 = 36;
  constexpr Square g7 = 37;
  constexpr Square h7 = 38;

  constexpr Square a6 = 41;
  constexpr Square b6 = 42;
  constexpr Square c6 = 43;
  constexpr Square d6 = 44;
  constexpr Square e6 = 45;
  constexpr Square f6 = 46;
  constexpr Square g6 = 47;
  constexpr Square h6 = 48;

  constexpr Square a5 = 51;
  constexpr Square b5 = 52;
  constexpr Square c5 = 53;
  constexpr Square d5 = 54;
  constexpr Square e5 = 55;
  constexpr Square f5 = 56;
  constexpr Square g5 = 57;
  constexpr Square h5 = 58;

  constexpr Square a4 = 61;
  constexpr Square b4 = 62;
  constexpr Square c4 = 63;
  constexpr Square d4 = 64;
  constexpr Square e4 = 65;
  constexpr Square f4 = 66;
  constexpr Square g4 = 67;
  constexpr Square h4 = 68;

  constexpr Square a3 = 71;
  constexpr Square b3 = 72;
  constexpr Square c3 = 73;
  constexpr Square d3 = 74;
  constexpr Square e3 = 75;
  constexpr Square f3 = 76;
  constexpr Square g3 = 77;
  constexpr Square h3 = 78;

  constexpr Square a2 = 81;
  constexpr Square b2 = 82;
  constexpr Square c2 = 83;
  constexpr Square d2 = 84;
  constexpr Square e2 = 85;
  constexpr Square f2 = 86;
  constexpr Square g2 = 87;
  constexpr Square h2 = 88;

  constexpr Square a1 = 91;
  constexpr Square b1 = 92;
  constexpr Square c1 = 93;
  constexpr Square d1 = 94;
  constexpr Square e1 = 95;
  constexpr Square f1 = 96;
  constexpr Square g1 = 97;
  constexpr Square h1 = 98;

  constexpr Square no_en_passant = 0;
}  // namespace squares

/// Frame detection array - true for squares outside the playable 8x8 area
inline constexpr std::array<bool, 120> square_is_frame = {
  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
  true,  false, false, false, false, false, false, false, false, true,
  true,  false, false, false, false, false, false, false, false, true,
  true,  false, false, false, false, false, false, false, false, true,
  true,  false, false, false, false, false, false, false, false, true,
  true,  false, false, false, false, false, false, false, false, true,
  true,  false, false, false, false, false, false, false, false, true,
  true,  false, false, false, false, false, false, false, false, true,
  true,  false, false, false, false, false, false, false, false, true,
  true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
  true,  true,  true,  true,  true,  true,  true,  true,  true,  true
};

/// Check if a square is a frame (out of bounds)
[[nodiscard]] constexpr bool is_frame(Square square) noexcept {
  if (square >= square_is_frame.size()) {
    return true;  // Treat out-of-range as frame (invalid)
  }
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
  return square_is_frame[square];
}

/// Check if a square is valid (not a frame)
[[nodiscard]] constexpr bool is_valid_square(Square square) noexcept {
  return !is_frame(square);
}

}  // namespace adachess

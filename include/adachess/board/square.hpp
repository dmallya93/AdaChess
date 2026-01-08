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

#ifndef ADACHESS_BOARD_SQUARE_HPP
#define ADACHESS_BOARD_SQUARE_HPP

#include "adachess/core/types.hpp"
#include <array>
#include <cstdint>

namespace adachess::board {

// ====== Square Type ======
using Square = uint8_t;
using Coordinate = int8_t;

// ====== Board Array Type ======
using BoardArray = std::array<adachess::core::Piece, 120>;

// ====== Square Constants (10x12 mailbox, Ada compatible) ======
constexpr Square A1 = 91;
constexpr Square B1 = 92;
constexpr Square C1 = 93;
constexpr Square D1 = 94;
constexpr Square E1 = 95;
constexpr Square F1 = 96;
constexpr Square G1 = 97;
constexpr Square H1 = 98;

constexpr Square A2 = 81;
constexpr Square B2 = 82;
constexpr Square C2 = 83;
constexpr Square D2 = 84;
constexpr Square E2 = 85;
constexpr Square F2 = 86;
constexpr Square G2 = 87;
constexpr Square H2 = 88;

constexpr Square A3 = 71;
constexpr Square B3 = 72;
constexpr Square C3 = 73;
constexpr Square D3 = 74;
constexpr Square E3 = 75;
constexpr Square F3 = 76;
constexpr Square G3 = 77;
constexpr Square H3 = 78;

constexpr Square A4 = 61;
constexpr Square B4 = 62;
constexpr Square C4 = 63;
constexpr Square D4 = 64;
constexpr Square E4 = 65;
constexpr Square F4 = 66;
constexpr Square G4 = 67;
constexpr Square H4 = 68;

constexpr Square A5 = 51;
constexpr Square B5 = 52;
constexpr Square C5 = 53;
constexpr Square D5 = 54;
constexpr Square E5 = 55;
constexpr Square F5 = 56;
constexpr Square G5 = 57;
constexpr Square H5 = 58;

constexpr Square A6 = 41;
constexpr Square B6 = 42;
constexpr Square C6 = 43;
constexpr Square D6 = 44;
constexpr Square E6 = 45;
constexpr Square F6 = 46;
constexpr Square G6 = 47;
constexpr Square H6 = 48;

constexpr Square A7 = 31;
constexpr Square B7 = 32;
constexpr Square C7 = 33;
constexpr Square D7 = 34;
constexpr Square E7 = 35;
constexpr Square F7 = 36;
constexpr Square G7 = 37;
constexpr Square H7 = 38;

constexpr Square A8 = 21;
constexpr Square B8 = 22;
constexpr Square C8 = 23;
constexpr Square D8 = 24;
constexpr Square E8 = 25;
constexpr Square F8 = 26;
constexpr Square G8 = 27;
constexpr Square H8 = 28;

// ====== Frame Detection ======
constexpr std::array<bool, 120> is_frame_square = {
    // Row 0 (indices 0-9) - all frame
    true, true, true, true, true, true, true, true, true, true,
    // Row 1 (indices 10-19) - all frame
    true, true, true, true, true, true, true, true, true, true,
    // Row 2 (indices 20-29) - frame, 8 valid, frame (rank 8)
    true, false, false, false, false, false, false, false, false, true,
    // Row 3 (indices 30-39) - frame, 8 valid, frame (rank 7)
    true, false, false, false, false, false, false, false, false, true,
    // Row 4 (indices 40-49) - frame, 8 valid, frame (rank 6)
    true, false, false, false, false, false, false, false, false, true,
    // Row 5 (indices 50-59) - frame, 8 valid, frame (rank 5)
    true, false, false, false, false, false, false, false, false, true,
    // Row 6 (indices 60-69) - frame, 8 valid, frame (rank 4)
    true, false, false, false, false, false, false, false, false, true,
    // Row 7 (indices 70-79) - frame, 8 valid, frame (rank 3)
    true, false, false, false, false, false, false, false, false, true,
    // Row 8 (indices 80-89) - frame, 8 valid, frame (rank 2)
    true, false, false, false, false, false, false, false, false, true,
    // Row 9 (indices 90-99) - frame, 8 valid, frame (rank 1)
    true, false, false, false, false, false, false, false, false, true,
    // Row 10 (indices 100-109) - all frame
    true, true, true, true, true, true, true, true, true, true,
    // Row 11 (indices 110-119) - all frame
    true, true, true, true, true, true, true, true, true, true
};

[[nodiscard]] constexpr inline bool is_frame(Square square) noexcept {
    return is_frame_square[square];
}

// ====== Coordinate Extraction ======
[[nodiscard]] constexpr inline Coordinate file(Square square) noexcept {
    if (is_frame(square)) {
        return -1;
    }
    return static_cast<Coordinate>(square % 10 - 1);
}

[[nodiscard]] constexpr inline Coordinate rank(Square square) noexcept {
    if (is_frame(square)) {
        return -1;
    }
    return static_cast<Coordinate>(10 - square / 10);
}

[[nodiscard]] constexpr inline Coordinate diagonal(Square square) noexcept {
    if (is_frame(square)) {
        return -1;
    }
    const Coordinate f = file(square);
    const Coordinate r = rank(square);
    return static_cast<Coordinate>(7 + r - f);
}

[[nodiscard]] constexpr inline Coordinate anti_diagonal(Square square) noexcept {
    if (is_frame(square)) {
        return -1;
    }
    const Coordinate f = file(square);
    const Coordinate r = rank(square);
    return static_cast<Coordinate>(f + r - 2);
}

}  // namespace adachess::board

#endif  // ADACHESS_BOARD_SQUARE_HPP

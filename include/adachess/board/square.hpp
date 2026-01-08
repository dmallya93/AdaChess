// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <cstdint>

namespace adachess {

// Square type for 10x12 mailbox board
// Range: 0..119 (10x12 = 120 squares)
// Valid chess squares: A1=91, A8=21, H1=98, H8=28
using Square = uint8_t;

// Square constants (preserving Ada's 10x12 mailbox layout)
// The board is laid out with frame squares around an 8x8 chess board
// Frame squares are off-board markers used for move generation efficiency

// Some common square constants
constexpr Square INVALID_SQUARE = 0;

// Rank 1 (white's back rank)
constexpr Square A1 = 91;
constexpr Square B1 = 92;
constexpr Square C1 = 93;
constexpr Square D1 = 94;
constexpr Square E1 = 95;
constexpr Square F1 = 96;
constexpr Square G1 = 97;
constexpr Square H1 = 98;

// Rank 2
constexpr Square A2 = 81;
constexpr Square B2 = 82;
constexpr Square C2 = 83;
constexpr Square D2 = 84;
constexpr Square E2 = 85;
constexpr Square F2 = 86;
constexpr Square G2 = 87;
constexpr Square H2 = 88;

// Rank 3
constexpr Square A3 = 71;
constexpr Square B3 = 72;
constexpr Square C3 = 73;
constexpr Square D3 = 74;
constexpr Square E3 = 75;
constexpr Square F3 = 76;
constexpr Square G3 = 77;
constexpr Square H3 = 78;

// Rank 4
constexpr Square A4 = 61;
constexpr Square B4 = 62;
constexpr Square C4 = 63;
constexpr Square D4 = 64;
constexpr Square E4 = 65;
constexpr Square F4 = 66;
constexpr Square G4 = 67;
constexpr Square H4 = 68;

// Rank 5
constexpr Square A5 = 51;
constexpr Square B5 = 52;
constexpr Square C5 = 53;
constexpr Square D5 = 54;
constexpr Square E5 = 55;
constexpr Square F5 = 56;
constexpr Square G5 = 57;
constexpr Square H5 = 58;

// Rank 6
constexpr Square A6 = 41;
constexpr Square B6 = 42;
constexpr Square C6 = 43;
constexpr Square D6 = 44;
constexpr Square E6 = 45;
constexpr Square F6 = 46;
constexpr Square G6 = 47;
constexpr Square H6 = 48;

// Rank 7
constexpr Square A7 = 31;
constexpr Square B7 = 32;
constexpr Square C7 = 33;
constexpr Square D7 = 34;
constexpr Square E7 = 35;
constexpr Square F7 = 36;
constexpr Square G7 = 37;
constexpr Square H7 = 38;

// Rank 8 (black's back rank)
constexpr Square A8 = 21;
constexpr Square B8 = 22;
constexpr Square C8 = 23;
constexpr Square D8 = 24;
constexpr Square E8 = 25;
constexpr Square F8 = 26;
constexpr Square G8 = 27;
constexpr Square H8 = 28;

// File and Rank types
using File = uint8_t;
using Rank = uint8_t;

constexpr File FILE_A = 0;
constexpr File FILE_H = 7;
constexpr Rank RANK_1 = 0;
constexpr Rank RANK_8 = 7;

} // namespace adachess

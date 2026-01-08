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
constexpr Square A1 = 91;
constexpr Square H1 = 98;
constexpr Square A8 = 21;
constexpr Square H8 = 28;

// File and Rank types
using File = uint8_t;
using Rank = uint8_t;

constexpr File FILE_A = 0;
constexpr File FILE_H = 7;
constexpr Rank RANK_1 = 0;
constexpr Rank RANK_8 = 7;

} // namespace adachess

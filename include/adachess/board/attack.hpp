// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/chessboard.hpp"
#include "adachess/board/square.hpp"
#include "adachess/core/types.hpp"

namespace adachess {

// Check if a square is attacked by a given color
// This is a simplified version for Task 7 to support king legality checking
// Full attack detection will be implemented in Task 6
// ignore_square: treat this square as empty when checking attacks (for king move legality)
bool is_square_attacked(const Chessboard& board, Square square, Color by_color, Square ignore_square = 0);

} // namespace adachess

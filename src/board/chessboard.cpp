// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#include "adachess/board/chessboard.hpp"
#include "adachess/board/moves.hpp"
#include <cstddef>

namespace adachess {

Chessboard::Chessboard() {
    // Initialize move pointers to 0
    moves_pointer.fill(0);

    // Initialize square array with empty squares
    square.fill(Piece::Empty);

    // Initialize piece lists
    pieces_list.fill(0);
    piece_table.fill(0);

    // Initialize castle rights to false
    white_castle_kingside.fill(false);
    white_castle_queenside.fill(false);
    black_castle_kingside.fill(false);
    black_castle_queenside.fill(false);

    // Initialize en passant to invalid
    en_passant.fill(INVALID_SQUARE);
}

size_t Chessboard::moves_count() const {
    return moves_count(ply);
}

size_t Chessboard::moves_count(Depth at_ply) const {
    if (at_ply >= MAX_DEPTH) {
        return 0;
    }

    // Number of moves at ply = moves_pointer[ply+1] - moves_pointer[ply]
    // If at last ply, count from current pointer to end (but we track it differently)
    if (at_ply + 1 >= MAX_DEPTH) {
        // For the last ply, we need to track the current index separately
        // For now, return 0 (will be properly implemented when move generation is added)
        return 0;
    }

    MoveStackIndex start = moves_pointer[at_ply];
    MoveStackIndex end = moves_pointer[at_ply + 1];

    return (end >= start) ? (end - start) : 0;
}

} // namespace adachess

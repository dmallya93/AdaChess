// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/square.hpp"
#include "adachess/board/moves.hpp"
#include "adachess/core/types.hpp"
#include <array>
#include <cstddef>
#include <cstdint>

namespace adachess {

// Depth types (from Task 1)
using HistoryDepth = uint16_t;
using Depth = uint16_t;

constexpr HistoryDepth MAX_HISTORY_DEPTH = 512;
constexpr Depth MAX_DEPTH = 256;
constexpr Depth ZERO_DEPTH = 1;

// Move stack types
constexpr size_t MAX_MOVE_STACK_SIZE = 4096;
using MoveStackIndex = uint16_t;

// Piece list types
constexpr size_t MAX_PIECES = 32;
using PieceIndex = uint8_t;

// Minimal Chessboard structure for Task 5
// This is a simplified version focusing on move stack management
// Full implementation will come in Task 3
class Chessboard {
public:
    // Board state (10x12 mailbox)
    std::array<Piece, 120> square{};

    // Current search depth (ply)
    Depth ply = ZERO_DEPTH;

    // Side to move
    Color side_to_move = Color::White;

    // Piece tracking
    std::array<Square, MAX_PIECES> pieces_list{};
    std::array<PieceIndex, 120> piece_table{};

    // King positions
    Square white_king_position = 0;
    Square black_king_position = 0;

    // Castle rights (simplified for now)
    std::array<bool, MAX_HISTORY_DEPTH> white_castle_kingside{};
    std::array<bool, MAX_HISTORY_DEPTH> white_castle_queenside{};
    std::array<bool, MAX_HISTORY_DEPTH> black_castle_kingside{};
    std::array<bool, MAX_HISTORY_DEPTH> black_castle_queenside{};

    // En passant history
    std::array<Square, MAX_HISTORY_DEPTH> en_passant{};

    // Move stack - stores all generated moves
    std::array<Move, MAX_MOVE_STACK_SIZE> moves_stack{};

    // Move pointers - index of first move at each ply
    std::array<MoveStackIndex, MAX_DEPTH> moves_pointer{};

    // Fifty move rule counter
    uint8_t fifty = 0;

    // History ply for move history
    HistoryDepth history_ply = 0;

    // Constructor
    Chessboard();

    // Get number of moves at current ply
    size_t moves_count() const;

    // Get number of moves at specific ply
    size_t moves_count(Depth at_ply) const;
};

} // namespace adachess

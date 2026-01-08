// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/chessboard.hpp"

namespace adachess {

// Clear_Moves_List
// Resets the move list for the current ply
// Sets Moves_Pointer[Ply+1] = Moves_Pointer[Ply] to mark empty list
void clear_moves_list(Chessboard& board);

// Generate_Moves
// Main entry point for move generation at current ply
// Generates all legal moves for the side to move
// This task implements basic move generation for pawns (without en passant/promotion),
// knights, and king (without castling). Sliding pieces will be added in Task 8.
void generate_moves(Chessboard& board);

// Helper functions for piece-specific move generation
// These are internal implementation details

// Generate pawn moves (single push, double push - no en passant/promotion yet)
void generate_pawn_moves(Chessboard& board, Square from);

// Generate knight moves (all 8 L-shaped directions)
void generate_knight_moves(Chessboard& board, Square from);

// Generate king moves (all 8 adjacent squares - no castling yet)
void generate_king_moves(Chessboard& board, Square from);

} // namespace adachess

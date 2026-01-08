// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/chessboard.hpp"
#include "adachess/board/moves.hpp"

namespace adachess {

// Register_Move procedures
// These functions create and register moves into the move stack

// Register_Move - Basic version with from/to squares
// Creates a move with default values and registers it
void register_move(Chessboard& board, Square from, Square to);

// Register_Move - Version with flag
// Creates a move with specified flag and registers it
// If flag is Promotion, generates 4 moves (one for each promotion type)
void register_move(Chessboard& board, Square from, Square to, MoveFlag flag);

// Register_Move - Version with pre-constructed move
// Validates and registers a pre-constructed move
// Performs legality check, detects check type, adds disambiguation
void register_move(Chessboard& board, const Move& move);

// Register_Tactical_Move procedures
// Similar to Register_Move but filters to only register tactical moves

// Register_Tactical_Move - Basic version with from/to squares
void register_tactical_move(Chessboard& board, Square from, Square to);

// Register_Tactical_Move - Version with flag
void register_tactical_move(Chessboard& board, Square from, Square to, MoveFlag flag);

// Register_Tactical_Move - Version with pre-constructed move
void register_tactical_move(Chessboard& board, const Move& move);

// Helper functions (stubs for now, will be implemented in future tasks)

// Check if a move leaves the king in check (legality test)
// Returns true if the move is illegal
bool move_leaves_king_in_check(const Chessboard& board, const Move& move);

// Check if a move gives check to the opponent king
// Returns the type of check
CheckType move_checks_opponent_king(const Chessboard& board, const Move& move);

// Detect if move notation requires disambiguation
// Returns the ambiguous flag for algebraic notation
AmbiguousFlag detect_ambiguous_move_notation(const Chessboard& board, const Move& move);

} // namespace adachess

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

#ifndef ADACHESS_ENGINE_MOVE_REGISTRATION_HPP
#define ADACHESS_ENGINE_MOVE_REGISTRATION_HPP

#include "adachess/board/chessboard.hpp"
#include "adachess/board/move.hpp"

namespace adachess::engine {

// ====== Register Move Functions ======

// Create a move based on the starting and destination square specified and
// set other values to their default
void register_move(board::Chessboard& chessboard, board::Square from, board::Square to);

// Create a move based on the starting and destination square specified and
// with the given flag. Set the other values to their default.
// Note that, if the flag indicates that the move represents a pawn promotion,
// the procedure will generate four moves once for each promotion kind
void register_move(board::Chessboard& chessboard, board::Square from, board::Square to,
                   board::Flag_Type flag);

// Save a move into the stack of legal moves. This function will perform the
// legality check - if needed - and add further information such as if the
// moves gives check (and which kind of check) and, in case the notation
// requires it, it will detect if a disambiguation is needed (for output
// purpose only)
void register_move(board::Chessboard& chessboard, const board::Move_Type& move);

// ====== Register Tactical Move Functions ======

// Create a move based on the starting and destination square specified and
// set other values to their default. This procedure is called from the
// special generator for tactical moves, in order to filter and register
// only tactical moves
void register_tactical_move(board::Chessboard& chessboard, board::Square from,
                             board::Square to);

// Create a move based on the starting and destination square specified and
// with the given flag. Set the other values to their default.
// Note that, if the flag indicates that the move represents a pawn promotion,
// the procedure will generate four moves once for each promotion kind.
// This procedure is called from the special generator for tactical moves
// with the purpose to filter and prevent the registration of non-tactical
// moves
void register_tactical_move(board::Chessboard& chessboard, board::Square from,
                             board::Square to, board::Flag_Type flag);

// Save a move into the stack of legal moves. This function will perform the
// legality check - if needed - and add further information such as if the
// moves gives check (and which kind of check) and, in case the notation
// requires it, it will detect if a disambiguation is needed (for output
// purpose only).
// This procedure is called from the special generator for tactical moves
// with the purpose to filter and prevent the registration of non-tactical
// moves
void register_tactical_move(board::Chessboard& chessboard, const board::Move_Type& move);

}  // namespace adachess::engine

#endif  // ADACHESS_ENGINE_MOVE_REGISTRATION_HPP

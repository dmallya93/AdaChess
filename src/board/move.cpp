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

#include "adachess/board/chessboard.hpp"
#include "adachess/board/move.hpp"

#include <cassert>

namespace adachess::board {

using adachess::core::Piece;

// ====== Register_Move Implementation ======

void register_move(Chessboard& chessboard, Square from, Square to) {
    // Create a move with default values
    Move_Type move;
    move.from = from;
    move.to = to;
    move.piece = chessboard.board[from];
    move.captured = chessboard.board[to];
    move.flag = Flag_Type::Standard_Move;
    move.promotion = Piece::Empty;
    move.check = Check_Type::No_Check;  // Will be updated by check detection
    move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;
    
    // Register the constructed move
    register_move(chessboard, move);
}

void register_move(Chessboard& chessboard, Square from, Square to, Flag_Type flag) {
    // If flag is Promotion, generate 4 moves (one for each promotion piece)
    if (flag == Flag_Type::Promotion) {
        // Generate promotion to Queen
        Move_Type move;
        move.from = from;
        move.to = to;
        move.piece = chessboard.board[from];
        move.captured = chessboard.board[to];
        move.flag = flag;
        move.promotion = (chessboard.side_to_move == adachess::core::Color::White) ?
                        Piece::White_Queen : Piece::Black_Queen;
        move.check = Check_Type::No_Check;
        move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;
        register_move(chessboard, move);
        
        // Generate promotion to Rook
        move.promotion = (chessboard.side_to_move == adachess::core::Color::White) ?
                        Piece::White_Rook : Piece::Black_Rook;
        register_move(chessboard, move);
        
        // Generate promotion to Bishop
        move.promotion = (chessboard.side_to_move == adachess::core::Color::White) ?
                        Piece::White_Bishop : Piece::Black_Bishop;
        register_move(chessboard, move);
        
        // Generate promotion to Knight
        move.promotion = (chessboard.side_to_move == adachess::core::Color::White) ?
                        Piece::White_Knight : Piece::Black_Knight;
        register_move(chessboard, move);
    } else {
        // Create a single move with the specified flag
        Move_Type move;
        move.from = from;
        move.to = to;
        move.piece = chessboard.board[from];
        move.captured = chessboard.board[to];
        move.flag = flag;
        move.promotion = Piece::Empty;
        move.check = Check_Type::No_Check;
        move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;
        register_move(chessboard, move);
    }
}

void register_move(Chessboard& chessboard, const Move_Type& move) {
    // Get the current move pointer for this ply
    const uint16_t move_index = chessboard.move_pointer[chessboard.ply];
    
    // Verify we haven't exceeded the move stack size
    assert(move_index < chessboard.moves.size() && "Move stack overflow");
    
    // Store the move in the move stack
    chessboard.moves[move_index] = move;
    
    // TODO: Add check detection logic here (will be implemented in later tasks)
    // TODO: Add disambiguation logic here (will be implemented in later tasks)
    // TODO: Add legality testing if force_validity_test is true (will be implemented in later tasks)
    
    // Increment the move pointer for the current ply
    chessboard.move_pointer[chessboard.ply]++;
}

// ====== Register_Tactical_Move Implementation ======

void register_tactical_move(Chessboard& chessboard, Square from, Square to) {
    // Create a move with default values
    Move_Type move;
    move.from = from;
    move.to = to;
    move.piece = chessboard.board[from];
    move.captured = chessboard.board[to];
    move.flag = Flag_Type::Standard_Move;
    move.promotion = Piece::Empty;
    move.check = Check_Type::No_Check;  // Will be updated by check detection
    move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;
    
    // Register if tactical
    register_tactical_move(chessboard, move);
}

void register_tactical_move(Chessboard& chessboard, Square from, Square to, Flag_Type flag) {
    // If flag is Promotion, generate 4 moves (one for each promotion piece)
    if (flag == Flag_Type::Promotion) {
        // Generate promotion to Queen
        Move_Type move;
        move.from = from;
        move.to = to;
        move.piece = chessboard.board[from];
        move.captured = chessboard.board[to];
        move.flag = flag;
        move.promotion = (chessboard.side_to_move == adachess::core::Color::White) ?
                        Piece::White_Queen : Piece::Black_Queen;
        move.check = Check_Type::No_Check;
        move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;
        register_tactical_move(chessboard, move);
        
        // Generate promotion to Rook
        move.promotion = (chessboard.side_to_move == adachess::core::Color::White) ?
                        Piece::White_Rook : Piece::Black_Rook;
        register_tactical_move(chessboard, move);
        
        // Generate promotion to Bishop
        move.promotion = (chessboard.side_to_move == adachess::core::Color::White) ?
                        Piece::White_Bishop : Piece::Black_Bishop;
        register_tactical_move(chessboard, move);
        
        // Generate promotion to Knight
        move.promotion = (chessboard.side_to_move == adachess::core::Color::White) ?
                        Piece::White_Knight : Piece::Black_Knight;
        register_tactical_move(chessboard, move);
    } else {
        // Create a single move with the specified flag
        Move_Type move;
        move.from = from;
        move.to = to;
        move.piece = chessboard.board[from];
        move.captured = chessboard.board[to];
        move.flag = flag;
        move.promotion = Piece::Empty;
        move.check = Check_Type::No_Check;
        move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;
        register_tactical_move(chessboard, move);
    }
}

void register_tactical_move(Chessboard& chessboard, const Move_Type& move) {
    // Only register if the move is tactical
    if (move_is_tactical(move)) {
        register_move(chessboard, move);
    }
}

}  // namespace adachess::board

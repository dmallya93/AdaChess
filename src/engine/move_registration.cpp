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

#include "adachess/engine/move_registration.hpp"
#include <cassert>

namespace adachess::engine {

using namespace adachess::board;
using adachess::core::Piece;

// Helper function to append a move to the moves stack
static void append_move_to_stack(Chessboard& chessboard, const Move_Type& move) {
    // Get current move pointer for this ply
    const uint16_t move_index = chessboard.move_pointer[chessboard.ply];

    // Ensure we don't overflow the moves stack
    assert(move_index < chessboard.moves.size() && "Moves stack overflow");

    // Add move to stack
    chessboard.moves[move_index] = move;

    // Increment move pointer for this ply
    chessboard.move_pointer[chessboard.ply]++;
}

// ====== Register Move Functions ======

void register_move(Chessboard& chessboard, Square from, Square to) {
    // Create move with default values
    Move_Type move;
    move.from = from;
    move.to = to;
    move.piece = chessboard.board[from];
    move.captured = chessboard.board[to];
    move.flag = Flag_Type::Standard_Move;
    move.promotion = Piece::Empty;
    move.check = Check_Type::No_Check;  // Will be set by check detection (future task)
    move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;  // Will be set by disambiguation (future task)

    append_move_to_stack(chessboard, move);
}

void register_move(Chessboard& chessboard, Square from, Square to, Flag_Type flag) {
    // If this is a promotion, generate 4 moves (one for each promotion piece)
    if (flag == Flag_Type::Promotion) {
        const Piece piece = chessboard.board[from];
        const Piece captured = chessboard.board[to];

        // Determine promotion pieces based on color
        const bool is_white = core::is_white_piece(piece);
        const Piece queen = is_white ? Piece::White_Queen : Piece::Black_Queen;
        const Piece rook = is_white ? Piece::White_Rook : Piece::Black_Rook;
        const Piece bishop = is_white ? Piece::White_Bishop : Piece::Black_Bishop;
        const Piece knight = is_white ? Piece::White_Knight : Piece::Black_Knight;

        // Generate promotion to Queen
        Move_Type move;
        move.from = from;
        move.to = to;
        move.piece = piece;
        move.captured = captured;
        move.flag = Flag_Type::Promotion;
        move.promotion = queen;
        move.check = Check_Type::No_Check;
        move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;
        append_move_to_stack(chessboard, move);

        // Generate promotion to Rook
        move.promotion = rook;
        append_move_to_stack(chessboard, move);

        // Generate promotion to Bishop
        move.promotion = bishop;
        append_move_to_stack(chessboard, move);

        // Generate promotion to Knight
        move.promotion = knight;
        append_move_to_stack(chessboard, move);
    } else {
        // Non-promotion move
        Move_Type move;
        move.from = from;
        move.to = to;
        move.piece = chessboard.board[from];
        move.captured = chessboard.board[to];
        move.flag = flag;
        move.promotion = Piece::Empty;
        move.check = Check_Type::No_Check;
        move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;

        append_move_to_stack(chessboard, move);
    }
}

void register_move(Chessboard& chessboard, const Move_Type& move) {
    // For now, just append the pre-constructed move
    // Future tasks will add:
    // - Legality check (if Force_Validity_Test is true)
    // - Check detection
    // - Disambiguation flag
    append_move_to_stack(chessboard, move);
}

// ====== Register Tactical Move Functions ======

void register_tactical_move(Chessboard& chessboard, Square from, Square to) {
    // Create move with default values
    Move_Type move;
    move.from = from;
    move.to = to;
    move.piece = chessboard.board[from];
    move.captured = chessboard.board[to];
    move.flag = Flag_Type::Standard_Move;
    move.promotion = Piece::Empty;
    move.check = Check_Type::No_Check;  // Will be set by check detection (future task)
    move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;

    // Only register if it's tactical
    // Note: Since check detection is not yet implemented, we can only filter
    // based on captures for now
    if (move_is_tactical(move)) {
        append_move_to_stack(chessboard, move);
    }
}

void register_tactical_move(Chessboard& chessboard, Square from, Square to,
                             Flag_Type flag) {
    // If this is a promotion, generate 4 moves (one for each promotion piece)
    if (flag == Flag_Type::Promotion) {
        const Piece piece = chessboard.board[from];
        const Piece captured = chessboard.board[to];

        // Determine promotion pieces based on color
        const bool is_white = core::is_white_piece(piece);
        const Piece queen = is_white ? Piece::White_Queen : Piece::Black_Queen;
        const Piece rook = is_white ? Piece::White_Rook : Piece::Black_Rook;
        const Piece bishop = is_white ? Piece::White_Bishop : Piece::Black_Bishop;
        const Piece knight = is_white ? Piece::White_Knight : Piece::Black_Knight;

        // Generate promotion to Queen (always tactical)
        Move_Type move;
        move.from = from;
        move.to = to;
        move.piece = piece;
        move.captured = captured;
        move.flag = Flag_Type::Promotion;
        move.promotion = queen;
        move.check = Check_Type::No_Check;
        move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;
        append_move_to_stack(chessboard, move);

        // Generate promotion to Rook
        move.promotion = rook;
        append_move_to_stack(chessboard, move);

        // Generate promotion to Bishop
        move.promotion = bishop;
        append_move_to_stack(chessboard, move);

        // Generate promotion to Knight
        move.promotion = knight;
        append_move_to_stack(chessboard, move);
    } else {
        // Non-promotion move
        Move_Type move;
        move.from = from;
        move.to = to;
        move.piece = chessboard.board[from];
        move.captured = chessboard.board[to];
        move.flag = flag;
        move.promotion = Piece::Empty;
        move.check = Check_Type::No_Check;
        move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_None;

        // Only register if it's tactical
        if (move_is_tactical(move)) {
            append_move_to_stack(chessboard, move);
        }
    }
}

void register_tactical_move(Chessboard& chessboard, const Move_Type& move) {
    // Only register if the move is tactical
    if (move_is_tactical(move)) {
        append_move_to_stack(chessboard, move);
    }
}

}  // namespace adachess::engine

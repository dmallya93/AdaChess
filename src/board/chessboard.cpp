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

#include <cassert>

namespace adachess::board {

// ====== Piece Management Operations ======

void add_white_piece(Chessboard& chessboard, Square square) {
    // Verify we haven't exceeded the maximum number of white pieces
    assert(chessboard.white_pieces_counter < 16 &&
           "White pieces counter must not exceed 16");

    // Increment counter (ranges from 0 to 15, so after increment: 1 to 16)
    chessboard.white_pieces_counter++;

    // Store square in piece list at the counter index (1-based indexing)
    chessboard.pieces_list[chessboard.white_pieces_counter] = square;

    // Update reverse lookup: piece_table[square] = index in piece list
    chessboard.piece_table[square] = chessboard.white_pieces_counter;
}

void add_black_piece(Chessboard& chessboard, Square square) {
    // Verify we haven't exceeded the maximum number of black pieces
    assert(chessboard.black_pieces_counter < 16 &&
           "Black pieces counter must not exceed 16");

    // Increment counter (ranges from 0 to 15)
    chessboard.black_pieces_counter++;

    // Black pieces occupy indices 17-32 in the piece list
    // Index = BLACK_PIECES_OFFSET (16) + counter (1-16) = 17-32
    const uint8_t index = BLACK_PIECES_OFFSET + chessboard.black_pieces_counter;

    // Store square in piece list
    chessboard.pieces_list[index] = square;

    // Update reverse lookup
    chessboard.piece_table[square] = index;
}

void delete_white_piece(Chessboard& chessboard, Square square) {
    // Get the index of the piece to delete from piece_table
    const uint8_t index = chessboard.piece_table[square];
    assert(index > 0 && index <= WHITE_PIECES_END &&
           "Index must be valid white piece index");

    // Use swap-and-pop: move the last piece in the list to the deleted position
    // This maintains list compactness in O(1) time
    const Square last_square =
        chessboard.pieces_list[chessboard.white_pieces_counter];

    // Move last piece to deleted position
    chessboard.pieces_list[index] = last_square;

    // Clear the last position
    chessboard.pieces_list[chessboard.white_pieces_counter] = 0;

    // Decrement counter
    chessboard.white_pieces_counter--;

    // Update piece_table for the deleted square
    chessboard.piece_table[square] = 0;

    // Update piece_table for the moved piece (if we moved a piece)
    if (last_square != 0) {
        chessboard.piece_table[last_square] = index;
    }
}

void delete_black_piece(Chessboard& chessboard, Square square) {
    // Get the index of the piece to delete from piece_table
    const uint8_t index = chessboard.piece_table[square];
    assert(index >= BLACK_PIECES_START && index <= BLACK_PIECES_END &&
           "Index must be valid black piece index");

    // Last black piece index = BLACK_PIECES_OFFSET + counter
    const uint8_t last_index =
        BLACK_PIECES_OFFSET + chessboard.black_pieces_counter;

    // Get the square of the last black piece
    const Square last_square = chessboard.pieces_list[last_index];

    // Move last piece to deleted position
    chessboard.pieces_list[index] = last_square;

    // Clear the last position
    chessboard.pieces_list[last_index] = 0;

    // Decrement counter
    chessboard.black_pieces_counter--;

    // Update piece_table for the deleted square
    chessboard.piece_table[square] = 0;

    // Update piece_table for the moved piece (if we moved a piece)
    if (last_square != 0) {
        chessboard.piece_table[last_square] = index;
    }
}

void update_white_piece(Chessboard& chessboard, Square from, Square to) {
    // Get the piece index from the source square
    const uint8_t index = chessboard.piece_table[from];
    assert(index > 0 && index <= WHITE_PIECES_END &&
           "Invalid white piece index");

    // Update piece list: piece now at 'to' square
    chessboard.pieces_list[index] = to;

    // Clear old square in piece_table
    chessboard.piece_table[from] = 0;

    // Set new square in piece_table
    chessboard.piece_table[to] = index;
}

void update_black_piece(Chessboard& chessboard, Square from, Square to) {
    // Get the piece index from the source square
    const uint8_t index = chessboard.piece_table[from];
    assert(index >= BLACK_PIECES_START && index <= BLACK_PIECES_END &&
           "Invalid black piece index");

    // Update piece list: piece now at 'to' square
    chessboard.pieces_list[index] = to;

    // Clear old square in piece_table
    chessboard.piece_table[from] = 0;

    // Set new square in piece_table
    chessboard.piece_table[to] = index;
}

// ====== Initialization and Reset ======

void reset(Chessboard& chessboard) {
    // Set side to move to White
    chessboard.side_to_move = Color::White;

    // Initialize board to empty with frame
    // Rows 0-1 and 10-11: Frame
    // Rows 2-9: Playable area with Frame on columns 0 and 9
    chessboard.square = {
        // Row 0 (all frame)
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        // Row 1 (all frame)
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        // Row 2 (Rank 8)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 3 (Rank 7)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 4 (Rank 6)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 5 (Rank 5)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 6 (Rank 4)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 7 (Rank 3)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 8 (Rank 2)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 9 (Rank 1)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 10 (all frame)
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        // Row 11 (all frame)
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame
    };

    // Clear piece tracking structures
    chessboard.pieces_list.fill(0);
    chessboard.piece_table.fill(0);

    // Reset piece counters
    chessboard.white_pieces_counter = 0;
    chessboard.black_pieces_counter = 0;

    // Reset king positions
    chessboard.white_king_position = 0;
    chessboard.black_king_position = 0;

    // Clear castle rights
    chessboard.white_castle_queenside.fill(false);
    chessboard.white_castle_kingside.fill(false);
    chessboard.black_castle_queenside.fill(false);
    chessboard.black_castle_kingside.fill(false);

    // Clear en passant history
    chessboard.en_passant.fill(0);

    // Reset move stack
    chessboard.moves_pointer.fill(1);  // All plies start at index 1
    // Note: moves_stack doesn't need clearing as it's accessed via moves_pointer

    // Reset ply counters
    chessboard.ply = MIN_DEPTH;
    chessboard.history_ply = 0;

    // Clear move history
    // Note: moves_history doesn't need clearing as it's accessed via history_ply

    // Reset fifty-move counter
    chessboard.fifty = 0;

    // Reset validity test flag
    chessboard.force_validity_test = false;
}

void initialize(Chessboard& chessboard) {
    // First reset to empty state
    reset(chessboard);

    // Set up the starting position
    chessboard.square = {
        // Row 0 (all frame)
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        // Row 1 (all frame)
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        // Row 2 (Rank 8 - Black pieces)
        Piece::Frame, Piece::BlackRook, Piece::BlackKnight, Piece::BlackBishop,
        Piece::BlackQueen, Piece::BlackKing, Piece::BlackBishop,
        Piece::BlackKnight, Piece::BlackRook, Piece::Frame,
        // Row 3 (Rank 7 - Black pawns)
        Piece::Frame, Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn,
        Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn, Piece::BlackPawn,
        Piece::BlackPawn, Piece::Frame,
        // Row 4 (Rank 6 - Empty)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 5 (Rank 5 - Empty)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 6 (Rank 4 - Empty)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 7 (Rank 3 - Empty)
        Piece::Frame, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty,
        Piece::Empty, Piece::Empty, Piece::Empty, Piece::Empty, Piece::Frame,
        // Row 8 (Rank 2 - White pawns)
        Piece::Frame, Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn,
        Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn, Piece::WhitePawn,
        Piece::WhitePawn, Piece::Frame,
        // Row 9 (Rank 1 - White pieces)
        Piece::Frame, Piece::WhiteRook, Piece::WhiteKnight, Piece::WhiteBishop,
        Piece::WhiteQueen, Piece::WhiteKing, Piece::WhiteBishop,
        Piece::WhiteKnight, Piece::WhiteRook, Piece::Frame,
        // Row 10 (all frame)
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        // Row 11 (all frame)
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame,
        Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame, Piece::Frame
    };

    // Initialize white pieces (back rank: A1-H1, pawns: A2-H2)
    add_white_piece(chessboard, A1);  // Rook
    add_white_piece(chessboard, B1);  // Knight
    add_white_piece(chessboard, C1);  // Bishop
    add_white_piece(chessboard, D1);  // Queen
    add_white_piece(chessboard, E1);  // King
    add_white_piece(chessboard, F1);  // Bishop
    add_white_piece(chessboard, G1);  // Knight
    add_white_piece(chessboard, H1);  // Rook
    add_white_piece(chessboard, A2);  // Pawn
    add_white_piece(chessboard, B2);  // Pawn
    add_white_piece(chessboard, C2);  // Pawn
    add_white_piece(chessboard, D2);  // Pawn
    add_white_piece(chessboard, E2);  // Pawn
    add_white_piece(chessboard, F2);  // Pawn
    add_white_piece(chessboard, G2);  // Pawn
    add_white_piece(chessboard, H2);  // Pawn

    // Initialize black pieces (back rank: A8-H8, pawns: A7-H7)
    add_black_piece(chessboard, A8);  // Rook
    add_black_piece(chessboard, B8);  // Knight
    add_black_piece(chessboard, C8);  // Bishop
    add_black_piece(chessboard, D8);  // Queen
    add_black_piece(chessboard, E8);  // King
    add_black_piece(chessboard, F8);  // Bishop
    add_black_piece(chessboard, G8);  // Knight
    add_black_piece(chessboard, H8);  // Rook
    add_black_piece(chessboard, A7);  // Pawn
    add_black_piece(chessboard, B7);  // Pawn
    add_black_piece(chessboard, C7);  // Pawn
    add_black_piece(chessboard, D7);  // Pawn
    add_black_piece(chessboard, E7);  // Pawn
    add_black_piece(chessboard, F7);  // Pawn
    add_black_piece(chessboard, G7);  // Pawn
    add_black_piece(chessboard, H7);  // Pawn

    // Set king positions
    chessboard.white_king_position = E1;
    chessboard.black_king_position = E8;

    // Enable all castle rights
    chessboard.white_castle_queenside.fill(true);
    chessboard.white_castle_kingside.fill(true);
    chessboard.black_castle_queenside.fill(true);
    chessboard.black_castle_kingside.fill(true);

    // Set side to move to White
    chessboard.side_to_move = Color::White;

    // Note: Zobrist hash initialization will be added in Task 4
}

}  // namespace adachess::board

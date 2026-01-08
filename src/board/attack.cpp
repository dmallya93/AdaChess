// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#include "adachess/board/attack.hpp"
#include "adachess/board/directions.hpp"

namespace adachess {

// Helper to check if a piece can attack a square
static bool piece_attacks_square(const Chessboard& board, Square from, Square target, Piece piece, Square ignore_square) {
    // Calculate the offset
    int offset = target - from;

    // Check based on piece type
    switch (piece) {
        case Piece::White_Pawn:
            // White pawns attack diagonally upward (northwest, northeast)
            return offset == NORTH_WEST || offset == NORTH_EAST;

        case Piece::Black_Pawn:
            // Black pawns attack diagonally downward (southwest, southeast)
            return offset == SOUTH_WEST || offset == SOUTH_EAST;

        case Piece::White_Knight:
        case Piece::Black_Knight:
            // Check all 8 knight offsets
            for (Direction knight_offset : KNIGHT_OFFSETS) {
                if (offset == knight_offset) {
                    return true;
                }
            }
            return false;

        case Piece::White_King:
        case Piece::Black_King:
            // Check all 8 king offsets
            for (Direction king_offset : KING_OFFSETS) {
                if (offset == king_offset) {
                    return true;
                }
            }
            return false;

        case Piece::White_Bishop:
        case Piece::Black_Bishop:
            // Check if target is on a diagonal
            for (Direction bishop_offset : BISHOP_OFFSETS) {
                Square current = from;
                while (true) {
                    current = static_cast<Square>(current + bishop_offset);
                    if (board.square[current] == Piece::Frame) {
                        break;
                    }
                    if (current == target) {
                        return true;
                    }
                    if (current != ignore_square && board.square[current] != Piece::Empty) {
                        break; // Blocked by another piece
                    }
                }
            }
            return false;

        case Piece::White_Rook:
        case Piece::Black_Rook:
            // Check if target is on a rank or file
            for (Direction rook_offset : ROOK_OFFSETS) {
                Square current = from;
                while (true) {
                    current = static_cast<Square>(current + rook_offset);
                    if (board.square[current] == Piece::Frame) {
                        break;
                    }
                    if (current == target) {
                        return true;
                    }
                    if (current != ignore_square && board.square[current] != Piece::Empty) {
                        break; // Blocked by another piece
                    }
                }
            }
            return false;

        case Piece::White_Queen:
        case Piece::Black_Queen:
            // Queen combines rook and bishop
            for (Direction queen_offset : QUEEN_OFFSETS) {
                Square current = from;
                while (true) {
                    current = static_cast<Square>(current + queen_offset);
                    if (board.square[current] == Piece::Frame) {
                        break;
                    }
                    if (current == target) {
                        return true;
                    }
                    if (current != ignore_square && board.square[current] != Piece::Empty) {
                        break; // Blocked by another piece
                    }
                }
            }
            return false;

        default:
            return false;
    }
}

// Check if a square is attacked by a given color
bool is_square_attacked(const Chessboard& board, Square square, Color by_color, Square ignore_square) {
    // Iterate through all pieces of the attacking color
    size_t start_index, end_index;
    if (by_color == Color::White) {
        start_index = 1;   // White pieces: 1-16
        end_index = 16;
    } else {
        start_index = 17;  // Black pieces: 17-32
        end_index = 32;
    }

    for (size_t i = start_index; i <= end_index; ++i) {
        Square from = board.pieces_list[i];

        // Piece list is terminated by 0
        if (from == 0) {
            break;
        }

        Piece piece = board.square[from];

        // Check if this piece attacks the target square
        if (piece_attacks_square(board, from, square, piece, ignore_square)) {
            return true;
        }
    }

    return false;
}

} // namespace adachess

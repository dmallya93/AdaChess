// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#include "adachess/engine/move_generation.hpp"
#include "adachess/engine/move_registration.hpp"
#include "adachess/board/directions.hpp"
#include "adachess/board/square.hpp"

namespace adachess {

// Piece list ranges
constexpr size_t WHITE_PIECES_START = 1;
constexpr size_t WHITE_PIECES_END = 16;
constexpr size_t BLACK_PIECES_START = 17;
constexpr size_t BLACK_PIECES_END = 32;

// Rank constants for pawn double push logic
// A2-H2 for white: squares 81-88 in 10x12 mailbox
// A7-H7 for black: squares 31-38 in 10x12 mailbox
constexpr Square WHITE_PAWN_RANK_2_START = 81;
constexpr Square WHITE_PAWN_RANK_2_END = 88;
constexpr Square BLACK_PAWN_RANK_7_START = 31;
constexpr Square BLACK_PAWN_RANK_7_END = 38;

// Rank 8 for white promotion: A8-H8 (21-28)
// Rank 1 for black promotion: A1-H1 (91-98)
constexpr Square WHITE_PAWN_RANK_8_START = 21;
constexpr Square WHITE_PAWN_RANK_8_END = 28;
constexpr Square BLACK_PAWN_RANK_1_START = 91;
constexpr Square BLACK_PAWN_RANK_1_END = 98;

// Clear_Moves_List
// Resets the move list for the current ply
void clear_moves_list(Chessboard& board) {
    // Set moves_pointer[ply+1] = moves_pointer[ply] to mark empty list
    board.moves_pointer[board.ply + 1] = board.moves_pointer[board.ply];
}

// Generate pawn moves (single push, double push - no en passant/promotion yet)
void generate_pawn_moves(Chessboard& board, Square from) {
    Piece piece = board.square[from];

    if (piece == Piece::White_Pawn) {
        // White pawn moves north
        Square target = from + NORTH;

        // Single push
        if (board.square[target] == Piece::Empty) {
            // Check for promotion (reaching rank 8)
            if (target >= WHITE_PAWN_RANK_8_START && target <= WHITE_PAWN_RANK_8_END) {
                // Promotion - not implemented in this task
                // register_move(board, from, target, MoveFlag::Promotion);
            } else {
                register_move(board, from, target);

                // Double push from rank 2
                if (from >= WHITE_PAWN_RANK_2_START && from <= WHITE_PAWN_RANK_2_END) {
                    Square double_target = target + NORTH;
                    if (board.square[double_target] == Piece::Empty) {
                        register_move(board, from, double_target, MoveFlag::PawnMoveTwoSquare);
                    }
                }
            }
        }

        // Captures are not implemented in this task (Task 8 or later)

    } else if (piece == Piece::Black_Pawn) {
        // Black pawn moves south
        Square target = from + SOUTH;

        // Single push
        if (board.square[target] == Piece::Empty) {
            // Check for promotion (reaching rank 1)
            if (target >= BLACK_PAWN_RANK_1_START && target <= BLACK_PAWN_RANK_1_END) {
                // Promotion - not implemented in this task
                // register_move(board, from, target, MoveFlag::Promotion);
            } else {
                register_move(board, from, target);

                // Double push from rank 7
                if (from >= BLACK_PAWN_RANK_7_START && from <= BLACK_PAWN_RANK_7_END) {
                    Square double_target = target + SOUTH;
                    if (board.square[double_target] == Piece::Empty) {
                        register_move(board, from, double_target, MoveFlag::PawnMoveTwoSquare);
                    }
                }
            }
        }

        // Captures are not implemented in this task (Task 8 or later)
    }
}

// Generate knight moves (all 8 L-shaped directions)
void generate_knight_moves(Chessboard& board, Square from) {
    Color side = board.side_to_move;

    // Iterate through all 8 knight offsets
    for (Direction offset : KNIGHT_OFFSETS) {
        Square target = static_cast<Square>(from + offset);
        Piece target_piece = board.square[target];

        // Skip frame squares
        if (target_piece == Piece::Frame) {
            continue;
        }

        // Skip friendly pieces
        if (is_piece(target_piece) && piece_color(target_piece) == side) {
            continue;
        }

        // Valid move: either empty square or enemy piece
        register_move(board, from, target);
    }
}

// Generate king moves (all 8 adjacent squares - no castling yet)
void generate_king_moves(Chessboard& board, Square from) {
    Color side = board.side_to_move;

    // Iterate through all 8 king offsets
    for (Direction offset : KING_OFFSETS) {
        Square target = static_cast<Square>(from + offset);
        Piece target_piece = board.square[target];

        // Skip frame squares
        if (target_piece == Piece::Frame) {
            continue;
        }

        // Skip friendly pieces
        if (is_piece(target_piece) && piece_color(target_piece) == side) {
            continue;
        }

        // Valid move: either empty square or enemy piece
        // King moves require legality checking (will be enforced by register_move)
        register_move(board, from, target);
    }
}

// Generate_Moves
// Main entry point for move generation at current ply
void generate_moves(Chessboard& board) {
    // Clear the move list for this ply
    clear_moves_list(board);

    // Determine which piece list to iterate based on side to move
    size_t start_index, end_index;
    if (board.side_to_move == Color::White) {
        start_index = WHITE_PIECES_START;
        end_index = WHITE_PIECES_END;
    } else {
        start_index = BLACK_PIECES_START;
        end_index = BLACK_PIECES_END;
    }

    // Iterate through the piece list for the side to move
    for (size_t i = start_index; i <= end_index; ++i) {
        Square square = board.pieces_list[i];

        // Piece list is terminated by 0
        if (square == 0) {
            break;
        }

        Piece piece = board.square[square];

        // Generate moves based on piece type
        switch (piece) {
            case Piece::White_Pawn:
            case Piece::Black_Pawn:
                generate_pawn_moves(board, square);
                break;

            case Piece::White_Knight:
            case Piece::Black_Knight:
                generate_knight_moves(board, square);
                break;

            case Piece::White_King:
            case Piece::Black_King:
                generate_king_moves(board, square);
                break;

            // Sliding pieces (Bishop, Rook, Queen) will be implemented in Task 8
            case Piece::White_Bishop:
            case Piece::Black_Bishop:
            case Piece::White_Rook:
            case Piece::Black_Rook:
            case Piece::White_Queen:
            case Piece::Black_Queen:
                // Not implemented yet - will be added in Task 8
                break;

            default:
                // Empty or Frame should not be in piece list
                break;
        }
    }
}

} // namespace adachess

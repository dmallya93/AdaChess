// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#include "adachess/board/chessboard.hpp"
#include "adachess/board/moves.hpp"
#include "adachess/board/attack.hpp"
#include "adachess/core/hash.hpp"
#include "adachess/board/directions.hpp"
#include <cstddef>
#include <cassert>

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

// Piece tracking constants
// Ada uses 1-based indexing: White pieces are indices 1-16, Black pieces are 17-32
constexpr PieceIndex WHITE_PIECES_START = 1;
constexpr PieceIndex BLACK_PIECES_START = 17;
constexpr uint8_t MAX_PIECES_PER_SIDE = 16;

// Add a white piece to the piece list and piece table
void Chessboard::add_white_piece(Square sq) {
    assert(white_pieces_counter < MAX_PIECES_PER_SIDE && "White pieces counter exceeded");
    white_pieces_counter++;
    PieceIndex index = WHITE_PIECES_START + white_pieces_counter - 1;
    pieces_list[index] = sq;
    piece_table[sq] = index;
}

// Add a black piece to the piece list and piece table
void Chessboard::add_black_piece(Square sq) {
    assert(black_pieces_counter < MAX_PIECES_PER_SIDE && "Black pieces counter exceeded");
    black_pieces_counter++;
    PieceIndex index = BLACK_PIECES_START + black_pieces_counter - 1;
    pieces_list[index] = sq;
    piece_table[sq] = index;
}

// Delete a white piece from the piece list and piece table
void Chessboard::delete_white_piece(Square sq) {
    PieceIndex index = piece_table[sq];
    assert(index >= WHITE_PIECES_START && index < BLACK_PIECES_START && "Invalid white piece index");

    // Move the last white piece to this slot (swap and pop pattern)
    PieceIndex last_index = WHITE_PIECES_START + white_pieces_counter - 1;
    Square last_square = pieces_list[last_index];

    pieces_list[index] = last_square;
    pieces_list[last_index] = 0;
    white_pieces_counter--;

    piece_table[sq] = 0;
    piece_table[last_square] = index;
}

// Delete a black piece from the piece list and piece table
void Chessboard::delete_black_piece(Square sq) {
    PieceIndex index = piece_table[sq];
    assert(index >= BLACK_PIECES_START && "Invalid black piece index");

    // Move the last black piece to this slot (swap and pop pattern)
    PieceIndex last_index = BLACK_PIECES_START + black_pieces_counter - 1;
    Square last_square = pieces_list[last_index];

    pieces_list[index] = last_square;
    pieces_list[last_index] = 0;
    black_pieces_counter--;

    piece_table[sq] = 0;
    piece_table[last_square] = index;
}

// Update a white piece position in the piece list and piece table
void Chessboard::update_white_piece(Square from, Square to) {
    PieceIndex index = piece_table[from];
    assert(index >= WHITE_PIECES_START && index < BLACK_PIECES_START && "Invalid white piece index");

    piece_table[from] = 0;
    piece_table[to] = index;
    pieces_list[index] = to;
}

// Update a black piece position in the piece list and piece table
void Chessboard::update_black_piece(Square from, Square to) {
    PieceIndex index = piece_table[from];
    assert(index >= BLACK_PIECES_START && "Invalid black piece index");

    piece_table[from] = 0;
    piece_table[to] = index;
    pieces_list[index] = to;
}

// Recompute the full hash from scratch
// This is used after loading a position or for validation
void Chessboard::update_hash() {
    hash = 0;

    // Hash all white pieces
    for (uint8_t i = 0; i < white_pieces_counter; ++i) {
        Square sq = pieces_list[WHITE_PIECES_START + i];
        if (sq == 0) break;
        Piece piece = square[sq];
        hash ^= zobrist::hash_pieces[static_cast<size_t>(piece)][sq];
    }

    // Hash all black pieces
    for (uint8_t i = 0; i < black_pieces_counter; ++i) {
        Square sq = pieces_list[BLACK_PIECES_START + i];
        if (sq == 0) break;
        Piece piece = square[sq];
        hash ^= zobrist::hash_pieces[static_cast<size_t>(piece)][sq];
    }

    // Hash en passant square if valid
    if (en_passant[history_ply] != INVALID_SQUARE) {
        hash ^= zobrist::hash_en_passant[en_passant[history_ply]];
    }

    // Hash castle rights
    if (white_castle_kingside[history_ply]) {
        hash ^= zobrist::hash_castle[4]; // White kingside
    }
    if (white_castle_queenside[history_ply]) {
        hash ^= zobrist::hash_castle[3]; // White queenside
    }
    if (black_castle_kingside[history_ply]) {
        hash ^= zobrist::hash_castle[2]; // Black kingside
    }
    if (black_castle_queenside[history_ply]) {
        hash ^= zobrist::hash_castle[1]; // Black queenside
    }

    // Hash side to move
    if (side_to_move == Color::Black) {
        hash ^= zobrist::hash_side;
    }
}

// Play a move on the board
void Chessboard::play(Move& move) {
    Square from = move.from;
    Square to = move.to;
    Piece piece = move.piece;
    Piece captured = move.captured;
    MoveFlag flag = move.flag;
    Piece promotion = move.promotion;

    // Store data for undo
    HistoryMove history_data(move, hash, fifty);

    // Initialize next ply's state by copying from current
    en_passant[history_ply + 1] = INVALID_SQUARE;
    fifty = fifty + 1;
    white_castle_kingside[history_ply + 1] = white_castle_kingside[history_ply];
    white_castle_queenside[history_ply + 1] = white_castle_queenside[history_ply];
    black_castle_kingside[history_ply + 1] = black_castle_kingside[history_ply];
    black_castle_queenside[history_ply + 1] = black_castle_queenside[history_ply];

    // Handle castling moves (move the rook)
    if (flag == MoveFlag::Castle) {
        // Based on the king's destination, move the corresponding rook
        if (to == G1) { // White kingside
            update_white_piece(H1, F1);
            square[F1] = square[H1];
            square[H1] = Piece::Empty;
            white_castle_kingside[history_ply + 1] = false;
        } else if (to == C1) { // White queenside
            update_white_piece(A1, D1);
            square[D1] = square[A1];
            square[A1] = Piece::Empty;
            white_castle_queenside[history_ply + 1] = false;
        } else if (to == G8) { // Black kingside
            update_black_piece(H8, F8);
            square[F8] = square[H8];
            square[H8] = Piece::Empty;
            black_castle_kingside[history_ply + 1] = false;
        } else if (to == C8) { // Black queenside
            update_black_piece(A8, D8);
            square[D8] = square[A8];
            square[A8] = Piece::Empty;
            black_castle_queenside[history_ply + 1] = false;
        }
    }

    // Detect en passant square for pawn double moves
    if (flag == MoveFlag::PawnMoveTwoSquare) {
        if (side_to_move == Color::White) {
            en_passant[history_ply + 1] = to + SOUTH;
        } else {
            en_passant[history_ply + 1] = to + NORTH;
        }
    }

    // Remove captured piece
    if (captured != Piece::Empty) {
        if (side_to_move == Color::White) {
            delete_black_piece(to);
        } else {
            delete_white_piece(to);
        }
    } else if (flag == MoveFlag::CaptureEnPassant) {
        // En passant capture - remove the pawn from behind the destination square
        fifty = 0;
        if (side_to_move == Color::White) {
            delete_black_piece(to + SOUTH);
            square[to + SOUTH] = Piece::Empty;
        } else {
            delete_white_piece(to + NORTH);
            square[to + NORTH] = Piece::Empty;
        }
    }

    // Move the piece on the board
    square[to] = square[from];
    square[from] = Piece::Empty;

    // Update piece tracking
    if (side_to_move == Color::White) {
        update_white_piece(from, to);

        // Update king position
        if (square[to] == Piece::White_King) {
            white_king_position = to;
            white_castle_kingside[history_ply + 1] = false;
            white_castle_queenside[history_ply + 1] = false;
        }

        // Handle promotion
        if (flag == MoveFlag::Promotion) {
            // If promoting captures a piece (already handled above), just change the piece
            square[to] = promotion;
        }
    } else {
        update_black_piece(from, to);

        // Update king position
        if (square[to] == Piece::Black_King) {
            black_king_position = to;
            black_castle_kingside[history_ply + 1] = false;
            black_castle_queenside[history_ply + 1] = false;
        }

        // Handle promotion
        if (flag == MoveFlag::Promotion) {
            square[to] = promotion;
        }
    }

    // Reset fifty-move counter for pawn moves or captures
    if ((piece == Piece::White_Pawn || piece == Piece::Black_Pawn) || captured != Piece::Empty) {
        fifty = 0;
    }

    // Revoke castle rights based on rook or king moves
    if (from == A1 || to == A1) {
        white_castle_queenside[history_ply + 1] = false;
    }
    if (from == H1 || to == H1) {
        white_castle_kingside[history_ply + 1] = false;
    }
    if (from == A8 || to == A8) {
        black_castle_queenside[history_ply + 1] = false;
    }
    if (from == H8 || to == H8) {
        black_castle_kingside[history_ply + 1] = false;
    }

    // Update hash incrementally
    // XOR out old en passant square (if any)
    if (en_passant[history_ply] != INVALID_SQUARE) {
        hash ^= zobrist::hash_en_passant[en_passant[history_ply]];
    }

    // XOR out old castle rights
    if (white_castle_kingside[history_ply]) {
        hash ^= zobrist::hash_castle[4];
    }
    if (white_castle_queenside[history_ply]) {
        hash ^= zobrist::hash_castle[3];
    }
    if (black_castle_kingside[history_ply]) {
        hash ^= zobrist::hash_castle[2];
    }
    if (black_castle_queenside[history_ply]) {
        hash ^= zobrist::hash_castle[1];
    }

    // XOR out piece at from square
    hash ^= zobrist::hash_pieces[static_cast<size_t>(piece)][from];

    // XOR out captured piece (if any)
    if (captured != Piece::Empty) {
        hash ^= zobrist::hash_pieces[static_cast<size_t>(captured)][to];
    }

    // Handle en passant capture hash
    if (flag == MoveFlag::CaptureEnPassant) {
        Piece captured_pawn = (side_to_move == Color::White) ? Piece::Black_Pawn : Piece::White_Pawn;
        Square captured_sq = (side_to_move == Color::White) ? to + SOUTH : to + NORTH;
        hash ^= zobrist::hash_pieces[static_cast<size_t>(captured_pawn)][captured_sq];
    }

    // XOR in piece at to square (or promotion piece)
    Piece final_piece = (flag == MoveFlag::Promotion) ? promotion : piece;
    hash ^= zobrist::hash_pieces[static_cast<size_t>(final_piece)][to];

    // Handle castling rook moves in hash
    if (flag == MoveFlag::Castle) {
        if (to == G1) {
            hash ^= zobrist::hash_pieces[static_cast<size_t>(Piece::White_Rook)][H1];
            hash ^= zobrist::hash_pieces[static_cast<size_t>(Piece::White_Rook)][F1];
        } else if (to == C1) {
            hash ^= zobrist::hash_pieces[static_cast<size_t>(Piece::White_Rook)][A1];
            hash ^= zobrist::hash_pieces[static_cast<size_t>(Piece::White_Rook)][D1];
        } else if (to == G8) {
            hash ^= zobrist::hash_pieces[static_cast<size_t>(Piece::Black_Rook)][H8];
            hash ^= zobrist::hash_pieces[static_cast<size_t>(Piece::Black_Rook)][F8];
        } else if (to == C8) {
            hash ^= zobrist::hash_pieces[static_cast<size_t>(Piece::Black_Rook)][A8];
            hash ^= zobrist::hash_pieces[static_cast<size_t>(Piece::Black_Rook)][D8];
        }
    }

    // XOR in new castle rights
    if (white_castle_kingside[history_ply + 1]) {
        hash ^= zobrist::hash_castle[4];
    }
    if (white_castle_queenside[history_ply + 1]) {
        hash ^= zobrist::hash_castle[3];
    }
    if (black_castle_kingside[history_ply + 1]) {
        hash ^= zobrist::hash_castle[2];
    }
    if (black_castle_queenside[history_ply + 1]) {
        hash ^= zobrist::hash_castle[1];
    }

    // XOR in new en passant square (if any)
    if (en_passant[history_ply + 1] != INVALID_SQUARE) {
        hash ^= zobrist::hash_en_passant[en_passant[history_ply + 1]];
    }

    // XOR side to move (toggle)
    hash ^= zobrist::hash_side;

    // Update side to move and counters
    side_to_move = (side_to_move == Color::White) ? Color::Black : Color::White;
    moves_history[history_ply] = history_data;
    ply = ply + 1;
    history_ply = history_ply + 1;

    // Update the check field in the move (for move history)
    // This requires checking if the opponent king is in check
    Color opponent = (side_to_move == Color::White) ? Color::Black : Color::White;
    Square king_sq = (side_to_move == Color::White) ? black_king_position : white_king_position;

    if (is_square_attacked(*this, king_sq, opponent, 0)) {
        move.check = CheckType::DirectCheck;
        // Note: We don't distinguish between DirectCheck, DiscoveryCheck, DoubleCheck here
        // That would require more complex analysis
    } else {
        move.check = CheckType::NoCheck;
    }
}

// Undo the last move
void Chessboard::undo() {
    // Decrement counters
    ply = ply - 1;
    history_ply = history_ply - 1;

    // Retrieve move from history
    HistoryMove hmove = moves_history[history_ply];
    Move move = hmove.move;

    fifty = hmove.fifty;
    hash = hmove.hash;

    // Switch side back
    side_to_move = (side_to_move == Color::White) ? Color::Black : Color::White;

    Square from = move.from;
    Square to = move.to;
    Piece captured = move.captured;
    MoveFlag flag = move.flag;

    // Handle castling undo (move rook back)
    if (flag == MoveFlag::Castle) {
        if (to == G1) { // White kingside
            update_white_piece(F1, H1);
            square[H1] = square[F1];
            square[F1] = Piece::Empty;
        } else if (to == C1) { // White queenside
            update_white_piece(D1, A1);
            square[A1] = square[D1];
            square[D1] = Piece::Empty;
        } else if (to == G8) { // Black kingside
            update_black_piece(F8, H8);
            square[H8] = square[F8];
            square[F8] = Piece::Empty;
        } else if (to == C8) { // Black queenside
            update_black_piece(D8, A8);
            square[A8] = square[D8];
            square[D8] = Piece::Empty;
        }
    }

    // Update piece position back
    if (side_to_move == Color::White) {
        update_white_piece(to, from);
        if (square[to] == Piece::White_King) {
            white_king_position = from;
        }
    } else {
        update_black_piece(to, from);
        if (square[to] == Piece::Black_King) {
            black_king_position = from;
        }
    }

    // Move piece back
    square[from] = square[to];
    square[to] = captured;

    // Handle promotion undo
    if (flag == MoveFlag::Promotion) {
        if (side_to_move == Color::White) {
            square[from] = Piece::White_Pawn;
        } else {
            square[from] = Piece::Black_Pawn;
        }
    }

    // Restore captured piece
    if (captured != Piece::Empty) {
        if (side_to_move == Color::White) {
            add_black_piece(to);
        } else {
            add_white_piece(to);
        }
    } else if (flag == MoveFlag::CaptureEnPassant) {
        // Restore en passant captured pawn
        if (side_to_move == Color::White) {
            add_black_piece(to + SOUTH);
            square[to + SOUTH] = Piece::Black_Pawn;
        } else {
            add_white_piece(to + NORTH);
            square[to + NORTH] = Piece::White_Pawn;
        }
    }

    // Clear history entry
    moves_history[history_ply + 1] = EMPTY_HISTORY_MOVE;
}

// Play a null move (pass turn)
void Chessboard::play_null_move() {
    // Create a null move based on last move (for tracking purposes)
    Move null_move = (history_ply > 0) ? moves_history[history_ply - 1].move : EMPTY_MOVE;
    null_move.flag = MoveFlag::NullMove;

    HistoryMove history_data(null_move, hash, fifty);

    // Update state
    moves_history[history_ply] = history_data;
    en_passant[history_ply + 1] = INVALID_SQUARE;
    fifty = fifty + 1;

    // Copy castle rights
    white_castle_kingside[history_ply + 1] = white_castle_kingside[history_ply];
    white_castle_queenside[history_ply + 1] = white_castle_queenside[history_ply];
    black_castle_kingside[history_ply + 1] = black_castle_kingside[history_ply];
    black_castle_queenside[history_ply + 1] = black_castle_queenside[history_ply];

    // Switch side
    side_to_move = (side_to_move == Color::White) ? Color::Black : Color::White;

    ply = ply + 1;
    history_ply = history_ply + 1;

    // Update hash (just toggle side)
    hash ^= zobrist::hash_side;
}

// Undo a null move
void Chessboard::undo_null_move() {
    ply = ply - 1;
    history_ply = history_ply - 1;
    fifty = fifty - 1;

    side_to_move = (side_to_move == Color::White) ? Color::Black : Color::White;

    // Clear history entry
    moves_history[history_ply + 1] = EMPTY_HISTORY_MOVE;

    // Restore hash by toggling side again
    hash ^= zobrist::hash_side;
}

// Get the last move made
Move Chessboard::last_move_made() const {
    if (history_ply == 0) {
        // No move has been played yet
        // Check if king is in check (for FEN positions)
        Move unknown_move = EMPTY_MOVE;
        if (is_square_attacked(*this,
                              (side_to_move == Color::White) ? white_king_position : black_king_position,
                              side_to_move,
                              0)) {
            unknown_move.check = CheckType::UnknownCheck;
        }
        return unknown_move;
    } else {
        return moves_history[history_ply - 1].move;
    }
}

} // namespace adachess

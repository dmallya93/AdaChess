// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#include "adachess/engine/move_registration.hpp"
#include "adachess/board/attack.hpp"
#include <cassert>

namespace adachess {

// Promotion piece types for white and black
static constexpr Piece WHITE_PROMOTIONS[] = {
    Piece::White_Queen,
    Piece::White_Rook,
    Piece::White_Bishop,
    Piece::White_Knight
};

static constexpr Piece BLACK_PROMOTIONS[] = {
    Piece::Black_Queen,
    Piece::Black_Rook,
    Piece::Black_Bishop,
    Piece::Black_Knight
};

// Register_Move - Basic version with from/to squares
void register_move(Chessboard& board, Square from, Square to) {
    Move move;
    move.piece = board.square[from];
    move.captured = board.square[to];
    move.from = from;
    move.to = to;
    move.flag = MoveFlag::StandardMove;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous = AmbiguousFlag::None;

    register_move(board, move);
}

// Register_Move - Version with flag
void register_move(Chessboard& board, Square from, Square to, MoveFlag flag) {
    Move move;
    move.piece = board.square[from];
    move.captured = board.square[to];
    move.from = from;
    move.to = to;
    move.flag = flag;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous = AmbiguousFlag::None;

    // If promotion, generate 4 moves (one for each promotion type)
    if (flag == MoveFlag::Promotion) {
        if (board.side_to_move == Color::White) {
            for (Piece promoted : WHITE_PROMOTIONS) {
                move.promotion = promoted;
                register_move(board, move);
            }
        } else {
            for (Piece promoted : BLACK_PROMOTIONS) {
                move.promotion = promoted;
                register_move(board, move);
            }
        }
    } else {
        register_move(board, move);
    }
}

// Register_Move - Version with pre-constructed move
void register_move(Chessboard& board, const Move& move) {
    Depth ply = board.ply;
    MoveStackIndex& moves_number = board.moves_pointer[ply + 1];

    // Assert that we're within bounds
    assert(moves_number < MAX_MOVE_STACK_SIZE);

    // Perform legality test if required
    // King moves always require legality test, or if force_validity_test is set
    Piece moving_piece = move.piece;
    bool is_king_move = (moving_piece == Piece::White_King || moving_piece == Piece::Black_King);

    if (board.force_validity_test || is_king_move) {
        // Need to make a mutable copy for the play/undo test
        Move test_move = move;
        if (move_leaves_king_in_check(board, test_move)) {
            return;  // Skip illegal move
        }
    }

    // Create the final move with check detection and disambiguation
    Move final_move = move;
    final_move.check = move_checks_opponent_king(board, move);
    final_move.ambiguous = detect_ambiguous_move_notation(board, move);

    // Add to move stack
    board.moves_stack[moves_number] = final_move;
    ++moves_number;
}

// Register_Tactical_Move - Basic version with from/to squares
void register_tactical_move(Chessboard& board, Square from, Square to) {
    Move move;
    move.piece = board.square[from];
    move.captured = board.square[to];
    move.from = from;
    move.to = to;
    move.flag = MoveFlag::StandardMove;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous = AmbiguousFlag::None;

    register_tactical_move(board, move);
}

// Register_Tactical_Move - Version with flag
void register_tactical_move(Chessboard& board, Square from, Square to, MoveFlag flag) {
    Move move;
    move.piece = board.square[from];
    move.captured = board.square[to];
    move.from = from;
    move.to = to;
    move.flag = flag;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous = AmbiguousFlag::None;

    // If promotion, generate 4 moves (one for each promotion type)
    if (flag == MoveFlag::Promotion) {
        if (board.side_to_move == Color::White) {
            for (Piece promoted : WHITE_PROMOTIONS) {
                move.promotion = promoted;
                register_tactical_move(board, move);
            }
        } else {
            for (Piece promoted : BLACK_PROMOTIONS) {
                move.promotion = promoted;
                register_tactical_move(board, move);
            }
        }
    } else {
        register_tactical_move(board, move);
    }
}

// Register_Tactical_Move - Version with pre-constructed move
void register_tactical_move(Chessboard& board, const Move& move) {
    Depth ply = board.ply;
    MoveStackIndex& moves_number = board.moves_pointer[ply + 1];

    // Assert that we're within bounds
    assert(moves_number < MAX_MOVE_STACK_SIZE);

    // Perform legality test if required
    // King moves always require legality test, or if force_validity_test is set
    Piece moving_piece = move.piece;
    bool is_king_move = (moving_piece == Piece::White_King || moving_piece == Piece::Black_King);

    if (board.force_validity_test || is_king_move) {
        // Need to make a mutable copy for the play/undo test
        Move test_move = move;
        if (move_leaves_king_in_check(board, test_move)) {
            return;  // Skip illegal move
        }
    }

    // Create the final move with check detection
    Move final_move = move;
    final_move.check = move_checks_opponent_king(board, move);
    final_move.ambiguous = AmbiguousFlag::None; // Will be set later if needed

    // Only register if tactical
    if (move_is_tactical(final_move)) {
        final_move.ambiguous = detect_ambiguous_move_notation(board, final_move);
        board.moves_stack[moves_number] = final_move;
        ++moves_number;
    }
}

// Helper function stubs (to be implemented in future tasks)

bool move_leaves_king_in_check(Chessboard& board, Move& move) {
    // Perform full legality test using Play/Undo
    // This properly handles pinned pieces, discovered checks, etc.

    Color our_color = board.side_to_move;

    // Play the move
    board.play(move);

    // Check if our king is in check after the move
    // Note: after play(), side_to_move has switched to opponent
    // We need to get the king position AFTER the move (in case king moved)
    Color enemy_color = board.side_to_move;
    Square our_king_pos = (our_color == Color::White) ? board.white_king_position : board.black_king_position;
    bool king_in_check = is_square_attacked(board, our_king_pos, enemy_color, 0);

    // Undo the move
    board.undo();

    return king_in_check;
}

CheckType move_checks_opponent_king(const Chessboard& /*board*/, const Move& /*move*/) {
    // Stub: always returns NoCheck
    // This will be implemented in Task 6-8 with proper check detection
    return CheckType::NoCheck;
}

AmbiguousFlag detect_ambiguous_move_notation(const Chessboard& /*board*/, const Move& /*move*/) {
    // Stub: always returns None
    // This will be implemented when move notation is needed (later milestones)
    return AmbiguousFlag::None;
}

} // namespace adachess

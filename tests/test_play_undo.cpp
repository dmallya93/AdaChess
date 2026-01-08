// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#include "adachess/board/chessboard.hpp"
#include "adachess/board/moves.hpp"
#include "adachess/board/square.hpp"
#include "adachess/core/types.hpp"
#include "adachess/core/hash.hpp"
#include <gtest/gtest.h>

using namespace adachess;

class PlayUndoTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Initialize Zobrist hash tables
        zobrist::initialize_hash();

        // Set up a basic position for testing
        board = Chessboard();

        // Setup starting position for white pieces
        board.square[E2] = Piece::White_Pawn;
        board.square[E1] = Piece::White_King;
        board.square[D1] = Piece::White_Queen;
        board.square[A1] = Piece::White_Rook;
        board.square[H1] = Piece::White_Rook;

        // Setup starting position for black pieces
        board.square[E7] = Piece::Black_Pawn;
        board.square[E8] = Piece::Black_King;
        board.square[D8] = Piece::Black_Queen;
        board.square[A8] = Piece::Black_Rook;
        board.square[H8] = Piece::Black_Rook;

        // Initialize piece lists
        board.white_pieces_counter = 0;
        board.add_white_piece(E2);
        board.add_white_piece(E1);
        board.add_white_piece(D1);
        board.add_white_piece(A1);
        board.add_white_piece(H1);

        board.black_pieces_counter = 0;
        board.add_black_piece(E7);
        board.add_black_piece(E8);
        board.add_black_piece(D8);
        board.add_black_piece(A8);
        board.add_black_piece(H8);

        board.white_king_position = E1;
        board.black_king_position = E8;

        board.side_to_move = Color::White;
        board.ply = ZERO_DEPTH;
        board.history_ply = 0;
        board.fifty = 0;

        // Initialize castle rights
        board.white_castle_kingside[0] = true;
        board.white_castle_queenside[0] = true;
        board.black_castle_kingside[0] = true;
        board.black_castle_queenside[0] = true;

        // Compute initial hash
        board.update_hash();
    }

    Chessboard board;
};

TEST_F(PlayUndoTest, SimplePlayUpdatesBoard) {
    // Create a simple pawn move E2-E4
    Move move;
    move.piece = Piece::White_Pawn;
    move.from = E2;
    move.to = E4;
    move.captured = Piece::Empty;
    move.flag = MoveFlag::PawnMoveTwoSquare;
    move.promotion = Piece::Empty;

    board.play(move);

    // Verify board state
    EXPECT_EQ(board.square[E2], Piece::Empty);
    EXPECT_EQ(board.square[E4], Piece::White_Pawn);
    EXPECT_EQ(board.side_to_move, Color::Black);
    EXPECT_EQ(board.ply, ZERO_DEPTH + 1);
    EXPECT_EQ(board.history_ply, 1);
}

TEST_F(PlayUndoTest, UndoRestoresOriginalState) {
    // Capture initial state
    auto initial_board = board.square;
    auto initial_side = board.side_to_move;
    auto initial_ply = board.ply;
    auto initial_history_ply = board.history_ply;
    auto initial_fifty = board.fifty;
    auto initial_hash = board.hash;

    // Play a move
    Move move;
    move.piece = Piece::White_Pawn;
    move.from = E2;
    move.to = E4;
    move.captured = Piece::Empty;
    move.flag = MoveFlag::PawnMoveTwoSquare;
    move.promotion = Piece::Empty;

    board.play(move);

    // Undo the move
    board.undo();

    // Verify all state is restored
    EXPECT_EQ(board.square[E2], initial_board[E2]);
    EXPECT_EQ(board.square[E4], initial_board[E4]);
    EXPECT_EQ(board.side_to_move, initial_side);
    EXPECT_EQ(board.ply, initial_ply);
    EXPECT_EQ(board.history_ply, initial_history_ply);
    EXPECT_EQ(board.fifty, initial_fifty);
    EXPECT_EQ(board.hash, initial_hash);
}

TEST_F(PlayUndoTest, PlayUpdatesFiftyMoveCounter) {
    // Pawn move should reset fifty counter
    board.fifty = 10;

    Move pawn_move;
    pawn_move.piece = Piece::White_Pawn;
    pawn_move.from = E2;
    pawn_move.to = E4;
    pawn_move.captured = Piece::Empty;
    pawn_move.flag = MoveFlag::PawnMoveTwoSquare;
    pawn_move.promotion = Piece::Empty;

    board.play(pawn_move);

    EXPECT_EQ(board.fifty, 0);
}

TEST_F(PlayUndoTest, PlayUpdatesEnPassantSquare) {
    Move pawn_move;
    pawn_move.piece = Piece::White_Pawn;
    pawn_move.from = E2;
    pawn_move.to = E4;
    pawn_move.captured = Piece::Empty;
    pawn_move.flag = MoveFlag::PawnMoveTwoSquare;
    pawn_move.promotion = Piece::Empty;

    board.play(pawn_move);

    // En passant square should be set to E3
    EXPECT_EQ(board.en_passant[1], E3);
}

TEST_F(PlayUndoTest, PlayHandlesCapture) {
    // Add a black pawn to capture
    board.square[E4] = Piece::Black_Pawn;
    board.add_black_piece(E4);

    auto initial_black_count = board.black_pieces_counter;

    // Create a capture move
    Move capture_move;
    capture_move.piece = Piece::White_Pawn;
    capture_move.from = E2;
    capture_move.to = E4;
    capture_move.captured = Piece::Black_Pawn;
    capture_move.flag = MoveFlag::StandardMove;
    capture_move.promotion = Piece::Empty;

    board.play(capture_move);

    // Verify capture
    EXPECT_EQ(board.square[E4], Piece::White_Pawn);
    EXPECT_EQ(board.black_pieces_counter, initial_black_count - 1);
    EXPECT_EQ(board.fifty, 0); // Capture resets fifty counter
}

TEST_F(PlayUndoTest, UndoRestoresCapture) {
    // Add a black pawn to capture
    board.square[E4] = Piece::Black_Pawn;
    board.add_black_piece(E4);

    auto initial_black_count = board.black_pieces_counter;

    // Create and play a capture move
    Move capture_move;
    capture_move.piece = Piece::White_Pawn;
    capture_move.from = E2;
    capture_move.to = E4;
    capture_move.captured = Piece::Black_Pawn;
    capture_move.flag = MoveFlag::StandardMove;
    capture_move.promotion = Piece::Empty;

    board.play(capture_move);
    board.undo();

    // Verify piece is restored
    EXPECT_EQ(board.square[E4], Piece::Black_Pawn);
    EXPECT_EQ(board.black_pieces_counter, initial_black_count);
}

TEST_F(PlayUndoTest, MultiplePlayUndoSequence) {
    auto initial_hash = board.hash;

    // Play multiple moves
    Move move1;
    move1.piece = Piece::White_Pawn;
    move1.from = E2;
    move1.to = E4;
    move1.captured = Piece::Empty;
    move1.flag = MoveFlag::PawnMoveTwoSquare;
    move1.promotion = Piece::Empty;
    board.play(move1);

    Move move2;
    move2.piece = Piece::Black_Pawn;
    move2.from = E7;
    move2.to = E5;
    move2.captured = Piece::Empty;
    move2.flag = MoveFlag::PawnMoveTwoSquare;
    move2.promotion = Piece::Empty;
    board.play(move2);

    // Undo both moves
    board.undo();
    board.undo();

    // Verify we're back to initial state
    EXPECT_EQ(board.hash, initial_hash);
    EXPECT_EQ(board.ply, ZERO_DEPTH);
    EXPECT_EQ(board.history_ply, 0);
}

TEST_F(PlayUndoTest, PlayUpdatesKingPosition) {
    Move king_move;
    king_move.piece = Piece::White_King;
    king_move.from = E1;
    king_move.to = E2;
    king_move.captured = Piece::Empty;
    king_move.flag = MoveFlag::StandardMove;
    king_move.promotion = Piece::Empty;

    // Move pawn out of the way first
    board.square[E2] = Piece::Empty;

    board.play(king_move);

    EXPECT_EQ(board.white_king_position, E2);
}

TEST_F(PlayUndoTest, PlayRevokeCastleRightsOnKingMove) {
    Move king_move;
    king_move.piece = Piece::White_King;
    king_move.from = E1;
    king_move.to = E2;
    king_move.captured = Piece::Empty;
    king_move.flag = MoveFlag::StandardMove;
    king_move.promotion = Piece::Empty;

    board.square[E2] = Piece::Empty;
    board.play(king_move);

    EXPECT_FALSE(board.white_castle_kingside[1]);
    EXPECT_FALSE(board.white_castle_queenside[1]);
}

TEST_F(PlayUndoTest, PlayNullMove) {
    auto initial_ply = board.ply;
    auto initial_history_ply = board.history_ply;

    board.play_null_move();

    EXPECT_EQ(board.side_to_move, Color::Black);
    EXPECT_EQ(board.ply, initial_ply + 1);
    EXPECT_EQ(board.history_ply, initial_history_ply + 1);
}

TEST_F(PlayUndoTest, UndoNullMove) {
    auto initial_side = board.side_to_move;
    auto initial_ply = board.ply;
    auto initial_history_ply = board.history_ply;
    auto initial_hash = board.hash;

    board.play_null_move();
    board.undo_null_move();

    EXPECT_EQ(board.side_to_move, initial_side);
    EXPECT_EQ(board.ply, initial_ply);
    EXPECT_EQ(board.history_ply, initial_history_ply);
    EXPECT_EQ(board.hash, initial_hash);
}

TEST_F(PlayUndoTest, LastMoveMadeReturnsCorrectMove) {
    // Initially, should return empty move
    Move last = board.last_move_made();
    EXPECT_EQ(last.from, EMPTY_MOVE.from);

    // Play a move
    Move move;
    move.piece = Piece::White_Pawn;
    move.from = E2;
    move.to = E4;
    move.captured = Piece::Empty;
    move.flag = MoveFlag::PawnMoveTwoSquare;
    move.promotion = Piece::Empty;

    board.play(move);

    // Now last_move_made should return this move
    last = board.last_move_made();
    EXPECT_EQ(last.from, E2);
    EXPECT_EQ(last.to, E4);
}

TEST_F(PlayUndoTest, HashChangesAfterMove) {
    auto initial_hash = board.hash;

    Move move;
    move.piece = Piece::White_Pawn;
    move.from = E2;
    move.to = E4;
    move.captured = Piece::Empty;
    move.flag = MoveFlag::PawnMoveTwoSquare;
    move.promotion = Piece::Empty;

    board.play(move);

    // Hash should change
    EXPECT_NE(board.hash, initial_hash);
    EXPECT_NE(board.hash, 0);
}

TEST_F(PlayUndoTest, PieceTrackingConsistency) {
    // Verify piece tracking is consistent after play
    Move move;
    move.piece = Piece::White_Pawn;
    move.from = E2;
    move.to = E4;
    move.captured = Piece::Empty;
    move.flag = MoveFlag::StandardMove;
    move.promotion = Piece::Empty;

    // Debug: Check initial state
    EXPECT_NE(board.piece_table[E2], 0) << "E2 should have a piece index before move";
    EXPECT_EQ(board.piece_table[E4], 0) << "E4 should be empty before move";

    board.play(move);

    // Check piece_table is updated
    EXPECT_EQ(board.piece_table[E2], 0) << "E2 should be empty after move";
    EXPECT_NE(board.piece_table[E4], 0) << "E4 should have piece index after move";

    // Check pieces_list is updated
    bool found = false;
    for (uint8_t i = 0; i < board.white_pieces_counter; ++i) {
        if (board.pieces_list[i] == E4) {
            found = true;
            break;
        }
    }
    EXPECT_TRUE(found) << "E4 should be in pieces_list after move";
}

TEST_F(PlayUndoTest, HashIncrementalMatchesFullRecomputation) {
    // Test that incremental hash updates match full recomputation
    Move move;
    move.piece = Piece::White_Pawn;
    move.from = E2;
    move.to = E4;
    move.captured = Piece::Empty;
    move.flag = MoveFlag::PawnMoveTwoSquare;
    move.promotion = Piece::Empty;

    board.play(move);

    // Capture hash after play (incremental)
    Hash incremental_hash = board.hash;

    // Recompute hash from scratch
    board.update_hash();
    Hash full_recompute_hash = board.hash;

    // They should match
    EXPECT_EQ(incremental_hash, full_recompute_hash);
}

TEST_F(PlayUndoTest, CastlingKingsideWhite) {
    // Setup castling position
    board.square[F1] = Piece::Empty;
    board.square[G1] = Piece::Empty;
    board.white_castle_kingside[0] = true;

    Move castle;
    castle.piece = Piece::White_King;
    castle.from = E1;
    castle.to = G1;
    castle.captured = Piece::Empty;
    castle.flag = MoveFlag::Castle;
    castle.promotion = Piece::Empty;

    board.play(castle);

    // Check king moved to G1
    EXPECT_EQ(board.square[G1], Piece::White_King);
    EXPECT_EQ(board.square[E1], Piece::Empty);

    // Check rook moved to F1
    EXPECT_EQ(board.square[F1], Piece::White_Rook);
    EXPECT_EQ(board.square[H1], Piece::Empty);

    // Check castle rights revoked
    EXPECT_FALSE(board.white_castle_kingside[1]);
}

TEST_F(PlayUndoTest, CastlingQueensideWhite) {
    // Setup castling position
    board.square[D1] = Piece::Empty;
    board.square[C1] = Piece::Empty;
    board.square[B1] = Piece::Empty;
    board.white_castle_queenside[0] = true;

    Move castle;
    castle.piece = Piece::White_King;
    castle.from = E1;
    castle.to = C1;
    castle.captured = Piece::Empty;
    castle.flag = MoveFlag::Castle;
    castle.promotion = Piece::Empty;

    board.play(castle);

    // Check king moved to C1
    EXPECT_EQ(board.square[C1], Piece::White_King);
    EXPECT_EQ(board.square[E1], Piece::Empty);

    // Check rook moved to D1
    EXPECT_EQ(board.square[D1], Piece::White_Rook);
    EXPECT_EQ(board.square[A1], Piece::Empty);

    // Check castle rights revoked
    EXPECT_FALSE(board.white_castle_queenside[1]);
}

TEST_F(PlayUndoTest, UndoCastlingKingside) {
    // Setup and play castling
    board.square[F1] = Piece::Empty;
    board.square[G1] = Piece::Empty;
    board.white_castle_kingside[0] = true;

    Move castle;
    castle.piece = Piece::White_King;
    castle.from = E1;
    castle.to = G1;
    castle.captured = Piece::Empty;
    castle.flag = MoveFlag::Castle;
    castle.promotion = Piece::Empty;

    board.play(castle);
    board.undo();

    // Check everything restored
    EXPECT_EQ(board.square[E1], Piece::White_King);
    EXPECT_EQ(board.square[G1], Piece::Empty);
    EXPECT_EQ(board.square[H1], Piece::White_Rook);
    EXPECT_EQ(board.square[F1], Piece::Empty);
    EXPECT_TRUE(board.white_castle_kingside[0]);
}

TEST_F(PlayUndoTest, EnPassantCapture) {
    // Setup en passant position
    // White pawn on E5, black pawn just moved D7-D5
    board.square[E5] = Piece::White_Pawn;
    board.square[D5] = Piece::Black_Pawn;
    board.add_white_piece(E5);
    board.add_black_piece(D5);
    board.en_passant[0] = D6; // En passant square

    auto black_count_before = board.black_pieces_counter;

    Move en_passant;
    en_passant.piece = Piece::White_Pawn;
    en_passant.from = E5;
    en_passant.to = D6;
    en_passant.captured = Piece::Empty; // En passant doesn't directly capture
    en_passant.flag = MoveFlag::CaptureEnPassant;
    en_passant.promotion = Piece::Empty;

    board.play(en_passant);

    // Check pawn moved to D6
    EXPECT_EQ(board.square[D6], Piece::White_Pawn);
    EXPECT_EQ(board.square[E5], Piece::Empty);

    // Check black pawn removed from D5
    EXPECT_EQ(board.square[D5], Piece::Empty);
    EXPECT_EQ(board.black_pieces_counter, black_count_before - 1);
}

TEST_F(PlayUndoTest, UndoEnPassantCapture) {
    // Setup and play en passant
    board.square[E5] = Piece::White_Pawn;
    board.square[D5] = Piece::Black_Pawn;
    board.add_white_piece(E5);
    board.add_black_piece(D5);
    board.en_passant[0] = D6;

    auto black_count_before = board.black_pieces_counter;

    Move en_passant;
    en_passant.piece = Piece::White_Pawn;
    en_passant.from = E5;
    en_passant.to = D6;
    en_passant.captured = Piece::Empty;
    en_passant.flag = MoveFlag::CaptureEnPassant;
    en_passant.promotion = Piece::Empty;

    board.play(en_passant);
    board.undo();

    // Check everything restored
    EXPECT_EQ(board.square[E5], Piece::White_Pawn);
    EXPECT_EQ(board.square[D6], Piece::Empty);
    EXPECT_EQ(board.square[D5], Piece::Black_Pawn);
    EXPECT_EQ(board.black_pieces_counter, black_count_before);
}

TEST_F(PlayUndoTest, PromotionToQueen) {
    // Setup promotion position (white pawn on 7th rank)
    board.square[E7] = Piece::White_Pawn;
    board.square[E8] = Piece::Empty;
    board.add_white_piece(E7);

    Move promotion;
    promotion.piece = Piece::White_Pawn;
    promotion.from = E7;
    promotion.to = E8;
    promotion.captured = Piece::Empty;
    promotion.flag = MoveFlag::Promotion;
    promotion.promotion = Piece::White_Queen;

    board.play(promotion);

    // Check pawn promoted to queen
    EXPECT_EQ(board.square[E8], Piece::White_Queen);
    EXPECT_EQ(board.square[E7], Piece::Empty);
}

TEST_F(PlayUndoTest, UndoPromotion) {
    // Setup and play promotion
    board.square[E7] = Piece::White_Pawn;
    board.square[E8] = Piece::Empty;
    board.add_white_piece(E7);

    Move promotion;
    promotion.piece = Piece::White_Pawn;
    promotion.from = E7;
    promotion.to = E8;
    promotion.captured = Piece::Empty;
    promotion.flag = MoveFlag::Promotion;
    promotion.promotion = Piece::White_Queen;

    board.play(promotion);
    board.undo();

    // Check pawn restored
    EXPECT_EQ(board.square[E7], Piece::White_Pawn);
    EXPECT_EQ(board.square[E8], Piece::Empty);
}

TEST_F(PlayUndoTest, PromotionWithCapture) {
    // Setup promotion with capture
    board.square[E7] = Piece::White_Pawn;
    board.square[D8] = Piece::Black_Queen; // Capture opponent queen
    board.add_white_piece(E7);
    board.add_black_piece(D8);

    auto black_count_before = board.black_pieces_counter;

    Move promo_capture;
    promo_capture.piece = Piece::White_Pawn;
    promo_capture.from = E7;
    promo_capture.to = D8;
    promo_capture.captured = Piece::Black_Queen;
    promo_capture.flag = MoveFlag::Promotion;
    promo_capture.promotion = Piece::White_Queen;

    board.play(promo_capture);

    // Check promotion and capture
    EXPECT_EQ(board.square[D8], Piece::White_Queen);
    EXPECT_EQ(board.square[E7], Piece::Empty);
    EXPECT_EQ(board.black_pieces_counter, black_count_before - 1);
}

TEST_F(PlayUndoTest, UndoPromotionWithCapture) {
    // Setup and play promotion with capture
    board.square[E7] = Piece::White_Pawn;
    board.square[D8] = Piece::Black_Queen;
    board.add_white_piece(E7);
    board.add_black_piece(D8);

    auto black_count_before = board.black_pieces_counter;

    Move promo_capture;
    promo_capture.piece = Piece::White_Pawn;
    promo_capture.from = E7;
    promo_capture.to = D8;
    promo_capture.captured = Piece::Black_Queen;
    promo_capture.flag = MoveFlag::Promotion;
    promo_capture.promotion = Piece::White_Queen;

    board.play(promo_capture);
    board.undo();

    // Check everything restored
    EXPECT_EQ(board.square[E7], Piece::White_Pawn);
    EXPECT_EQ(board.square[D8], Piece::Black_Queen);
    EXPECT_EQ(board.black_pieces_counter, black_count_before);
}

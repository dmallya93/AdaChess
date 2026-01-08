//
// AdaChess - Smart Chess Engine
//
// Unit tests for Zobrist Hashing System
//

#include "adachess/board/chessboard.hpp"
#include "adachess/core/zobrist.hpp"
#include <gtest/gtest.h>
#include <set>

using namespace adachess::board;
using namespace adachess::core;

// ====== Hash Table Population Tests ======

TEST(ZobristHashTest, HashTablesPopulatedWithDistinctValues) {
    Chessboard board;
    initialize(board);

    // Check that hash tables have non-zero values
    bool has_nonzero = false;
    for (const auto& piece_row : hash_pieces) {
        for (const auto& value : piece_row) {
            if (value != 0) {
                has_nonzero = true;
                break;
            }
        }
        if (has_nonzero) break;
    }
    EXPECT_TRUE(has_nonzero) << "Hash_Pieces should contain non-zero values";

    // Check for distinct values (high collision would indicate a problem)
    std::set<uint64_t> unique_values;
    for (const auto& piece_row : hash_pieces) {
        for (const auto& value : piece_row) {
            unique_values.insert(value);
        }
    }
    for (const auto& value : hash_en_passant) {
        unique_values.insert(value);
    }
    for (const auto& value : hash_castle) {
        unique_values.insert(value);
    }
    unique_values.insert(hash_side);

    // We should have a high number of unique values
    // Total entries: 1680 (pieces) + 120 (en passant) + 4 (castle) + 1 (side) = 1805
    EXPECT_GT(unique_values.size(), 1700) << "Should have mostly distinct hash values";
}

TEST(ZobristHashTest, HashSideIsNonZero) {
    Chessboard board;
    initialize(board);

    EXPECT_NE(hash_side, 0) << "Hash_Side should be non-zero";
}

// ====== Hash Determinism Tests ======

TEST(ZobristHashTest, InitializeHashIsDeterministic) {
    Chessboard board1;
    initialize(board1);
    const uint64_t hash1 = board1.zobrist_hash;

    Chessboard board2;
    initialize(board2);
    const uint64_t hash2 = board2.zobrist_hash;

    EXPECT_EQ(hash1, hash2) << "Same position should produce same hash across runs";
}

TEST(ZobristHashTest, StartingPositionHashIsConsistent) {
    Chessboard board;
    initialize(board);
    const uint64_t initial_hash = board.zobrist_hash;

    // Reinitialize and check again
    initialize(board);
    EXPECT_EQ(board.zobrist_hash, initial_hash) << "Starting position hash should be consistent";
}

// ====== Position Difference Tests ======

TEST(ZobristHashTest, DifferentPiecePlacementProducesDifferentHash) {
    Chessboard board1;
    initialize(board1);
    const uint64_t hash1 = board1.zobrist_hash;

    // Move a piece (e.g., remove white pawn from E2)
    Chessboard board2;
    initialize(board2);
    board2.board[E2] = Piece::Empty;
    update_hash(board2);

    EXPECT_NE(board2.zobrist_hash, hash1) << "Different piece placement should produce different hash";
}

TEST(ZobristHashTest, DifferentSideToMoveProducesDifferentHash) {
    Chessboard board1;
    initialize(board1);
    const uint64_t hash1 = board1.zobrist_hash;

    // Change side to move
    Chessboard board2;
    initialize(board2);
    board2.side_to_move = Color::Black;
    update_hash(board2);

    EXPECT_NE(board2.zobrist_hash, hash1) << "Different side to move should produce different hash";
}

TEST(ZobristHashTest, DifferentCastleRightsProduceDifferentHash) {
    Chessboard board1;
    initialize(board1);
    const uint64_t hash1 = board1.zobrist_hash;

    // Remove white kingside castle right
    Chessboard board2;
    initialize(board2);
    board2.white_castle_kingside[0] = false;
    update_hash(board2);

    EXPECT_NE(board2.zobrist_hash, hash1) << "Different castle rights should produce different hash";
}

TEST(ZobristHashTest, DifferentEnPassantSquareProducesDifferentHash) {
    Chessboard board1;
    initialize(board1);
    const uint64_t hash1 = board1.zobrist_hash;

    // Set en passant square
    Chessboard board2;
    initialize(board2);
    board2.en_passant_square[0] = E3;  // Set en passant square
    update_hash(board2);

    EXPECT_NE(board2.zobrist_hash, hash1) << "Different en passant square should produce different hash";
}

// ====== Hash Update Tests ======

TEST(ZobristHashTest, UpdateHashRecomputesFullHash) {
    Chessboard board;
    initialize(board);
    const uint64_t initial_hash = board.zobrist_hash;

    // Manually change zobrist_hash to verify update_hash recomputes it
    board.zobrist_hash = 0;
    update_hash(board);

    EXPECT_EQ(board.zobrist_hash, initial_hash) << "update_hash should recompute the correct hash";
}

TEST(ZobristHashTest, EmptyBoardHashIsZeroWhenWhiteToMove) {
    Chessboard board;
    reset(board);

    // Reset produces an empty board with White to move
    // Since no pieces, no en passant, no castle rights, hash should be 0
    EXPECT_EQ(board.zobrist_hash, 0) << "Empty board with White to move should have hash 0";
}

TEST(ZobristHashTest, EmptyBoardWithBlackToMoveHashesCorrectly) {
    Chessboard board;
    reset(board);
    board.side_to_move = Color::Black;
    update_hash(board);

    // Empty board with Black to move should just have hash_side XORed in
    EXPECT_EQ(board.zobrist_hash, hash_side) << "Empty board with Black to move should hash to hash_side";
}

// ====== Castle Rights Hash Tests ======

TEST(ZobristHashTest, AllCastleRightsHashIndependently) {
    Chessboard board;
    reset(board);

    // Enable only White Kingside
    board.white_castle_kingside[0] = true;
    board.white_castle_queenside[0] = false;
    board.black_castle_kingside[0] = false;
    board.black_castle_queenside[0] = false;
    update_hash(board);
    const uint64_t hash_wk = board.zobrist_hash;

    // Enable only White Queenside
    board.white_castle_kingside[0] = false;
    board.white_castle_queenside[0] = true;
    update_hash(board);
    const uint64_t hash_wq = board.zobrist_hash;

    // Enable only Black Kingside
    board.white_castle_queenside[0] = false;
    board.black_castle_kingside[0] = true;
    update_hash(board);
    const uint64_t hash_bk = board.zobrist_hash;

    // Enable only Black Queenside
    board.black_castle_kingside[0] = false;
    board.black_castle_queenside[0] = true;
    update_hash(board);
    const uint64_t hash_bq = board.zobrist_hash;

    // All should be different
    EXPECT_NE(hash_wk, hash_wq);
    EXPECT_NE(hash_wk, hash_bk);
    EXPECT_NE(hash_wk, hash_bq);
    EXPECT_NE(hash_wq, hash_bk);
    EXPECT_NE(hash_wq, hash_bq);
    EXPECT_NE(hash_bk, hash_bq);
}

// ====== Hash Reproducibility Across Runs ======

TEST(ZobristHashTest, SamePositionSameHashAfterMultipleInits) {
    Chessboard board1;
    initialize(board1);
    const uint64_t hash1 = board1.zobrist_hash;

    // Initialize hash tables again (simulating multiple runs)
    initialize_hash(board1);
    const uint64_t hash2 = board1.zobrist_hash;

    Chessboard board3;
    initialize(board3);
    const uint64_t hash3 = board3.zobrist_hash;

    EXPECT_EQ(hash1, hash2) << "Hash should be reproducible after re-initialization";
    EXPECT_EQ(hash1, hash3) << "Hash should be reproducible across different board instances";
}

// ====== Piece Placement Hash Tests ======

TEST(ZobristHashTest, EachPieceTypHashesUniquely) {
    Chessboard board;
    reset(board);

    // Place white pawn on E4
    board.board[E4] = Piece::White_Pawn;
    board.pieces_list[1] = E4;
    board.white_pieces_counter = 1;
    board.piece_table[E4] = 1;
    update_hash(board);
    const uint64_t hash_white_pawn = board.zobrist_hash;

    // Clear and place white knight on E4
    board.board[E4] = Piece::White_Knight;
    update_hash(board);
    const uint64_t hash_white_knight = board.zobrist_hash;

    // Clear and place black pawn on E4
    board.board[E4] = Piece::Black_Pawn;
    board.pieces_list[1] = 0;
    board.white_pieces_counter = 0;
    board.pieces_list[17] = E4;
    board.black_pieces_counter = 1;
    board.piece_table[E4] = 17;
    update_hash(board);
    const uint64_t hash_black_pawn = board.zobrist_hash;

    EXPECT_NE(hash_white_pawn, hash_white_knight);
    EXPECT_NE(hash_white_pawn, hash_black_pawn);
    EXPECT_NE(hash_white_knight, hash_black_pawn);
}

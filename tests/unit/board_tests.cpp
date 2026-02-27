// ---------------------------------------------------------------------------
// AdaChess – Unit tests for board representation
// ---------------------------------------------------------------------------
#include <gtest/gtest.h>

#include "chess/board/board.hpp"
#include "chess/board/directions.hpp"
#include "chess/board/attacks_data.hpp"

using namespace chess;
using namespace chess::board;

// -----------------------------------------------------------------------
// Square constant tests
// -----------------------------------------------------------------------
TEST(BoardTest, SquareConstants) {
    // Verify key square values match Ada constants
    EXPECT_EQ(A1, 91);
    EXPECT_EQ(H1, 98);
    EXPECT_EQ(A8, 21);
    EXPECT_EQ(H8, 28);
    EXPECT_EQ(E1, 95);
    EXPECT_EQ(E8, 25);
    EXPECT_EQ(D4, 64);
    EXPECT_EQ(E4, 65);
}

// -----------------------------------------------------------------------
// Frame detection tests
// -----------------------------------------------------------------------
TEST(BoardTest, FrameDetection) {
    // Corners of the 10x12 board are frame
    EXPECT_TRUE(Square_Is_Frame[0]);
    EXPECT_TRUE(Square_Is_Frame[9]);
    EXPECT_TRUE(Square_Is_Frame[10]);
    EXPECT_TRUE(Square_Is_Frame[19]);
    EXPECT_TRUE(Square_Is_Frame[110]);
    EXPECT_TRUE(Square_Is_Frame[119]);

    // First/last column within playable rows are frame
    EXPECT_TRUE(Square_Is_Frame[20]);  // row 2, col 0
    EXPECT_TRUE(Square_Is_Frame[29]);  // row 2, col 9

    // Valid squares are not frame
    EXPECT_FALSE(Square_Is_Frame[A1]);
    EXPECT_FALSE(Square_Is_Frame[H8]);
    EXPECT_FALSE(Square_Is_Frame[E4]);

    // Check all 64 valid squares are not frame
    int valid_count = 0;
    for (int sq = 0; sq < Board_Size; ++sq) {
        if (!Square_Is_Frame[static_cast<std::size_t>(sq)]) {
            ++valid_count;
        }
    }
    EXPECT_EQ(valid_count, 64);
}

// -----------------------------------------------------------------------
// Coordinate function tests
// -----------------------------------------------------------------------
TEST(BoardTest, FileOf) {
    EXPECT_EQ(file_of(A1), File_A); // 0
    EXPECT_EQ(file_of(H1), File_H); // 7
    EXPECT_EQ(file_of(A8), File_A);
    EXPECT_EQ(file_of(H8), File_H);
    EXPECT_EQ(file_of(E4), File_E); // 4
}

TEST(BoardTest, RankOf) {
    EXPECT_EQ(rank_of(A1), Rank_1); // 1
    EXPECT_EQ(rank_of(A8), Rank_8); // 8
    EXPECT_EQ(rank_of(H1), Rank_1);
    EXPECT_EQ(rank_of(H8), Rank_8);
    EXPECT_EQ(rank_of(E4), Rank_4); // 4
}

TEST(BoardTest, SquareNames) {
    EXPECT_STREQ(Pc_Sqr[static_cast<std::size_t>(A1)], "a1");
    EXPECT_STREQ(Pc_Sqr[static_cast<std::size_t>(H8)], "h8");
    EXPECT_STREQ(Pc_Sqr[static_cast<std::size_t>(E4)], "e4");
    EXPECT_STREQ(Pc_Sqr[static_cast<std::size_t>(D5)], "d5");
}

// -----------------------------------------------------------------------
// Direction tests
// -----------------------------------------------------------------------
TEST(DirectionTest, Offsets) {
    // Moving north from A1 should reach A2
    EXPECT_EQ(A1 + North, A2);
    // Moving south from A2 should reach A1
    EXPECT_EQ(A2 + South, A1);
    // Moving east from A1 should reach B1
    EXPECT_EQ(A1 + East, B1);
    // Moving west from B1 should reach A1
    EXPECT_EQ(B1 + West, A1);
    // Moving north-east from A1 should reach B2
    EXPECT_EQ(A1 + North_East, B2);
}

TEST(DirectionTest, KnightOffsets) {
    // Knight on E4: one valid move should be D6 (North_North_West)
    EXPECT_EQ(E4 + North_North_West, D6);
    // Knight on E4 to F6 (North_North_East)
    EXPECT_EQ(E4 + North_North_East, F6);
}

// -----------------------------------------------------------------------
// Color board tests
// -----------------------------------------------------------------------
TEST(BoardTest, ColorBoard) {
    // A1 is a dark square (conventionally), but in Ada it's White based on
    // the color board table. We just verify the pattern alternates.
    EXPECT_NE(Color_Board[static_cast<std::size_t>(A1)],
              Color_Board[static_cast<std::size_t>(B1)]);
    EXPECT_EQ(Color_Board[static_cast<std::size_t>(A1)],
              Color_Board[static_cast<std::size_t>(C1)]);
}

// -----------------------------------------------------------------------
// Attack data initialization test
// -----------------------------------------------------------------------
TEST(AttackDataTest, InitializationDoesNotCrash) {
    // init_attacks_data is called in main, but we can call it again safely
    init_attacks_data();
    // Verify that valid directions have non-placeholder functions
    auto north_idx = static_cast<std::size_t>(North - Direction_Min);
    EXPECT_NE(Attacks_To[north_idx], attacks_from_placeholder);
}

TEST(AttackDataTest, PlaceholderReturnsFalse) {
    Board board{};
    board.fill(Piece::Frame);
    EXPECT_FALSE(attacks_from_placeholder(board, Color::White, 0));
}

// -----------------------------------------------------------------------
// Sliding attack test
// -----------------------------------------------------------------------
TEST(AttackDataTest, SlidingAttackNorth) {
    init_attacks_data();

    Board board{};
    board.fill(Piece::Frame);

    // Set up: White Rook on A1, empty squares between A1 and A8
    for (int rank = 1; rank <= 8; ++rank) {
        int sq = A8 + (8 - rank) * 10; // A-file squares
        board[static_cast<std::size_t>(sq)] = Piece::Empty;
    }
    board[static_cast<std::size_t>(A1)] = Piece::White_Rook;

    // A3 should be attacked from the south by White's rook
    EXPECT_TRUE(attacks_from_south(board, Color::White, A3));
    // But not by Black
    EXPECT_FALSE(attacks_from_south(board, Color::Black, A3));
}

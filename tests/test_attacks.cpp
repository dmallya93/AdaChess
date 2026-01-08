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

#include "adachess/board/attacks.hpp"
#include "adachess/board/square.hpp"
#include <gtest/gtest.h>

using namespace adachess::board;
using namespace adachess::core;

// Helper to create an empty board with frame
BoardArray create_empty_board() {
    BoardArray board;
    board.fill(Piece::Frame);

    // Set playable area to empty (rows 2-9, columns 1-8 in 10x12 layout)
    for (int row = 2; row < 10; ++row) {
        for (int col = 1; col < 9; ++col) {
            board[row * 10 + col] = Piece::Empty;
        }
    }

    return board;
}

// Note: Square constants (A1-H8) are imported from adachess/board/square.hpp via using namespace

// ====== Tests for Orthogonal Attacks (Rook/Queen) ======

TEST(AttacksTest, AttacksFromNorthDetectsWhiteRook) {
    auto board = create_empty_board();
    board[E8] = White_Rook;  // Rook on E8
    
    // E4 is attacked from north by white rook on E8
    EXPECT_TRUE(attacks_from_north(board, Color::White, E4));
}

TEST(AttacksTest, AttacksFromNorthDetectsWhiteQueen) {
    auto board = create_empty_board();
    board[E8] = White_Queen;  // Queen on E8
    
    EXPECT_TRUE(attacks_from_north(board, Color::White, E4));
}

TEST(AttacksTest, AttacksFromNorthDoesNotDetectBlackRook) {
    auto board = create_empty_board();
    board[E8] = White_Rook;  // White rook on E8
    
    // Checking for black attack should return false
    EXPECT_FALSE(attacks_from_north(board, Color::Black, E4));
}

TEST(AttacksTest, AttacksFromNorthBlockedByPiece) {
    auto board = create_empty_board();
    board[E8] = White_Rook;  // Rook on E8
    board[E6] = White_Pawn;  // Blocking pawn on E6
    
    // E4 is not attacked because E6 blocks
    EXPECT_FALSE(attacks_from_north(board, Color::White, E4));
}

TEST(AttacksTest, AttacksFromSouthDetectsBlackRook) {
    auto board = create_empty_board();
    board[E1] = Black_Rook;  // Rook on E1
    
    // E5 is attacked from south by black rook on E1
    EXPECT_TRUE(attacks_from_south(board, Color::Black, E5));
}

TEST(AttacksTest, AttacksFromEastDetectsWhiteQueen) {
    auto board = create_empty_board();
    board[H4] = White_Queen;  // Queen on H4
    
    // D4 is attacked from east by white queen on H4
    EXPECT_TRUE(attacks_from_east(board, Color::White, D4));
}

TEST(AttacksTest, AttacksFromWestDetectsBlackQueen) {
    auto board = create_empty_board();
    board[A4] = Black_Queen;  // Queen on A4
    
    // E4 is attacked from west by black queen on A4
    EXPECT_TRUE(attacks_from_west(board, Color::Black, E4));
}

// ====== Tests for Diagonal Attacks (Bishop/Queen) ======

TEST(AttacksTest, AttacksFromNorthEastDetectsWhiteBishop) {
    auto board = create_empty_board();
    board[H8] = White_Bishop;  // Bishop on H8
    
    // D4 is attacked from north-east by white bishop on H8
    EXPECT_TRUE(attacks_from_north_east(board, Color::White, D4));
}

TEST(AttacksTest, AttacksFromNorthEastDetectsBlackQueen) {
    auto board = create_empty_board();
    board[H8] = Black_Queen;  // Queen on H8
    
    EXPECT_TRUE(attacks_from_north_east(board, Color::Black, D4));
}

TEST(AttacksTest, AttacksFromNorthWestDetectsWhiteBishop) {
    auto board = create_empty_board();
    board[A8] = White_Bishop;  // Bishop on A8
    
    // E4 is attacked from north-west by white bishop on A8
    EXPECT_TRUE(attacks_from_north_west(board, Color::White, E4));
}

TEST(AttacksTest, AttacksFromSouthEastDetectsBlackBishop) {
    auto board = create_empty_board();
    board[H1] = Black_Bishop;  // Bishop on H1

    // D5 going SE: E4, F3, G2, H1 - attacked by black bishop on H1
    constexpr Square D5 = 54;
    EXPECT_TRUE(attacks_from_south_east(board, Color::Black, D5));
}

TEST(AttacksTest, AttacksFromSouthWestDetectsWhiteQueen) {
    auto board = create_empty_board();
    board[A1] = White_Queen;  // Queen on A1

    // E5 going SW: D4, C3, B2, A1 - attacked by white queen on A1
    EXPECT_TRUE(attacks_from_south_west(board, Color::White, E5));
}


// ====== Tests for Knight Attacks ======

TEST(AttacksTest, AttacksFromNorthNorthEastDetectsWhiteKnight) {
    auto board = create_empty_board();
    // Knight can attack E4 from north-north-east direction
    // From E4, north-north-east is E4 + (-19) = E4 - 19
    const Square knight_square = E4 + NORTH_NORTH_EAST;
    board[knight_square] = White_Knight;
    
    EXPECT_TRUE(attacks_from_north_north_east(board, Color::White, knight_square));
}

TEST(AttacksTest, AttacksFromNorthEastEastDetectsBlackKnight) {
    auto board = create_empty_board();
    const Square knight_square = E4 + NORTH_EAST_EAST;
    board[knight_square] = Black_Knight;
    
    EXPECT_TRUE(attacks_from_north_east_east(board, Color::Black, knight_square));
}

TEST(AttacksTest, AttacksFromSouthEastEastDetectsWhiteKnight) {
    auto board = create_empty_board();
    const Square knight_square = E4 + SOUTH_EAST_EAST;
    board[knight_square] = White_Knight;
    
    EXPECT_TRUE(attacks_from_south_east_east(board, Color::White, knight_square));
}

TEST(AttacksTest, AttacksFromSouthSouthEastDetectsBlackKnight) {
    auto board = create_empty_board();
    const Square knight_square = E4 + SOUTH_SOUTH_EAST;
    board[knight_square] = Black_Knight;
    
    EXPECT_TRUE(attacks_from_south_south_east(board, Color::Black, knight_square));
}

TEST(AttacksTest, AttacksFromSouthSouthWestDetectsWhiteKnight) {
    auto board = create_empty_board();
    const Square knight_square = E4 + SOUTH_SOUTH_WEST;
    board[knight_square] = White_Knight;
    
    EXPECT_TRUE(attacks_from_south_south_west(board, Color::White, knight_square));
}

TEST(AttacksTest, AttacksFromSouthWestWestDetectsBlackKnight) {
    auto board = create_empty_board();
    const Square knight_square = E4 + SOUTH_WEST_WEST;
    board[knight_square] = Black_Knight;
    
    EXPECT_TRUE(attacks_from_south_west_west(board, Color::Black, knight_square));
}

TEST(AttacksTest, AttacksFromNorthWestWestDetectsWhiteKnight) {
    auto board = create_empty_board();
    const Square knight_square = E4 + NORTH_WEST_WEST;
    board[knight_square] = White_Knight;
    
    EXPECT_TRUE(attacks_from_north_west_west(board, Color::White, knight_square));
}

TEST(AttacksTest, AttacksFromNorthNorthWestDetectsBlackKnight) {
    auto board = create_empty_board();
    const Square knight_square = E4 + NORTH_NORTH_WEST;
    board[knight_square] = Black_Knight;
    
    EXPECT_TRUE(attacks_from_north_north_west(board, Color::Black, knight_square));
}

TEST(AttacksTest, KnightDirectionDetectsNoAttackWhenEmpty) {
    auto board = create_empty_board();
    const Square empty_square = E4 + NORTH_NORTH_EAST;
    // Square is empty
    
    EXPECT_FALSE(attacks_from_north_north_east(board, Color::White, empty_square));
    EXPECT_FALSE(attacks_from_north_north_east(board, Color::Black, empty_square));
}

TEST(AttacksTest, KnightDirectionDetectsNoAttackWhenWrongPiece) {
    auto board = create_empty_board();
    const Square square = E4 + NORTH_NORTH_EAST;
    board[square] = White_Rook;  // Rook, not knight
    
    EXPECT_FALSE(attacks_from_north_north_east(board, Color::White, square));
}

// ====== Tests for Placeholder Function ======

TEST(AttacksTest, PlaceholderAlwaysReturnsFalse) {
    auto board = create_empty_board();
    board[E4] = White_Queen;  // Any piece
    
    EXPECT_FALSE(attacks_from_placeholder(board, Color::White, E4));
    EXPECT_FALSE(attacks_from_placeholder(board, Color::Black, E4));
}

// ====== Tests for attacks_to Array ======

TEST(AttacksTest, AttacksToArrayInitialization) {
    initialize_attacks_to();
    
    // Verify that valid directions map to correct functions
    EXPECT_EQ(attacks_to[NORTH + DIRECTION_OFFSET], attacks_from_north);
    EXPECT_EQ(attacks_to[SOUTH + DIRECTION_OFFSET], attacks_from_south);
    EXPECT_EQ(attacks_to[EAST + DIRECTION_OFFSET], attacks_from_east);
    EXPECT_EQ(attacks_to[WEST + DIRECTION_OFFSET], attacks_from_west);
    
    EXPECT_EQ(attacks_to[NORTH_EAST + DIRECTION_OFFSET], attacks_from_north_east);
    EXPECT_EQ(attacks_to[NORTH_WEST + DIRECTION_OFFSET], attacks_from_north_west);
    EXPECT_EQ(attacks_to[SOUTH_EAST + DIRECTION_OFFSET], attacks_from_south_east);
    EXPECT_EQ(attacks_to[SOUTH_WEST + DIRECTION_OFFSET], attacks_from_south_west);
    
    EXPECT_EQ(attacks_to[NORTH_NORTH_EAST + DIRECTION_OFFSET], attacks_from_north_north_east);
    EXPECT_EQ(attacks_to[NORTH_EAST_EAST + DIRECTION_OFFSET], attacks_from_north_east_east);
    EXPECT_EQ(attacks_to[SOUTH_EAST_EAST + DIRECTION_OFFSET], attacks_from_south_east_east);
    EXPECT_EQ(attacks_to[SOUTH_SOUTH_EAST + DIRECTION_OFFSET], attacks_from_south_south_east);
    EXPECT_EQ(attacks_to[SOUTH_SOUTH_WEST + DIRECTION_OFFSET], attacks_from_south_south_west);
    EXPECT_EQ(attacks_to[SOUTH_WEST_WEST + DIRECTION_OFFSET], attacks_from_south_west_west);
    EXPECT_EQ(attacks_to[NORTH_WEST_WEST + DIRECTION_OFFSET], attacks_from_north_west_west);
    EXPECT_EQ(attacks_to[NORTH_NORTH_WEST + DIRECTION_OFFSET], attacks_from_north_north_west);
}

TEST(AttacksTest, AttacksToArrayUsesPlaceholderForInvalidDirections) {
    initialize_attacks_to();
    
    // Test a few invalid directions (not one of the 16 valid directions)
    // Direction 0 (NO_DIRECTION) should map to placeholder
    EXPECT_EQ(attacks_to[NO_DIRECTION + DIRECTION_OFFSET], attacks_from_placeholder);
    
    // Other unused directions should also map to placeholder
    const Direction unused_direction_1 = 2;  // Not a valid direction
    EXPECT_EQ(attacks_to[unused_direction_1 + DIRECTION_OFFSET], attacks_from_placeholder);
    
    const Direction unused_direction_2 = -5;  // Not a valid direction
    EXPECT_EQ(attacks_to[unused_direction_2 + DIRECTION_OFFSET], attacks_from_placeholder);
}

TEST(AttacksTest, AttacksToArrayFunctionalTest) {
    initialize_attacks_to();
    
    auto board = create_empty_board();
    board[E8] = White_Rook;  // Rook on E8
    
    // Use attacks_to array to check attack from north
    const auto attack_func = attacks_to[NORTH + DIRECTION_OFFSET];
    EXPECT_TRUE(attack_func(board, Color::White, E4));
}

// ====== Tests for Frame Handling ======

TEST(AttacksTest, SlidingAttackStopsAtFrame) {
    auto board = create_empty_board();
    board[A4] = White_Rook;  // Rook on A4
    
    // Rook on A4 cannot attack beyond the west frame
    // The scan should stop at the frame before going off board
    EXPECT_TRUE(attacks_from_west(board, Color::White, E4));  // Can attack E4
}

TEST(AttacksTest, EmptySquareNoAttack) {
    auto board = create_empty_board();
    // No pieces, just empty board
    
    EXPECT_FALSE(attacks_from_north(board, Color::White, E4));
    EXPECT_FALSE(attacks_from_south(board, Color::Black, E4));
    EXPECT_FALSE(attacks_from_east(board, Color::White, E4));
    EXPECT_FALSE(attacks_from_west(board, Color::Black, E4));
    EXPECT_FALSE(attacks_from_north_east(board, Color::White, E4));
    EXPECT_FALSE(attacks_from_north_west(board, Color::Black, E4));
    EXPECT_FALSE(attacks_from_south_east(board, Color::White, E4));
    EXPECT_FALSE(attacks_from_south_west(board, Color::Black, E4));
}

// ====== Tests for Attack Type Structures ======

TEST(AttacksTest, AttackStructHoldsOriginAndPiece) {
    Attack attack;
    attack.origin = E4;
    attack.piece = White_Knight;
    
    EXPECT_EQ(attack.origin, E4);
    EXPECT_EQ(attack.piece, White_Knight);
}

TEST(AttacksTest, AttackCollectionCanStoreMultipleAttackers) {
    AttackCollection collection;
    collection.number_of_attackers = 2;
    
    collection.attackers[0].origin = E8;
    collection.attackers[0].piece = White_Rook;
    
    collection.attackers[1].origin = H5;
    collection.attackers[1].piece = White_Bishop;
    
    EXPECT_EQ(collection.number_of_attackers, 2);
    EXPECT_EQ(collection.attackers[0].origin, E8);
    EXPECT_EQ(collection.attackers[0].piece, White_Rook);
    EXPECT_EQ(collection.attackers[1].origin, H5);
    EXPECT_EQ(collection.attackers[1].piece, White_Bishop);
}

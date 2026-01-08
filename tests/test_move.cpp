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
#include "adachess/board/move.hpp"
#include "adachess/board/square.hpp"
#include <gtest/gtest.h>

using namespace adachess::board;
using namespace adachess::core;

// ====== Tests for Move_Type Structure ======

TEST(MoveTest, Move_TypeSizeIsReasonable) {
    // Move_Type should be compact (Ada aims for 64-80 bits)
    // With packing, we aim for ≤16 bytes
    EXPECT_LE(sizeof(Move_Type), 16) << "Move_Type should be compact for cache efficiency";
}

TEST(MoveTest, EmptyMoveHasCorrectDefaults) {
    EXPECT_EQ(Empty_Move.piece, Piece::Empty);
    EXPECT_EQ(Empty_Move.captured, Piece::Empty);
    EXPECT_EQ(Empty_Move.from, 0);
    EXPECT_EQ(Empty_Move.to, 0);
    EXPECT_EQ(Empty_Move.flag, Flag_Type::No_Move);
    EXPECT_EQ(Empty_Move.promotion, Piece::Empty);
    EXPECT_EQ(Empty_Move.check, Check_Type::No_Check);
    EXPECT_EQ(Empty_Move.ambiguous_flag, Ambiguous_Flag_Type::Ambiguous_None);
}

TEST(MoveTest, MoveFieldsAreAccessible) {
    Move_Type move;
    move.piece = Piece::White_Pawn;
    move.captured = Piece::Black_Knight;
    move.from = E2;
    move.to = E4;
    move.flag = Flag_Type::Pawn_Move_Two_Square;
    move.promotion = Piece::Empty;
    move.check = Check_Type::No_Check;
    move.ambiguous_flag = Ambiguous_Flag_Type::Ambiguous_File;
    
    EXPECT_EQ(move.piece, Piece::White_Pawn);
    EXPECT_EQ(move.captured, Piece::Black_Knight);
    EXPECT_EQ(move.from, E2);
    EXPECT_EQ(move.to, E4);
    EXPECT_EQ(move.flag, Flag_Type::Pawn_Move_Two_Square);
    EXPECT_EQ(move.promotion, Piece::Empty);
    EXPECT_EQ(move.check, Check_Type::No_Check);
    EXPECT_EQ(move.ambiguous_flag, Ambiguous_Flag_Type::Ambiguous_File);
}

// ====== Tests for Move Equality ======

TEST(MoveTest, EqualMovesCompareEqual) {
    Move_Type move1;
    move1.piece = Piece::White_Knight;
    move1.from = B1;
    move1.to = C3;
    move1.captured = Piece::Empty;
    move1.promotion = Piece::Empty;
    move1.flag = Flag_Type::Standard_Move;
    move1.check = Check_Type::No_Check;
    
    Move_Type move2 = move1;
    
    EXPECT_TRUE(move1 == move2);
    EXPECT_FALSE(move1 != move2);
}

TEST(MoveTest, DifferentFromSquareComparesNotEqual) {
    Move_Type move1;
    move1.from = E2;
    move1.to = E4;
    
    Move_Type move2 = move1;
    move2.from = E3;
    
    EXPECT_FALSE(move1 == move2);
    EXPECT_TRUE(move1 != move2);
}

TEST(MoveTest, DifferentToSquareComparesNotEqual) {
    Move_Type move1;
    move1.from = E2;
    move1.to = E4;
    
    Move_Type move2 = move1;
    move2.to = E5;
    
    EXPECT_FALSE(move1 == move2);
    EXPECT_TRUE(move1 != move2);
}

TEST(MoveTest, DifferentPieceComparesNotEqual) {
    Move_Type move1;
    move1.piece = Piece::White_Pawn;
    move1.from = E2;
    move1.to = E4;
    
    Move_Type move2 = move1;
    move2.piece = Piece::White_Knight;
    
    EXPECT_FALSE(move1 == move2);
}

TEST(MoveTest, DifferentPromotionComparesNotEqual) {
    Move_Type move1;
    move1.from = E7;
    move1.to = E8;
    move1.promotion = Piece::White_Queen;
    
    Move_Type move2 = move1;
    move2.promotion = Piece::White_Rook;
    
    EXPECT_FALSE(move1 == move2);
}

// ====== Tests for Move Classification Helpers ======

TEST(MoveTest, Move_Is_CaptureDetectsCapturedPiece) {
    Move_Type move;
    move.captured = Piece::Black_Knight;
    move.flag = Flag_Type::Standard_Move;
    
    EXPECT_TRUE(move_is_capture(move));
}

TEST(MoveTest, Move_Is_CaptureDetectsEnPassant) {
    Move_Type move;
    move.captured = Piece::Empty;
    move.flag = Flag_Type::Capture_En_Passant;
    
    EXPECT_TRUE(move_is_capture(move));
}

TEST(MoveTest, Move_Is_CaptureReturnsFalseForQuietMove) {
    Move_Type move;
    move.captured = Piece::Empty;
    move.flag = Flag_Type::Standard_Move;
    
    EXPECT_FALSE(move_is_capture(move));
}

TEST(MoveTest, Move_Is_TacticalDetectsCapture) {
    Move_Type move;
    move.captured = Piece::Black_Pawn;
    move.check = Check_Type::No_Check;
    move.promotion = Piece::Empty;
    move.flag = Flag_Type::Standard_Move;
    
    EXPECT_TRUE(move_is_tactical(move));
}

TEST(MoveTest, Move_Is_TacticalDetectsCheck) {
    Move_Type move;
    move.captured = Piece::Empty;
    move.check = Check_Type::Direct_Check;
    move.promotion = Piece::Empty;
    move.flag = Flag_Type::Standard_Move;
    
    EXPECT_TRUE(move_is_tactical(move));
}

TEST(MoveTest, Move_Is_TacticalDetectsPromotion) {
    Move_Type move;
    move.captured = Piece::Empty;
    move.check = Check_Type::No_Check;
    move.promotion = Piece::White_Queen;
    move.flag = Flag_Type::Promotion;
    
    EXPECT_TRUE(move_is_tactical(move));
}

TEST(MoveTest, Move_Is_TacticalDetectsCastle) {
    Move_Type move;
    move.captured = Piece::Empty;
    move.check = Check_Type::No_Check;
    move.promotion = Piece::Empty;
    move.flag = Flag_Type::Castle;
    
    EXPECT_TRUE(move_is_tactical(move));
}

TEST(MoveTest, Move_Is_TacticalReturnsFalseForQuietMove) {
    Move_Type move;
    move.captured = Piece::Empty;
    move.check = Check_Type::No_Check;
    move.promotion = Piece::Empty;
    move.flag = Flag_Type::Standard_Move;
    
    EXPECT_FALSE(move_is_tactical(move));
}

TEST(MoveTest, Move_Is_QuietReturnsTrueForQuietMove) {
    Move_Type move;
    move.captured = Piece::Empty;
    move.check = Check_Type::No_Check;
    move.promotion = Piece::Empty;
    move.flag = Flag_Type::Standard_Move;
    
    EXPECT_TRUE(move_is_quiet(move));
}

TEST(MoveTest, Move_Is_QuietReturnsFalseForTacticalMove) {
    Move_Type move;
    move.captured = Piece::Black_Pawn;
    move.check = Check_Type::No_Check;
    move.promotion = Piece::Empty;
    move.flag = Flag_Type::Standard_Move;
    
    EXPECT_FALSE(move_is_quiet(move));
}

// ====== Tests for Annotation Types ======

TEST(MoveTest, AnnotatedMoveHasCorrectDefaults) {
    EXPECT_EQ(Empty_Annotated_Move.move, Empty_Move);
    EXPECT_EQ(Empty_Annotated_Move.annotation, Annotation_Type::None);
}

TEST(MoveTest, AnnotatedMoveFieldsAreAccessible) {
    Annotated_Move_Type annotated;
    annotated.move.from = E2;
    annotated.move.to = E4;
    annotated.annotation = Annotation_Type::Good;
    
    EXPECT_EQ(annotated.move.from, E2);
    EXPECT_EQ(annotated.move.to, E4);
    EXPECT_EQ(annotated.annotation, Annotation_Type::Good);
}

TEST(MoveTest, AnnotatedMoveEqualityComparesMovesOnly) {
    Annotated_Move_Type ann1;
    ann1.move.from = E2;
    ann1.move.to = E4;
    ann1.annotation = Annotation_Type::Good;
    
    Annotated_Move_Type ann2;
    ann2.move.from = E2;
    ann2.move.to = E4;
    ann2.annotation = Annotation_Type::Brilliant;  // Different annotation
    
    // Equality only compares moves, not annotations
    EXPECT_TRUE(ann1 == ann2);
}

TEST(MoveTest, AnnotatedMoveCanCompareWithPlainMove) {
    Move_Type move;
    move.from = E2;
    move.to = E4;
    
    Annotated_Move_Type annotated;
    annotated.move = move;
    annotated.annotation = Annotation_Type::Interesting;
    
    EXPECT_TRUE(move == annotated);
    EXPECT_TRUE(annotated == move);
}

// ====== Tests for History_Move_Type ======

TEST(MoveTest, HistoryMoveHasCorrectDefaults) {
    EXPECT_EQ(Empty_History_Move.move, Empty_Move);
    EXPECT_EQ(Empty_History_Move.hash, 0);
    EXPECT_EQ(Empty_History_Move.fifty, 0);
}

TEST(MoveTest, HistoryMoveFieldsAreAccessible) {
    History_Move_Type history;
    history.move.from = E2;
    history.move.to = E4;
    history.hash = 0x123456789ABCDEF0ULL;
    history.fifty = 5;
    
    EXPECT_EQ(history.move.from, E2);
    EXPECT_EQ(history.move.to, E4);
    EXPECT_EQ(history.hash, 0x123456789ABCDEF0ULL);
    EXPECT_EQ(history.fifty, 5);
}

TEST(MoveTest, HistoryMoveEqualityComparesMovesOnly) {
    History_Move_Type hist1;
    hist1.move.from = E2;
    hist1.move.to = E4;
    hist1.hash = 0x111;
    hist1.fifty = 1;
    
    History_Move_Type hist2;
    hist2.move.from = E2;
    hist2.move.to = E4;
    hist2.hash = 0x222;  // Different hash
    hist2.fifty = 2;     // Different fifty
    
    // Equality only compares moves
    EXPECT_TRUE(hist1 == hist2);
}

// ====== Tests for Register_Move System ======

TEST(MoveTest, RegisterMoveAppendsToMoveStack) {
    Chessboard chessboard;
    initialize(chessboard);
    
    // Reset move pointer to 1 (start position for ply 0)
    chessboard.move_pointer[0] = 1;
    
    // Register a move
    register_move(chessboard, E2, E4);
    
    // Verify move pointer was incremented
    EXPECT_EQ(chessboard.move_pointer[0], 2);
    
    // Verify move was stored
    EXPECT_EQ(chessboard.moves[1].from, E2);
    EXPECT_EQ(chessboard.moves[1].to, E4);
}

TEST(MoveTest, RegisterMoveWithFlagSetsFlag) {
    Chessboard chessboard;
    initialize(chessboard);
    
    chessboard.move_pointer[0] = 1;
    
    register_move(chessboard, E2, E4, Flag_Type::Pawn_Move_Two_Square);
    
    EXPECT_EQ(chessboard.moves[1].flag, Flag_Type::Pawn_Move_Two_Square);
}

TEST(MoveTest, RegisterMoveWithPromotionGeneratesFourMoves) {
    Chessboard chessboard;
    initialize(chessboard);
    
    chessboard.move_pointer[0] = 1;
    
    // Register promotion (should generate 4 moves)
    register_move(chessboard, E7, E8, Flag_Type::Promotion);
    
    // Verify 4 moves were added
    EXPECT_EQ(chessboard.move_pointer[0], 5);  // 1 + 4 = 5
    
    // Verify promotions are to different pieces
    EXPECT_TRUE(chessboard.moves[1].promotion == Piece::White_Queen ||
                chessboard.moves[1].promotion == Piece::Black_Queen);
    EXPECT_TRUE(chessboard.moves[2].promotion == Piece::White_Rook ||
                chessboard.moves[2].promotion == Piece::Black_Rook);
    EXPECT_TRUE(chessboard.moves[3].promotion == Piece::White_Bishop ||
                chessboard.moves[3].promotion == Piece::Black_Bishop);
    EXPECT_TRUE(chessboard.moves[4].promotion == Piece::White_Knight ||
                chessboard.moves[4].promotion == Piece::Black_Knight);
}

TEST(MoveTest, RegisterMoveWithPreConstructedMoveStoresMove) {
    Chessboard chessboard;
    initialize(chessboard);
    
    chessboard.move_pointer[0] = 1;
    
    Move_Type move;
    move.from = B1;
    move.to = C3;
    move.piece = Piece::White_Knight;
    move.captured = Piece::Empty;
    move.flag = Flag_Type::Standard_Move;
    
    register_move(chessboard, move);
    
    EXPECT_EQ(chessboard.moves[1].from, B1);
    EXPECT_EQ(chessboard.moves[1].to, C3);
    EXPECT_EQ(chessboard.moves[1].piece, Piece::White_Knight);
}

// ====== Tests for Register_Tactical_Move System ======

TEST(MoveTest, RegisterTacticalMoveRegistersCapture) {
    Chessboard chessboard;
    initialize(chessboard);
    
    // Place a black piece on E5 to be captured
    chessboard.board[E5] = Piece::Black_Pawn;
    
    chessboard.move_pointer[0] = 1;
    
    // This move should be registered because it's a capture
    register_tactical_move(chessboard, E4, E5);
    
    // Verify move was registered
    EXPECT_EQ(chessboard.move_pointer[0], 2);
}

TEST(MoveTest, RegisterTacticalMoveFiltersQuietMove) {
    Chessboard chessboard;
    initialize(chessboard);
    
    chessboard.move_pointer[0] = 1;
    
    // Create a quiet move (no capture, no check, etc.)
    Move_Type quiet_move;
    quiet_move.from = E2;
    quiet_move.to = E4;
    quiet_move.piece = Piece::White_Pawn;
    quiet_move.captured = Piece::Empty;
    quiet_move.flag = Flag_Type::Standard_Move;
    quiet_move.check = Check_Type::No_Check;
    quiet_move.promotion = Piece::Empty;
    
    register_tactical_move(chessboard, quiet_move);
    
    // Verify move was NOT registered (pointer unchanged)
    EXPECT_EQ(chessboard.move_pointer[0], 1);
}

TEST(MoveTest, RegisterTacticalMoveRegistersCastle) {
    Chessboard chessboard;
    initialize(chessboard);
    
    chessboard.move_pointer[0] = 1;
    
    // Castle is considered tactical
    Move_Type castle_move;
    castle_move.from = E1;
    castle_move.to = G1;
    castle_move.piece = Piece::White_King;
    castle_move.captured = Piece::Empty;
    castle_move.flag = Flag_Type::Castle;
    castle_move.check = Check_Type::No_Check;
    castle_move.promotion = Piece::Empty;
    
    register_tactical_move(chessboard, castle_move);
    
    // Verify move was registered
    EXPECT_EQ(chessboard.move_pointer[0], 2);
}

// ====== Tests for Check_Type operator! ======

TEST(MoveTest, CheckTypeNegationOperator) {
    EXPECT_TRUE(!Check_Type::No_Check);
    EXPECT_FALSE(!Check_Type::Direct_Check);
    EXPECT_FALSE(!Check_Type::Discovery_Check);
    EXPECT_FALSE(!Check_Type::Double_Check);
    EXPECT_FALSE(!Check_Type::Checkmate);
}

// AdaChess - C++ Chess Engine
// Unit tests for Move structures and operations

#include "adachess/board/moves.hpp"
#include "adachess/board/chessboard.hpp"
#include "adachess/engine/annotations.hpp"
#include "adachess/engine/move_registration.hpp"
#include "adachess/core/history.hpp"
#include <gtest/gtest.h>

using namespace adachess;

// Test Move_Type structure size
TEST(MoveTest, MoveSize) {
    // Move should be reasonably compact (target ≤ 16 bytes)
    // Document actual size for reference
    EXPECT_LE(sizeof(Move), 32u) << "Move size: " << sizeof(Move) << " bytes";
}

// Test Move_Type field layout
TEST(MoveTest, MoveFieldLayout) {
    Move move;
    move.piece = Piece::White_Pawn;
    move.captured = Piece::Black_Knight;
    move.from = 91;  // A1
    move.to = 81;    // A2
    move.flag = MoveFlag::StandardMove;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous = AmbiguousFlag::None;

    EXPECT_EQ(move.piece, Piece::White_Pawn);
    EXPECT_EQ(move.captured, Piece::Black_Knight);
    EXPECT_EQ(move.from, 91u);
    EXPECT_EQ(move.to, 81u);
    EXPECT_EQ(move.flag, MoveFlag::StandardMove);
    EXPECT_EQ(move.promotion, Piece::Empty);
    EXPECT_EQ(move.check, CheckType::NoCheck);
    EXPECT_EQ(move.ambiguous, AmbiguousFlag::None);
}

// Test Empty_Move constant
TEST(MoveTest, EmptyMove) {
    EXPECT_EQ(EMPTY_MOVE.piece, Piece::Empty);
    EXPECT_EQ(EMPTY_MOVE.captured, Piece::Empty);
    EXPECT_EQ(EMPTY_MOVE.from, 0u);
    EXPECT_EQ(EMPTY_MOVE.to, 0u);
    EXPECT_EQ(EMPTY_MOVE.flag, MoveFlag::StandardMove);
    EXPECT_EQ(EMPTY_MOVE.promotion, Piece::Empty);
    EXPECT_EQ(EMPTY_MOVE.check, CheckType::NoCheck);
    EXPECT_EQ(EMPTY_MOVE.ambiguous, AmbiguousFlag::None);
}

// Test Move equality operator
TEST(MoveTest, MoveEquality) {
    Move move1;
    move1.piece = Piece::White_Pawn;
    move1.from = 91;
    move1.to = 81;
    move1.captured = Piece::Empty;
    move1.flag = MoveFlag::StandardMove;
    move1.promotion = Piece::Empty;
    move1.check = CheckType::NoCheck;

    Move move2;
    move2.piece = Piece::White_Pawn;
    move2.from = 91;
    move2.to = 81;
    move2.captured = Piece::Empty;
    move2.flag = MoveFlag::StandardMove;
    move2.promotion = Piece::Empty;
    move2.check = CheckType::NoCheck;

    EXPECT_EQ(move1, move2);

    // Change one field
    move2.to = 71;
    EXPECT_NE(move1, move2);
}

// Test Move_Is_Capture
TEST(MoveTest, MoveIsCapture) {
    Move move;
    move.captured = Piece::Black_Knight;
    EXPECT_TRUE(move_is_capture(move));

    move.captured = Piece::Empty;
    EXPECT_FALSE(move_is_capture(move));

    // En passant is also a capture
    move.flag = MoveFlag::CaptureEnPassant;
    EXPECT_TRUE(move_is_capture(move));
}

// Test Move_Is_Tactical
TEST(MoveTest, MoveIsTactical) {
    Move move;

    // Capture is tactical
    move.captured = Piece::Black_Pawn;
    EXPECT_TRUE(move_is_tactical(move));

    // Check is tactical
    move.captured = Piece::Empty;
    move.check = CheckType::DirectCheck;
    EXPECT_TRUE(move_is_tactical(move));

    // Promotion is tactical
    move.check = CheckType::NoCheck;
    move.promotion = Piece::White_Queen;
    EXPECT_TRUE(move_is_tactical(move));

    // Castle is tactical
    move.promotion = Piece::Empty;
    move.flag = MoveFlag::Castle;
    EXPECT_TRUE(move_is_tactical(move));

    // En passant is tactical
    move.flag = MoveFlag::CaptureEnPassant;
    EXPECT_TRUE(move_is_tactical(move));

    // Standard quiet move is not tactical
    move.flag = MoveFlag::StandardMove;
    EXPECT_FALSE(move_is_tactical(move));
}

// Test Move_Is_Quiet
TEST(MoveTest, MoveIsQuiet) {
    Move move;
    move.flag = MoveFlag::StandardMove;
    move.captured = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.promotion = Piece::Empty;

    EXPECT_TRUE(move_is_quiet(move));

    // Any tactical move is not quiet
    move.captured = Piece::Black_Pawn;
    EXPECT_FALSE(move_is_quiet(move));
}

// Test CheckType helper
TEST(MoveTest, IsCheck) {
    EXPECT_FALSE(is_check(CheckType::NoCheck));
    EXPECT_TRUE(is_check(CheckType::DirectCheck));
    EXPECT_TRUE(is_check(CheckType::DiscoveryCheck));
    EXPECT_TRUE(is_check(CheckType::DoubleCheck));
    EXPECT_TRUE(is_check(CheckType::Checkmate));
    EXPECT_TRUE(is_check(CheckType::UnknownCheck));
}

// Test Annotation_Type
TEST(AnnotationTest, AnnotationType) {
    Annotation annot = Annotation::None;
    EXPECT_EQ(annot, Annotation::None);

    annot = Annotation::Brilliant;
    EXPECT_EQ(annot, Annotation::Brilliant);
}

// Test Annotated_Move_Type
TEST(AnnotationTest, AnnotatedMove) {
    Move move;
    move.from = 91;
    move.to = 81;

    AnnotatedMove annotated(move, Annotation::Good);
    EXPECT_EQ(annotated.move, move);
    EXPECT_EQ(annotated.annotation, Annotation::Good);
}

// Test Annotated_Move equality (compares only move part)
TEST(AnnotationTest, AnnotatedMoveEquality) {
    Move move1;
    move1.from = 91;
    move1.to = 81;

    AnnotatedMove annotated1(move1, Annotation::Good);
    AnnotatedMove annotated2(move1, Annotation::Brilliant);

    // Equality compares only move, not annotation
    EXPECT_EQ(annotated1, annotated2);

    Move move2;
    move2.from = 92;
    move2.to = 82;

    AnnotatedMove annotated3(move2, Annotation::Good);
    EXPECT_NE(annotated1, annotated3);
}

// Test mixed equality: Move and AnnotatedMove
TEST(AnnotationTest, MixedEquality) {
    Move move;
    move.from = 91;
    move.to = 81;

    AnnotatedMove annotated(move, Annotation::Good);

    EXPECT_EQ(move, annotated);
    EXPECT_EQ(annotated, move);
}

// Test Empty_Annotated_Move
TEST(AnnotationTest, EmptyAnnotatedMove) {
    EXPECT_EQ(EMPTY_ANNOTATED_MOVE.move, EMPTY_MOVE);
    EXPECT_EQ(EMPTY_ANNOTATED_MOVE.annotation, Annotation::None);
}

// Test History_Move_Type
TEST(HistoryTest, HistoryMove) {
    Move move;
    move.from = 91;
    move.to = 81;

    HistoryMove history(move, 12345678ULL, 5);
    EXPECT_EQ(history.move, move);
    EXPECT_EQ(history.hash, 12345678ULL);
    EXPECT_EQ(history.fifty, 5u);
}

// Test History_Move equality (compares only move part)
TEST(HistoryTest, HistoryMoveEquality) {
    Move move1;
    move1.from = 91;
    move1.to = 81;

    HistoryMove history1(move1, 12345678ULL, 5);
    HistoryMove history2(move1, 87654321ULL, 10);

    // Equality compares only move
    EXPECT_EQ(history1, history2);

    Move move2;
    move2.from = 92;
    move2.to = 82;

    HistoryMove history3(move2, 12345678ULL, 5);
    EXPECT_NE(history1, history3);
}

// Test Empty_History_Move
TEST(HistoryTest, EmptyHistoryMove) {
    EXPECT_EQ(EMPTY_HISTORY_MOVE.move, EMPTY_MOVE);
    EXPECT_EQ(EMPTY_HISTORY_MOVE.hash, 0ULL);
    EXPECT_EQ(EMPTY_HISTORY_MOVE.fifty, 0u);
}

// Test Chessboard initialization
TEST(ChessboardTest, ChessboardInit) {
    Chessboard board;
    EXPECT_EQ(board.ply, ZERO_DEPTH);
    EXPECT_EQ(board.side_to_move, Color::White);
    EXPECT_EQ(board.fifty, 0u);
}

// Test Register_Move basic
TEST(MoveRegistrationTest, RegisterMoveBasic) {
    Chessboard board;
    board.square[91] = Piece::White_Pawn;
    board.square[81] = Piece::Empty;
    board.ply = 1;
    board.moves_pointer[1] = 0;  // Start of moves at ply 1
    board.moves_pointer[2] = 0;  // Will be incremented

    register_move(board, 91, 81);

    // Check that a move was added
    EXPECT_EQ(board.moves_pointer[2], 1u);

    // Check move fields
    Move& registered = board.moves_stack[0];
    EXPECT_EQ(registered.from, 91u);
    EXPECT_EQ(registered.to, 81u);
    EXPECT_EQ(registered.piece, Piece::White_Pawn);
    EXPECT_EQ(registered.captured, Piece::Empty);
}

// Test Register_Move with promotion flag
TEST(MoveRegistrationTest, RegisterMovePromotion) {
    Chessboard board;
    board.square[28] = Piece::White_Pawn;  // H8 (about to promote)
    board.square[27] = Piece::Empty;
    board.ply = 1;
    board.side_to_move = Color::White;
    board.moves_pointer[1] = 0;
    board.moves_pointer[2] = 0;

    register_move(board, 28, 27, MoveFlag::Promotion);

    // Should generate 4 moves (Queen, Rook, Bishop, Knight)
    EXPECT_EQ(board.moves_pointer[2], 4u);

    // Check promotion pieces
    EXPECT_EQ(board.moves_stack[0].promotion, Piece::White_Queen);
    EXPECT_EQ(board.moves_stack[1].promotion, Piece::White_Rook);
    EXPECT_EQ(board.moves_stack[2].promotion, Piece::White_Bishop);
    EXPECT_EQ(board.moves_stack[3].promotion, Piece::White_Knight);
}

// Test Register_Tactical_Move filters non-tactical moves
TEST(MoveRegistrationTest, RegisterTacticalMoveFilter) {
    Chessboard board;
    board.square[91] = Piece::White_Pawn;
    board.square[81] = Piece::Empty;
    board.ply = 1;
    board.moves_pointer[1] = 0;
    board.moves_pointer[2] = 0;

    // This is a quiet move, should not be registered
    register_tactical_move(board, 91, 81);

    // No moves should be added (since it's quiet and check detection stub returns NoCheck)
    EXPECT_EQ(board.moves_pointer[2], 0u);
}

// Test Register_Tactical_Move with capture
TEST(MoveRegistrationTest, RegisterTacticalMoveCapture) {
    Chessboard board;
    board.square[91] = Piece::White_Pawn;
    board.square[81] = Piece::Black_Knight;  // Capture
    board.ply = 1;
    board.moves_pointer[1] = 0;
    board.moves_pointer[2] = 0;

    register_tactical_move(board, 91, 81);

    // Capture should be registered
    EXPECT_EQ(board.moves_pointer[2], 1u);
    EXPECT_EQ(board.moves_stack[0].captured, Piece::Black_Knight);
}

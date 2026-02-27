// ---------------------------------------------------------------------------
// AdaChess – Unit tests for foundational types
// ---------------------------------------------------------------------------
#include <gtest/gtest.h>

#include "chess/chess.hpp"
#include "chess/colors.hpp"
#include "chess/pieces.hpp"
#include "chess/depths.hpp"
#include "chess/nodes.hpp"
#include "chess/score.hpp"
#include "chess/notations.hpp"
#include "chess/distances.hpp"
#include "chess/hashes.hpp"
#include "chess/pins.hpp"
#include "chess/players.hpp"
#include "chess/moves.hpp"
#include "chess/moves_annotations.hpp"
#include "chess/history.hpp"
#include "chess/info.hpp"

// -----------------------------------------------------------------------
// Color tests
// -----------------------------------------------------------------------
TEST(ColorTest, InvertWhite) {
    EXPECT_EQ(~chess::Color::White, chess::Color::Black);
}

TEST(ColorTest, InvertBlack) {
    EXPECT_EQ(~chess::Color::Black, chess::Color::White);
}

TEST(ColorTest, DoubleInvert) {
    EXPECT_EQ(~(~chess::Color::White), chess::Color::White);
}

// -----------------------------------------------------------------------
// Piece tests
// -----------------------------------------------------------------------
TEST(PieceTest, EnumValues) {
    // Verify Ada representation clause values
    EXPECT_EQ(static_cast<int>(chess::Piece::Frame), 0);
    EXPECT_EQ(static_cast<int>(chess::Piece::Empty), 1);
    EXPECT_EQ(static_cast<int>(chess::Piece::White_Pawn), 2);
    EXPECT_EQ(static_cast<int>(chess::Piece::White_Knight), 3);
    EXPECT_EQ(static_cast<int>(chess::Piece::White_Bishop), 4);
    EXPECT_EQ(static_cast<int>(chess::Piece::White_Rook), 5);
    EXPECT_EQ(static_cast<int>(chess::Piece::White_Queen), 6);
    EXPECT_EQ(static_cast<int>(chess::Piece::White_King), 7);
    EXPECT_EQ(static_cast<int>(chess::Piece::Black_Pawn), 8);
    EXPECT_EQ(static_cast<int>(chess::Piece::Black_Knight), 9);
    EXPECT_EQ(static_cast<int>(chess::Piece::Black_Bishop), 10);
    EXPECT_EQ(static_cast<int>(chess::Piece::Black_Rook), 11);
    EXPECT_EQ(static_cast<int>(chess::Piece::Black_Queen), 12);
    EXPECT_EQ(static_cast<int>(chess::Piece::Black_King), 13);
}

TEST(PieceTest, SubtypePredicates) {
    EXPECT_TRUE(chess::is_white_piece(chess::Piece::White_Pawn));
    EXPECT_TRUE(chess::is_white_piece(chess::Piece::White_King));
    EXPECT_FALSE(chess::is_white_piece(chess::Piece::Black_Pawn));
    EXPECT_FALSE(chess::is_white_piece(chess::Piece::Empty));

    EXPECT_TRUE(chess::is_black_piece(chess::Piece::Black_Pawn));
    EXPECT_TRUE(chess::is_black_piece(chess::Piece::Black_King));
    EXPECT_FALSE(chess::is_black_piece(chess::Piece::White_Pawn));

    EXPECT_TRUE(chess::is_chess_piece(chess::Piece::White_Pawn));
    EXPECT_TRUE(chess::is_chess_piece(chess::Piece::Black_King));
    EXPECT_FALSE(chess::is_chess_piece(chess::Piece::Empty));
    EXPECT_FALSE(chess::is_chess_piece(chess::Piece::Frame));

    EXPECT_TRUE(chess::is_pawn(chess::Piece::White_Pawn));
    EXPECT_TRUE(chess::is_pawn(chess::Piece::Black_Pawn));
    EXPECT_FALSE(chess::is_pawn(chess::Piece::White_Knight));

    EXPECT_TRUE(chess::is_slider(chess::Piece::White_Bishop));
    EXPECT_TRUE(chess::is_slider(chess::Piece::Black_Rook));
    EXPECT_TRUE(chess::is_slider(chess::Piece::White_Queen));
    EXPECT_FALSE(chess::is_slider(chess::Piece::White_Knight));
    EXPECT_FALSE(chess::is_slider(chess::Piece::White_King));
}

TEST(PieceTest, Symbols) {
    EXPECT_EQ(chess::Symbols[static_cast<int>(chess::Piece::White_Pawn)], 'P');
    EXPECT_EQ(chess::Symbols[static_cast<int>(chess::Piece::Black_King)], 'k');
    EXPECT_EQ(chess::Symbols[static_cast<int>(chess::Piece::Empty)], ' ');
}

TEST(PieceTest, PieceIsEmpty) {
    EXPECT_TRUE(chess::piece_is_empty(chess::Piece::Empty));
    EXPECT_FALSE(chess::piece_is_empty(chess::Piece::White_Pawn));
}

// -----------------------------------------------------------------------
// Depth tests
// -----------------------------------------------------------------------
TEST(DepthTest, Constants) {
    EXPECT_EQ(chess::Zero_Depth, 1u);
    EXPECT_EQ(chess::Frontier_Depth, 2u);
    EXPECT_EQ(chess::Pre_Frontier_Depth, 3u);
    EXPECT_EQ(chess::Horizon, 255u);
    EXPECT_EQ(chess::Pre_Horizon, 253u);
}

// -----------------------------------------------------------------------
// Node tests
// -----------------------------------------------------------------------
TEST(NodeTest, TreeNodeFlip) {
    EXPECT_EQ(~chess::TreeNode::Pv_Node, chess::TreeNode::Pv_Node);
    EXPECT_EQ(~chess::TreeNode::Cut_Node, chess::TreeNode::All_Node);
    EXPECT_EQ(~chess::TreeNode::All_Node, chess::TreeNode::Cut_Node);
}

// -----------------------------------------------------------------------
// Score tests
// -----------------------------------------------------------------------
TEST(ScoreTest, Constants) {
    EXPECT_EQ(chess::Infinity, 900900);
    EXPECT_EQ(chess::Mate, 32767);
    EXPECT_EQ(chess::Draw, 0);
}

TEST(ScoreTest, MateDetection) {
    EXPECT_TRUE(chess::is_winning_mate(chess::Mate));
    EXPECT_TRUE(chess::is_winning_mate(chess::Mate - 50));
    EXPECT_FALSE(chess::is_winning_mate(chess::Mate - 200));

    EXPECT_TRUE(chess::is_losing_mate(-chess::Mate));
    EXPECT_TRUE(chess::is_losing_mate(-chess::Mate + 50));
    EXPECT_FALSE(chess::is_losing_mate(-chess::Mate + 200));

    EXPECT_TRUE(chess::is_mate(chess::Mate));
    EXPECT_TRUE(chess::is_mate(-chess::Mate));
    EXPECT_FALSE(chess::is_mate(100));
}

// -----------------------------------------------------------------------
// Distance tests
// -----------------------------------------------------------------------
TEST(DistanceTest, Constants) {
    EXPECT_EQ(chess::No_Distance, 0u);
    EXPECT_EQ(chess::Unreachable, 9u);
}

// -----------------------------------------------------------------------
// Move tests
// -----------------------------------------------------------------------
TEST(MoveTest, EmptyMove) {
    chess::Move m;
    EXPECT_EQ(m, chess::Empty_Move);
}

TEST(MoveTest, Equality) {
    chess::Move a;
    a.piece = chess::Piece::White_Pawn;
    a.from = 85; // e2
    a.to = 65;   // e4
    a.flag = chess::Flag::Pawn_Move_Two_Square;

    chess::Move b = a;
    b.ambiguous_flag = chess::AmbiguousFlag::Ambiguous_File; // differs

    // Ambiguous_Flag is not compared
    EXPECT_EQ(a, b);
}

TEST(MoveTest, IsCapture) {
    chess::Move m;
    EXPECT_FALSE(chess::Move_Is_Capture(m));

    m.captured = chess::Piece::Black_Pawn;
    EXPECT_TRUE(chess::Move_Is_Capture(m));

    chess::Move ep;
    ep.flag = chess::Flag::Capture_En_Passant;
    EXPECT_TRUE(chess::Move_Is_Capture(ep));
}

TEST(MoveTest, CheckNot) {
    EXPECT_TRUE(!chess::Check::No_Check);
    EXPECT_FALSE(!chess::Check::Direct_Check);
    EXPECT_FALSE(!chess::Check::Checkmate);
}

// -----------------------------------------------------------------------
// History tests
// -----------------------------------------------------------------------
TEST(HistoryTest, EmptyHistoryMove) {
    chess::HistoryMove hm;
    EXPECT_EQ(hm, chess::Empty_History_Move);
}

// -----------------------------------------------------------------------
// Info tests
// -----------------------------------------------------------------------
TEST(InfoTest, EngineName) {
    EXPECT_EQ(chess::Engine_Name(), "AdaChess");
}

TEST(InfoTest, Version) {
    EXPECT_EQ(chess::Version(), "4.0");
}

TEST(InfoTest, Author) {
    EXPECT_EQ(chess::Author(), "Alessandro Iavicoli");
}

TEST(InfoTest, ConsoleLogo) {
    auto logo = chess::Console_Logo();
    EXPECT_FALSE(logo.empty());
    EXPECT_NE(logo.find("@"), std::string_view::npos); // contains @ chars
}

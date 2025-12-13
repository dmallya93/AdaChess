//
// AdaChess - Smart Chess Engine
//
// Unit tests for core chess types (pieces, colors, board)
//

#include <adachess/pieces.hpp>
#include <adachess/colors.hpp>
#include <adachess/board.hpp>
#include <gtest/gtest.h>

using namespace adachess;

// ============================================================================
// Piece Tests
// ============================================================================

TEST(PieceTest, PieceEnumValues) {
  // Verify the enum values match Ada representation
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::Frame), 0);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::Empty), 1);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::WhitePawn), 2);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::WhiteKnight), 3);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::WhiteBishop), 4);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::WhiteRook), 5);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::WhiteQueen), 6);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::WhiteKing), 7);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::BlackPawn), 8);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::BlackKnight), 9);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::BlackBishop), 10);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::BlackRook), 11);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::BlackQueen), 12);
  EXPECT_EQ(static_cast<std::uint8_t>(Piece::BlackKing), 13);
}

TEST(PieceTest, IsEmpty) {
  EXPECT_TRUE(is_empty(Piece::Empty));
  EXPECT_FALSE(is_empty(Piece::Frame));
  EXPECT_FALSE(is_empty(Piece::WhitePawn));
  EXPECT_FALSE(is_empty(Piece::BlackKing));
}

TEST(PieceTest, IsPiece) {
  EXPECT_FALSE(is_piece(Piece::Empty));
  EXPECT_FALSE(is_piece(Piece::Frame));
  EXPECT_TRUE(is_piece(Piece::WhitePawn));
  EXPECT_TRUE(is_piece(Piece::BlackQueen));
}

TEST(PieceTest, IsBoardPiece) {
  EXPECT_FALSE(is_board_piece(Piece::Frame));
  EXPECT_TRUE(is_board_piece(Piece::Empty));
  EXPECT_TRUE(is_board_piece(Piece::WhitePawn));
  EXPECT_TRUE(is_board_piece(Piece::BlackKing));
}

TEST(PieceTest, IsChessPiece) {
  EXPECT_FALSE(is_chess_piece(Piece::Frame));
  EXPECT_FALSE(is_chess_piece(Piece::Empty));
  EXPECT_TRUE(is_chess_piece(Piece::WhitePawn));
  EXPECT_TRUE(is_chess_piece(Piece::WhiteKnight));
  EXPECT_TRUE(is_chess_piece(Piece::BlackPawn));
  EXPECT_TRUE(is_chess_piece(Piece::BlackKing));
}

TEST(PieceTest, IsWhitePiece) {
  EXPECT_TRUE(is_white_piece(Piece::WhitePawn));
  EXPECT_TRUE(is_white_piece(Piece::WhiteKnight));
  EXPECT_TRUE(is_white_piece(Piece::WhiteKing));
  EXPECT_FALSE(is_white_piece(Piece::BlackPawn));
  EXPECT_FALSE(is_white_piece(Piece::Empty));
}

TEST(PieceTest, IsBlackPiece) {
  EXPECT_TRUE(is_black_piece(Piece::BlackPawn));
  EXPECT_TRUE(is_black_piece(Piece::BlackKnight));
  EXPECT_TRUE(is_black_piece(Piece::BlackKing));
  EXPECT_FALSE(is_black_piece(Piece::WhitePawn));
  EXPECT_FALSE(is_black_piece(Piece::Empty));
}

TEST(PieceTest, IsWhitePromotion) {
  EXPECT_FALSE(is_white_promotion(Piece::WhitePawn));
  EXPECT_TRUE(is_white_promotion(Piece::WhiteKnight));
  EXPECT_TRUE(is_white_promotion(Piece::WhiteBishop));
  EXPECT_TRUE(is_white_promotion(Piece::WhiteRook));
  EXPECT_TRUE(is_white_promotion(Piece::WhiteQueen));
  EXPECT_FALSE(is_white_promotion(Piece::WhiteKing));
  EXPECT_FALSE(is_white_promotion(Piece::BlackKnight));
}

TEST(PieceTest, IsBlackPromotion) {
  EXPECT_FALSE(is_black_promotion(Piece::BlackPawn));
  EXPECT_TRUE(is_black_promotion(Piece::BlackKnight));
  EXPECT_TRUE(is_black_promotion(Piece::BlackBishop));
  EXPECT_TRUE(is_black_promotion(Piece::BlackRook));
  EXPECT_TRUE(is_black_promotion(Piece::BlackQueen));
  EXPECT_FALSE(is_black_promotion(Piece::BlackKing));
  EXPECT_FALSE(is_black_promotion(Piece::WhiteKnight));
}

TEST(PieceTest, IsWhiteNonPawn) {
  EXPECT_FALSE(is_white_non_pawn(Piece::WhitePawn));
  EXPECT_TRUE(is_white_non_pawn(Piece::WhiteKnight));
  EXPECT_TRUE(is_white_non_pawn(Piece::WhiteBishop));
  EXPECT_TRUE(is_white_non_pawn(Piece::WhiteRook));
  EXPECT_TRUE(is_white_non_pawn(Piece::WhiteQueen));
  EXPECT_FALSE(is_white_non_pawn(Piece::WhiteKing));
}

TEST(PieceTest, IsBlackNonPawn) {
  EXPECT_FALSE(is_black_non_pawn(Piece::BlackPawn));
  EXPECT_TRUE(is_black_non_pawn(Piece::BlackKnight));
  EXPECT_TRUE(is_black_non_pawn(Piece::BlackBishop));
  EXPECT_TRUE(is_black_non_pawn(Piece::BlackRook));
  EXPECT_TRUE(is_black_non_pawn(Piece::BlackQueen));
  EXPECT_FALSE(is_black_non_pawn(Piece::BlackKing));
}

TEST(PieceTest, IsPawn) {
  EXPECT_TRUE(is_pawn(Piece::WhitePawn));
  EXPECT_TRUE(is_pawn(Piece::BlackPawn));
  EXPECT_FALSE(is_pawn(Piece::WhiteKnight));
  EXPECT_FALSE(is_pawn(Piece::BlackQueen));
}

TEST(PieceTest, IsKnight) {
  EXPECT_TRUE(is_knight(Piece::WhiteKnight));
  EXPECT_TRUE(is_knight(Piece::BlackKnight));
  EXPECT_FALSE(is_knight(Piece::WhitePawn));
  EXPECT_FALSE(is_knight(Piece::BlackBishop));
}

TEST(PieceTest, IsBishop) {
  EXPECT_TRUE(is_bishop(Piece::WhiteBishop));
  EXPECT_TRUE(is_bishop(Piece::BlackBishop));
  EXPECT_FALSE(is_bishop(Piece::WhiteRook));
  EXPECT_FALSE(is_bishop(Piece::BlackKnight));
}

TEST(PieceTest, IsRook) {
  EXPECT_TRUE(is_rook(Piece::WhiteRook));
  EXPECT_TRUE(is_rook(Piece::BlackRook));
  EXPECT_FALSE(is_rook(Piece::WhiteBishop));
  EXPECT_FALSE(is_rook(Piece::BlackQueen));
}

TEST(PieceTest, IsQueen) {
  EXPECT_TRUE(is_queen(Piece::WhiteQueen));
  EXPECT_TRUE(is_queen(Piece::BlackQueen));
  EXPECT_FALSE(is_queen(Piece::WhiteRook));
  EXPECT_FALSE(is_queen(Piece::BlackKing));
}

TEST(PieceTest, IsKing) {
  EXPECT_TRUE(is_king(Piece::WhiteKing));
  EXPECT_TRUE(is_king(Piece::BlackKing));
  EXPECT_FALSE(is_king(Piece::WhiteQueen));
  EXPECT_FALSE(is_king(Piece::BlackPawn));
}

TEST(PieceTest, IsSlidingPiece) {
  EXPECT_TRUE(is_sliding_piece(Piece::WhiteBishop));
  EXPECT_TRUE(is_sliding_piece(Piece::WhiteRook));
  EXPECT_TRUE(is_sliding_piece(Piece::WhiteQueen));
  EXPECT_TRUE(is_sliding_piece(Piece::BlackBishop));
  EXPECT_TRUE(is_sliding_piece(Piece::BlackRook));
  EXPECT_TRUE(is_sliding_piece(Piece::BlackQueen));
  EXPECT_FALSE(is_sliding_piece(Piece::WhitePawn));
  EXPECT_FALSE(is_sliding_piece(Piece::WhiteKnight));
  EXPECT_FALSE(is_sliding_piece(Piece::BlackKing));
}

TEST(PieceTest, PieceSymbols) {
  EXPECT_EQ(piece_to_symbol(Piece::Frame), ' ');
  EXPECT_EQ(piece_to_symbol(Piece::Empty), ' ');
  EXPECT_EQ(piece_to_symbol(Piece::WhitePawn), 'P');
  EXPECT_EQ(piece_to_symbol(Piece::WhiteKnight), 'N');
  EXPECT_EQ(piece_to_symbol(Piece::WhiteBishop), 'B');
  EXPECT_EQ(piece_to_symbol(Piece::WhiteRook), 'R');
  EXPECT_EQ(piece_to_symbol(Piece::WhiteQueen), 'Q');
  EXPECT_EQ(piece_to_symbol(Piece::WhiteKing), 'K');
  EXPECT_EQ(piece_to_symbol(Piece::BlackPawn), 'p');
  EXPECT_EQ(piece_to_symbol(Piece::BlackKnight), 'n');
  EXPECT_EQ(piece_to_symbol(Piece::BlackBishop), 'b');
  EXPECT_EQ(piece_to_symbol(Piece::BlackRook), 'r');
  EXPECT_EQ(piece_to_symbol(Piece::BlackQueen), 'q');
  EXPECT_EQ(piece_to_symbol(Piece::BlackKing), 'k');
}

TEST(PieceTest, PieceCounterConstants) {
  EXPECT_EQ(max_pieces_per_side, 16);
  EXPECT_EQ(max_pawns_per_side, 8);
}

// ============================================================================
// Color Tests
// ============================================================================

TEST(ColorTest, ColorEnumValues) {
  EXPECT_EQ(static_cast<std::uint8_t>(Color::White), 0);
  EXPECT_EQ(static_cast<std::uint8_t>(Color::Black), 1);
}

TEST(ColorTest, OppositeColor) {
  EXPECT_EQ(opposite(Color::White), Color::Black);
  EXPECT_EQ(opposite(Color::Black), Color::White);
}

TEST(ColorTest, SwapColor) {
  EXPECT_EQ(swap_color(Color::White), Color::Black);
  EXPECT_EQ(swap_color(Color::Black), Color::White);
}

TEST(ColorTest, DoubleSwap) {
  EXPECT_EQ(opposite(opposite(Color::White)), Color::White);
  EXPECT_EQ(opposite(opposite(Color::Black)), Color::Black);
}

// ============================================================================
// Board Tests
// ============================================================================

TEST(BoardTest, SquareConstants) {
  // Test a few key squares to verify the layout
  EXPECT_EQ(squares::a8, 21);
  EXPECT_EQ(squares::h8, 28);
  EXPECT_EQ(squares::a1, 91);
  EXPECT_EQ(squares::h1, 98);
  EXPECT_EQ(squares::e1, 95);
  EXPECT_EQ(squares::e8, 25);
  EXPECT_EQ(squares::no_en_passant, 0);
}

TEST(BoardTest, SquareRange) {
  EXPECT_EQ(min_square, 0);
  EXPECT_EQ(max_square, 119);
}

TEST(BoardTest, IsFrame) {
  // Frame squares (borders)
  EXPECT_TRUE(is_frame(0));
  EXPECT_TRUE(is_frame(10));
  EXPECT_TRUE(is_frame(20));
  EXPECT_TRUE(is_frame(29));
  EXPECT_TRUE(is_frame(100));
  EXPECT_TRUE(is_frame(119));

  // Valid playable squares
  EXPECT_FALSE(is_frame(21));  // a8
  EXPECT_FALSE(is_frame(28));  // h8
  EXPECT_FALSE(is_frame(91));  // a1
  EXPECT_FALSE(is_frame(98));  // h1
  EXPECT_FALSE(is_frame(55));  // e5
}

TEST(BoardTest, IsValidSquare) {
  // Valid playable squares
  EXPECT_TRUE(is_valid_square(21));   // a8
  EXPECT_TRUE(is_valid_square(28));   // h8
  EXPECT_TRUE(is_valid_square(91));   // a1
  EXPECT_TRUE(is_valid_square(98));   // h1
  EXPECT_TRUE(is_valid_square(55));   // e5

  // Frame squares
  EXPECT_FALSE(is_valid_square(0));
  EXPECT_FALSE(is_valid_square(10));
  EXPECT_FALSE(is_valid_square(20));
  EXPECT_FALSE(is_valid_square(29));
  EXPECT_FALSE(is_valid_square(100));
  EXPECT_FALSE(is_valid_square(119));
}

TEST(BoardTest, FrameDetectionArray) {
  // Verify the frame array has correct size
  EXPECT_EQ(square_is_frame.size(), 120);

  // Check all rank 8 squares (21-28) are not frame
  for (Square sq = 21; sq <= 28; ++sq) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    EXPECT_FALSE(square_is_frame[sq]) << "Square " << static_cast<int>(sq) << " should not be frame";
  }

  // Check all rank 1 squares (91-98) are not frame
  for (Square sq = 91; sq <= 98; ++sq) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    EXPECT_FALSE(square_is_frame[sq]) << "Square " << static_cast<int>(sq) << " should not be frame";
  }

  // Check first two rows are frame
  for (Square sq = 0; sq < 20; ++sq) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    EXPECT_TRUE(square_is_frame[sq]) << "Square " << static_cast<int>(sq) << " should be frame";
  }

  // Check last two rows are frame
  for (Square sq = 100; sq < 120; ++sq) {
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-constant-array-index)
    EXPECT_TRUE(square_is_frame[sq]) << "Square " << static_cast<int>(sq) << " should be frame";
  }
}

TEST(BoardTest, BoardArraySize) {
  BoardArray board{};
  EXPECT_EQ(board.size(), 120);
}

TEST(BoardTest, BoardArrayInitialization) {
  // Test that BoardArray can be initialized with pieces
  BoardArray board{};

  // All squares should default to Frame (enum class default is 0)
  EXPECT_EQ(board[0], Piece::Frame);

  // Set some pieces
  board[squares::e2] = Piece::WhitePawn;
  board[squares::e7] = Piece::BlackPawn;
  board[squares::e1] = Piece::WhiteKing;
  board[squares::e8] = Piece::BlackKing;

  EXPECT_EQ(board[squares::e2], Piece::WhitePawn);
  EXPECT_EQ(board[squares::e7], Piece::BlackPawn);
  EXPECT_EQ(board[squares::e1], Piece::WhiteKing);
  EXPECT_EQ(board[squares::e8], Piece::BlackKing);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(IntegrationTest, PieceColorConsistency) {
  // White pieces
  EXPECT_TRUE(is_white_piece(Piece::WhitePawn));
  EXPECT_FALSE(is_black_piece(Piece::WhitePawn));

  // Black pieces
  EXPECT_TRUE(is_black_piece(Piece::BlackPawn));
  EXPECT_FALSE(is_white_piece(Piece::BlackPawn));
}

TEST(IntegrationTest, SquareAndFrameConsistency) {
  // For all playable squares, is_frame should be false
  for (int rank = 2; rank <= 9; ++rank) {
    for (int file = 1; file <= 8; ++file) {
      auto sq = static_cast<Square>((rank * 10) + file);
      EXPECT_FALSE(is_frame(sq)) << "Square " << static_cast<int>(sq) << " should be playable";
      EXPECT_TRUE(is_valid_square(sq)) << "Square " << static_cast<int>(sq) << " should be valid";
    }
  }
}

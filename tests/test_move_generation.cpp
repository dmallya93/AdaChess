// AdaChess - C++ Chess Engine
// Unit tests for Move Generation (Task 7)

#include "adachess/engine/move_generation.hpp"
#include "adachess/board/chessboard.hpp"
#include "adachess/board/moves.hpp"
#include "adachess/board/square.hpp"
#include "adachess/board/directions.hpp"
#include <gtest/gtest.h>

using namespace adachess;

// Helper to set up a basic empty board with frame
void setup_empty_board(Chessboard& board) {
    // Initialize all squares as frame
    board.square.fill(Piece::Frame);

    // Set the 8x8 playing area to empty
    // Rows 2-9 (indices 21-98), columns 1-8
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int square = 21 + rank * 10 + file;
            board.square[square] = Piece::Empty;
        }
    }

    // Initialize piece lists
    board.pieces_list.fill(0);
    board.piece_table.fill(0);

    // Set up basic state
    board.ply = 1;
    board.side_to_move = Color::White;
    board.moves_pointer[board.ply] = 0;
    board.moves_pointer[board.ply + 1] = 0;
}

// Helper to add a piece to the board and piece list
void add_piece(Chessboard& board, Square square, Piece piece, size_t piece_index) {
    board.square[square] = piece;
    board.pieces_list[piece_index] = square;
    board.piece_table[square] = static_cast<PieceIndex>(piece_index);
}

// Test Clear_Moves_List
TEST(MoveGenerationTest, ClearMovesList) {
    Chessboard board;
    board.ply = 1;
    board.moves_pointer[1] = 10;
    board.moves_pointer[2] = 20;

    clear_moves_list(board);

    // After clearing, moves_pointer[ply+1] should equal moves_pointer[ply]
    EXPECT_EQ(board.moves_pointer[2], board.moves_pointer[1]);
    EXPECT_EQ(board.moves_pointer[2], 10u);
}

// Test pawn single push for white
TEST(MoveGenerationTest, WhitePawnSinglePush) {
    Chessboard board;
    setup_empty_board(board);

    // Place white pawn on E3 (square 75 in 10x12) - NOT on starting rank
    add_piece(board, 75, Piece::White_Pawn, 1);
    board.side_to_move = Color::White;

    generate_pawn_moves(board, 75);

    // Should generate one move: E3-E4 (single push only, not on starting rank)
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 1u);

    Move& move = board.moves_stack[board.moves_pointer[1]];
    EXPECT_EQ(move.from, 75u);
    EXPECT_EQ(move.to, 65u);  // E4 = E3 - 10
    EXPECT_EQ(move.piece, Piece::White_Pawn);
    EXPECT_EQ(move.flag, MoveFlag::StandardMove);
}

// Test pawn double push for white from starting rank
TEST(MoveGenerationTest, WhitePawnDoublePush) {
    Chessboard board;
    setup_empty_board(board);

    // Place white pawn on E2 (square 82 in 10x12)
    add_piece(board, 82, Piece::White_Pawn, 1);
    board.side_to_move = Color::White;

    generate_pawn_moves(board, 82);

    // Should generate two moves: E2-E3 and E2-E4
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 2u);

    // First move: single push
    Move& move1 = board.moves_stack[board.moves_pointer[1]];
    EXPECT_EQ(move1.from, 82u);
    EXPECT_EQ(move1.to, 72u);  // E3
    EXPECT_EQ(move1.flag, MoveFlag::StandardMove);

    // Second move: double push
    Move& move2 = board.moves_stack[board.moves_pointer[1] + 1];
    EXPECT_EQ(move2.from, 82u);
    EXPECT_EQ(move2.to, 62u);  // E4 = E2 - 20
    EXPECT_EQ(move2.flag, MoveFlag::PawnMoveTwoSquare);
}

// Test pawn double push blocked
TEST(MoveGenerationTest, WhitePawnDoublePushBlocked) {
    Chessboard board;
    setup_empty_board(board);

    // Place white pawn on E2 and a piece on E4
    add_piece(board, 82, Piece::White_Pawn, 1);
    board.square[62] = Piece::Black_Pawn;
    board.side_to_move = Color::White;

    generate_pawn_moves(board, 82);

    // Should generate only one move: E2-E3 (E4 is blocked)
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 1u);

    Move& move = board.moves_stack[board.moves_pointer[1]];
    EXPECT_EQ(move.to, 72u);  // E3
}

// Test black pawn single push
TEST(MoveGenerationTest, BlackPawnSinglePush) {
    Chessboard board;
    setup_empty_board(board);

    // Place black pawn on E6 (square 45 in 10x12) - NOT on starting rank
    add_piece(board, 45, Piece::Black_Pawn, 17);
    board.side_to_move = Color::Black;

    generate_pawn_moves(board, 45);

    // Should generate one move: E6-E5 (single push only, not on starting rank)
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 1u);

    Move& move = board.moves_stack[board.moves_pointer[1]];
    EXPECT_EQ(move.from, 45u);
    EXPECT_EQ(move.to, 55u);  // E5 = E6 + 10
    EXPECT_EQ(move.piece, Piece::Black_Pawn);
    EXPECT_EQ(move.flag, MoveFlag::StandardMove);
}

// Test black pawn double push
TEST(MoveGenerationTest, BlackPawnDoublePush) {
    Chessboard board;
    setup_empty_board(board);

    // Place black pawn on E7 (square 32 in 10x12)
    add_piece(board, 32, Piece::Black_Pawn, 17);
    board.side_to_move = Color::Black;

    generate_pawn_moves(board, 32);

    // Should generate two moves: E7-E6 and E7-E5
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 2u);

    // First move: single push
    Move& move1 = board.moves_stack[board.moves_pointer[1]];
    EXPECT_EQ(move1.from, 32u);
    EXPECT_EQ(move1.to, 42u);  // E6
    EXPECT_EQ(move1.flag, MoveFlag::StandardMove);

    // Second move: double push
    Move& move2 = board.moves_stack[board.moves_pointer[1] + 1];
    EXPECT_EQ(move2.from, 32u);
    EXPECT_EQ(move2.to, 52u);  // E5 = E7 + 20
    EXPECT_EQ(move2.flag, MoveFlag::PawnMoveTwoSquare);
}

// Test knight moves from center
TEST(MoveGenerationTest, KnightMovesCenter) {
    Chessboard board;
    setup_empty_board(board);

    // Place white knight on E4 (square 65 in 10x12)
    add_piece(board, 65, Piece::White_Knight, 1);
    board.side_to_move = Color::White;

    generate_knight_moves(board, 65);

    // From E4, knight should have 8 possible moves (all squares empty)
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 8u);

    // Verify moves go to valid squares (not Frame)
    for (size_t i = 0; i < move_count; ++i) {
        Move& move = board.moves_stack[board.moves_pointer[1] + i];
        EXPECT_EQ(move.from, 65u);
        EXPECT_NE(board.square[move.to], Piece::Frame);
    }
}

// Test knight moves from corner (limited moves)
TEST(MoveGenerationTest, KnightMovesCorner) {
    Chessboard board;
    setup_empty_board(board);

    // Place white knight on A1 (square 91 in 10x12)
    add_piece(board, 91, Piece::White_Knight, 1);
    board.side_to_move = Color::White;

    generate_knight_moves(board, 91);

    // From A1, knight should have 2 possible moves: B3 and C2
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 2u);
}

// Test knight blocked by friendly pieces
TEST(MoveGenerationTest, KnightBlockedByFriendly) {
    Chessboard board;
    setup_empty_board(board);

    // Place white knight on E4 (square 65) and friendly pieces on some target squares
    add_piece(board, 65, Piece::White_Knight, 1);
    board.square[46] = Piece::White_Pawn;  // F6 (one of knight's 8 targets)
    board.square[44] = Piece::White_Pawn;  // D6 (another of knight's 8 targets)
    board.side_to_move = Color::White;

    generate_knight_moves(board, 65);

    // Should generate 6 moves (8 - 2 blocked by friendly pieces)
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 6u);
}

// Test knight captures enemy pieces
TEST(MoveGenerationTest, KnightCapturesEnemy) {
    Chessboard board;
    setup_empty_board(board);

    // Place white knight on E4 (square 65) and enemy pieces on some target squares
    add_piece(board, 65, Piece::White_Knight, 1);
    board.square[46] = Piece::Black_Pawn;  // F6 (one of knight's 8 targets)
    board.square[44] = Piece::Black_Pawn;  // D6 (another of knight's 8 targets)
    board.side_to_move = Color::White;

    generate_knight_moves(board, 65);

    // Should generate 8 moves (including 2 captures)
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 8u);

    // Check that captures are recorded
    int capture_count = 0;
    for (size_t i = 0; i < move_count; ++i) {
        Move& move = board.moves_stack[board.moves_pointer[1] + i];
        if (move.captured != Piece::Empty) {
            ++capture_count;
        }
    }
    EXPECT_EQ(capture_count, 2);
}

// Test king moves from center
TEST(MoveGenerationTest, KingMovesCenter) {
    Chessboard board;
    setup_empty_board(board);

    // Place white king on E4 (square 65 in 10x12)
    add_piece(board, 65, Piece::White_King, 1);
    board.side_to_move = Color::White;

    generate_king_moves(board, 65);

    // From E4, king should have 8 possible moves
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 8u);
}

// Test king moves from corner (limited moves)
TEST(MoveGenerationTest, KingMovesCorner) {
    Chessboard board;
    setup_empty_board(board);

    // Place white king on A1 (square 91 in 10x12)
    add_piece(board, 91, Piece::White_King, 1);
    board.side_to_move = Color::White;

    generate_king_moves(board, 91);

    // From A1, king should have 3 possible moves: A2, B1, B2
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 3u);
}

// Test king blocked by friendly pieces
TEST(MoveGenerationTest, KingBlockedByFriendly) {
    Chessboard board;
    setup_empty_board(board);

    // Place white king on E1 and friendly pieces around it
    add_piece(board, 95, Piece::White_King, 1);  // E1
    board.square[94] = Piece::White_Pawn;  // D1
    board.square[96] = Piece::White_Pawn;  // F1
    board.square[85] = Piece::White_Pawn;  // E2
    board.side_to_move = Color::White;

    generate_king_moves(board, 95);

    // From E1, king can only move to D2 and F2 (2 moves)
    // 3 squares blocked by friendlies, 3 squares are off-board (frame)
    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];
    EXPECT_EQ(move_count, 2u);
}

// Test Generate_Moves from starting position (limited)
TEST(MoveGenerationTest, GenerateMovesStartingPosition) {
    Chessboard board;
    setup_empty_board(board);

    // Set up a simplified starting position with just pawns and knights
    // White pawns on rank 2
    for (int file = 0; file < 8; ++file) {
        Square square = static_cast<Square>(81 + file);
        add_piece(board, square, Piece::White_Pawn, static_cast<size_t>(1 + file));
    }

    // White knights on B1 and G1
    add_piece(board, 92, Piece::White_Knight, 9);   // B1
    add_piece(board, 97, Piece::White_Knight, 10);  // G1

    // White king on E1
    add_piece(board, 95, Piece::White_King, 11);  // E1

    board.side_to_move = Color::White;
    board.ply = 1;
    board.moves_pointer[1] = 0;

    generate_moves(board);

    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];

    // Expected moves:
    // - 8 pawns x 2 moves each (single + double push) = 16 moves
    // - 2 knights x 2 moves each (B1->A3, B1->C3, G1->F3, G1->H3) = 4 moves
    // - King has 2 moves: D1 and F1 (not blocked, south side is off-board)
    // Total: 22 moves
    EXPECT_EQ(move_count, 22u);
}

// Test Generate_Moves with black pieces
TEST(MoveGenerationTest, GenerateMovesBlackPieces) {
    Chessboard board;
    setup_empty_board(board);

    // Place black pawn and knight
    add_piece(board, 35, Piece::Black_Pawn, 17);    // E7 (35 = 31 + 4)
    add_piece(board, 65, Piece::Black_Knight, 18);  // E4 (65)
    board.side_to_move = Color::Black;
    board.ply = 1;
    board.moves_pointer[1] = 0;

    generate_moves(board);

    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];

    // Expected moves:
    // - 1 pawn x 2 moves (single + double push) = 2 moves
    // - 1 knight x 8 moves = 8 moves
    // Total: 10 moves
    EXPECT_EQ(move_count, 10u);
}

// Test empty piece list terminates correctly
TEST(MoveGenerationTest, EmptyPieceListTermination) {
    Chessboard board;
    setup_empty_board(board);

    // Place only one white pawn, rest of piece list is zeros
    add_piece(board, 82, Piece::White_Pawn, 1);
    board.pieces_list[2] = 0;  // Terminate list
    board.side_to_move = Color::White;
    board.ply = 1;
    board.moves_pointer[1] = 0;

    generate_moves(board);

    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];

    // Should only process one pawn (2 moves: single + double push)
    EXPECT_EQ(move_count, 2u);
}

// Test moves_count helper function
TEST(MoveGenerationTest, MovesCountHelper) {
    Chessboard board;
    setup_empty_board(board);

    // Add a single pawn
    add_piece(board, 82, Piece::White_Pawn, 1);
    board.side_to_move = Color::White;
    board.ply = 1;
    board.moves_pointer[1] = 0;

    generate_moves(board);

    // Check moves_count matches
    size_t move_count = board.moves_count();
    EXPECT_EQ(move_count, 2u);
}

// Test king cannot move into check from enemy rook
TEST(MoveGenerationTest, KingCannotMoveIntoCheckFromRook) {
    Chessboard board;
    setup_empty_board(board);

    // Place white king on E4 (65) and black rook on E8 (25)
    add_piece(board, 65, Piece::White_King, 1);
    add_piece(board, 25, Piece::Black_Rook, 17);
    board.side_to_move = Color::White;
    board.ply = 1;
    board.moves_pointer[1] = 0;

    generate_king_moves(board, 65);

    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];

    // King on E4 with rook on E8 cannot move to:
    // - E3 (75) - on same file as rook (E4 + SOUTH = 65 + 10 = 75)
    // - E5 (55) - on same file as rook (E4 + NORTH = 65 - 10 = 55)
    // King CAN move to: D3, D4, D5, F3, F4, F5 (6 moves)
    EXPECT_EQ(move_count, 6u);

    // Verify none of the generated moves have to=55 or to=75
    for (size_t i = 0; i < move_count; ++i) {
        Move& move = board.moves_stack[board.moves_pointer[1] + i];
        EXPECT_NE(move.to, 55u);  // E5
        EXPECT_NE(move.to, 75u);  // E3
    }
}

// Test king cannot move adjacent to enemy king
TEST(MoveGenerationTest, KingCannotMoveAdjacentToEnemyKing) {
    Chessboard board;
    setup_empty_board(board);

    // Place white king on E4 (65) and black king on E6 (45)
    add_piece(board, 65, Piece::White_King, 1);
    add_piece(board, 45, Piece::Black_King, 17);
    board.side_to_move = Color::White;
    board.ply = 1;
    board.moves_pointer[1] = 0;

    generate_king_moves(board, 65);

    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];

    // King on E4 with enemy king on E6 cannot move to:
    // - D5 (54) - adjacent to black king
    // - E5 (55) - adjacent to black king
    // - F5 (56) - adjacent to black king
    // King CAN move to: D3, D4, E3, F3, F4 (5 moves)
    EXPECT_EQ(move_count, 5u);

    // Verify none of the generated moves go to squares adjacent to black king
    for (size_t i = 0; i < move_count; ++i) {
        Move& move = board.moves_stack[board.moves_pointer[1] + i];
        EXPECT_NE(move.to, 54u);  // D5
        EXPECT_NE(move.to, 55u);  // E5
        EXPECT_NE(move.to, 56u);  // F5
    }
}

// Test king cannot move into check from enemy knight
TEST(MoveGenerationTest, KingCannotMoveIntoCheckFromKnight) {
    Chessboard board;
    setup_empty_board(board);

    // Place white king on E4 (65) and black knight on C5 (53)
    // Knight on C5 attacks: A4, A6, B3, B7, D3, D7, E4, E6
    add_piece(board, 65, Piece::White_King, 1);
    add_piece(board, 53, Piece::Black_Knight, 17);
    board.side_to_move = Color::White;
    board.ply = 1;
    board.moves_pointer[1] = 0;

    generate_king_moves(board, 65);

    size_t move_count = board.moves_pointer[2] - board.moves_pointer[1];

    // King on E4 cannot move to D3 (73) which is attacked by knight on C5
    // King CAN move to: D4, D5, E3, E5, F3, F4, F5 (7 moves)
    EXPECT_EQ(move_count, 7u);

    // Verify the king doesn't move to D3 (attacked by knight)
    for (size_t i = 0; i < move_count; ++i) {
        Move& move = board.moves_stack[board.moves_pointer[1] + i];
        EXPECT_NE(move.to, 73u);  // D3
    }
}

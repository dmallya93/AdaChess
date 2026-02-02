/**
 * AdaChess - Smart Chess Engine
 *
 * Main entry point for the AdaChess chess engine.
 * C++ port of the original Ada implementation.
 *
 * Copyright (c) AdaChess Project
 */

#include <iostream>

// Include all core type headers to verify they compile
#include "adachess/chess.hpp"
#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/score.hpp"
#include "adachess/depths.hpp"
#include "adachess/nodes.hpp"
#include "adachess/distances.hpp"
#include "adachess/hashes.hpp"
#include "adachess/pins.hpp"
#include "adachess/history.hpp"
#include "adachess/moves.hpp"
#include "adachess/board/board.hpp"
#include "adachess/engine/engine.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    std::cout << "AdaChess - Smart Chess Engine" << std::endl;
    std::cout << std::endl;

    // Basic type verification
    static_assert(chess::flip(chess::Color::White) == chess::Color::Black,
                  "Color flip test failed");
    static_assert(chess::is_white_piece(chess::Piece::WhiteKing),
                  "White piece test failed");
    static_assert(chess::is_black_piece(chess::Piece::BlackKing),
                  "Black piece test failed");
    static_assert(chess::is_mate(chess::kMate), "Mate detection test failed");
    static_assert(!chess::is_mate(chess::kDraw), "Draw mate test failed");
    static_assert(chess::kZeroDepth == 1, "Zero depth test failed");
    static_assert(chess::kUnreachable == 9, "Unreachable distance test failed");
    static_assert(chess::kEmptyHash == 0, "Empty hash test failed");
    static_assert(chess::is_not_pinned(chess::Pin::NoPin), "Pin test failed");

    // Test Chessboard initialization
    std::cout << "Testing Chessboard initialization..." << std::endl;

    chess::engine::Chessboard board;
    board.initialize();

    // Verify initial state
    std::cout << "Side to move: " << (board.side_to_move == chess::Color::White ? "White" : "Black") << std::endl;
    std::cout << "White pieces: " << static_cast<int>(board.white_pieces_counter) << std::endl;
    std::cout << "Black pieces: " << static_cast<int>(board.black_pieces_counter) << std::endl;
    std::cout << "White king position: " << chess::board::square_to_string(board.white_king_position) << std::endl;
    std::cout << "Black king position: " << chess::board::square_to_string(board.black_king_position) << std::endl;
    std::cout << "Fifty-move counter: " << static_cast<int>(board.fifty) << std::endl;
    std::cout << "White can castle kingside: " << (board.white_castle_kingside[board.history_ply] ? "Yes" : "No") << std::endl;
    std::cout << "White can castle queenside: " << (board.white_castle_queenside[board.history_ply] ? "Yes" : "No") << std::endl;
    std::cout << "Black can castle kingside: " << (board.black_castle_kingside[board.history_ply] ? "Yes" : "No") << std::endl;
    std::cout << "Black can castle queenside: " << (board.black_castle_queenside[board.history_ply] ? "Yes" : "No") << std::endl;
    std::cout << "Position hash: " << chess::engine::g_hash << std::endl;
    std::cout << std::endl;

    // Display the piece table
    std::cout << "Piece table:" << std::endl;
    board.display_piece_table();

    // Verify piece positions
    bool all_correct = true;

    // Check white pieces on rank 1
    if (board.square[chess::board::A1] != chess::Piece::WhiteRook) {
        std::cout << "ERROR: Expected WhiteRook at A1" << std::endl;
        all_correct = false;
    }
    if (board.square[chess::board::E1] != chess::Piece::WhiteKing) {
        std::cout << "ERROR: Expected WhiteKing at E1" << std::endl;
        all_correct = false;
    }

    // Check black pieces on rank 8
    if (board.square[chess::board::A8] != chess::Piece::BlackRook) {
        std::cout << "ERROR: Expected BlackRook at A8" << std::endl;
        all_correct = false;
    }
    if (board.square[chess::board::E8] != chess::Piece::BlackKing) {
        std::cout << "ERROR: Expected BlackKing at E8" << std::endl;
        all_correct = false;
    }

    // Check empty squares in the middle
    if (board.square[chess::board::E4] != chess::Piece::Empty) {
        std::cout << "ERROR: Expected Empty at E4" << std::endl;
        all_correct = false;
    }

    // Check piece counters
    if (board.white_pieces_counter != 16) {
        std::cout << "ERROR: Expected 16 white pieces, got " << static_cast<int>(board.white_pieces_counter) << std::endl;
        all_correct = false;
    }
    if (board.black_pieces_counter != 16) {
        std::cout << "ERROR: Expected 16 black pieces, got " << static_cast<int>(board.black_pieces_counter) << std::endl;
        all_correct = false;
    }

    // Check king positions
    if (board.white_king_position != chess::board::E1) {
        std::cout << "ERROR: White king position incorrect" << std::endl;
        all_correct = false;
    }
    if (board.black_king_position != chess::board::E8) {
        std::cout << "ERROR: Black king position incorrect" << std::endl;
        all_correct = false;
    }

    if (all_correct) {
        std::cout << "All initialization checks passed!" << std::endl;
    } else {
        std::cout << "Some initialization checks failed!" << std::endl;
        return 1;
    }

    // Test reset
    std::cout << std::endl << "Testing Chessboard reset..." << std::endl;
    board.reset();

    if (board.white_pieces_counter != 0) {
        std::cout << "ERROR: White pieces counter should be 0 after reset" << std::endl;
        return 1;
    }
    if (board.black_pieces_counter != 0) {
        std::cout << "ERROR: Black pieces counter should be 0 after reset" << std::endl;
        return 1;
    }
    if (board.square[chess::board::E1] != chess::Piece::Empty) {
        std::cout << "ERROR: E1 should be empty after reset" << std::endl;
        return 1;
    }

    std::cout << "Reset checks passed!" << std::endl;

    std::cout << std::endl << "AdaChess initialization tests completed successfully!" << std::endl;

    return 0;
}

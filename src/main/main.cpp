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
#include "adachess/board/attacks_data.hpp"
#include "adachess/engine/engine.hpp"

// Include new utility headers
#include "adachess/libs/string_lib.hpp"
#include "adachess/libs/timers/timers.hpp"
#include "adachess/info.hpp"
#include "adachess/io/consoles.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    // Initialize attack tables at startup
    chess::board::preload_sliding_direction();
    chess::board::initialize_attacks_dispatch_table();

    // Display engine info
    std::cout << chess::info::console_logo() << std::endl;
    std::cout << chess::info::engine_name() << " " << chess::info::version()
              << " - " << chess::info::engine_motto() << std::endl;
    std::cout << "Author: " << chess::info::author() << std::endl;
    std::cout << "Email: " << chess::info::e_mail() << std::endl;
    std::cout << "Website: " << chess::info::site() << std::endl;
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

    // ========================================================================
    // Test string_lib utilities
    // ========================================================================
    std::cout << std::endl << "Testing string_lib utilities..." << std::endl;

    // Test count_tokens
    if (string_lib::count_tokens("one two three", ' ') != 3) {
        std::cout << "ERROR: count_tokens failed" << std::endl;
        return 1;
    }

    // Test count_tokens with multiple delimiters
    if (string_lib::count_tokens("one  two   three", ' ') != 3) {
        std::cout << "ERROR: count_tokens with multiple delimiters failed" << std::endl;
        return 1;
    }

    // Test has_token
    if (!string_lib::has_token("hello world")) {
        std::cout << "ERROR: has_token failed" << std::endl;
        return 1;
    }

    // Test has_token on empty string
    if (string_lib::has_token("   ")) {
        std::cout << "ERROR: has_token on whitespace-only string should return false" << std::endl;
        return 1;
    }

    // Test extract_token
    if (string_lib::extract_token("hello world") != "hello") {
        std::cout << "ERROR: extract_token failed" << std::endl;
        return 1;
    }

    // Test extract_last_token
    if (string_lib::extract_last_token("hello world") != "world") {
        std::cout << "ERROR: extract_last_token failed" << std::endl;
        return 1;
    }

    // Test extract_token_at
    if (string_lib::extract_token_at("one two three", 2, ' ') != "two") {
        std::cout << "ERROR: extract_token_at failed" << std::endl;
        return 1;
    }

    // Test trim
    if (string_lib::trim("  hello  ") != "hello") {
        std::cout << "ERROR: trim both failed" << std::endl;
        return 1;
    }
    if (string_lib::trim("  hello  ", ' ', string_lib::TrimSide::Left) != "hello  ") {
        std::cout << "ERROR: trim left failed" << std::endl;
        return 1;
    }
    if (string_lib::trim("  hello  ", ' ', string_lib::TrimSide::Right) != "  hello") {
        std::cout << "ERROR: trim right failed" << std::endl;
        return 1;
    }

    // Test index_of
    if (string_lib::index_of("hello world", ' ') != 5) {
        std::cout << "ERROR: index_of failed" << std::endl;
        return 1;
    }

    // Test replace
    std::string test_str = "hello-world";
    string_lib::replace(test_str, '-', '_');
    if (test_str != "hello_world") {
        std::cout << "ERROR: replace failed" << std::endl;
        return 1;
    }

    // Test is_empty
    if (!string_lib::is_empty("")) {
        std::cout << "ERROR: is_empty on empty string failed" << std::endl;
        return 1;
    }
    if (!string_lib::is_empty("   ")) {
        std::cout << "ERROR: is_empty on whitespace string failed" << std::endl;
        return 1;
    }
    if (string_lib::is_empty("hello")) {
        std::cout << "ERROR: is_empty on non-empty string failed" << std::endl;
        return 1;
    }

    std::cout << "String library tests passed!" << std::endl;

    // ========================================================================
    // Test timers types
    // ========================================================================
    std::cout << std::endl << "Testing timer types..." << std::endl;

    // Test TimerStatus enum
    static_assert(chess::timers::is_not_started(chess::timers::TimerStatus::NotYetStarted),
                  "TimerStatus NotYetStarted test failed");
    static_assert(chess::timers::is_running(chess::timers::TimerStatus::Started),
                  "TimerStatus Started test failed");
    static_assert(chess::timers::is_stopped(chess::timers::TimerStatus::Stopped),
                  "TimerStatus Stopped test failed");

    chess::timers::TimerStatus status = chess::timers::TimerStatus::NotYetStarted;
    if (!chess::timers::is_not_started(status)) {
        std::cout << "ERROR: TimerStatus runtime test failed" << std::endl;
        return 1;
    }

    std::cout << "Timer types tests passed!" << std::endl;

    // ========================================================================
    // Test console display
    // ========================================================================
    std::cout << std::endl << "Testing console display..." << std::endl;
    board.initialize();  // Re-initialize the board
    std::cout << "Initial position:" << std::endl;
    chess::io::consoles::display_on_console(board);

    std::cout << std::endl << "AdaChess initialization tests completed successfully!" << std::endl;

    return 0;
}

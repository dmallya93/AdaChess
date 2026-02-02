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
#include "adachess/io/io.hpp"
#include "adachess/notations.hpp"

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

    // ========================================================================
    // Test move notation and parsing
    // ========================================================================
    std::cout << std::endl << "Testing move notation..." << std::endl;

    // Create a simple move (e2-e4)
    chess::Move e2e4_move{
        chess::Piece::WhitePawn,
        chess::board::E2,
        chess::board::E4,
        chess::Piece::Empty,
        chess::MoveFlag::PawnMoveTwoSquares
    };

    // Test Pure Algebraic notation
    std::string pure_alg = chess::io::move_to_pure_algebraic(e2e4_move);
    if (pure_alg != "e2e4") {
        std::cout << "ERROR: Pure algebraic notation failed. Expected 'e2e4', got '" << pure_alg << "'" << std::endl;
        return 1;
    }
    std::cout << "Pure Algebraic: " << pure_alg << std::endl;

    // Test Standard Algebraic notation
    std::string san = chess::io::move_to_standard_algebraic(e2e4_move);
    if (san != "e4") {
        std::cout << "ERROR: SAN notation failed. Expected 'e4', got '" << san << "'" << std::endl;
        return 1;
    }
    std::cout << "Standard Algebraic (SAN): " << san << std::endl;

    // Test Long Algebraic notation
    std::string lan = chess::io::move_to_long_algebraic(e2e4_move);
    if (lan != "e2-e4") {
        std::cout << "ERROR: LAN notation failed. Expected 'e2-e4', got '" << lan << "'" << std::endl;
        return 1;
    }
    std::cout << "Long Algebraic (LAN): " << lan << std::endl;

    // Test ICCF notation
    std::string iccf = chess::io::move_to_iccf(e2e4_move);
    if (iccf != "5254") {
        std::cout << "ERROR: ICCF notation failed. Expected '5254', got '" << iccf << "'" << std::endl;
        return 1;
    }
    std::cout << "ICCF: " << iccf << std::endl;

    // Test knight move with SAN
    chess::Move nf3_move{
        chess::Piece::WhiteKnight,
        chess::board::G1,
        chess::board::F3,
        chess::Piece::Empty,
        chess::MoveFlag::Standard
    };
    std::string nf3_san = chess::io::move_to_standard_algebraic(nf3_move);
    if (nf3_san != "Nf3") {
        std::cout << "ERROR: Knight SAN notation failed. Expected 'Nf3', got '" << nf3_san << "'" << std::endl;
        return 1;
    }
    std::cout << "Knight SAN: " << nf3_san << std::endl;

    // Test castling move
    chess::Move castle_move{
        chess::Piece::WhiteKing,
        chess::board::E1,
        chess::board::G1,
        chess::Piece::Empty,
        chess::MoveFlag::Castle
    };
    std::string castle_san = chess::io::move_to_standard_algebraic(castle_move);
    if (castle_san != "O-O") {
        std::cout << "ERROR: Castle SAN notation failed. Expected 'O-O', got '" << castle_san << "'" << std::endl;
        return 1;
    }
    std::cout << "Castle SAN: " << castle_san << std::endl;

    // Test capture move
    chess::Move capture_move{
        chess::Piece::WhitePawn,
        chess::board::E4,
        chess::board::D5,
        chess::Piece::BlackPawn,
        chess::MoveFlag::Standard
    };
    std::string capture_san = chess::io::move_to_standard_algebraic(capture_move);
    if (capture_san != "exd5") {
        std::cout << "ERROR: Capture SAN notation failed. Expected 'exd5', got '" << capture_san << "'" << std::endl;
        return 1;
    }
    std::cout << "Capture SAN: " << capture_san << std::endl;

    // Test promotion move
    chess::Move promo_move{
        chess::Piece::WhitePawn,
        chess::board::E7,
        chess::board::E8,
        chess::Piece::Empty,
        chess::MoveFlag::Promotion,
        chess::Piece::WhiteQueen
    };
    std::string promo_san = chess::io::move_to_standard_algebraic(promo_move);
    if (promo_san != "e8=q") {
        std::cout << "ERROR: Promotion SAN notation failed. Expected 'e8=q', got '" << promo_san << "'" << std::endl;
        return 1;
    }
    std::cout << "Promotion SAN: " << promo_san << std::endl;

    // Test check notation
    chess::Move check_move{
        chess::Piece::WhiteQueen,
        chess::board::D1,
        chess::board::H5,
        chess::Piece::Empty,
        chess::MoveFlag::Standard,
        chess::Piece::Empty,
        chess::CheckType::DirectCheck
    };
    std::string check_san = chess::io::move_to_standard_algebraic(check_move);
    if (check_san != "Qh5+") {
        std::cout << "ERROR: Check SAN notation failed. Expected 'Qh5+', got '" << check_san << "'" << std::endl;
        return 1;
    }
    std::cout << "Check SAN: " << check_san << std::endl;

    std::cout << "Move notation tests passed!" << std::endl;

    // ========================================================================
    // Test print_moves_list and parse_move
    // ========================================================================
    std::cout << std::endl << "Testing print_moves_list..." << std::endl;

    // Print all legal moves from the initial position in various notations
    std::cout << "Legal moves from starting position (SAN):" << std::endl;
    board.print_moves_list(chess::NotationType::StandardAlgebraic);

    std::cout << "Legal moves from starting position (coordinate):" << std::endl;
    board.print_moves_list(chess::NotationType::PureAlgebraic);

    // Test parse_move
    std::cout << std::endl << "Testing parse_move..." << std::endl;

    // Parse "e4" from SAN
    chess::Move parsed_e4 = board.parse_move("e4");
    if (!chess::move_is_valid(parsed_e4)) {
        std::cout << "ERROR: Failed to parse 'e4'" << std::endl;
        return 1;
    }
    if (parsed_e4.to != chess::board::E4) {
        std::cout << "ERROR: Parsed move destination incorrect" << std::endl;
        return 1;
    }
    std::cout << "Parsed 'e4': " << chess::io::move_to_string(parsed_e4, chess::NotationType::LongAlgebraic) << std::endl;

    // Parse "e2e4" from coordinate notation
    chess::Move parsed_coord = board.parse_move("e2e4");
    if (!chess::move_is_valid(parsed_coord)) {
        std::cout << "ERROR: Failed to parse 'e2e4'" << std::endl;
        return 1;
    }
    std::cout << "Parsed 'e2e4': " << chess::io::move_to_string(parsed_coord, chess::NotationType::StandardAlgebraic) << std::endl;

    // Parse "Nf3" from SAN
    chess::Move parsed_nf3 = board.parse_move("Nf3");
    if (!chess::move_is_valid(parsed_nf3)) {
        std::cout << "ERROR: Failed to parse 'Nf3'" << std::endl;
        return 1;
    }
    std::cout << "Parsed 'Nf3': " << chess::io::move_to_string(parsed_nf3, chess::NotationType::LongAlgebraic) << std::endl;

    // Parse case-insensitive "NF3"
    chess::Move parsed_nf3_upper = board.parse_move("NF3");
    if (!chess::move_is_valid(parsed_nf3_upper)) {
        std::cout << "ERROR: Failed to parse 'NF3' (case insensitive)" << std::endl;
        return 1;
    }
    std::cout << "Parsed 'NF3' (case insensitive): " << chess::io::move_to_string(parsed_nf3_upper) << std::endl;

    // Parse invalid move
    chess::Move invalid_move = board.parse_move("Ke2");
    if (chess::move_is_valid(invalid_move)) {
        std::cout << "ERROR: 'Ke2' should be invalid in starting position" << std::endl;
        return 1;
    }
    std::cout << "Correctly rejected invalid move 'Ke2'" << std::endl;

    std::cout << "Parse move tests passed!" << std::endl;

    // ========================================================================
    // Test notation type functions
    // ========================================================================
    std::cout << std::endl << "Testing notation type functions..." << std::endl;

    // Test default notation
    chess::set_default_notation(chess::NotationType::LongAlgebraic);
    if (chess::default_notation() != chess::NotationType::LongAlgebraic) {
        std::cout << "ERROR: set_default_notation failed" << std::endl;
        return 1;
    }

    // Reset to standard algebraic
    chess::set_default_notation(chess::NotationType::StandardAlgebraic);

    // Test notation_to_string
    std::cout << "Notation types:" << std::endl;
    std::cout << "  " << chess::notation_to_string(chess::NotationType::StandardAlgebraic) << std::endl;
    std::cout << "  " << chess::notation_to_string(chess::NotationType::LongAlgebraic) << std::endl;
    std::cout << "  " << chess::notation_to_string(chess::NotationType::PureAlgebraic) << std::endl;
    std::cout << "  " << chess::notation_to_string(chess::NotationType::ICCF) << std::endl;

    std::cout << "Notation type tests passed!" << std::endl;

    std::cout << std::endl << "AdaChess initialization tests completed successfully!" << std::endl;

    return 0;
}

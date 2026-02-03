/**
 * AdaChess - Smart Chess Engine
 *
 * Main entry point with minimal console REPL for interactive chess play.
 * C++ port of the original Ada implementation.
 *
 * Copyright (c) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>

// Core type headers
#include "adachess/chess.hpp"
#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/moves.hpp"
#include "adachess/notations.hpp"
#include "adachess/board/board.hpp"
#include "adachess/board/attacks_data.hpp"
#include "adachess/engine/engine.hpp"
#include "adachess/engine/perfts.hpp"

// Utility headers
#include "adachess/libs/string_lib.hpp"
#include "adachess/info.hpp"
#include "adachess/io/consoles.hpp"
#include "adachess/io/io.hpp"

namespace {

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Convert a string to lowercase.
 */
std::string to_lower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

/**
 * Display usage information.
 */
void print_usage() {
    std::cout << "\n"
              << chess::info::engine_name() << " is a chess engine written by "
              << chess::info::author() << ".\n"
              << "Current release is n." << chess::info::version() << "\n\n"
              << "Visit " << chess::info::site() << " for information on how to connect "
              << chess::info::engine_name() << "\n"
              << "to an external GUI (like Winboard or Arena).\n"
              << "Write to " << chess::info::e_mail() << " for comments and bug reports!\n\n"
              << "Enjoy, share, have fun!\n\n";
}

/**
 * Display help for REPL commands.
 */
void print_help() {
    std::cout << "\nAvailable commands:\n"
              << "  display, d       - Show the current board position\n"
              << "  moves, m         - List all legal moves\n"
              << "  play <move>, p   - Make a move (e.g., 'play e4' or 'p Nf3')\n"
              << "  undo, u          - Take back the last move\n"
              << "  new              - Start a new game\n"
              << "  perft <depth>    - Run perft to the specified depth (1-10)\n"
              << "  divide <depth>   - Run divide to the specified depth (0-10)\n"
              << "  notation [type]  - Show or set notation (san, lan, winboard, iccf)\n"
              << "  help, h          - Show this help message\n"
              << "  quit, q          - Exit the program\n"
              << "\nMove notation:\n"
              << "  Accepts SAN (e4, Nf3, O-O), coordinate (e2e4), or LAN (e2-e4)\n"
              << "\nPerft testing:\n"
              << "  perft 5          - Count all positions at depth 5 with statistics\n"
              << "  divide 5         - Show perft counts for each first move\n"
              << "\n";
}

/**
 * Parse a depth value from a string.
 * Returns 0 on failure, otherwise the parsed depth.
 */
chess::Depth parse_depth(const std::string& str, chess::Depth max_depth = 10) {
    if (str.empty()) {
        return 0;
    }

    try {
        int value = std::stoi(str);
        if (value < 0 || value > static_cast<int>(max_depth)) {
            return 0;
        }
        return static_cast<chess::Depth>(value);
    } catch (...) {
        return 0;
    }
}

/**
 * Display the prompt.
 */
void print_prompt() {
    std::cout << "AdaChess v." << chess::info::version() << " => ";
    std::cout.flush();
}

/**
 * Parse command-line arguments.
 * Returns true if the program should continue running, false if it should exit.
 */
bool parse_arguments(int argc, char* argv[]) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "-n" || arg == "--notation") {
            if (i + 1 < argc) {
                std::string notation_arg = to_lower(argv[++i]);

                if (notation_arg == "san" || notation_arg == "standard") {
                    chess::set_default_notation(chess::NotationType::StandardAlgebraic);
                    std::cout << "Using Standard Algebraic Notation (SAN)\n";
                } else if (notation_arg == "lan" || notation_arg == "long") {
                    chess::set_default_notation(chess::NotationType::LongAlgebraic);
                    std::cout << "Using Long Algebraic Notation (LAN)\n";
                } else if (notation_arg == "winboard" || notation_arg == "coordinate" || notation_arg == "pure") {
                    chess::set_default_notation(chess::NotationType::PureAlgebraic);
                    std::cout << "Using Pure Algebraic Notation (coordinate)\n";
                } else if (notation_arg == "iccf" || notation_arg == "numeric") {
                    chess::set_default_notation(chess::NotationType::ICCF);
                    std::cout << "Using ICCF Notation (numeric)\n";
                } else {
                    std::cerr << "Unknown notation: " << argv[i] << "\n";
                    std::cerr << "Available notations: san, lan, winboard/coordinate, iccf\n";
                    return false;
                }
            } else {
                std::cerr << "Error: --notation requires an argument\n";
                return false;
            }
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: adachess [options]\n\n"
                      << "Options:\n"
                      << "  -n, --notation <type>   Set move notation (san, lan, winboard, iccf)\n"
                      << "  -h, --help              Show this help message\n"
                      << "  --version               Show version information\n"
                      << "\nDefault notation is Standard Algebraic (SAN).\n";
            return false;
        } else if (arg == "--version") {
            std::cout << chess::info::engine_name() << " " << chess::info::version()
                      << " - " << chess::info::engine_motto() << "\n";
            return false;
        } else {
            std::cerr << "Unknown argument: " << arg << "\n";
            std::cerr << "Use --help for usage information.\n";
            return false;
        }
    }
    return true;
}

} // anonymous namespace

// ============================================================================
// Main Entry Point
// ============================================================================

int main(int argc, char* argv[]) {
    // ========================================================================
    // Display engine info
    // ========================================================================
    std::cout << chess::info::engine_name() << " " << chess::info::version()
              << " - " << chess::info::engine_motto() << "\n";

    // ========================================================================
    // Parse command-line arguments
    // ========================================================================
    if (!parse_arguments(argc, argv)) {
        return 0;  // Exit after showing help/version or on error
    }

    // ========================================================================
    // Initialize attack tables at startup
    // ========================================================================
    chess::board::preload_sliding_direction();
    chess::board::initialize_attacks_dispatch_table();

    // ========================================================================
    // Create and initialize the chessboard
    // ========================================================================
    chess::engine::Chessboard board;
    board.initialize_hash();
    board.initialize();

    std::cout << "Type 'help' for a list of commands.\n\n";

    // Display initial position
    chess::io::consoles::display_on_console(board);
    std::cout << "\n";

    // ========================================================================
    // Main REPL Loop
    // ========================================================================
    std::string line;
    bool running = true;

    while (running) {
        print_prompt();

        // Read a line of input
        if (!std::getline(std::cin, line)) {
            // EOF or error - exit gracefully
            std::cout << "\n";
            break;
        }

        // Skip empty lines
        std::string trimmed = string_lib::trim(line);
        if (trimmed.empty()) {
            continue;
        }

        // Parse command and parameter
        std::string command;
        std::string parameter;

        std::size_t token_count = string_lib::count_tokens(trimmed, ' ');
        if (token_count > 0) {
            command = to_lower(string_lib::extract_token(trimmed, ' '));
            if (token_count > 1) {
                // Extract everything after the first token as the parameter
                std::size_t space_pos = trimmed.find(' ');
                if (space_pos != std::string::npos) {
                    parameter = string_lib::trim(trimmed.substr(space_pos + 1));
                }
            }
        }

        // ====================================================================
        // Command Dispatch
        // ====================================================================

        if (command == "quit" || command == "q" || command == "exit") {
            // Quit command
            running = false;

        } else if (command == "help" || command == "h" || command == "?") {
            // Help command
            print_help();

        } else if (command == "display" || command == "d" || command == "show" || command == "board") {
            // Display command - show the current board
            chess::io::consoles::display_on_console(board);

        } else if (command == "moves" || command == "m" || command == "list") {
            // Moves command - list all legal moves
            board.generate_moves();
            std::size_t num_moves = board.moves_counter(board.ply);

            std::cout << "Legal moves (" << num_moves << "):\n";
            board.print_moves_list(chess::default_notation());
            std::cout << "\n";

        } else if (command == "play" || command == "p" || command == "move") {
            // Play command - make a move
            if (parameter.empty()) {
                std::cout << "Error: Please specify a move. Usage: play <move>\n";
                std::cout << "Example: play e4, play Nf3, play e2e4\n";
            } else {
                // Generate legal moves
                board.generate_moves();

                // Parse the move
                chess::Move move = board.parse_move(parameter);

                if (chess::move_is_valid(move)) {
                    // Play the move
                    board.play(move);

                    // Display confirmation
                    std::cout << "Played: " << chess::io::move_to_string(move, chess::default_notation()) << "\n";

                    // Show the updated board
                    chess::io::consoles::display_on_console(board);

                    // Check for check/checkmate
                    // Checkmate is detected by checking if opponent has no legal moves while in check
                    if (chess::is_check(move.check)) {
                        board.generate_moves();
                        std::size_t num_moves = board.moves_counter(board.ply);
                        if (num_moves == 0) {
                            // No legal moves while in check = checkmate
                            chess::Color winner = chess::flip(board.side_to_move);
                            std::cout << (winner == chess::Color::White ? "White" : "Black") << " wins by checkmate!\n";
                        } else {
                            std::cout << "Check!\n";
                        }
                    } else {
                        // Check for stalemate
                        board.generate_moves();
                        std::size_t num_moves = board.moves_counter(board.ply);
                        if (num_moves == 0) {
                            std::cout << "Stalemate - game drawn.\n";
                        }
                    }
                } else {
                    std::cout << "Error: Invalid or illegal move '" << parameter << "'\n";
                    std::cout << "Type 'moves' to see legal moves.\n";
                }
            }

        } else if (command == "undo" || command == "u" || command == "takeback") {
            // Undo command - take back the last move
            if (board.history_ply == 0) {
                std::cout << "Error: No moves to undo.\n";
            } else {
                board.undo();
                std::cout << "Move undone.\n";
                chess::io::consoles::display_on_console(board);
            }

        } else if (command == "new" || command == "reset" || command == "restart") {
            // New game command - reset to starting position
            board.initialize();
            std::cout << "New game started.\n";
            chess::io::consoles::display_on_console(board);

        } else if (command == "usage" || command == "about") {
            // Usage/about command
            print_usage();

        } else if (command == "notation") {
            // Change notation on the fly
            if (parameter.empty()) {
                std::cout << "Current notation: " << chess::notation_to_string(chess::default_notation()) << "\n";
            } else {
                std::string notation_arg = to_lower(parameter);
                if (notation_arg == "san" || notation_arg == "standard") {
                    chess::set_default_notation(chess::NotationType::StandardAlgebraic);
                    std::cout << "Notation set to: Standard Algebraic (SAN)\n";
                } else if (notation_arg == "lan" || notation_arg == "long") {
                    chess::set_default_notation(chess::NotationType::LongAlgebraic);
                    std::cout << "Notation set to: Long Algebraic (LAN)\n";
                } else if (notation_arg == "winboard" || notation_arg == "coordinate" || notation_arg == "pure") {
                    chess::set_default_notation(chess::NotationType::PureAlgebraic);
                    std::cout << "Notation set to: Pure Algebraic (coordinate)\n";
                } else if (notation_arg == "iccf" || notation_arg == "numeric") {
                    chess::set_default_notation(chess::NotationType::ICCF);
                    std::cout << "Notation set to: ICCF (numeric)\n";
                } else {
                    std::cout << "Unknown notation: " << parameter << "\n";
                    std::cout << "Available: san, lan, winboard/coordinate, iccf\n";
                }
            }

        } else if (command == "perft") {
            // Perft command - performance test for move generation validation
            if (parameter.empty()) {
                std::cout << "Error: Please specify a depth. Usage: perft <depth>\n";
                std::cout << "Example: perft 5\n";
            } else {
                chess::Depth depth = parse_depth(parameter, 10);
                if (depth == 0 && parameter != "0") {
                    std::cout << "Error: Invalid depth '" << parameter << "'. Depth must be 1-10.\n";
                } else if (depth == 0) {
                    std::cout << "Error: Perft depth must be at least 1.\n";
                } else {
                    std::cout << "\nRunning perft to depth " << depth << "...\n\n";
                    chess::engine::perfts::perft(board, depth);
                }
            }

        } else if (command == "divide") {
            // Divide command - perft split by first move for debugging
            if (parameter.empty()) {
                std::cout << "Error: Please specify a depth. Usage: divide <depth>\n";
                std::cout << "Example: divide 5\n";
            } else {
                chess::Depth depth = parse_depth(parameter, 10);
                if (depth == 0 && parameter != "0") {
                    std::cout << "Error: Invalid depth '" << parameter << "'. Depth must be 0-10.\n";
                } else {
                    std::cout << "\nRunning divide to depth " << depth << "...\n\n";
                    chess::engine::perfts::divide(board, depth);
                }
            }

        } else {
            // Unknown command - try parsing as a move
            board.generate_moves();
            chess::Move move = board.parse_move(command);

            if (chess::move_is_valid(move)) {
                // Valid move entered without 'play' prefix
                board.play(move);

                std::cout << "Played: " << chess::io::move_to_string(move, chess::default_notation()) << "\n";
                chess::io::consoles::display_on_console(board);

                // Check for check/checkmate
                if (chess::is_check(move.check)) {
                    board.generate_moves();
                    std::size_t num_moves = board.moves_counter(board.ply);
                    if (num_moves == 0) {
                        chess::Color winner = chess::flip(board.side_to_move);
                        std::cout << (winner == chess::Color::White ? "White" : "Black") << " wins by checkmate!\n";
                    } else {
                        std::cout << "Check!\n";
                    }
                } else {
                    board.generate_moves();
                    std::size_t num_moves = board.moves_counter(board.ply);
                    if (num_moves == 0) {
                        std::cout << "Stalemate - game drawn.\n";
                    }
                }
            } else {
                // Truly unknown command
                std::cout << "Unknown command: " << command << "\n";
                std::cout << "Type 'help' for a list of commands.\n";
            }
        }
    }

    // ========================================================================
    // Cleanup and exit
    // ========================================================================
    std::cout << "Thanks for playing with AdaChess!\n";

    return 0;
}

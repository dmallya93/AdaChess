/**
 * AdaChess - Smart Chess Engine
 *
 * Perft (Performance Test) and Divide implementation.
 * C++ port of the original Ada implementation (chess-engine-perfts.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/engine/perfts.hpp"
#include "adachess/engine/engine.hpp"
#include "adachess/moves.hpp"
#include "adachess/pieces.hpp"
#include "adachess/notations.hpp"
#include "adachess/io/io.hpp"

#include <chrono>
#include <iomanip>
#include <string>

namespace chess::engine::perfts {

// ============================================================================
// Global State
// ============================================================================

MovePathCollector g_perft_move_paths{};

// ============================================================================
// Local Constants
// ============================================================================

namespace {

// Column width for number formatting (matches Ada's output)
constexpr int kNumberWidth = 14;

// Maximum width for move display in divide output
constexpr int kMoveWidth = 10;

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Collect statistics from a move at a leaf node.
 * Updates the global perft_move_paths collector.
 *
 * @param move The move to collect statistics from.
 */
void collect_move_statistics(const Move& move) {
    // Update stats about captures
    // Note: En passant is counted separately AND included in captures total
    if (move.captured != Piece::Empty) {
        g_perft_move_paths.captures++;
    }

    // Update stats about en passant
    if (move.flag == MoveFlag::CaptureEnPassant) {
        g_perft_move_paths.en_passant++;
        g_perft_move_paths.captures++;  // En passant is also a capture
    }

    // Update stats about castles
    if (move.flag == MoveFlag::Castle) {
        g_perft_move_paths.castles++;
    }

    // Update stats about promotions
    if (move.promotion != Piece::Empty) {
        g_perft_move_paths.promotions++;
    }

    // Update stats about checks/checkmates
    if (move.check != CheckType::NoCheck) {
        g_perft_move_paths.checks++;

        if (move.check == CheckType::DiscoveryCheck) {
            g_perft_move_paths.discovery_checks++;
        }
        if (move.check == CheckType::DoubleCheck) {
            g_perft_move_paths.double_checks++;
        }
        if (move.check == CheckType::Checkmate) {
            g_perft_move_paths.checkmates++;
        }
    }

    // Count this as a node
    g_perft_move_paths.nodes++;
}

/**
 * Internal perft search function (recursive).
 * Collects statistics during traversal.
 *
 * @param board The chessboard state.
 * @param depth The remaining depth to search.
 */
void perft_search(Chessboard& board, Depth depth) {
    board.generate_moves();

    const Depth ply = board.ply;
    const std::size_t start_idx = board.moves_pointer[ply];
    const std::size_t end_idx = board.moves_pointer[ply + 1];

    for (std::size_t i = start_idx; i < end_idx; ++i) {
        const Move& move = board.moves_stack[i];

        if (depth == 1) {
            // Leaf node: collect statistics without playing the move
            collect_move_statistics(move);
        } else {
            // Internal node: play, recurse, undo
            board.play(move);
            perft_search(board, depth - 1);
            board.undo();
        }
    }
}

/**
 * Internal divide search function (recursive, for depth > 1).
 * Collects statistics during traversal.
 *
 * @param board The chessboard state.
 * @param depth The remaining depth to search.
 */
void divide_search(Chessboard& board, Depth depth) {
    board.generate_moves();

    const Depth ply = board.ply;
    const std::size_t start_idx = board.moves_pointer[ply];
    const std::size_t end_idx = board.moves_pointer[ply + 1];

    for (std::size_t i = start_idx; i < end_idx; ++i) {
        const Move& move = board.moves_stack[i];

        board.play(move);

        if (depth == 1) {
            // Leaf node: collect statistics
            collect_move_statistics(move);
        } else {
            // Internal node: recurse deeper
            divide_search(board, depth - 1);
        }

        board.undo();
    }
}

/**
 * Print a formatted number with the specified width.
 *
 * @param out Output stream.
 * @param value The value to print.
 * @param width Column width.
 */
void print_number(std::ostream& out, PerftNode value, int width = kNumberWidth) {
    out << std::setw(width) << value;
}

/**
 * Print the header row for perft output.
 *
 * @param out Output stream.
 */
void print_perft_header(std::ostream& out) {
    out << std::setw(5) << "Depth"
        << std::setw(kNumberWidth) << "Nodes"
        << std::setw(kNumberWidth) << "Captures"
        << std::setw(kNumberWidth) << "En-Passant"
        << std::setw(kNumberWidth) << "Castles"
        << std::setw(kNumberWidth) << "Promotions"
        << std::setw(kNumberWidth) << "Checks"
        << std::setw(kNumberWidth) << "Discovery C."
        << std::setw(kNumberWidth) << "Double C."
        << std::setw(kNumberWidth) << "Checkmates"
        << std::setw(10) << "Time"
        << "\n";
}

/**
 * Print the statistics from the move path collector.
 *
 * @param out Output stream.
 * @param stats The statistics to print.
 */
void print_path_statistics(std::ostream& out, const MovePathCollector& stats) {
    print_number(out, stats.nodes);
    print_number(out, stats.captures);
    print_number(out, stats.en_passant);
    print_number(out, stats.castles);
    print_number(out, stats.promotions);
    print_number(out, stats.checks);
    print_number(out, stats.discovery_checks);
    print_number(out, stats.double_checks);
    print_number(out, stats.checkmates);
}

/**
 * Print the header row for divide output.
 *
 * @param out Output stream.
 */
void print_divide_header(std::ostream& out) {
    out << std::left << std::setw(kMoveWidth) << "Move" << std::right
        << std::setw(kNumberWidth) << "Nodes"
        << std::setw(kNumberWidth) << "Captures"
        << std::setw(kNumberWidth) << "En-Passant"
        << std::setw(kNumberWidth) << "Castles"
        << std::setw(kNumberWidth) << "Promotions"
        << std::setw(kNumberWidth) << "Checks"
        << std::setw(kNumberWidth) << "Discovery C."
        << std::setw(kNumberWidth) << "Double C."
        << std::setw(kNumberWidth) << "Checkmates"
        << "\n";
}

/**
 * Pretty print a move with padding to fixed width.
 *
 * @param out Output stream.
 * @param move The move to print.
 * @param notation The notation to use.
 */
void pretty_print_move(std::ostream& out, const Move& move, NotationType notation) {
    std::string move_str = io::move_to_string(move, notation);
    out << std::left << std::setw(kMoveWidth) << move_str << std::right;
}

} // anonymous namespace

// ============================================================================
// Public Functions
// ============================================================================

void perft(Chessboard& board, Depth depth, std::ostream& out) {
    using Clock = std::chrono::steady_clock;
    using Duration = std::chrono::duration<double>;

    // Print header
    print_perft_header(out);

    // Run perft for each depth from 1 to the requested depth
    for (Depth current_depth = 1; current_depth <= depth; ++current_depth) {
        // Reset statistics for this depth
        g_perft_move_paths.reset();

        // Time the search
        auto start_time = Clock::now();
        perft_search(board, current_depth);
        auto end_time = Clock::now();

        Duration elapsed = end_time - start_time;
        double elapsed_seconds = elapsed.count();

        // Print depth and statistics
        out << std::setw(5) << current_depth;
        print_path_statistics(out, g_perft_move_paths);

        // Print time
        out << std::fixed << std::setprecision(2)
            << std::setw(9) << elapsed_seconds << "s";

        out << "\n";
    }

    out << "\n";

    // Calculate and print NPS (nodes per second) for the last depth
    // Using centiseconds calculation similar to Ada: (nodes * 100) / milliseconds
    double elapsed_seconds = 0.0;
    {
        // Re-time the last perft for accurate NPS
        using Clock = std::chrono::steady_clock;
        g_perft_move_paths.reset();
        auto start_time = Clock::now();
        perft_search(board, depth);
        auto end_time = Clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        elapsed_seconds = elapsed.count();
    }

    if (elapsed_seconds > 0.001) {
        PerftNode nps = static_cast<PerftNode>(
            static_cast<double>(g_perft_move_paths.nodes) / elapsed_seconds);
        out << "Nps: " << nps << "\n";
    } else {
        out << "Nps: (too fast to measure)\n";
    }
}

void divide(Chessboard& board, Depth depth, std::ostream& out) {
    // Generate moves at the root position
    board.generate_moves();

    const Depth ply = board.ply;
    const std::size_t start_idx = board.moves_pointer[ply];
    const std::size_t end_idx = board.moves_pointer[ply + 1];

    // Print header
    print_divide_header(out);

    if (depth == 0) {
        // Special case: depth 0 just shows the available moves with zero counts
        MovePathCollector zero_stats{};

        for (std::size_t i = start_idx; i < end_idx; ++i) {
            const Move& move = board.moves_stack[i];
            pretty_print_move(out, move, default_notation());
            print_path_statistics(out, zero_stats);
            out << "\n";
        }
    } else {
        // Normal case: for each root move, show its subtree statistics
        PerftNode total_nodes = 0;

        for (std::size_t i = start_idx; i < end_idx; ++i) {
            // Reset statistics for this move's subtree
            g_perft_move_paths.reset();

            const Move& move = board.moves_stack[i];

            // Play the root move
            board.play(move);

            // Print the move
            pretty_print_move(out, move, default_notation());

            if (depth == 1) {
                // At depth 1, the root move itself is the leaf - count it
                // But we need to "re-collect" statistics for this move
                // since we played it and can't collect from the unplayed move
                g_perft_move_paths.nodes = 1;

                // Capture stats from the move itself
                if (move.captured != Piece::Empty) {
                    g_perft_move_paths.captures = 1;
                }
                if (move.flag == MoveFlag::CaptureEnPassant) {
                    g_perft_move_paths.en_passant = 1;
                    g_perft_move_paths.captures = 1;
                }
                if (move.flag == MoveFlag::Castle) {
                    g_perft_move_paths.castles = 1;
                }
                if (move.promotion != Piece::Empty) {
                    g_perft_move_paths.promotions = 1;
                }
                if (move.check != CheckType::NoCheck) {
                    g_perft_move_paths.checks = 1;
                    if (move.check == CheckType::DiscoveryCheck) {
                        g_perft_move_paths.discovery_checks = 1;
                    }
                    if (move.check == CheckType::DoubleCheck) {
                        g_perft_move_paths.double_checks = 1;
                    }
                    if (move.check == CheckType::Checkmate) {
                        g_perft_move_paths.checkmates = 1;
                    }
                }
            } else {
                // Search the subtree
                divide_search(board, depth - 1);
            }

            // Print statistics for this move's subtree
            print_path_statistics(out, g_perft_move_paths);
            out << "\n";

            total_nodes += g_perft_move_paths.nodes;

            // Undo the root move
            board.undo();
        }

        out << "\nTotal nodes: " << total_nodes << "\n";
    }
}

PerftNode perft_count(Chessboard& board, Depth depth) {
    if (depth == 0) {
        return 1;
    }

    board.generate_moves();

    const Depth ply = board.ply;
    const std::size_t start_idx = board.moves_pointer[ply];
    const std::size_t end_idx = board.moves_pointer[ply + 1];

    if (depth == 1) {
        // At depth 1, just count the number of legal moves
        return static_cast<PerftNode>(end_idx - start_idx);
    }

    PerftNode nodes = 0;

    for (std::size_t i = start_idx; i < end_idx; ++i) {
        const Move& move = board.moves_stack[i];
        board.play(move);
        nodes += perft_count(board, depth - 1);
        board.undo();
    }

    return nodes;
}

} // namespace chess::engine::perfts

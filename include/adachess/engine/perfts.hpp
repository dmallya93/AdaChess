/**
 * AdaChess - Smart Chess Engine
 *
 * Perft (Performance Test) and Divide commands for move generation validation.
 * C++ port of the original Ada implementation (chess-engine-perfts.ads/adb).
 *
 * Perft is the gold standard for validating move generation correctness.
 * By comparing perft counts against known-correct values from standard test
 * positions, we can verify that move generation has no bugs.
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_ENGINE_PERFTS_HPP
#define ADACHESS_ENGINE_PERFTS_HPP

#include <cstdint>
#include <iostream>

#include "adachess/depths.hpp"
#include "adachess/nodes.hpp"

// Forward declaration to avoid circular includes
namespace chess::engine {
    class Chessboard;
}

namespace chess::engine::perfts {

// ============================================================================
// Perft Types
// ============================================================================

/**
 * Move path collector structure.
 * Collects statistics about moves during perft traversal.
 *
 * This tracks:
 * - Total leaf nodes
 * - Captures (including en passant)
 * - En passant captures specifically
 * - Castling moves
 * - Pawn promotions
 * - Checks (by type)
 * - Checkmates
 */
struct MovePathCollector {
    PerftNode nodes{0};             // Total leaf nodes
    PerftNode captures{0};          // Number of captures
    PerftNode en_passant{0};        // Number of en passant captures
    PerftNode castles{0};           // Number of castling moves
    PerftNode promotions{0};        // Number of promotions
    PerftNode checks{0};            // Number of checks
    PerftNode discovery_checks{0};  // Number of discovery checks
    PerftNode double_checks{0};     // Number of double checks
    PerftNode checkmates{0};        // Number of checkmates

    /**
     * Reset all counters to zero.
     */
    void reset() noexcept {
        nodes = 0;
        captures = 0;
        en_passant = 0;
        castles = 0;
        promotions = 0;
        checks = 0;
        discovery_checks = 0;
        double_checks = 0;
        checkmates = 0;
    }
};

// ============================================================================
// Global Perft State
// ============================================================================

/**
 * Global move path collector for perft statistics.
 * Used during perft and divide operations.
 */
extern MovePathCollector g_perft_move_paths;

// ============================================================================
// Perft Functions
// ============================================================================

/**
 * Perform a perft (performance test) on the current position.
 *
 * Perft counts all leaf nodes at each depth from 1 to the specified depth.
 * Results are printed in a formatted table showing:
 * - Depth level
 * - Node count
 * - Captures, en passant, castles, promotions
 * - Checks (direct, discovery, double)
 * - Checkmates
 * - Time elapsed
 *
 * At the end, the nodes-per-second (NPS) rate is displayed.
 *
 * Standard starting position perft values:
 * - perft(1) = 20
 * - perft(2) = 400
 * - perft(3) = 8,902
 * - perft(4) = 197,281
 * - perft(5) = 4,865,609
 *
 * @param board The current chess position (will be modified and restored).
 * @param depth The maximum depth to search (1 to depth).
 * @param out Output stream for results (default: std::cout).
 */
void perft(Chessboard& board, Depth depth, std::ostream& out = std::cout);

/**
 * Perform a divide operation on the current position.
 *
 * Divide splits the perft count for each first-ply move.
 * This is useful for debugging when perft counts don't match
 * expected values - it shows which move(s) have incorrect subtree counts.
 *
 * For each root move, prints:
 * - Move notation
 * - Subtree node count
 * - Statistics (captures, checks, etc.)
 *
 * @param board The current chess position (will be modified and restored).
 * @param depth The depth to search (0 shows moves with no recursion).
 * @param out Output stream for results (default: std::cout).
 */
void divide(Chessboard& board, Depth depth, std::ostream& out = std::cout);

/**
 * Perform a simple perft count (no statistics, just node count).
 * This is faster than the full perft as it skips statistics collection.
 *
 * @param board The current chess position.
 * @param depth The depth to search.
 * @return The total number of leaf nodes.
 */
[[nodiscard]] PerftNode perft_count(Chessboard& board, Depth depth);

} // namespace chess::engine::perfts

#endif // ADACHESS_ENGINE_PERFTS_HPP

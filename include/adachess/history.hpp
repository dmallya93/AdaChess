/**
 * AdaChess - Smart Chess Engine
 *
 * History type definitions for tracking game move history.
 * C++ port of the original Ada implementation (chess-history.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_HISTORY_HPP
#define ADACHESS_HISTORY_HPP

#include <cstdint>

#include "adachess/hashes.hpp"

namespace chess {

// Forward declaration for Move type (defined in moves.hpp)
// This will be properly included when moves.hpp is implemented
struct Move;

/**
 * History move structure for tracking game history.
 * Corresponds to Ada's History_Move_Type record.
 *
 * Each history entry stores:
 * - The move that was played
 * - The position hash before or after the move
 * - The fifty-move rule counter at that point
 *
 * Note: The Move type is forward declared. Full definition will be
 * available when moves.hpp is implemented in a subsequent task.
 */
struct HistoryMove {
    /**
     * The move that was played.
     * Note: This uses a pointer to forward-declared Move type.
     * Will be replaced with actual Move type when moves.hpp is available.
     */
    // Move move;  // TODO: Uncomment when Move type is available

    /**
     * Position hash for detecting repetitions.
     */
    Hash hash{kEmptyHash};

    /**
     * Fifty-move rule counter.
     * Tracks half-moves since last capture or pawn move.
     */
    std::uint16_t fifty{0};

    /**
     * Default constructor - creates an empty history move.
     */
    constexpr HistoryMove() noexcept = default;

    /**
     * Constructor with hash and fifty-move counter.
     *
     * @param h The position hash.
     * @param f The fifty-move counter.
     */
    constexpr HistoryMove(Hash h, std::uint16_t f) noexcept
        : hash(h), fifty(f) {}
};

/**
 * Empty history move constant.
 * Corresponds to Ada's Empty_History_Move.
 */
inline constexpr HistoryMove kEmptyHistoryMove{};

/**
 * Equality comparison for history moves.
 * Note: Ada's implementation only compares the Move field.
 * This simplified version compares hash and fifty counter.
 * When Move type is available, this should be updated to compare moves.
 *
 * @param lhs Left-hand side history move.
 * @param rhs Right-hand side history move.
 * @return True if the history moves are equal.
 */
[[nodiscard]] constexpr bool operator==(const HistoryMove& lhs,
                                         const HistoryMove& rhs) noexcept {
    // TODO: When Move type is available, compare moves instead:
    // return lhs.move == rhs.move;
    return lhs.hash == rhs.hash && lhs.fifty == rhs.fifty;
}

/**
 * Inequality comparison for history moves.
 *
 * @param lhs Left-hand side history move.
 * @param rhs Right-hand side history move.
 * @return True if the history moves are not equal.
 */
[[nodiscard]] constexpr bool operator!=(const HistoryMove& lhs,
                                         const HistoryMove& rhs) noexcept {
    return !(lhs == rhs);
}

} // namespace chess

#endif // ADACHESS_HISTORY_HPP

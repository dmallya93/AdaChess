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
#include "adachess/moves.hpp"

namespace chess {

/**
 * History move structure for tracking game history.
 * Corresponds to Ada's History_Move_Type record.
 *
 * Each history entry stores:
 * - The move that was played
 * - The position hash before or after the move
 * - The fifty-move rule counter at that point
 */
struct HistoryMove {
    /**
     * The move that was played.
     */
    Move move{};

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
     * Constructor with move, hash and fifty-move counter.
     *
     * @param m The move that was played.
     * @param h The position hash.
     * @param f The fifty-move counter.
     */
    constexpr HistoryMove(Move m, Hash h, std::uint16_t f) noexcept
        : move(m), hash(h), fifty(f) {}

    /**
     * Constructor with hash and fifty-move counter (for backward compatibility).
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
 * Ada's implementation compares only the Move field.
 *
 * @param lhs Left-hand side history move.
 * @param rhs Right-hand side history move.
 * @return True if the history moves are equal.
 */
[[nodiscard]] constexpr bool operator==(const HistoryMove& lhs,
                                         const HistoryMove& rhs) noexcept {
    return lhs.move == rhs.move;
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

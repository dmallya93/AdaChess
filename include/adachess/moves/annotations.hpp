/**
 * AdaChess - Smart Chess Engine
 *
 * Move annotation types and annotated move structure.
 * C++ port of the original Ada implementation (chess-moves-annotations.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_MOVES_ANNOTATIONS_HPP
#define ADACHESS_MOVES_ANNOTATIONS_HPP

#include <cstdint>

#include "adachess/moves.hpp"

namespace chess {
namespace moves {
namespace annotations {

/**
 * Move annotation type enumeration.
 *
 * Used to classify the quality of moves during analysis.
 * These annotations follow standard chess notation conventions:
 * - !! = Brilliant
 * - !  = Good
 * - !? = Interesting
 * - ?! = Dubious
 * - ?  = Mistake
 * - ?? = Blunder
 *
 * Uses explicit underlying type std::uint8_t to minimize storage,
 * matching Ada's Size => 3 specification.
 *
 * Default value is None, matching Ada's Default_Value => None.
 *
 * Note: "Dubious" is spelled correctly here, though the Ada source
 * has a typo ("Dobious").
 */
enum class AnnotationType : std::uint8_t {
    None = 0,        ///< No annotation (default)
    Blunder = 1,     ///< Terrible move (??)
    Mistake = 2,     ///< Bad move (?)
    Dubious = 3,     ///< Questionable move (?!)
    Interesting = 4, ///< A move that leads to a small improvement (!?)
    Good = 5,        ///< A move that brings a valuable advantage (!)
    Brilliant = 6    ///< Amazing move (!!)
};

/**
 * Default annotation type (None).
 * Matches Ada's Default_Value => None specification.
 */
inline constexpr AnnotationType kDefaultAnnotationType = AnnotationType::None;

/**
 * Annotated move structure.
 *
 * Pairs a chess move with its quality annotation.
 * Used during analysis to classify moves (e.g., for PGN export
 * or displaying move quality to users).
 */
struct AnnotatedMove {
    Move move{};                                    ///< The chess move
    AnnotationType annotation{AnnotationType::None}; ///< Move quality annotation

    /**
     * Default constructor creates an empty annotated move.
     */
    constexpr AnnotatedMove() noexcept = default;

    /**
     * Constructor from a move and annotation.
     *
     * @param m The chess move.
     * @param a The annotation (default None).
     */
    constexpr AnnotatedMove(const Move& m,
                            AnnotationType a = AnnotationType::None) noexcept
        : move(m)
        , annotation(a)
    {}
};

// ============================================================================
// Annotated Move Comparison Operators
// ============================================================================

/**
 * Equality comparison for annotated moves.
 * Two annotated moves are equal if their underlying moves are equal.
 * The annotation is NOT considered in equality comparison.
 * Matches Ada's overloaded "=" operator for Annotated_Move_Type.
 *
 * @param lhs Left-hand side annotated move.
 * @param rhs Right-hand side annotated move.
 * @return true if the underlying moves are equal.
 */
[[nodiscard]] constexpr bool operator==(const AnnotatedMove& lhs,
                                         const AnnotatedMove& rhs) noexcept {
    return lhs.move == rhs.move;
}

/**
 * Inequality comparison for annotated moves.
 *
 * @param lhs Left-hand side annotated move.
 * @param rhs Right-hand side annotated move.
 * @return true if the underlying moves are not equal.
 */
[[nodiscard]] constexpr bool operator!=(const AnnotatedMove& lhs,
                                         const AnnotatedMove& rhs) noexcept {
    return !(lhs == rhs);
}

/**
 * Equality comparison between a Move and an AnnotatedMove.
 * Matches Ada's overloaded "=" operator: Move_Type = Annotated_Move_Type.
 *
 * @param lhs A regular move.
 * @param rhs An annotated move.
 * @return true if the move equals the annotated move's underlying move.
 */
[[nodiscard]] constexpr bool operator==(const Move& lhs,
                                         const AnnotatedMove& rhs) noexcept {
    return lhs == rhs.move;
}

/**
 * Inequality comparison between a Move and an AnnotatedMove.
 *
 * @param lhs A regular move.
 * @param rhs An annotated move.
 * @return true if the move does not equal the annotated move's underlying move.
 */
[[nodiscard]] constexpr bool operator!=(const Move& lhs,
                                         const AnnotatedMove& rhs) noexcept {
    return !(lhs == rhs);
}

/**
 * Equality comparison between an AnnotatedMove and a Move.
 * Matches Ada's overloaded "=" operator: Annotated_Move_Type = Move_Type.
 *
 * @param lhs An annotated move.
 * @param rhs A regular move.
 * @return true if the annotated move's underlying move equals the move.
 */
[[nodiscard]] constexpr bool operator==(const AnnotatedMove& lhs,
                                         const Move& rhs) noexcept {
    return lhs.move == rhs;
}

/**
 * Inequality comparison between an AnnotatedMove and a Move.
 *
 * @param lhs An annotated move.
 * @param rhs A regular move.
 * @return true if the annotated move's underlying move does not equal the move.
 */
[[nodiscard]] constexpr bool operator!=(const AnnotatedMove& lhs,
                                         const Move& rhs) noexcept {
    return !(lhs == rhs);
}

// ============================================================================
// Annotated Move Constants
// ============================================================================

/**
 * Empty/invalid annotated move constant.
 * Used as a sentinel value or default.
 * Matches Ada's Empty_Annotated_Move constant.
 */
inline constexpr AnnotatedMove kEmptyAnnotatedMove{};

} // namespace annotations
} // namespace moves
} // namespace chess

#endif // ADACHESS_MOVES_ANNOTATIONS_HPP

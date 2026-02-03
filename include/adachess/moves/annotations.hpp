/**
 * AdaChess - Smart Chess Engine
 *
 * Move annotation types for quality assessment of chess moves.
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

namespace chess::moves::annotations {

// ============================================================================
// Annotation Type
// ============================================================================

/**
 * Annotation type enumeration for move quality assessment.
 *
 * This type is used for:
 * - Marking moves during analysis (blunders, brilliant moves, etc.)
 * - PGN export with standard annotation symbols
 * - Analysis output formatting
 *
 * Corresponds to Ada's Annotation_Type enumeration with:
 * - Size => 3 (3 bits)
 * - Default_Value => None
 *
 * The underlying type std::uint8_t is the smallest standard integer type
 * that can hold 3 bits.
 *
 * Note: "Dubious" is spelled correctly here; the Ada source has "Dobious" (typo).
 */
enum class AnnotationType : std::uint8_t {
    None = 0,        // No annotation
    Blunder = 1,     // A very bad move (??), loses significant material or position
    Mistake = 2,     // A bad move (?), loses some advantage
    Dubious = 3,     // A questionable move (?!), may not be optimal
    Interesting = 4, // An interesting move (!?), leads to a small improvement
    Good = 5,        // A good move (!), brings valuable advantages
    Brilliant = 6    // An amazing move (!!), exceptional quality
};

/**
 * Default annotation type (None), matching Ada's Default_Value specification.
 */
inline constexpr AnnotationType kDefaultAnnotation = AnnotationType::None;

// ============================================================================
// Annotated Move Type
// ============================================================================

/**
 * Annotated move structure combining a move with its annotation.
 *
 * This structure pairs a chess move with a quality annotation,
 * allowing moves to be tagged with assessment information.
 *
 * The equality operators compare only the move component (not the annotation),
 * matching Ada's semantics where two annotated moves are equal if their
 * underlying moves are equal, regardless of annotation.
 */
struct AnnotatedMove {
    Move move{};                                       // The chess move
    AnnotationType annotation{AnnotationType::None};   // Quality annotation

    /**
     * Default constructor creates an empty annotated move.
     */
    constexpr AnnotatedMove() noexcept = default;

    /**
     * Constructor for creating an annotated move.
     *
     * @param move_arg The chess move.
     * @param annotation_arg The quality annotation (default None).
     */
    constexpr AnnotatedMove(
        const Move& move_arg,
        AnnotationType annotation_arg = AnnotationType::None
    ) noexcept
        : move(move_arg)
        , annotation(annotation_arg)
    {}
};

// ============================================================================
// Annotated Move Constants
// ============================================================================

/**
 * Empty annotated move constant.
 * Used as a sentinel value or default.
 * Matches Ada's Empty_Annotated_Move constant.
 */
inline constexpr AnnotatedMove kEmptyAnnotatedMove{kEmptyMove, AnnotationType::None};

// ============================================================================
// Annotated Move Comparison Operators
// ============================================================================

/**
 * Equality comparison for annotated moves.
 * Two annotated moves are equal if their underlying moves are equal.
 * The annotation is NOT considered in the comparison.
 * Matches Ada's overloaded "=" operator semantics.
 *
 * @param lhs Left-hand side annotated move.
 * @param rhs Right-hand side annotated move.
 * @return true if the underlying moves are equal.
 */
[[nodiscard]] constexpr bool operator==(const AnnotatedMove& lhs, const AnnotatedMove& rhs) noexcept {
    return lhs.move == rhs.move;
}

/**
 * Inequality comparison for annotated moves.
 *
 * @param lhs Left-hand side annotated move.
 * @param rhs Right-hand side annotated move.
 * @return true if the underlying moves are not equal.
 */
[[nodiscard]] constexpr bool operator!=(const AnnotatedMove& lhs, const AnnotatedMove& rhs) noexcept {
    return !(lhs == rhs);
}

/**
 * Equality comparison between a Move and an AnnotatedMove.
 * Compares the move against the annotated move's underlying move.
 * Matches Ada's overloaded "=" operator.
 *
 * @param lhs A chess move.
 * @param rhs An annotated move.
 * @return true if the move equals the annotated move's underlying move.
 */
[[nodiscard]] constexpr bool operator==(const Move& lhs, const AnnotatedMove& rhs) noexcept {
    return lhs == rhs.move;
}

/**
 * Inequality comparison between a Move and an AnnotatedMove.
 *
 * @param lhs A chess move.
 * @param rhs An annotated move.
 * @return true if the move does not equal the annotated move's underlying move.
 */
[[nodiscard]] constexpr bool operator!=(const Move& lhs, const AnnotatedMove& rhs) noexcept {
    return !(lhs == rhs);
}

/**
 * Equality comparison between an AnnotatedMove and a Move.
 * Compares the annotated move's underlying move against the move.
 * Matches Ada's overloaded "=" operator.
 *
 * @param lhs An annotated move.
 * @param rhs A chess move.
 * @return true if the annotated move's underlying move equals the move.
 */
[[nodiscard]] constexpr bool operator==(const AnnotatedMove& lhs, const Move& rhs) noexcept {
    return lhs.move == rhs;
}

/**
 * Inequality comparison between an AnnotatedMove and a Move.
 *
 * @param lhs An annotated move.
 * @param rhs A chess move.
 * @return true if the annotated move's underlying move does not equal the move.
 */
[[nodiscard]] constexpr bool operator!=(const AnnotatedMove& lhs, const Move& rhs) noexcept {
    return !(lhs == rhs);
}

// ============================================================================
// Annotation Utility Functions
// ============================================================================

/**
 * Convert an AnnotationType to its PGN symbol representation.
 * @param annotation The annotation type.
 * @return The PGN symbol string for the annotation.
 */
[[nodiscard]] inline const char* annotation_to_symbol(AnnotationType annotation) noexcept {
    switch (annotation) {
        case AnnotationType::None:
            return "";
        case AnnotationType::Blunder:
            return "??";
        case AnnotationType::Mistake:
            return "?";
        case AnnotationType::Dubious:
            return "?!";
        case AnnotationType::Interesting:
            return "!?";
        case AnnotationType::Good:
            return "!";
        case AnnotationType::Brilliant:
            return "!!";
    }
    return "";
}

/**
 * Convert an AnnotationType to its string representation.
 * @param annotation The annotation type.
 * @return A human-readable string describing the annotation.
 */
[[nodiscard]] inline const char* annotation_to_string(AnnotationType annotation) noexcept {
    switch (annotation) {
        case AnnotationType::None:
            return "None";
        case AnnotationType::Blunder:
            return "Blunder";
        case AnnotationType::Mistake:
            return "Mistake";
        case AnnotationType::Dubious:
            return "Dubious";
        case AnnotationType::Interesting:
            return "Interesting";
        case AnnotationType::Good:
            return "Good";
        case AnnotationType::Brilliant:
            return "Brilliant";
    }
    return "Unknown";
}

/**
 * Check if a move has an annotation.
 * @param annotated_move The annotated move to check.
 * @return true if the move has a non-None annotation.
 */
[[nodiscard]] constexpr bool has_annotation(const AnnotatedMove& annotated_move) noexcept {
    return annotated_move.annotation != AnnotationType::None;
}

} // namespace chess::moves::annotations

#endif // ADACHESS_MOVES_ANNOTATIONS_HPP

/**
 * AdaChess - Smart Chess Engine
 *
 * Move notation I/O functions for converting moves to and from string representation.
 * C++ port of the original Ada implementation (chess-io.ads/adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_IO_IO_HPP
#define ADACHESS_IO_IO_HPP

#include <string>
#include <iostream>

#include "adachess/moves.hpp"
#include "adachess/notations.hpp"
#include "adachess/board/board.hpp"

namespace chess::io {

// ============================================================================
// Annotation Types
// ============================================================================

/**
 * Move annotation type for PGN-style annotations.
 * These appear after the move notation.
 */
enum class AnnotationType : std::uint8_t {
    None = 0,        // No annotation
    Blunder = 1,     // ?? - Very bad move
    Mistake = 2,     // ? - Bad move
    Dubious = 3,     // ?! - Questionable move
    Interesting = 4, // !? - Interesting move
    Good = 5,        // ! - Good move
    Brilliant = 6    // !! - Excellent move
};

/**
 * Annotated move structure combining a move with an annotation.
 */
struct AnnotatedMove {
    Move move{};
    AnnotationType annotation{AnnotationType::None};

    /**
     * Default constructor.
     */
    constexpr AnnotatedMove() noexcept = default;

    /**
     * Constructor with move and annotation.
     */
    constexpr AnnotatedMove(const Move& m, AnnotationType a = AnnotationType::None) noexcept
        : move(m), annotation(a) {}

    /**
     * Implicit conversion from Move.
     */
    constexpr AnnotatedMove(const Move& m) noexcept
        : move(m), annotation(AnnotationType::None) {}
};

// ============================================================================
// Move To String Functions
// ============================================================================

/**
 * Convert a move to its string representation in the specified notation.
 *
 * @param move The move to convert.
 * @param notation The notation format to use (default: current default notation).
 * @return The move string.
 */
[[nodiscard]] std::string move_to_string(
    const Move& move,
    NotationType notation = default_notation());

/**
 * Convert an annotated move to its string representation.
 *
 * @param annotated_move The annotated move to convert.
 * @param notation The notation format to use (default: current default notation).
 * @return The move string with annotation.
 */
[[nodiscard]] std::string move_to_string(
    const AnnotatedMove& annotated_move,
    NotationType notation = default_notation());

// ============================================================================
// Move Notation Conversion Helpers
// ============================================================================

/**
 * Convert a move to Pure Algebraic (coordinate) notation.
 * Format: e2e4, e7e8q (for promotion)
 *
 * @param move The move to convert.
 * @return The move string in coordinate notation.
 */
[[nodiscard]] std::string move_to_pure_algebraic(const Move& move);

/**
 * Convert a move to Standard Algebraic Notation (SAN).
 * Format: e4, Nf3, O-O, exd5, e8=Q+
 *
 * @param move The move to convert.
 * @return The move string in SAN notation.
 */
[[nodiscard]] std::string move_to_standard_algebraic(const Move& move);

/**
 * Convert a move to Long Algebraic Notation (LAN).
 * Format: e2-e4, Ng1-f3, O-O, Qa5xc3+, e7-e8=Q
 *
 * @param move The move to convert.
 * @return The move string in LAN notation.
 */
[[nodiscard]] std::string move_to_long_algebraic(const Move& move);

/**
 * Convert a move to ICCF numeric notation.
 * Format: 5254 (file+rank of from, file+rank of to), 5758[1-4] for promotion
 *
 * @param move The move to convert.
 * @return The move string in ICCF notation.
 */
[[nodiscard]] std::string move_to_iccf(const Move& move);

// ============================================================================
// Move Printing Functions
// ============================================================================

/**
 * Print a move to the specified output stream.
 *
 * @param move The move to print.
 * @param notation The notation format to use.
 * @param out The output stream (default: std::cout).
 */
void print_move(
    const Move& move,
    NotationType notation = default_notation(),
    std::ostream& out = std::cout);

/**
 * Print an annotated move to the specified output stream.
 *
 * @param annotated_move The annotated move to print.
 * @param notation The notation format to use.
 * @param out The output stream (default: std::cout).
 */
void print_move(
    const AnnotatedMove& annotated_move,
    NotationType notation = default_notation(),
    std::ostream& out = std::cout);

// ============================================================================
// Annotation String Conversion
// ============================================================================

/**
 * Convert an annotation type to its string representation.
 *
 * @param annotation The annotation type.
 * @return The annotation string (e.g., "!!", "?", etc.) or empty string for None.
 */
[[nodiscard]] std::string annotation_to_string(AnnotationType annotation);

// ============================================================================
// Check Symbol Conversion
// ============================================================================

/**
 * Convert a check type to its string representation.
 *
 * @param check The check type.
 * @return The check symbol ("+", "++", "#") or empty string for NoCheck.
 */
[[nodiscard]] std::string check_to_string(CheckType check);

} // namespace chess::io

#endif // ADACHESS_IO_IO_HPP

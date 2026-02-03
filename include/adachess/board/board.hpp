/**
 * AdaChess - Smart Chess Engine
 *
 * Board representation using 10x12 mailbox with Square type and coordinate utilities.
 * C++ port of the original Ada implementation (chess-board.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_BOARD_BOARD_HPP
#define ADACHESS_BOARD_BOARD_HPP

#include <array>
#include <cstdint>
#include <stdexcept>
#include <string>

#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"

namespace chess::board {

// ============================================================================
// Constants
// ============================================================================

/**
 * Total number of squares in the 10x12 mailbox representation.
 */
inline constexpr std::size_t kBoardSize = 120;

/**
 * Number of playable squares (8x8 = 64).
 */
inline constexpr std::size_t kPlayableSquares = 64;

// ============================================================================
// Coordinate Type
// ============================================================================

/**
 * Coordinate type for file, rank, diagonal values.
 * Range -1 (invalid) to 14 (matches Ada's Coordinate_Type).
 * -1 indicates an invalid/frame coordinate.
 */
using Coordinate = std::int8_t;

inline constexpr Coordinate kInvalidCoordinate = -1;

// File constants (0-7 for a-h)
inline constexpr Coordinate kFileA = 0;
inline constexpr Coordinate kFileB = 1;
inline constexpr Coordinate kFileC = 2;
inline constexpr Coordinate kFileD = 3;
inline constexpr Coordinate kFileE = 4;
inline constexpr Coordinate kFileF = 5;
inline constexpr Coordinate kFileG = 6;
inline constexpr Coordinate kFileH = 7;

// Rank constants (1-8)
inline constexpr Coordinate kRank1 = 1;
inline constexpr Coordinate kRank2 = 2;
inline constexpr Coordinate kRank3 = 3;
inline constexpr Coordinate kRank4 = 4;
inline constexpr Coordinate kRank5 = 5;
inline constexpr Coordinate kRank6 = 6;
inline constexpr Coordinate kRank7 = 7;
inline constexpr Coordinate kRank8 = 8;

// ============================================================================
// Square Class
// ============================================================================

/**
 * Square class representing a position on the 10x12 mailbox board.
 * Valid indices are 0-119. The class provides validation at construction
 * and type-safe access to board squares.
 *
 * The 10x12 mailbox layout:
 *   x x x x x x x x x x  (indices  0- 9,  frame row)
 *   x x x x x x x x x x  (indices 10-19,  frame row)
 *   x a8 b8 c8 d8 e8 f8 g8 h8 x  (indices 20-29, rank 8)
 *   x a7 b7 c7 d7 e7 f7 g7 h7 x  (indices 30-39, rank 7)
 *   x a6 b6 c6 d6 e6 f6 g6 h6 x  (indices 40-49, rank 6)
 *   x a5 b5 c5 d5 e5 f5 g5 h5 x  (indices 50-59, rank 5)
 *   x a4 b4 c4 d4 e4 f4 g4 h4 x  (indices 60-69, rank 4)
 *   x a3 b3 c3 d3 e3 f3 g3 h3 x  (indices 70-79, rank 3)
 *   x a2 b2 c2 d2 e2 f2 g2 h2 x  (indices 80-89, rank 2)
 *   x a1 b1 c1 d1 e1 f1 g1 h1 x  (indices 90-99, rank 1)
 *   x x x x x x x x x x  (indices 100-109, frame row)
 *   x x x x x x x x x x  (indices 110-119, frame row)
 */
class Square {
public:
    /**
     * Default constructor creates an invalid square (index 0, which is frame).
     */
    constexpr Square() noexcept : value_(0) {}

    /**
     * Construct a Square from an index value.
     * Does not perform validation (use from_index for validation).
     * This constructor is constexpr to allow compile-time square constants.
     *
     * @param index The board index (0-119).
     */
    constexpr explicit Square(std::uint8_t index) noexcept : value_(index) {}

    /**
     * Create a Square from an index with validation.
     * Throws std::out_of_range if index >= 120.
     *
     * @param index The board index to validate.
     * @return A valid Square.
     * @throws std::out_of_range if index is out of range.
     */
    [[nodiscard]] static constexpr Square from_index(std::uint8_t index) {
        if (index >= kBoardSize) {
            throw std::out_of_range("Square index out of range");
        }
        return Square(index);
    }

    /**
     * Get the raw index value.
     * @return The underlying index (0-119).
     */
    [[nodiscard]] constexpr std::uint8_t index() const noexcept {
        return value_;
    }

    /**
     * Get the value for array indexing.
     * @return The index as size_t for array access.
     */
    [[nodiscard]] constexpr std::size_t to_index() const noexcept {
        return static_cast<std::size_t>(value_);
    }

    /**
     * Equality comparison.
     */
    [[nodiscard]] constexpr bool operator==(const Square& other) const noexcept {
        return value_ == other.value_;
    }

    /**
     * Inequality comparison.
     */
    [[nodiscard]] constexpr bool operator!=(const Square& other) const noexcept {
        return value_ != other.value_;
    }

    /**
     * Less-than comparison (for ordering).
     */
    [[nodiscard]] constexpr bool operator<(const Square& other) const noexcept {
        return value_ < other.value_;
    }

    /**
     * Less-than-or-equal comparison.
     */
    [[nodiscard]] constexpr bool operator<=(const Square& other) const noexcept {
        return value_ <= other.value_;
    }

    /**
     * Greater-than comparison.
     */
    [[nodiscard]] constexpr bool operator>(const Square& other) const noexcept {
        return value_ > other.value_;
    }

    /**
     * Greater-than-or-equal comparison.
     */
    [[nodiscard]] constexpr bool operator>=(const Square& other) const noexcept {
        return value_ >= other.value_;
    }

    /**
     * Add an offset (direction) to a square.
     * @param offset The direction offset.
     * @return A new Square at the offset position.
     */
    [[nodiscard]] constexpr Square operator+(int offset) const noexcept {
        return Square(static_cast<std::uint8_t>(static_cast<int>(value_) + offset));
    }

    /**
     * Subtract an offset from a square.
     * @param offset The direction offset.
     * @return A new Square at the offset position.
     */
    [[nodiscard]] constexpr Square operator-(int offset) const noexcept {
        return Square(static_cast<std::uint8_t>(static_cast<int>(value_) - offset));
    }

    /**
     * Add-assign an offset.
     */
    constexpr Square& operator+=(int offset) noexcept {
        value_ = static_cast<std::uint8_t>(static_cast<int>(value_) + offset);
        return *this;
    }

    /**
     * Subtract-assign an offset.
     */
    constexpr Square& operator-=(int offset) noexcept {
        value_ = static_cast<std::uint8_t>(static_cast<int>(value_) - offset);
        return *this;
    }

private:
    std::uint8_t value_;
};

// ============================================================================
// Square Constants (matching Ada's board layout)
// ============================================================================

// Rank 8 (indices 21-28)
inline constexpr Square A8(21);
inline constexpr Square B8(22);
inline constexpr Square C8(23);
inline constexpr Square D8(24);
inline constexpr Square E8(25);
inline constexpr Square F8(26);
inline constexpr Square G8(27);
inline constexpr Square H8(28);

// Rank 7 (indices 31-38)
inline constexpr Square A7(31);
inline constexpr Square B7(32);
inline constexpr Square C7(33);
inline constexpr Square D7(34);
inline constexpr Square E7(35);
inline constexpr Square F7(36);
inline constexpr Square G7(37);
inline constexpr Square H7(38);

// Rank 6 (indices 41-48)
inline constexpr Square A6(41);
inline constexpr Square B6(42);
inline constexpr Square C6(43);
inline constexpr Square D6(44);
inline constexpr Square E6(45);
inline constexpr Square F6(46);
inline constexpr Square G6(47);
inline constexpr Square H6(48);

// Rank 5 (indices 51-58)
inline constexpr Square A5(51);
inline constexpr Square B5(52);
inline constexpr Square C5(53);
inline constexpr Square D5(54);
inline constexpr Square E5(55);
inline constexpr Square F5(56);
inline constexpr Square G5(57);
inline constexpr Square H5(58);

// Rank 4 (indices 61-68)
inline constexpr Square A4(61);
inline constexpr Square B4(62);
inline constexpr Square C4(63);
inline constexpr Square D4(64);
inline constexpr Square E4(65);
inline constexpr Square F4(66);
inline constexpr Square G4(67);
inline constexpr Square H4(68);

// Rank 3 (indices 71-78)
inline constexpr Square A3(71);
inline constexpr Square B3(72);
inline constexpr Square C3(73);
inline constexpr Square D3(74);
inline constexpr Square E3(75);
inline constexpr Square F3(76);
inline constexpr Square G3(77);
inline constexpr Square H3(78);

// Rank 2 (indices 81-88)
inline constexpr Square A2(81);
inline constexpr Square B2(82);
inline constexpr Square C2(83);
inline constexpr Square D2(84);
inline constexpr Square E2(85);
inline constexpr Square F2(86);
inline constexpr Square G2(87);
inline constexpr Square H2(88);

// Rank 1 (indices 91-98)
inline constexpr Square A1(91);
inline constexpr Square B1(92);
inline constexpr Square C1(93);
inline constexpr Square D1(94);
inline constexpr Square E1(95);
inline constexpr Square F1(96);
inline constexpr Square G1(97);
inline constexpr Square H1(98);

/**
 * Special value indicating no en passant square is available.
 */
inline constexpr Square kNoEnPassant(0);

// ============================================================================
// Lookup Tables
// ============================================================================

/**
 * Table indicating whether each square is a frame (border) square.
 * Frame squares are used as sentinels in the 10x12 mailbox representation
 * to detect out-of-bounds moves without explicit boundary checks.
 */
inline constexpr std::array<bool, kBoardSize> kSquareIsFrame = {{
    true, true,  true,  true,  true,  true,  true,  true,  true,  true,   // 0-9
    true, true,  true,  true,  true,  true,  true,  true,  true,  true,   // 10-19
    true, false, false, false, false, false, false, false, false, true,   // 20-29 (rank 8)
    true, false, false, false, false, false, false, false, false, true,   // 30-39 (rank 7)
    true, false, false, false, false, false, false, false, false, true,   // 40-49 (rank 6)
    true, false, false, false, false, false, false, false, false, true,   // 50-59 (rank 5)
    true, false, false, false, false, false, false, false, false, true,   // 60-69 (rank 4)
    true, false, false, false, false, false, false, false, false, true,   // 70-79 (rank 3)
    true, false, false, false, false, false, false, false, false, true,   // 80-89 (rank 2)
    true, false, false, false, false, false, false, false, false, true,   // 90-99 (rank 1)
    true, true,  true,  true,  true,  true,  true,  true,  true,  true,   // 100-109
    true, true,  true,  true,  true,  true,  true,  true,  true,  true    // 110-119
}};

/**
 * Check if a square is a frame (border) square.
 * @param square The square to check.
 * @return true if the square is part of the border frame.
 */
[[nodiscard]] constexpr bool is_frame(Square square) noexcept {
    return kSquareIsFrame[square.to_index()];
}

/**
 * Check if a square is a playable (non-frame) square.
 * @param square The square to check.
 * @return true if the square is playable.
 */
[[nodiscard]] constexpr bool is_playable(Square square) noexcept {
    return !kSquareIsFrame[square.to_index()];
}

/**
 * File lookup table for each square.
 * Returns -1 for frame squares, 0-7 for playable squares (a-h).
 */
inline constexpr std::array<Coordinate, kBoardSize> kFiles = {{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
}};

/**
 * Rank lookup table for each square.
 * Returns -1 for frame squares, 1-8 for playable squares.
 */
inline constexpr std::array<Coordinate, kBoardSize> kRanks = {{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  8,  8,  8,  8,  8,  8,  8,  8, -1,
    -1,  7,  7,  7,  7,  7,  7,  7,  7, -1,
    -1,  6,  6,  6,  6,  6,  6,  6,  6, -1,
    -1,  5,  5,  5,  5,  5,  5,  5,  5, -1,
    -1,  4,  4,  4,  4,  4,  4,  4,  4, -1,
    -1,  3,  3,  3,  3,  3,  3,  3,  3, -1,
    -1,  2,  2,  2,  2,  2,  2,  2,  2, -1,
    -1,  1,  1,  1,  1,  1,  1,  1,  1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
}};

/**
 * Diagonal lookup table (NW-SE diagonals, like a1-h8 is diagonal 0).
 * Returns -1 for frame squares, 0-14 for playable squares.
 */
inline constexpr std::array<Coordinate, kBoardSize> kDiagonals = {{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  7,  6,  5,  4,  3,  2,  1,  0, -1,
    -1,  6,  5,  4,  3,  2,  1,  0,  8, -1,
    -1,  5,  4,  3,  2,  1,  0,  8,  9, -1,
    -1,  4,  3,  2,  1,  0,  8,  9, 10, -1,
    -1,  3,  2,  1,  0,  8,  9, 10, 11, -1,
    -1,  2,  1,  0,  8,  9, 10, 11, 12, -1,
    -1,  1,  0,  8,  9, 10, 11, 12, 13, -1,
    -1,  0,  8,  9, 10, 11, 12, 13, 14, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
}};

/**
 * Anti-diagonal lookup table (NE-SW diagonals, like a8-h1 is anti-diagonal 7).
 * Returns -1 for frame squares, 0-14 for playable squares.
 */
inline constexpr std::array<Coordinate, kBoardSize> kAntiDiagonals = {{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  7,  8,  9, 10, 11, 12, 13, 14, -1,
    -1,  6,  7,  8,  9, 10, 11, 12, 13, -1,
    -1,  5,  6,  7,  8,  9, 10, 11, 12, -1,
    -1,  4,  5,  6,  7,  8,  9, 10, 11, -1,
    -1,  3,  4,  5,  6,  7,  8,  9, 10, -1,
    -1,  2,  3,  4,  5,  6,  7,  8,  9, -1,
    -1,  1,  2,  3,  4,  5,  6,  7,  8, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
}};

/**
 * Square color table (which squares are light/dark).
 * Used for bishop color evaluation and display.
 */
inline constexpr std::array<Color, kBoardSize> kColorBoard = {{
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White
}};

/**
 * Square string representation table for converting squares to algebraic notation.
 * Frame squares are represented as "  " (two spaces).
 */
inline constexpr std::array<const char*, kBoardSize> kSquareStrings = {{
    "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ",
    "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ",
    "  ", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "  ",
    "  ", "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "  ",
    "  ", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "  ",
    "  ", "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "  ",
    "  ", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "  ",
    "  ", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "  ",
    "  ", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "  ",
    "  ", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "  ",
    "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ",
    "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "
}};

// ============================================================================
// Coordinate Functions
// ============================================================================

/**
 * Get the file (column) of a square.
 * @param square The square to query.
 * @return File coordinate 0-7 for a-h, or -1 for frame squares.
 */
[[nodiscard]] constexpr Coordinate file(Square square) noexcept {
    return kFiles[square.to_index()];
}

/**
 * Get the rank (row) of a square.
 * @param square The square to query.
 * @return Rank coordinate 1-8, or -1 for frame squares.
 */
[[nodiscard]] constexpr Coordinate rank(Square square) noexcept {
    return kRanks[square.to_index()];
}

/**
 * Get the diagonal (NW-SE) of a square.
 * @param square The square to query.
 * @return Diagonal coordinate 0-14, or -1 for frame squares.
 */
[[nodiscard]] constexpr Coordinate diagonal(Square square) noexcept {
    return kDiagonals[square.to_index()];
}

/**
 * Get the anti-diagonal (NE-SW) of a square.
 * @param square The square to query.
 * @return Anti-diagonal coordinate 0-14, or -1 for frame squares.
 */
[[nodiscard]] constexpr Coordinate anti_diagonal(Square square) noexcept {
    return kAntiDiagonals[square.to_index()];
}

/**
 * Get the color of a square (light or dark).
 * @param square The square to query.
 * @return The color of the square.
 */
[[nodiscard]] constexpr Color square_color(Square square) noexcept {
    return kColorBoard[square.to_index()];
}

// ============================================================================
// Square String Conversion
// ============================================================================

/**
 * Convert a square to its algebraic notation string.
 * @param square The square to convert.
 * @return A null-terminated string like "a1", "e4", etc. (or "  " for frame).
 */
[[nodiscard]] constexpr const char* square_to_string(Square square) noexcept {
    return kSquareStrings[square.to_index()];
}

/**
 * Parse a square from an algebraic notation string.
 * @param str The string to parse (e.g., "a1", "e4").
 * @return The corresponding Square.
 * @throws std::invalid_argument if the string is not a valid square.
 */
[[nodiscard]] Square square_from_string(const std::string& str);

// ============================================================================
// Board Class
// ============================================================================

/**
 * The Board class represents the 10x12 mailbox board.
 * It wraps a std::array<Piece, 120> and provides type-safe access.
 */
class Board {
public:
    /**
     * Default constructor creates an empty board (all Frame pieces).
     */
    constexpr Board() noexcept : pieces_{} {
        // Initialize all squares to Frame
        for (std::size_t i = 0; i < kBoardSize; ++i) {
            pieces_[i] = Piece::Frame;
        }
    }

    /**
     * Access a piece at a given square (const).
     * @param square The square to access.
     * @return The piece at that square.
     */
    [[nodiscard]] constexpr Piece operator[](Square square) const noexcept {
        return pieces_[square.to_index()];
    }

    /**
     * Access a piece at a given square (mutable).
     * @param square The square to access.
     * @return Reference to the piece at that square.
     */
    [[nodiscard]] constexpr Piece& operator[](Square square) noexcept {
        return pieces_[square.to_index()];
    }

    /**
     * Access piece by raw index (const).
     * @param index The board index (0-119).
     * @return The piece at that index.
     */
    [[nodiscard]] constexpr Piece at_index(std::size_t index) const noexcept {
        return pieces_[index];
    }

    /**
     * Access piece by raw index (mutable).
     * @param index The board index (0-119).
     * @return Reference to the piece at that index.
     */
    [[nodiscard]] constexpr Piece& at_index(std::size_t index) noexcept {
        return pieces_[index];
    }

    /**
     * Get the underlying array (const).
     * @return Const reference to the internal array.
     */
    [[nodiscard]] constexpr const std::array<Piece, kBoardSize>& data() const noexcept {
        return pieces_;
    }

    /**
     * Get the underlying array (mutable).
     * @return Reference to the internal array.
     */
    [[nodiscard]] constexpr std::array<Piece, kBoardSize>& data() noexcept {
        return pieces_;
    }

    /**
     * Equality comparison.
     * Note: Not constexpr in C++17 because std::array::operator== is not constexpr.
     */
    [[nodiscard]] bool operator==(const Board& other) const noexcept {
        return pieces_ == other.pieces_;
    }

    /**
     * Inequality comparison.
     * Note: Not constexpr in C++17 because std::array::operator!= is not constexpr.
     */
    [[nodiscard]] bool operator!=(const Board& other) const noexcept {
        return pieces_ != other.pieces_;
    }

private:
    std::array<Piece, kBoardSize> pieces_;
};

/**
 * Board type alias for the underlying array type.
 * Matches Ada's Board_Type = array (Square_Type'Range) of Piece_Type.
 */
using BoardArray = std::array<Piece, kBoardSize>;

} // namespace chess::board

#endif // ADACHESS_BOARD_BOARD_HPP

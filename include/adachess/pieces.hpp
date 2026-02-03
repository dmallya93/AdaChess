/**
 * AdaChess - Smart Chess Engine
 *
 * Piece type definitions including all piece types, subtypes, and utilities.
 * C++ port of the original Ada implementation (chess-pieces.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_PIECES_HPP
#define ADACHESS_PIECES_HPP

#include <cstdint>
#include <array>

#include "adachess/colors.hpp"

namespace chess {

/**
 * Piece enumeration representing all possible board contents.
 * Uses explicit underlying type and values matching the Ada representation clause.
 *
 * Frame = 0: Border squares (sentinel for 10x12 mailbox representation)
 * Empty = 1: Empty square
 * White pieces: 2-7
 * Black pieces: 8-13
 */
enum class Piece : std::uint8_t {
    Frame        = 0,
    Empty        = 1,
    WhitePawn    = 2,
    WhiteKnight  = 3,
    WhiteBishop  = 4,
    WhiteRook    = 5,
    WhiteQueen   = 6,
    WhiteKing    = 7,
    BlackPawn    = 8,
    BlackKnight  = 9,
    BlackBishop  = 10,
    BlackRook    = 11,
    BlackQueen   = 12,
    BlackKing    = 13
};

/**
 * Number of distinct piece values (for array sizing).
 */
inline constexpr std::size_t kNumPieceValues = 14;

// ============================================================================
// Piece Type Predicates
// These correspond to Ada's subtypes with Static_Predicate
// ============================================================================

/**
 * Check if the piece is a valid board piece (excludes Frame).
 * Corresponds to Ada's Board_Piece_Type subtype.
 */
[[nodiscard]] constexpr bool is_board_piece(Piece piece) noexcept {
    return piece != Piece::Frame;
}

/**
 * Check if the piece is an actual chess piece (excludes Frame and Empty).
 * Corresponds to Ada's Chess_Piece_Type subtype.
 */
[[nodiscard]] constexpr bool is_chess_piece(Piece piece) noexcept {
    return piece != Piece::Frame && piece != Piece::Empty;
}

/**
 * Check if the piece is empty.
 * Corresponds to Ada's overloaded "not" operator for pieces.
 * Returns true if piece is Empty, false otherwise.
 */
[[nodiscard]] constexpr bool is_empty(Piece piece) noexcept {
    return piece == Piece::Empty;
}

/**
 * Check if the piece is a white piece.
 * Corresponds to Ada's White_Piece_Type subtype.
 */
[[nodiscard]] constexpr bool is_white_piece(Piece piece) noexcept {
    const auto val = static_cast<std::uint8_t>(piece);
    return val >= 2 && val <= 7;
}

/**
 * Check if the piece is a black piece.
 * Corresponds to Ada's Black_Piece_Type subtype.
 */
[[nodiscard]] constexpr bool is_black_piece(Piece piece) noexcept {
    const auto val = static_cast<std::uint8_t>(piece);
    return val >= 8 && val <= 13;
}

/**
 * Get the color of a chess piece.
 * Precondition: piece must be a chess piece (not Frame or Empty).
 *
 * @param piece A chess piece (White or Black).
 * @return The color of the piece.
 */
[[nodiscard]] constexpr Color piece_color(Piece piece) noexcept {
    return is_white_piece(piece) ? Color::White : Color::Black;
}

/**
 * Check if piece belongs to the given side/color.
 */
[[nodiscard]] constexpr bool is_piece_of_color(Piece piece, Color color) noexcept {
    return (color == Color::White) ? is_white_piece(piece) : is_black_piece(piece);
}

// ============================================================================
// Piece Kind Predicates
// ============================================================================

/**
 * Check if the piece is a pawn (white or black).
 * Corresponds to Ada's Pawn_Type subtype.
 */
[[nodiscard]] constexpr bool is_pawn(Piece piece) noexcept {
    return piece == Piece::WhitePawn || piece == Piece::BlackPawn;
}

/**
 * Check if the piece is a knight (white or black).
 * Corresponds to Ada's Knight_Type subtype.
 */
[[nodiscard]] constexpr bool is_knight(Piece piece) noexcept {
    return piece == Piece::WhiteKnight || piece == Piece::BlackKnight;
}

/**
 * Check if the piece is a bishop (white or black).
 * Corresponds to Ada's Bishop_Type subtype.
 */
[[nodiscard]] constexpr bool is_bishop(Piece piece) noexcept {
    return piece == Piece::WhiteBishop || piece == Piece::BlackBishop;
}

/**
 * Check if the piece is a rook (white or black).
 * Corresponds to Ada's Rook_Type subtype.
 */
[[nodiscard]] constexpr bool is_rook(Piece piece) noexcept {
    return piece == Piece::WhiteRook || piece == Piece::BlackRook;
}

/**
 * Check if the piece is a queen (white or black).
 * Corresponds to Ada's Queen_Type subtype.
 */
[[nodiscard]] constexpr bool is_queen(Piece piece) noexcept {
    return piece == Piece::WhiteQueen || piece == Piece::BlackQueen;
}

/**
 * Check if the piece is a king (white or black).
 * Corresponds to Ada's King_Type subtype.
 */
[[nodiscard]] constexpr bool is_king(Piece piece) noexcept {
    return piece == Piece::WhiteKing || piece == Piece::BlackKing;
}

/**
 * Check if the piece is a sliding piece (bishop, rook, or queen).
 * Corresponds to Ada's Slide_Type subtype.
 */
[[nodiscard]] constexpr bool is_sliding_piece(Piece piece) noexcept {
    return is_bishop(piece) || is_rook(piece) || is_queen(piece);
}

/**
 * Check if the piece is a non-pawn piece (for the given color).
 * Corresponds to Ada's White_Non_Pawn and Black_Non_Pawn subtypes.
 */
[[nodiscard]] constexpr bool is_non_pawn(Piece piece) noexcept {
    return is_chess_piece(piece) && !is_pawn(piece) && !is_king(piece);
}

/**
 * Check if the piece is a valid promotion piece for white.
 * Corresponds to Ada's White_Promotion_Type subtype.
 */
[[nodiscard]] constexpr bool is_white_promotion_piece(Piece piece) noexcept {
    return piece == Piece::WhiteKnight || piece == Piece::WhiteBishop ||
           piece == Piece::WhiteRook || piece == Piece::WhiteQueen;
}

/**
 * Check if the piece is a valid promotion piece for black.
 * Corresponds to Ada's Black_Promotion_Type subtype.
 */
[[nodiscard]] constexpr bool is_black_promotion_piece(Piece piece) noexcept {
    return piece == Piece::BlackKnight || piece == Piece::BlackBishop ||
           piece == Piece::BlackRook || piece == Piece::BlackQueen;
}

/**
 * Check if the piece is a valid promotion piece for the given color.
 */
[[nodiscard]] constexpr bool is_promotion_piece(Piece piece, Color color) noexcept {
    return (color == Color::White) ? is_white_promotion_piece(piece)
                                    : is_black_promotion_piece(piece);
}

// ============================================================================
// Piece Symbols
// ============================================================================

/**
 * Character symbols for each piece type.
 * Matches Ada's Symbols constant:
 * - White pieces: uppercase (P, N, B, R, Q, K)
 * - Black pieces: lowercase (p, n, b, r, q, k)
 * - Frame and Empty: space character
 */
inline constexpr std::array<char, kNumPieceValues> kPieceSymbols = {{
    ' ',  // Frame
    ' ',  // Empty
    'P',  // WhitePawn
    'N',  // WhiteKnight
    'B',  // WhiteBishop
    'R',  // WhiteRook
    'Q',  // WhiteQueen
    'K',  // WhiteKing
    'p',  // BlackPawn
    'n',  // BlackKnight
    'b',  // BlackBishop
    'r',  // BlackRook
    'q',  // BlackQueen
    'k'   // BlackKing
}};

/**
 * Get the character symbol for a piece.
 *
 * @param piece The piece to get the symbol for.
 * @return The character symbol (uppercase for white, lowercase for black).
 */
[[nodiscard]] constexpr char piece_symbol(Piece piece) noexcept {
    return kPieceSymbols[static_cast<std::size_t>(piece)];
}

/**
 * Convert a piece symbol character to a Piece enum value.
 *
 * @param symbol The character symbol (P, N, B, R, Q, K or p, n, b, r, q, k).
 * @return The corresponding Piece, or Piece::Empty if not recognized.
 */
[[nodiscard]] Piece piece_from_symbol(char symbol) noexcept;

// ============================================================================
// Piece Counter Types
// ============================================================================

/**
 * Type for counting pieces (0-16 per side).
 * Corresponds to Ada's Piece_Counter_Type.
 */
using PieceCounter = std::uint8_t;
inline constexpr PieceCounter kMaxPieceCount = 16;

/**
 * Type for counting pawns (0-8 per side).
 * Corresponds to Ada's Pawn_Counter_Type.
 */
using PawnCounter = std::uint8_t;
inline constexpr PawnCounter kMaxPawnCount = 8;

// ============================================================================
// Piece Conversion Utilities
// ============================================================================

/**
 * Convert a piece to its array index.
 *
 * @param piece The piece to convert.
 * @return The underlying integer value (0-13).
 */
[[nodiscard]] constexpr std::size_t to_index(Piece piece) noexcept {
    return static_cast<std::size_t>(piece);
}

/**
 * Convert an index to a Piece.
 * Warning: No bounds checking is performed.
 *
 * @param index The index (0-13).
 * @return The corresponding Piece.
 */
[[nodiscard]] constexpr Piece piece_from_index(std::size_t index) noexcept {
    return static_cast<Piece>(index);
}

} // namespace chess

#endif // ADACHESS_PIECES_HPP

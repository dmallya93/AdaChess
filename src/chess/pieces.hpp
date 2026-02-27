// ---------------------------------------------------------------------------
// AdaChess – Piece type definitions
// Translated from: chess-pieces.ads
// ---------------------------------------------------------------------------
#pragma once

#include <array>
#include <cstdint>
#include "chess/assert.hpp"

namespace chess {

// -----------------------------------------------------------------------
// Piece_Type: All piece values with explicit Ada representation clause.
// Ada: for Piece_Type use (Frame => 0, Empty => 1, ... Black_King => 13);
// Design Decision #1: enum class with explicit std::uint8_t values.
// -----------------------------------------------------------------------
enum class Piece : std::uint8_t {
    Frame        = 0,
    Empty        = 1,
    White_Pawn   = 2,
    White_Knight = 3,
    White_Bishop = 4,
    White_Rook   = 5,
    White_Queen  = 6,
    White_King   = 7,
    Black_Pawn   = 8,
    Black_Knight = 9,
    Black_Bishop = 10,
    Black_Rook   = 11,
    Black_Queen  = 12,
    Black_King   = 13
};

/// Total number of Piece enum values (for array sizing).
inline constexpr std::size_t Piece_Count = 14;

// -----------------------------------------------------------------------
// Subtype predicates (translated from Ada static predicates)
// -----------------------------------------------------------------------

/// Ada: Board_Piece_Type — everything except Frame.
[[nodiscard]] constexpr bool is_board_piece(Piece p) noexcept {
    return p != Piece::Frame;
}

/// Ada: Chess_Piece_Type — actual chess pieces (not Empty, not Frame).
[[nodiscard]] constexpr bool is_chess_piece(Piece p) noexcept {
    auto v = static_cast<std::uint8_t>(p);
    return v >= 2 && v <= 13;
}

/// Ada: White_Piece_Type
[[nodiscard]] constexpr bool is_white_piece(Piece p) noexcept {
    auto v = static_cast<std::uint8_t>(p);
    return v >= 2 && v <= 7;
}

/// Ada: Black_Piece_Type
[[nodiscard]] constexpr bool is_black_piece(Piece p) noexcept {
    auto v = static_cast<std::uint8_t>(p);
    return v >= 8 && v <= 13;
}

/// Ada: White_Promotion_Type (Knight, Bishop, Rook, Queen)
[[nodiscard]] constexpr bool is_white_promotion(Piece p) noexcept {
    return p == Piece::White_Knight || p == Piece::White_Bishop ||
           p == Piece::White_Rook   || p == Piece::White_Queen;
}

/// Ada: Black_Promotion_Type
[[nodiscard]] constexpr bool is_black_promotion(Piece p) noexcept {
    return p == Piece::Black_Knight || p == Piece::Black_Bishop ||
           p == Piece::Black_Rook   || p == Piece::Black_Queen;
}

/// Ada: White_Non_Pawn
[[nodiscard]] constexpr bool is_white_non_pawn(Piece p) noexcept {
    return is_white_promotion(p); // Same set: N, B, R, Q
}

/// Ada: Black_Non_Pawn
[[nodiscard]] constexpr bool is_black_non_pawn(Piece p) noexcept {
    return is_black_promotion(p);
}

/// Ada: Pawn_Type
[[nodiscard]] constexpr bool is_pawn(Piece p) noexcept {
    return p == Piece::White_Pawn || p == Piece::Black_Pawn;
}

/// Ada: Knight_Type
[[nodiscard]] constexpr bool is_knight(Piece p) noexcept {
    return p == Piece::White_Knight || p == Piece::Black_Knight;
}

/// Ada: Bishop_Type
[[nodiscard]] constexpr bool is_bishop(Piece p) noexcept {
    return p == Piece::White_Bishop || p == Piece::Black_Bishop;
}

/// Ada: Rook_Type
[[nodiscard]] constexpr bool is_rook(Piece p) noexcept {
    return p == Piece::White_Rook || p == Piece::Black_Rook;
}

/// Ada: Queen_Type
[[nodiscard]] constexpr bool is_queen(Piece p) noexcept {
    return p == Piece::White_Queen || p == Piece::Black_Queen;
}

/// Ada: King_Type
[[nodiscard]] constexpr bool is_king(Piece p) noexcept {
    return p == Piece::White_King || p == Piece::Black_King;
}

/// Ada: Slide_Type (Bishop, Rook, or Queen)
[[nodiscard]] constexpr bool is_slider(Piece p) noexcept {
    return is_bishop(p) || is_rook(p) || is_queen(p);
}

// -----------------------------------------------------------------------
// Ada's "not" operator on Piece_Type:
//   function "not" (Piece : in Piece_Type) return Boolean is
//     (if Piece = Empty then True else False) with Pre => (Piece /= Frame);
// -----------------------------------------------------------------------
/// Returns true if piece is Empty. Precondition: piece must not be Frame.
[[nodiscard]] constexpr bool piece_is_empty(Piece p) noexcept {
    ADACHESS_PRECOND(p != Piece::Frame);
    return p == Piece::Empty;
}

// -----------------------------------------------------------------------
// Symbols array — maps Piece to its single-character representation.
// -----------------------------------------------------------------------
inline constexpr std::array<char, Piece_Count> Symbols = {{
    ' ', ' ',                       // Frame, Empty
    'P', 'N', 'B', 'R', 'Q', 'K', // White pieces
    'p', 'n', 'b', 'r', 'q', 'k'  // Black pieces
}};

// -----------------------------------------------------------------------
// Piece counter types (Ada subtypes with range constraints).
// -----------------------------------------------------------------------
using PieceCounter = unsigned int; // range 0..16
using PawnCounter  = unsigned int; // range 0..8

inline constexpr PieceCounter Max_Piece_Count = 16;
inline constexpr PawnCounter  Max_Pawn_Count  = 8;

} // namespace chess

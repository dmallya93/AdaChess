// ---------------------------------------------------------------------------
// AdaChess – Move type definitions
// Translated from: chess-moves.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include "chess/pieces.hpp"

namespace chess {

// Forward-declared alias for board square indices.
// Ada: Square_Type is defined in Chess.Board; we only need the integer type.
using Square = int;

// -----------------------------------------------------------------------
// Ambiguous_Flag_Type
// Ada: (Ambiguous_None, Ambiguous_File, Ambiguous_Rank, Ambiguous_Both)
//      with Size => 2;
// Helps output moves correctly in algebraic notation.
// -----------------------------------------------------------------------
enum class AmbiguousFlag : std::uint8_t {
    Ambiguous_None = 0,
    Ambiguous_File = 1,
    Ambiguous_Rank = 2,
    Ambiguous_Both = 3
};

// -----------------------------------------------------------------------
// Flag_Type
// Ada: (Standard_Move, Pawn_Move_Two_Square, Capture_En_Passant,
//       Promotion, Castle, Null_Move, No_Move) with Size => 3;
// One-bit-style flags that help the move generator work faster.
// -----------------------------------------------------------------------
enum class Flag : std::uint8_t {
    Standard_Move        = 0,
    Pawn_Move_Two_Square = 1,
    Capture_En_Passant   = 2,
    Promotion            = 3,
    Castle               = 4,
    Null_Move            = 5,
    No_Move              = 6
};

// -----------------------------------------------------------------------
// Castle_Type
// Ada: (No_Castle, White_Kingside, White_Queenside,
//       Black_Kingside, Black_Queenside) with Size => 3;
// -----------------------------------------------------------------------
enum class Castle : std::uint8_t {
    No_Castle        = 0,
    White_Kingside   = 1,
    White_Queenside  = 2,
    Black_Kingside   = 3,
    Black_Queenside  = 4
};

// -----------------------------------------------------------------------
// Check_Type
// Ada: (No_Check, Direct_Check, Discovery_Check, Double_Check,
//       Checkmate, Unknown_Check) with Size => 3;
// All possible check types. Unknown_Check is used when a FEN position
// with a king in check appears.
// -----------------------------------------------------------------------
enum class Check : std::uint8_t {
    No_Check        = 0,
    Direct_Check    = 1,
    Discovery_Check = 2,
    Double_Check    = 3,
    Checkmate       = 4,
    Unknown_Check   = 5
};

// Ada: function "not" (Checks : in Check_Type) return Boolean is
//        (if Checks = No_Check then True else False);
// Helper to cast a Check as a boolean: True when there is no check.
[[nodiscard]] constexpr bool operator!(Check c) noexcept {
    return c == Check::No_Check;
}

// -----------------------------------------------------------------------
// Move_Type (struct Move)
// Ada record with: Piece, Captured, From, To, Flag, Promotion, Check,
// Ambiguous_Flag.
// -----------------------------------------------------------------------
struct Move {
    Piece        piece          = Piece::Empty;
    Piece        captured       = Piece::Empty;
    Square       from           = 0;
    Square       to             = 0;
    Flag         flag           = Flag::No_Move;
    Piece        promotion      = Piece::Empty;
    Check        check          = Check::No_Check;
    AmbiguousFlag ambiguous_flag = AmbiguousFlag::Ambiguous_None;
};

// Ada: function "=" compares From, To, Piece, Captured, Promotion, Check,
//      Flag — but NOT Ambiguous_Flag.
[[nodiscard]] constexpr bool operator==(const Move& lhs, const Move& rhs) noexcept {
    return lhs.from      == rhs.from
        && lhs.to        == rhs.to
        && lhs.piece     == rhs.piece
        && lhs.captured  == rhs.captured
        && lhs.promotion == rhs.promotion
        && lhs.check     == rhs.check
        && lhs.flag      == rhs.flag;
}

[[nodiscard]] constexpr bool operator!=(const Move& lhs, const Move& rhs) noexcept {
    return !(lhs == rhs);
}

// -----------------------------------------------------------------------
// Empty_Move constant
// Ada: all Empty/0/No_Move/No_Check/Ambiguous_None
// -----------------------------------------------------------------------
inline constexpr Move Empty_Move = {
    Piece::Empty,                   // piece
    Piece::Empty,                   // captured
    0,                              // from
    0,                              // to
    Flag::No_Move,                  // flag
    Piece::Empty,                   // promotion
    Check::No_Check,                // check
    AmbiguousFlag::Ambiguous_None   // ambiguous_flag
};

// -----------------------------------------------------------------------
// Move_Is_Capture
// Ada: (Move.Captured /= Empty or else Move.Flag = Capture_En_Passant)
// -----------------------------------------------------------------------
[[nodiscard]] constexpr bool Move_Is_Capture(const Move& m) noexcept {
    return m.captured != Piece::Empty || m.flag == Flag::Capture_En_Passant;
}

} // namespace chess

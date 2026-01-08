// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/square.hpp"
#include "adachess/core/types.hpp"
#include <cstdint>

namespace adachess {

// Forward declaration
class Chessboard;

// Ambiguous flag for algebraic notation
// Helps to output moves correctly in algebraic notation
enum class AmbiguousFlag : uint8_t {
    None = 0,          // No ambiguity
    ByFile = 1,        // Disambiguate by file
    ByRank = 2,        // Disambiguate by rank
    ByBoth = 3         // Disambiguate by both file and rank
};

// Move flags - single bit information to help move operations
enum class MoveFlag : uint8_t {
    StandardMove = 0,       // Normal move
    PawnMoveTwoSquare = 1,  // Pawn moving two squares (allows en passant)
    CaptureEnPassant = 2,   // En passant capture
    Promotion = 3,          // Pawn promotion
    Castle = 4,             // Castling move
    NullMove = 5,           // Null move (for null move pruning)
    NoMove = 6              // Invalid/empty move
};

// Castle type
enum class CastleType : uint8_t {
    NoCastle = 0,
    WhiteKingside = 1,
    WhiteQueenside = 2,
    BlackKingside = 3,
    BlackQueenside = 4
};

// Check type enumeration
enum class CheckType : uint8_t {
    NoCheck = 0,        // No check
    DirectCheck = 1,    // Direct check from moving piece
    DiscoveryCheck = 2, // Discovered check
    DoubleCheck = 3,    // Double check (direct + discovery)
    Checkmate = 4,      // Checkmate
    UnknownCheck = 5    // Check status unknown (e.g., from FEN)
};

// Helper function to cast Check as boolean
constexpr bool is_check(CheckType check) {
    return check != CheckType::NoCheck;
}

// Move structure
// This is instantiated billions of times during search, so size matters
struct Move {
    Piece piece = Piece::Empty;             // Piece that moves
    Piece captured = Piece::Empty;          // Captured piece (or Empty)
    Square from = 0;                        // Origin square
    Square to = 0;                          // Destination square
    MoveFlag flag = MoveFlag::StandardMove; // Move flag
    Piece promotion = Piece::Empty;         // Promotion piece (or Empty)
    CheckType check = CheckType::NoCheck;   // Check type
    AmbiguousFlag ambiguous = AmbiguousFlag::None; // Ambiguous flag for notation

    // Default constructor
    Move() = default;

    // Constructor with from/to
    Move(Square from_sq, Square to_sq)
        : from(from_sq), to(to_sq) {}

    // Constructor with from/to/flag
    Move(Square from_sq, Square to_sq, MoveFlag mv_flag)
        : from(from_sq), to(to_sq), flag(mv_flag) {}
};

// Empty move constant
constexpr Move EMPTY_MOVE{};

// Move equality operator
// Compares all relevant fields
inline bool operator==(const Move& left, const Move& right) {
    return left.from == right.from &&
           left.to == right.to &&
           left.piece == right.piece &&
           left.captured == right.captured &&
           left.promotion == right.promotion &&
           left.check == right.check &&
           left.flag == right.flag;
}

inline bool operator!=(const Move& left, const Move& right) {
    return !(left == right);
}

// Move_Is_Capture helper function
// Returns true if the move is a capture (including en passant)
inline bool move_is_capture(const Move& move) {
    return move.captured != Piece::Empty ||
           move.flag == MoveFlag::CaptureEnPassant;
}

// Move_Is_Tactical helper function
// Tactical moves: checks, captures, promotions, castles
inline bool move_is_tactical(const Move& move) {
    return move.check != CheckType::NoCheck ||
           move.captured != Piece::Empty ||
           move.flag == MoveFlag::CaptureEnPassant ||
           move.promotion != Piece::Empty ||
           move.flag == MoveFlag::Castle;
}

// Move_Is_Quiet helper function
// Quiet moves are the opposite of tactical moves
inline bool move_is_quiet(const Move& move) {
    return !move_is_tactical(move);
}

} // namespace adachess

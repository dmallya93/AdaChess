/**
 * AdaChess - Smart Chess Engine
 *
 * Move representation including move types, flags, and enumerations.
 * C++ port of the original Ada implementation (chess-moves.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_MOVES_HPP
#define ADACHESS_MOVES_HPP

#include <cstdint>

#include "adachess/pieces.hpp"
#include "adachess/board/board.hpp"

namespace chess {

// ============================================================================
// Move Flag Enumerations
// ============================================================================

/**
 * Ambiguous flag type for algebraic notation disambiguation.
 * When multiple pieces of the same type can move to the same square,
 * we need to specify which one with file, rank, or both.
 */
enum class AmbiguousFlag : std::uint8_t {
    None = 0,       // No disambiguation needed
    File = 1,       // Disambiguate by file (e.g., Rab1)
    Rank = 2,       // Disambiguate by rank (e.g., R1b3)
    Both = 3        // Disambiguate by both file and rank (e.g., Qa1b2)
};

/**
 * Move flag type indicating the nature of the move.
 * These flags help the move generator and make/unmake functions
 * handle special moves efficiently.
 */
enum class MoveFlag : std::uint8_t {
    Standard = 0,           // Normal move (non-pawn)
    PawnMoveTwoSquares = 1, // Pawn double advance (creates en passant target)
    CaptureEnPassant = 2,   // En passant capture
    Promotion = 3,          // Pawn promotion
    Castle = 4,             // Castling move
    NullMove = 5,           // Null move (for null-move pruning)
    NoMove = 6              // Invalid/empty move marker
};

/**
 * Castle type enumeration for the four possible castling moves.
 */
enum class CastleType : std::uint8_t {
    None = 0,            // No castle
    WhiteKingside = 1,   // White O-O (e1-g1)
    WhiteQueenside = 2,  // White O-O-O (e1-c1)
    BlackKingside = 3,   // Black O-O (e8-g8)
    BlackQueenside = 4   // Black O-O-O (e8-c8)
};

/**
 * Check type enumeration for the different types of checks.
 * This helps with move notation (adding + or #) and search heuristics.
 */
enum class CheckType : std::uint8_t {
    NoCheck = 0,        // Not a checking move
    DirectCheck = 1,    // Check delivered by the moving piece
    DiscoveryCheck = 2, // Check revealed by moving a blocking piece
    DoubleCheck = 3,    // Both direct and discovery check
    Checkmate = 4,      // Checkmate (game over)
    UnknownCheck = 5    // Check detected from FEN but type unknown
};

// ============================================================================
// Check Type Utilities
// ============================================================================

/**
 * Check if a CheckType indicates the king is in check.
 * Matches Ada's overloaded "not" operator for Check_Type.
 *
 * @param check The check type.
 * @return true if there is no check (NoCheck), false otherwise.
 */
[[nodiscard]] constexpr bool is_no_check(CheckType check) noexcept {
    return check == CheckType::NoCheck;
}

/**
 * Check if a CheckType indicates the king is in check.
 *
 * @param check The check type.
 * @return true if there is a check.
 */
[[nodiscard]] constexpr bool is_check(CheckType check) noexcept {
    return check != CheckType::NoCheck;
}

// ============================================================================
// Move Structure
// ============================================================================

/**
 * Move structure representing a chess move.
 *
 * This structure contains all information needed to:
 * - Execute the move on the board (play)
 * - Undo the move (undo)
 * - Display the move in various notations
 *
 * Fields:
 * - piece: The piece that is moving
 * - captured: The piece being captured (Empty if none)
 * - from: The source square
 * - to: The destination square
 * - flag: Move type flag (castle, en passant, promotion, etc.)
 * - promotion: The piece to promote to (Empty if not a promotion)
 * - check: The type of check delivered (NoCheck if none)
 * - ambiguous_flag: Disambiguation info for algebraic notation
 */
struct Move {
    Piece piece{Piece::Empty};                      // Moving piece
    Piece captured{Piece::Empty};                   // Captured piece (Empty if none)
    board::Square from{};                           // Source square
    board::Square to{};                             // Destination square
    MoveFlag flag{MoveFlag::NoMove};                // Move type flag
    Piece promotion{Piece::Empty};                  // Promotion piece (Empty if not promotion)
    CheckType check{CheckType::NoCheck};            // Check type
    AmbiguousFlag ambiguous_flag{AmbiguousFlag::None}; // Notation disambiguation

    /**
     * Default constructor creates an empty/invalid move.
     */
    constexpr Move() noexcept = default;

    /**
     * Constructor for creating a standard move.
     *
     * @param piece_arg The moving piece.
     * @param from_arg Source square.
     * @param to_arg Destination square.
     * @param captured_arg Captured piece (default Empty).
     * @param flag_arg Move flag (default Standard).
     * @param promotion_arg Promotion piece (default Empty).
     * @param check_arg Check type (default NoCheck).
     * @param ambiguous_arg Ambiguous flag (default None).
     */
    constexpr Move(
        Piece piece_arg,
        board::Square from_arg,
        board::Square to_arg,
        Piece captured_arg = Piece::Empty,
        MoveFlag flag_arg = MoveFlag::Standard,
        Piece promotion_arg = Piece::Empty,
        CheckType check_arg = CheckType::NoCheck,
        AmbiguousFlag ambiguous_arg = AmbiguousFlag::None
    ) noexcept
        : piece(piece_arg)
        , captured(captured_arg)
        , from(from_arg)
        , to(to_arg)
        , flag(flag_arg)
        , promotion(promotion_arg)
        , check(check_arg)
        , ambiguous_flag(ambiguous_arg)
    {}
};

// ============================================================================
// Move Constants
// ============================================================================

/**
 * Empty/invalid move constant.
 * Used as a sentinel value or default.
 */
inline constexpr Move kEmptyMove{};

// ============================================================================
// Move Comparison Operators
// ============================================================================

/**
 * Equality comparison for moves.
 * Two moves are equal if all their components match.
 * Matches Ada's overloaded "=" operator for Move_Type.
 *
 * @param lhs Left-hand side move.
 * @param rhs Right-hand side move.
 * @return true if the moves are equal.
 */
[[nodiscard]] constexpr bool operator==(const Move& lhs, const Move& rhs) noexcept {
    return lhs.from == rhs.from &&
           lhs.to == rhs.to &&
           lhs.piece == rhs.piece &&
           lhs.captured == rhs.captured &&
           lhs.promotion == rhs.promotion &&
           lhs.check == rhs.check &&
           lhs.flag == rhs.flag;
}

/**
 * Inequality comparison for moves.
 *
 * @param lhs Left-hand side move.
 * @param rhs Right-hand side move.
 * @return true if the moves are not equal.
 */
[[nodiscard]] constexpr bool operator!=(const Move& lhs, const Move& rhs) noexcept {
    return !(lhs == rhs);
}

// ============================================================================
// Move Utility Functions
// ============================================================================

/**
 * Check if a move is a capture.
 * A move is a capture if it takes an opponent's piece or is an en passant capture.
 * Matches Ada's Move_Is_Capture function.
 *
 * @param move The move to check.
 * @return true if the move is a capture.
 */
[[nodiscard]] constexpr bool move_is_capture(const Move& move) noexcept {
    return move.captured != Piece::Empty || move.flag == MoveFlag::CaptureEnPassant;
}

/**
 * Check if a move is a promotion.
 *
 * @param move The move to check.
 * @return true if the move is a promotion.
 */
[[nodiscard]] constexpr bool move_is_promotion(const Move& move) noexcept {
    return move.flag == MoveFlag::Promotion;
}

/**
 * Check if a move is a castling move.
 *
 * @param move The move to check.
 * @return true if the move is a castle.
 */
[[nodiscard]] constexpr bool move_is_castle(const Move& move) noexcept {
    return move.flag == MoveFlag::Castle;
}

/**
 * Check if a move is an en passant capture.
 *
 * @param move The move to check.
 * @return true if the move is an en passant capture.
 */
[[nodiscard]] constexpr bool move_is_en_passant(const Move& move) noexcept {
    return move.flag == MoveFlag::CaptureEnPassant;
}

/**
 * Check if a move is a pawn two-square advance.
 *
 * @param move The move to check.
 * @return true if the move is a pawn double advance.
 */
[[nodiscard]] constexpr bool move_is_pawn_two_squares(const Move& move) noexcept {
    return move.flag == MoveFlag::PawnMoveTwoSquares;
}

/**
 * Check if a move is a null move (used in null-move pruning).
 *
 * @param move The move to check.
 * @return true if the move is a null move.
 */
[[nodiscard]] constexpr bool move_is_null(const Move& move) noexcept {
    return move.flag == MoveFlag::NullMove;
}

/**
 * Check if a move is valid (not a no-move marker).
 *
 * @param move The move to check.
 * @return true if the move is a valid move.
 */
[[nodiscard]] constexpr bool move_is_valid(const Move& move) noexcept {
    return move.flag != MoveFlag::NoMove;
}

/**
 * Check if the move gives check.
 *
 * @param move The move to check.
 * @return true if the move gives check (any type).
 */
[[nodiscard]] constexpr bool move_gives_check(const Move& move) noexcept {
    return is_check(move.check);
}

} // namespace chess

#endif // ADACHESS_MOVES_HPP

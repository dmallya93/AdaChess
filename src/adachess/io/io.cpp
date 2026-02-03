/**
 * AdaChess - Smart Chess Engine
 *
 * Move notation I/O functions implementation.
 * C++ port of the original Ada implementation (chess-io.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/io/io.hpp"

#include <cctype>
#include <sstream>

namespace chess::io {

namespace {

// ============================================================================
// Internal Helper Functions
// ============================================================================

/**
 * Convert a character to lowercase.
 */
[[nodiscard]] char to_lower(char c) noexcept {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
}

/**
 * Convert a character to uppercase.
 */
[[nodiscard]] char to_upper(char c) noexcept {
    return static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
}

/**
 * Get the piece symbol for move notation (uppercase for non-pawns).
 * Returns empty string for pawns.
 */
[[nodiscard]] std::string get_piece_letter(Piece piece) {
    if (is_pawn(piece)) {
        return "";
    }
    // For non-pawn pieces, use uppercase symbol
    char symbol = piece_symbol(piece);
    return std::string(1, to_upper(symbol));
}

/**
 * Get the lowercase promotion piece symbol.
 */
[[nodiscard]] char get_promotion_symbol(Piece promotion) {
    if (promotion == Piece::Empty) {
        return '\0';
    }
    return to_lower(piece_symbol(promotion));
}

/**
 * Get the square string (e.g., "e4").
 */
[[nodiscard]] std::string get_square_string(board::Square sq) {
    return board::square_to_string(sq);
}

/**
 * Get file character (a-h) from square.
 */
[[nodiscard]] char get_file_char(board::Square sq) {
    board::Coordinate f = board::file(sq);
    return static_cast<char>('a' + f);
}

/**
 * Get rank character (1-8) from square.
 */
[[nodiscard]] char get_rank_char(board::Square sq) {
    board::Coordinate r = board::rank(sq);
    return static_cast<char>('0' + r);
}

} // anonymous namespace

// ============================================================================
// Move To String Implementation
// ============================================================================

std::string move_to_string(const Move& move, NotationType notation) {
    AnnotatedMove annotated{move, AnnotationType::None};
    return move_to_string(annotated, notation);
}

std::string move_to_string(const AnnotatedMove& annotated_move, NotationType notation) {
    std::string result;

    switch (notation) {
        case NotationType::PureAlgebraic:
            result = move_to_pure_algebraic(annotated_move.move);
            break;
        case NotationType::StandardAlgebraic:
            result = move_to_standard_algebraic(annotated_move.move);
            break;
        case NotationType::LongAlgebraic:
            result = move_to_long_algebraic(annotated_move.move);
            break;
        case NotationType::ICCF:
            result = move_to_iccf(annotated_move.move);
            break;
    }

    // Add annotation (only for SAN and LAN, not for coordinate/ICCF)
    if (notation == NotationType::StandardAlgebraic ||
        notation == NotationType::LongAlgebraic) {
        result += annotation_to_string(annotated_move.annotation);
    }

    return result;
}

// ============================================================================
// Pure Algebraic (Coordinate) Notation
// ============================================================================

std::string move_to_pure_algebraic(const Move& move) {
    std::string result;

    // Source square
    result += get_square_string(move.from);

    // Destination square
    result += get_square_string(move.to);

    // Promotion piece (lowercase)
    if (move.promotion != Piece::Empty) {
        result += get_promotion_symbol(move.promotion);
    }

    return result;
}

// ============================================================================
// Standard Algebraic Notation (SAN)
// ============================================================================

std::string move_to_standard_algebraic(const Move& move) {
    std::string result;

    // Handle castling specially
    if (move.flag == MoveFlag::Castle) {
        // Determine kingside vs queenside by destination file
        board::Coordinate dest_file = board::file(move.to);
        if (dest_file == board::kFileG) {
            result = "O-O";
        } else {
            result = "O-O-O";
        }
    } else {
        // Non-pawn piece letter
        if (!is_pawn(move.piece)) {
            result += get_piece_letter(move.piece);
        }

        // Ambiguity resolution
        switch (move.ambiguous_flag) {
            case AmbiguousFlag::Both:
                // Full source square for complete disambiguation
                result += get_square_string(move.from);
                break;
            case AmbiguousFlag::Rank:
                // Add rank only (e.g., "1" in R1a3)
                result += get_rank_char(move.from);
                break;
            case AmbiguousFlag::File:
                // Add file only (e.g., "a" in Rab1)
                result += get_file_char(move.from);
                break;
            case AmbiguousFlag::None:
                // No disambiguation needed
                break;
        }

        // Capture indicator
        if (move.captured != Piece::Empty || move.flag == MoveFlag::CaptureEnPassant) {
            // Pawn captures include file letter before 'x'
            if (is_pawn(move.piece)) {
                result += get_file_char(move.from);
            }
            result += 'x';
        }

        // Destination square
        result += get_square_string(move.to);

        // Promotion
        if (move.promotion != Piece::Empty) {
            result += '=';
            result += get_promotion_symbol(move.promotion);
        }
    }

    // Check/checkmate indicator
    result += check_to_string(move.check);

    return result;
}

// ============================================================================
// Long Algebraic Notation (LAN)
// ============================================================================

std::string move_to_long_algebraic(const Move& move) {
    std::string result;

    // Handle castling specially
    if (move.flag == MoveFlag::Castle) {
        board::Coordinate dest_file = board::file(move.to);
        if (dest_file == board::kFileG) {
            result = "O-O";
        } else {
            result = "O-O-O";
        }
    } else {
        // Non-pawn piece letter
        if (!is_pawn(move.piece)) {
            result += get_piece_letter(move.piece);
        }

        // Source square (always included in LAN)
        result += get_square_string(move.from);

        // Move indicator: '-' for quiet moves, 'x' for captures
        if (move.captured == Piece::Empty && move.flag != MoveFlag::CaptureEnPassant) {
            result += '-';
        } else {
            result += 'x';
        }

        // Destination square
        result += get_square_string(move.to);

        // Promotion
        if (move.promotion != Piece::Empty) {
            result += '=';
            result += get_promotion_symbol(move.promotion);
        }
    }

    // Check/checkmate indicator
    result += check_to_string(move.check);

    return result;
}

// ============================================================================
// ICCF Notation
// ============================================================================

std::string move_to_iccf(const Move& move) {
    std::string result;

    // File + rank of from square (1-based file)
    board::Coordinate from_file = board::file(move.from);
    board::Coordinate from_rank = board::rank(move.from);
    result += std::to_string(from_file + 1);
    result += std::to_string(from_rank);

    // File + rank of to square (1-based file)
    board::Coordinate to_file = board::file(move.to);
    board::Coordinate to_rank = board::rank(move.to);
    result += std::to_string(to_file + 1);
    result += std::to_string(to_rank);

    // Promotion code (1=Queen, 2=Rook, 3=Bishop, 4=Knight)
    if (move.promotion != Piece::Empty) {
        if (is_queen(move.promotion)) {
            result += '1';
        } else if (is_rook(move.promotion)) {
            result += '2';
        } else if (is_bishop(move.promotion)) {
            result += '3';
        } else if (is_knight(move.promotion)) {
            result += '4';
        }
    }

    return result;
}

// ============================================================================
// Move Printing Functions
// ============================================================================

void print_move(const Move& move, NotationType notation, std::ostream& out) {
    out << move_to_string(move, notation);
}

void print_move(const AnnotatedMove& annotated_move, NotationType notation, std::ostream& out) {
    out << move_to_string(annotated_move, notation);
}

// ============================================================================
// Annotation String Conversion
// ============================================================================

std::string annotation_to_string(AnnotationType annotation) {
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

// ============================================================================
// Check Symbol Conversion
// ============================================================================

std::string check_to_string(CheckType check) {
    switch (check) {
        case CheckType::NoCheck:
            return "";
        case CheckType::DirectCheck:
        case CheckType::DiscoveryCheck:
        case CheckType::UnknownCheck:
            return "+";
        case CheckType::DoubleCheck:
            return "++";
        case CheckType::Checkmate:
            return "#";
    }
    return "";
}

} // namespace chess::io

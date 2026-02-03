/**
 * AdaChess - Smart Chess Engine
 *
 * FEN (Forsyth-Edwards Notation) parser and serializer.
 * C++ port of the original Ada implementation (chess-io-fen.ads/adb).
 *
 * FEN is a standard notation for describing a particular board position of a
 * chess game. AdaChess can read and write chess positions in FEN format.
 *
 * Further info about FEN description can be found at:
 * https://www.chessprogramming.org/Forsyth-Edwards_Notation
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_IO_FEN_HPP
#define ADACHESS_IO_FEN_HPP

#include <stdexcept>
#include <string>
#include <string_view>

// Forward declaration to avoid circular includes
namespace chess::engine {
class Chessboard;
}

namespace chess::io::fen {

// ============================================================================
// Exceptions
// ============================================================================

/**
 * Exception thrown when a FEN string does not represent a valid chess position.
 * Corresponds to Ada's Invalid_Fen exception.
 */
class InvalidFen : public std::runtime_error {
public:
    explicit InvalidFen(const std::string& msg = "Invalid FEN string")
        : std::runtime_error(msg) {}

    explicit InvalidFen(const char* msg)
        : std::runtime_error(msg) {}
};

// ============================================================================
// FEN Constants
// ============================================================================

/**
 * The standard starting position FEN string.
 */
inline const std::string kStartingPositionFen =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// ============================================================================
// FEN Parsing and Serialization Functions
// ============================================================================

/**
 * Parse a FEN string and set up the chessboard accordingly.
 *
 * The input FEN should contain:
 * - Piece placement data
 * - Side to move ('w' or 'b')
 * - Castling rights ('KQkq' or '-')
 * - En passant target square (e.g., 'e3' or '-')
 * - (Optional) Halfmove clock (fifty-move rule counter)
 * - (Optional) Fullmove number
 *
 * Example FEN: "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
 *
 * @param chessboard Reference to the chessboard to set up.
 * @param fen The FEN string representing the position.
 * @throws InvalidFen if the FEN string is malformed or represents an illegal position.
 */
void from_string(engine::Chessboard& chessboard, std::string_view fen);

/**
 * Serialize a chessboard position to a FEN string.
 *
 * The output is a complete, valid FEN string containing all position information.
 *
 * @param chessboard The chessboard to serialize.
 * @return The FEN string representation of the current board state.
 */
[[nodiscard]] std::string to_string(const engine::Chessboard& chessboard);

/**
 * Alias for to_string for compatibility with Ada naming convention.
 *
 * @param chessboard The chessboard to serialize.
 * @return The FEN string representation of the current board state.
 */
[[nodiscard]] inline std::string fen_save_to_string(const engine::Chessboard& chessboard) {
    return to_string(chessboard);
}

} // namespace chess::io::fen

#endif // ADACHESS_IO_FEN_HPP

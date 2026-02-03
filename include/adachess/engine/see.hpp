/**
 * AdaChess - Smart Chess Engine
 *
 * Static Exchange Evaluation (SEE) for tactical capture analysis.
 * C++ port of the original Ada implementation (chess-engine-see.ads/adb).
 *
 * SEE evaluates capture sequences on a target square to determine
 * whether an exchange is winning, equal, or losing.
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_ENGINE_SEE_HPP
#define ADACHESS_ENGINE_SEE_HPP

#include <array>
#include <cstdint>

#include "adachess/score.hpp"
#include "adachess/pieces.hpp"
#include "adachess/moves.hpp"

// Forward declarations
namespace chess::engine {
    class Chessboard;
}

namespace chess::engine::see {

// ============================================================================
// SEE Score Type
// ============================================================================

/**
 * SEE score type.
 * Range: -32767 to +32767
 * A SEE cannot gain more material than the highest piece value.
 */
using SeeScore = Score;

inline constexpr SeeScore kSeeScoreMin = -32767;
inline constexpr SeeScore kSeeScoreMax = 32767;

// ============================================================================
// Capture Result Type
// ============================================================================

/**
 * Capture result enumeration indicating the outcome of an exchange.
 * Matches Ada's Capture_Result_Type.
 */
enum class CaptureResult : std::uint8_t {
    Winning = 0,  ///< Exchange wins material
    Equal = 1,    ///< Exchange is equal
    Losing = 2    ///< Exchange loses material
};

// ============================================================================
// Piece Values for SEE
// ============================================================================

/**
 * Piece value table for SEE calculations.
 * Indexed by Piece enum value.
 * These values are specifically for SEE and may differ from full evaluation values.
 */
inline constexpr std::array<Score, kNumPieceValues> kSeePieceValues = {{
    -kInfinity,  // Frame (should never be accessed, but -Infinity as hack)
    0,           // Empty
    100,         // WhitePawn
    300,         // WhiteKnight
    300,         // WhiteBishop
    500,         // WhiteRook
    900,         // WhiteQueen
    32767,       // WhiteKing
    100,         // BlackPawn
    300,         // BlackKnight
    300,         // BlackBishop
    500,         // BlackRook
    900,         // BlackQueen
    32767        // BlackKing
}};

/**
 * Get the SEE value of a piece.
 *
 * @param piece The piece to get the value of.
 * @return The SEE value of the piece.
 */
[[nodiscard]] constexpr Score piece_see_value(Piece piece) noexcept {
    return kSeePieceValues[static_cast<std::size_t>(piece)];
}

// Promotion and pawn score constants for SEE calculations
inline constexpr Score kPromotionScore = 900;  // Queen value
inline constexpr Score kPawnScore = 100;

// ============================================================================
// Static Exchange Evaluation Functions
// ============================================================================

/**
 * Evaluate a sequence of captures towards a target square.
 *
 * Calculates the material balance after a sequence of captures on the
 * destination square of the given move. A positive result indicates that
 * the exchange sequence gains material, while a negative value indicates
 * that material will be lost.
 *
 * The very first move that triggers the SEE evaluation could also be a
 * non-capturing move.
 *
 * @param chessboard The chess position where SEE will be evaluated.
 * @param move The move that starts the sequence, targeting the destination square.
 * @return The score difference in the static exchange.
 */
[[nodiscard]] SeeScore static_exchange_evaluation_score(
    Chessboard& chessboard,
    const Move& move);

/**
 * Evaluate a capture sequence and return the result type.
 *
 * Same as static_exchange_evaluation_score but returns only whether
 * the move leads to a winning, losing, or equal capture sequence.
 *
 * @param chessboard The chess position where SEE will be evaluated.
 * @param move The move that starts the sequence.
 * @return The result of the static exchange (Winning/Equal/Losing).
 */
[[nodiscard]] CaptureResult static_exchange_evaluation(
    Chessboard& chessboard,
    const Move& move);

/**
 * Evaluate a recapture exchange statically.
 *
 * Evaluates the exchange by considering a recapture following a previous move.
 *
 * @param chessboard The chess position where SEE will be evaluated.
 * @param recapture_move The recapture move to evaluate.
 * @param previous_move The previous move that preceded the recapture.
 * @return The score of the recapture exchange.
 */
[[nodiscard]] Score static_recapture_exchange_evaluation(
    Chessboard& chessboard,
    const Move& recapture_move,
    const Move& previous_move);

} // namespace chess::engine::see

#endif // ADACHESS_ENGINE_SEE_HPP

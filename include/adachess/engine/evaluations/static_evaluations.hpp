/**
 * AdaChess - Smart Chess Engine
 *
 * Static evaluation interface, piece-square tables, and evaluation constants.
 * C++ port of the original Ada implementation (chess-engine-evaluations-static_evaluations.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_ENGINE_EVALUATIONS_STATIC_EVALUATIONS_HPP
#define ADACHESS_ENGINE_EVALUATIONS_STATIC_EVALUATIONS_HPP

#include <array>
#include <cstdint>

#include "adachess/score.hpp"
#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/distances.hpp"
#include "adachess/board/board.hpp"
#include "adachess/engine/evaluations/evaluations.hpp"

// Forward declaration
namespace chess::engine {
class Chessboard;
}

namespace chess::engine::evaluations {

// ============================================================================
// Evaluation Function
// ============================================================================

/**
 * Initialize the evaluation engine.
 * Call this procedure once at startup to prepare the evaluation engine.
 * Pre-calculates knight distance table and other initialization.
 * Corresponds to Ada's Initialize_Evaluation_Engine procedure.
 */
void initialize_evaluation_engine();

/**
 * Statically evaluate the current chessboard.
 * The evaluation takes care of all the main factors for both sides:
 * - Material balance
 * - Pawn structure and passed pawns
 * - Piece mobility
 * - King safety
 * - Rook on (semi)open file
 * - Trapped pieces and specific endgames
 *
 * @param chessboard The current chess state.
 * @return The evaluation from the side-to-move perspective.
 * Corresponds to Ada's Evaluate function.
 */
[[nodiscard]] Evaluation evaluate(const Chessboard& chessboard);

// ============================================================================
// Material Values
// ============================================================================

inline constexpr TaperedScore kPawnScore{100, 105};
inline constexpr TaperedScore kKnightScore{325, 325};
inline constexpr TaperedScore kBishopScore{330, 330};
inline constexpr TaperedScore kRookScore{550, 550};
inline constexpr TaperedScore kQueenScore{990, 990};

// ============================================================================
// Endgame Pattern Recognition
// ============================================================================

/**
 * Endgame pattern types for special evaluation handling.
 * Corresponds to Ada's Endgame_Pattern_Type.
 */
enum class EndgamePattern : std::uint8_t {
    KK,           // King vs King

    // Symmetric situations
    KP_KP,        // King + Pawn(s) vs King + Pawn(s)
    KQ_KQ,        // King + Queen vs King + Queen
    KR_KR,        // King + Rook vs King + Rook
    KB_KB,        // King + Bishop vs King + Bishop
    KN_KN,        // King + Knight vs King + Knight

    // White perspective
    KQ_K,         // King + Queen vs King
    KR_K,         // King + Rook vs King
    KB_K,         // King + Bishop vs King
    KN_K,         // King + Knight vs King
    KP_K,         // King + Pawn vs King
    KQ_KP,        // King + Queen vs King + Pawn
    KR_KP,        // King + Rook vs King + Pawn
    KB_KP,        // King + Bishop vs King + Pawn
    KN_KP,        // King + Knight vs King + Pawn
    KBP_K,        // King + Bishop + Pawn vs King
    KNP_K,        // King + Knight + Pawn vs King
    KRP_KR,       // King + Rook + Pawn vs King + Rook
    KBP_KB,       // King + Bishop + Pawn vs King + Bishop

    // Black perspective
    K_KR,         // King vs King + Rook
    K_KQ,         // King vs King + Queen
    K_KB,         // King vs King + Bishop
    K_KN,         // King vs King + Knight
    K_KP,         // King vs King + Pawn
    KP_KQ,        // King + Pawn vs King + Queen
    KP_KR,        // King + Pawn vs King + Rook
    KP_KB,        // King + Pawn vs King + Bishop
    KP_KN,        // King + Pawn vs King + Knight
    K_KBP,        // King vs King + Bishop + Pawn
    K_KNP,        // King vs King + Knight + Pawn
    KR_KRP,       // King + Rook vs King + Rook + Pawn
    KB_KBP,       // King + Bishop vs King + Bishop + Pawn

    NoPattern     // No pattern of particular interest found
};

// ============================================================================
// Pawn Structure Evaluation Constants
// ============================================================================

inline constexpr TaperedScore kDoubledPawnPenalty{-10, -10};
inline constexpr TaperedScore kIsolatedPawnPenalty{-8, -12};
inline constexpr TaperedScore kIsolaniPawnPenalty{-10, -12};  // Isolated queen pawn (on D file)
inline constexpr TaperedScore kWeakPawnPenalty{-6, -10};
inline constexpr TaperedScore kBackwardPawnPenalty{-4, -2};
inline constexpr TaperedScore kOnSemiOpenFilePenalty{0, -2};
inline constexpr TaperedScore kExposedPawnPenalty{-4, -7};
inline constexpr TaperedScore kConnectedPawnBonus{0, 10};
inline constexpr TaperedScore kPawnIslandPenaltyFactor{-5, -10};
inline constexpr TaperedScore kHangingPawnPenalty{-5, -10};

// Passed pawn bonuses indexed by [phase][rank]
// White ranks: Rank_2 = index 0, Rank_7 = index 5
inline constexpr std::array<std::array<Score, 6>, kNumGamePhases> kWhiteCandidatePassedPawnBonus = {{
    // Opening
    {{0, 0, 0, 0, 0, 0}},  // Rank 2-7
    // Endgame
    {{0, 5, 10, 15, 30, 0}}  // Rank 2-7
}};

inline constexpr std::array<std::array<Score, 6>, kNumGamePhases> kBlackCandidatePassedPawnBonus = {{
    // Opening
    {{0, 0, 0, 0, 0, 0}},  // Rank 7-2
    // Endgame
    {{0, 30, 15, 10, 5, 0}}  // Rank 7-2
}};

inline constexpr std::array<std::array<Score, 6>, kNumGamePhases> kWhitePassedPawnBonus = {{
    // Opening: Rank 2-7
    {{10, 20, 30, 40, 50, 100}},
    // Endgame: Rank 2-7
    {{20, 40, 60, 80, 100, 150}}
}};

inline constexpr std::array<std::array<Score, 6>, kNumGamePhases> kBlackPassedPawnBonus = {{
    // Opening: Rank 7-2 (reversed)
    {{100, 50, 40, 30, 20, 10}},
    // Endgame: Rank 7-2 (reversed)
    {{150, 100, 80, 60, 40, 20}}
}};

inline constexpr Score kUnstoppablePasser = 500;
inline constexpr Score kKingProximityFactor = 5;

// ============================================================================
// Piece-Square Tables
// ============================================================================

// White Pawn Position Table
inline constexpr PieceSquareTable kWhitePawnPositionTable = {{
    //  F:   A    B    C    D    E    F    G    H
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // Rank 8
    0,  10,  10,  10,  10,  10,  10,  10,  10,   0, // Rank 7
    0,   5,   5,   5,  10,  10,   5,   5,   5,   0, // Rank 6
    0,   3,   4,   4,   5,   5,   4,   4,   3,   0, // Rank 5
    0,   0,   0,   0,   2,   2,   0,   0,   0,   0, // Rank 4
    0,   2,  -2,  -2,   0,   0,  -2,  -2,   2,   0, // Rank 3
    0,   2,   5,   5,  -5,  -5,   5,   5,   2,   0, // Rank 2
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// Black Pawn Position Table
inline constexpr PieceSquareTable kBlackPawnPositionTable = {{
    //  F:   A    B    C    D    E    F    G    H
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // Rank 8
    0,   2,   5,   5,  -5,  -5,   5,   5,   2,   0, // Rank 7
    0,   2,  -2,  -2,   0,   0,  -2,  -2,   2,   0, // Rank 6
    0,   0,   0,   0,   2,   2,   0,   0,   0,   0, // Rank 5
    0,   3,   4,   4,   5,   5,   4,   4,   3,   0, // Rank 4
    0,   5,   5,   5,  10,  10,   5,   5,   5,   0, // Rank 3
    0,  10,  10,  10,  10,  10,  10,  10,  10,   0, // Rank 2
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// Knight Position Table (same for both colors)
inline constexpr PieceSquareTable kKnightPositionTable = {{
    //  F:   A    B    C    D    E    F    G    H
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0, -30, -20, -10, -10, -10, -10, -20, -30,   0, // Rank 8
    0, -20, -10,   0,   5,   5,   0, -10, -20,   0, // Rank 7
    0, -20,  -5,   5,  10,  10,   5,  -5, -20,   0, // Rank 6
    0, -20,   0,  10,  15,  15,  10,   0, -20,   0, // Rank 5
    0, -20,   0,  10,  15,  15,  10,   0, -20,   0, // Rank 4
    0, -20,  -5,   5,  10,  10,   5,  -5, -20,   0, // Rank 3
    0, -20, -10,   0,   5,   5,   0, -10, -20,   0, // Rank 2
    0, -30, -20, -10, -10, -10, -10, -20, -30,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// White Bishop Position Table
inline constexpr PieceSquareTable kWhiteBishopPositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0, -20, -10, -10, -10, -10, -10, -10, -20,   0, // Rank 8
    0, -10,   0,   0,   0,   0,   0,   0, -10,   0, // Rank 7
    0, -10,   0,   5,  10,  10,   5,   0, -10,   0, // Rank 6
    0, -10,   5,   5,  10,  10,   5,   5, -10,   0, // Rank 5
    0, -10,   0,  10,  10,  10,  10,   0, -10,   0, // Rank 4
    0, -10,  10,  10,  10,  10,  10,  10, -10,   0, // Rank 3
    0, -10,   5,   0,   0,   0,   0,   5, -10,   0, // Rank 2
    0, -20, -10, -10, -10, -10, -10, -10, -20,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// Black Bishop Position Table
inline constexpr PieceSquareTable kBlackBishopPositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0, -20, -10, -10, -10, -10, -10, -10, -20,   0, // Rank 8
    0, -10,   5,   0,   0,   0,   0,   5, -10,   0, // Rank 7
    0, -10,  10,  10,  10,  10,  10,  10, -10,   0, // Rank 6
    0, -10,   0,  10,  10,  10,  10,   0, -10,   0, // Rank 5
    0, -10,   5,   5,  10,  10,   5,   5, -10,   0, // Rank 4
    0, -10,   0,   5,  10,  10,   5,   0, -10,   0, // Rank 3
    0, -10,   0,   0,   0,   0,   0,   0, -10,   0, // Rank 2
    0, -20, -10, -10, -10, -10, -10, -10, -20,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// White Rook Position Table
inline constexpr PieceSquareTable kWhiteRookPositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // Rank 8
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // Rank 7
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 6
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 5
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 4
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 3
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 2
    0,   0,   0,   0,   5,   5,   0,   0,   0,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// Black Rook Position Table
inline constexpr PieceSquareTable kBlackRookPositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   5,   5,   0,   0,   0,   0, // Rank 8
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 7
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 6
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 5
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 4
    0,  -5,   0,   0,   0,   0,   0,   0,  -5,   0, // Rank 3
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // Rank 2
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// White Queen Position Table
inline constexpr PieceSquareTable kWhiteQueenPositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0, -20, -10, -10,  -5,  -5, -10, -10, -20,   0, // Rank 8
    0, -10,   0,   0,   0,   0,   0,   0, -10,   0, // Rank 7
    0, -10,   0,   5,   5,   5,   5,   0, -10,   0, // Rank 6
    0,  -5,   0,   5,   5,   5,   5,   0,  -5,   0, // Rank 5
    0,   0,   0,   5,   5,   5,   5,   0,  -5,   0, // Rank 4
    0, -10,   5,   5,   5,   5,   5,   0, -10,   0, // Rank 3
    0, -10,   0,   5,   0,   0,   0,   0, -10,   0, // Rank 2
    0, -20, -10, -10,  -5,  -5, -10, -10, -20,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// Black Queen Position Table
inline constexpr PieceSquareTable kBlackQueenPositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0, -20, -10, -10,  -5,  -5, -10, -10, -20,   0, // Rank 8
    0, -10,   0,   5,   0,   0,   0,   0, -10,   0, // Rank 7
    0, -10,   5,   5,   5,   5,   5,   0, -10,   0, // Rank 6
    0,   0,   0,   5,   5,   5,   5,   0,  -5,   0, // Rank 5
    0,  -5,   0,   5,   5,   5,   5,   0,  -5,   0, // Rank 4
    0, -10,   0,   5,   5,   5,   5,   0, -10,   0, // Rank 3
    0, -10,   0,   0,   0,   0,   0,   0, -10,   0, // Rank 2
    0, -20, -10, -10,  -5,  -5, -10, -10, -20,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// White King Position Table (Opening/Middlegame)
inline constexpr PieceSquareTable kWhiteKingPositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0, -30, -40, -40, -50, -50, -40, -40, -30,   0, // Rank 8
    0, -30, -40, -40, -50, -50, -40, -40, -30,   0, // Rank 7
    0, -30, -40, -40, -50, -50, -40, -40, -30,   0, // Rank 6
    0, -30, -40, -40, -50, -50, -40, -40, -30,   0, // Rank 5
    0, -20, -30, -30, -40, -40, -30, -30, -20,   0, // Rank 4
    0, -10, -20, -20, -20, -20, -20, -20, -10,   0, // Rank 3
    0,  20,  20,   0,   0,   0,   0,  20,  20,   0, // Rank 2
    0,  20,  30,  -5, -10,   0,  -5,  30,  20,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// Black King Position Table (Opening/Middlegame)
inline constexpr PieceSquareTable kBlackKingPositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,  20,  30,  -5, -10,   0,  -5,  30,  20,   0, // Rank 8
    0,  20,  20,   0,   0,   0,   0,  20,  20,   0, // Rank 7
    0, -10, -20, -20, -20, -20, -20, -20, -10,   0, // Rank 6
    0, -20, -30, -30, -40, -40, -30, -30, -20,   0, // Rank 5
    0, -30, -40, -40, -50, -50, -40, -40, -30,   0, // Rank 4
    0, -30, -40, -40, -50, -50, -40, -40, -30,   0, // Rank 3
    0, -30, -40, -40, -50, -50, -40, -40, -30,   0, // Rank 2
    0, -30, -40, -40, -50, -50, -40, -40, -30,   0, // Rank 1
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// King Endgame Position Table (same for both colors)
inline constexpr PieceSquareTable kKingEndGamePositionTable = {{
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0, -50, -30, -20, -20, -20, -20, -30, -50,   0,
    0, -30,   0,  10,  10,  10,  10,   0, -30,   0,
    0, -20,  10,  25,  25,  25,  25,  10, -20,   0,
    0, -20,  10,  25,  50,  50,  25,  10, -20,   0,
    0, -20,  10,  25,  50,  50,  25,  10, -20,   0,
    0, -20,  10,  25,  25,  25,  25,  10, -20,   0,
    0, -30,   0,  10,  10,  10,  10,   0, -30,   0,
    0, -50, -30, -20, -20, -20, -20, -30, -50,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0
}};

// ============================================================================
// Mobility Evaluation Constants
// ============================================================================

inline constexpr Score kKnightMobilityPenalty = 4;
inline constexpr TaperedScore kKnightMobilityBonus{4, 4};
inline constexpr Score kBishopMobilityPenalty = 6;
inline constexpr TaperedScore kBishopMobilityBonus{5, 5};
inline constexpr Score kRookMobilityPenalty = 8;
inline constexpr TaperedScore kRookMobilityBonus{2, 4};
inline constexpr Score kQueenMobilityPenalty = 12;
inline constexpr TaperedScore kQueenMobilityBonus{1, 2};

inline constexpr Score kMobilityFactor = 1;
inline constexpr Score kAttackFactor = 1;
inline constexpr Score kDefenceFactor = 0;

// Mobility unit arrays indexed by piece type
inline constexpr std::array<Score, kNumPieceValues> kWhiteMobilityUnit = {{
    0,                  // Frame
    kMobilityFactor,    // Empty
    kDefenceFactor,     // WhitePawn
    kDefenceFactor,     // WhiteKnight
    kDefenceFactor,     // WhiteBishop
    kDefenceFactor,     // WhiteRook
    kDefenceFactor,     // WhiteQueen
    kDefenceFactor,     // WhiteKing
    kAttackFactor,      // BlackPawn
    kAttackFactor,      // BlackKnight
    kAttackFactor,      // BlackBishop
    kAttackFactor,      // BlackRook
    kAttackFactor,      // BlackQueen
    kAttackFactor       // BlackKing
}};

inline constexpr std::array<Score, kNumPieceValues> kBlackMobilityUnit = {{
    0,                  // Frame
    kMobilityFactor,    // Empty
    kAttackFactor,      // WhitePawn
    kAttackFactor,      // WhiteKnight
    kAttackFactor,      // WhiteBishop
    kAttackFactor,      // WhiteRook
    kAttackFactor,      // WhiteQueen
    kAttackFactor,      // WhiteKing
    kDefenceFactor,     // BlackPawn
    kDefenceFactor,     // BlackKnight
    kDefenceFactor,     // BlackBishop
    kDefenceFactor,     // BlackRook
    kDefenceFactor,     // BlackQueen
    kDefenceFactor      // BlackKing
}};

// ============================================================================
// Rook and Bishop Specific Scores
// ============================================================================

inline constexpr Score kRookOnSemiOpenWithBackwardPawn = 5;
inline constexpr Score kRookOnSemiOpenFile = 10;
inline constexpr Score kRookOnOpenFile = 20;
inline constexpr Score kRookOnKingFile = 5;
inline constexpr Score kRookOnSemiKingFile = 2;
inline constexpr TaperedScore kRookOn7thRank{0, 20};
inline constexpr TaperedScore kRookConnected{5, 22};
inline constexpr Score kRookOpposition = 10;
inline constexpr Score kTarrashRule = 15;

inline constexpr TaperedScore kQueenOn7thRank{0, 15};

inline constexpr Score kBlockedRook = 90;

inline constexpr TaperedScore kBishopPairBonus{20, 40};
inline constexpr TaperedScore kBishopPairFreeCenter{8, 4};
inline constexpr Score kFianchetto = 6;
inline constexpr Score kTrappedBishop = 75;
inline constexpr Score kBlockedBishop = 40;
inline constexpr Score kExposingQueen = 20;

// ============================================================================
// King Safety Constants
// ============================================================================

inline constexpr Score kPawnShelter = 5;
inline constexpr Score kPawnShelterPenalty = -20;  // Negative because subtracted
inline constexpr Score kPawnShelterDefuse = -35;   // Negative because subtracted

inline constexpr Score kOpenFileNearKing = 40;
inline constexpr Score kOpenFileInFrontOfKing = 20;
inline constexpr Score kSemiOpenFileNearKing = 15;
inline constexpr Score kSemiOpenFileInFrontOfKing = 30;

inline constexpr Score kPawnStormSevereThreat = 50;
inline constexpr Score kPawnStormThreat = 35;
inline constexpr Score kPawnStormAlert = 25;

inline constexpr Score kProfilacticMovePenalty = -6;

// King attack values indexed by piece type
inline constexpr std::array<Score, kNumPieceValues> kKingAttackValue = {{
    0,      // Frame
    0,      // Empty
    0,      // WhitePawn
    10,     // WhiteKnight
    10,     // WhiteBishop
    20,     // WhiteRook
    40,     // WhiteQueen
    0,      // WhiteKing
    0,      // BlackPawn
    10,     // BlackKnight
    10,     // BlackBishop
    20,     // BlackRook
    40,     // BlackQueen
    0       // BlackKing
}};

// Attack weight indexed by number of attackers (0-15)
inline constexpr std::array<Score, 16> kAttackWeight = {{
    0, 0, 50, 75, 90, 95, 98, 99, 99, 99, 99, 99, 99, 99, 99, 99
}};

inline constexpr Score kAttackingKingFactor = 100;

// ============================================================================
// Weak Square and Outpost Scores
// ============================================================================

inline constexpr std::array<TaperedScore, kNumPieceValues> kWeakSquareOccupancy = {{
    {0, 0},       // Frame
    {0, 0},       // Empty
    {0, 0},       // WhitePawn
    {5, 5},       // WhiteKnight
    {5, 5},       // WhiteBishop
    {0, 5},       // WhiteRook
    {0, 5},       // WhiteQueen
    {0, 0},       // WhiteKing
    {0, 0},       // BlackPawn
    {5, 5},       // BlackKnight
    {5, 5},       // BlackBishop
    {0, 5},       // BlackRook
    {0, 5},       // BlackQueen
    {0, 0}        // BlackKing
}};

inline constexpr std::array<TaperedScore, kNumPieceValues> kOutpostOccupancyPenalty = {{
    {0, 0},       // Frame
    {0, 0},       // Empty
    {0, 0},       // WhitePawn
    {-15, -15},   // WhiteKnight
    {-10, -10},   // WhiteBishop
    {-5, -10},    // WhiteRook
    {0, -5},      // WhiteQueen
    {0, 0},       // WhiteKing
    {0, 0},       // BlackPawn
    {-15, -15},   // BlackKnight
    {-10, -10},   // BlackBishop
    {-5, -10},    // BlackRook
    {0, -5},      // BlackQueen
    {0, 0}        // BlackKing
}};

// ============================================================================
// Tempo Bonus
// ============================================================================

inline constexpr Score kTempoBonus = 15;

// ============================================================================
// Pawn Promotion Square Tables
// ============================================================================

inline constexpr std::array<std::uint8_t, board::kBoardSize> kWhitePromotionSquare = {{
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0, 21, 22, 23, 24, 25, 26, 27, 28, 0,  // A8-H8
    0, 21, 22, 23, 24, 25, 26, 27, 28, 0,
    0, 21, 22, 23, 24, 25, 26, 27, 28, 0,
    0, 21, 22, 23, 24, 25, 26, 27, 28, 0,
    0, 21, 22, 23, 24, 25, 26, 27, 28, 0,
    0, 21, 22, 23, 24, 25, 26, 27, 28, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0
}};

inline constexpr std::array<std::uint8_t, board::kBoardSize> kBlackPromotionSquare = {{
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0, 91, 92, 93, 94, 95, 96, 97, 98, 0,  // A1-H1
    0, 91, 92, 93, 94, 95, 96, 97, 98, 0,
    0, 91, 92, 93, 94, 95, 96, 97, 98, 0,
    0, 91, 92, 93, 94, 95, 96, 97, 98, 0,
    0, 91, 92, 93, 94, 95, 96, 97, 98, 0,
    0, 91, 92, 93, 94, 95, 96, 97, 98, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0,
    0,  0,  0,  0,  0,  0,  0,  0,  0, 0
}};

// ============================================================================
// Distance Functions
// ============================================================================

/**
 * Distance match result for piece races.
 * Corresponds to Ada's Distance_Match_Type.
 */
enum class DistanceMatch : std::uint8_t {
    Equals,        // Both pieces have same distance to target
    Closer,        // Piece gets faster than opponent
    Farther,       // Piece gets slower than opponent
    Winner,        // Opponent piece cannot reach target square, piece can
    Loser,         // Piece cannot reach target square, opponent can
    NotReachable   // Neither piece nor opponent can reach target
};

/**
 * Knight distance lookup table.
 * Pre-computed distances between all square pairs for knight moves.
 */
extern std::array<std::array<Distance, board::kBoardSize>, board::kBoardSize> g_knight_distance_table;

/**
 * Fill the knight distance table.
 * Calculate the amount of moves that a Knight needs from one square to reach
 * another square, assuming an empty chessboard.
 */
void fill_knight_distance_table();

/**
 * Calculate raw distance (without tempo consideration).
 *
 * @param piece The piece type.
 * @param square Starting square.
 * @param target Target square.
 * @return The number of moves needed, or kUnreachable.
 */
[[nodiscard]] Distance raw_distance(Piece piece, board::Square square, board::Square target);

/**
 * Calculate distance considering tempo (side to move).
 *
 * @param chessboard The current chess position.
 * @param piece The piece type.
 * @param square Starting square.
 * @param target Target square.
 * @return The number of moves needed, or kUnreachable.
 */
[[nodiscard]] Distance distance(const Chessboard& chessboard, Piece piece,
                                  board::Square square, board::Square target);

/**
 * Calculate the race between two pieces to a target square.
 *
 * @param chessboard The current chess position.
 * @param piece The first piece.
 * @param square The first piece's square.
 * @param opponent The second piece.
 * @param opponent_square The second piece's square.
 * @param target The destination square.
 * @return The race result from the first piece's perspective.
 */
[[nodiscard]] DistanceMatch race(const Chessboard& chessboard,
                                  Piece piece, board::Square square,
                                  Piece opponent, board::Square opponent_square,
                                  board::Square target);

} // namespace chess::engine::evaluations

#endif // ADACHESS_ENGINE_EVALUATIONS_STATIC_EVALUATIONS_HPP

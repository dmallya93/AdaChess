/**
 * AdaChess - Smart Chess Engine
 *
 * Evaluation types and structures for position scoring.
 * C++ port of the original Ada implementation (chess-engine-evaluations.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_ENGINE_EVALUATIONS_HPP
#define ADACHESS_ENGINE_EVALUATIONS_HPP

#include <cstdint>
#include <string>

#include "adachess/score.hpp"
#include "adachess/board/board.hpp"

namespace chess::engine::evaluations {

// ============================================================================
// Match Status Type
// ============================================================================

/**
 * Status of the match from the evaluation perspective.
 * Corresponds to Ada's Match_Status_Type.
 */
enum class MatchStatus : std::uint8_t {
    InProgress = 0,              // Game playing
    Checkmate = 1,
    DrawByStalemate = 2,
    DrawByFiftyMovesRule = 3,
    DrawByInsufficientMaterial = 4,
    DrawByThreefoldRepetitions = 5,
    DrawByPerpetualCheck = 6
};

// ============================================================================
// Evaluation Type
// ============================================================================

/**
 * Evaluation result structure containing score and game status.
 * Corresponds to Ada's Evaluation_Type.
 */
struct Evaluation {
    Score score{0};
    MatchStatus game_phase{MatchStatus::InProgress};

    /**
     * Default constructor.
     */
    constexpr Evaluation() noexcept = default;

    /**
     * Constructor with score and game phase.
     */
    constexpr Evaluation(Score s, MatchStatus phase) noexcept
        : score(s), game_phase(phase) {}

    /**
     * Negate the evaluation (swap perspective).
     * Corresponds to Ada's "-" operator for Evaluation_Type.
     */
    [[nodiscard]] constexpr Evaluation operator-() const noexcept {
        return Evaluation{-score, game_phase};
    }
};

// ============================================================================
// Game Phase Type
// ============================================================================

/**
 * Game phase for tapered evaluation.
 * Corresponds to Ada's Game_Phase_Type.
 */
enum class GamePhase : std::uint8_t {
    Opening = 0,
    EndGame = 1
};

/**
 * Number of game phases (for array sizing).
 */
inline constexpr std::size_t kNumGamePhases = 2;

// ============================================================================
// Phased Score Type
// ============================================================================

/**
 * Phased score type for interpolating between opening and endgame.
 * Range: 0 to 100 where 0 = endgame, 100 = opening.
 * Corresponds to Ada's Phased_Score_Type.
 */
using PhasedScore = Score;

inline constexpr PhasedScore kMinPhasedScore = 0;
inline constexpr PhasedScore kMaxPhasedScore = 100;

// ============================================================================
// Piece Square Table Type
// ============================================================================

/**
 * Piece-square table type - scores for each square on the board.
 * Corresponds to Ada's Piece_Square_Table_Type.
 */
using PieceSquareTable = std::array<Score, board::kBoardSize>;

// ============================================================================
// Tapered Score Type
// ============================================================================

/**
 * Tapered score structure containing opening and endgame scores.
 * The tapered evaluation interpolates between these based on game phase.
 * Corresponds to Ada's Tapered_Score_Type.
 */
struct TaperedScore {
    Score opening{0};
    Score endgame{0};

    /**
     * Default constructor - zero score.
     */
    constexpr TaperedScore() noexcept = default;

    /**
     * Constructor with explicit values.
     */
    constexpr TaperedScore(Score op, Score eg) noexcept
        : opening(op), endgame(eg) {}

    /**
     * Access by game phase.
     */
    [[nodiscard]] constexpr Score operator[](GamePhase phase) const noexcept {
        return (phase == GamePhase::Opening) ? opening : endgame;
    }

    /**
     * Access by game phase (mutable).
     */
    [[nodiscard]] constexpr Score& operator[](GamePhase phase) noexcept {
        return (phase == GamePhase::Opening) ? opening : endgame;
    }

    /**
     * Addition of two tapered scores.
     */
    [[nodiscard]] constexpr TaperedScore operator+(const TaperedScore& other) const noexcept {
        return TaperedScore{opening + other.opening, endgame + other.endgame};
    }

    /**
     * Subtraction of two tapered scores.
     */
    [[nodiscard]] constexpr TaperedScore operator-(const TaperedScore& other) const noexcept {
        return TaperedScore{opening - other.opening, endgame - other.endgame};
    }

    /**
     * Add a flat bonus to both phases.
     */
    [[nodiscard]] constexpr TaperedScore operator+(Score bonus) const noexcept {
        return TaperedScore{opening + bonus, endgame + bonus};
    }

    /**
     * Subtract a flat penalty from both phases.
     */
    [[nodiscard]] constexpr TaperedScore operator-(Score penalty) const noexcept {
        return TaperedScore{opening - penalty, endgame - penalty};
    }

    /**
     * Multiply by an integer.
     */
    [[nodiscard]] constexpr TaperedScore operator*(int amount) const noexcept {
        return TaperedScore{opening * amount, endgame * amount};
    }

    /**
     * Compound addition.
     */
    constexpr TaperedScore& operator+=(const TaperedScore& other) noexcept {
        opening += other.opening;
        endgame += other.endgame;
        return *this;
    }

    /**
     * Compound subtraction.
     */
    constexpr TaperedScore& operator-=(const TaperedScore& other) noexcept {
        opening -= other.opening;
        endgame -= other.endgame;
        return *this;
    }

    /**
     * Compound addition of flat bonus.
     */
    constexpr TaperedScore& operator+=(Score bonus) noexcept {
        opening += bonus;
        endgame += bonus;
        return *this;
    }

    /**
     * Compound subtraction of flat penalty.
     */
    constexpr TaperedScore& operator-=(Score penalty) noexcept {
        opening -= penalty;
        endgame -= penalty;
        return *this;
    }
};

/**
 * Multiply integer by tapered score (commutative).
 */
[[nodiscard]] inline constexpr TaperedScore operator*(int amount, const TaperedScore& score) noexcept {
    return score * amount;
}

/**
 * Zero tapered score constant.
 * Corresponds to Ada's Zero constant.
 */
inline constexpr TaperedScore kZeroTaperedScore{0, 0};

/**
 * Convert a tapered score to a string for debugging.
 * Corresponds to Ada's Image function for Tapered_Score_Type.
 */
[[nodiscard]] inline std::string to_string(const TaperedScore& s) {
    return "(Opening => " + std::to_string(s.opening) +
           ", End_Game => " + std::to_string(s.endgame) + ")";
}

// ============================================================================
// Resign Settings
// ============================================================================

/**
 * Global resign mode flag.
 * When activated, the engine will resign when evaluation is systematically
 * negative and there's no hope to improve it.
 * Corresponds to Ada's Resign_Mode variable.
 */
extern bool g_resign_mode;

/**
 * Threshold for consecutive moves with losing score before resigning.
 * Corresponds to Ada's Resign_Threshold constant.
 */
inline constexpr int kResignThreshold = 6;

/**
 * Score threshold below which resignation is considered.
 * Corresponds to Ada's Resign_Threshold_Score constant.
 */
inline constexpr Score kResignThresholdScore = -800;

} // namespace chess::engine::evaluations

#endif // ADACHESS_ENGINE_EVALUATIONS_HPP

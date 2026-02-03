/**
 * AdaChess - Smart Chess Engine
 *
 * Score type definitions for position evaluation.
 * C++ port of the original Ada implementation (chess-score.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_SCORE_HPP
#define ADACHESS_SCORE_HPP

#include <cstdint>
#include <random>

namespace chess {

/**
 * Score type for position evaluation.
 * Range: -1,000,000 to 1,000,000 (matches Ada's Score_Type).
 *
 * Using int32_t to ensure the range is always available.
 */
using Score = std::int32_t;

/**
 * Minimum valid score value.
 */
inline constexpr Score kScoreMin = -1'000'000;

/**
 * Maximum valid score value.
 */
inline constexpr Score kScoreMax = 1'000'000;

/**
 * Infinity score value (used for alpha-beta bounds).
 */
inline constexpr Score kInfinity = 900'900;

/**
 * Mate score value.
 * Actual mate scores are adjusted by ply distance.
 */
inline constexpr Score kMate = 32'767;

/**
 * Draw score value.
 */
inline constexpr Score kDraw = 0;

/**
 * Threshold for detecting winning mate (within 128 plies of mate).
 */
inline constexpr Score kMateThreshold = 128;

/**
 * Check if a score represents a winning mate.
 * A score is considered a winning mate if it's >= Mate - 128.
 *
 * @param score The score to check.
 * @return True if the score represents a winning mate.
 */
[[nodiscard]] constexpr bool is_winning_mate(Score score) noexcept {
    return score >= kMate - kMateThreshold;
}

/**
 * Check if a score represents a losing mate.
 * A score is considered a losing mate if it's <= -Mate + 128.
 *
 * @param score The score to check.
 * @return True if the score represents a losing mate.
 */
[[nodiscard]] constexpr bool is_losing_mate(Score score) noexcept {
    return score <= -kMate + kMateThreshold;
}

/**
 * Check if a score represents any mate (winning or losing).
 *
 * @param score The score to check.
 * @return True if the score represents a mate.
 */
[[nodiscard]] constexpr bool is_mate(Score score) noexcept {
    return is_winning_mate(score) || is_losing_mate(score);
}

// ============================================================================
// Random Mode for Score Variation
// ============================================================================

/**
 * Random score type for small variations.
 * Range: -4 to 4 (matches Ada's Random_Score_Type).
 */
using RandomScore = std::int8_t;

/**
 * Minimum random score adjustment.
 */
inline constexpr RandomScore kRandomScoreMin = -4;

/**
 * Maximum random score adjustment.
 */
inline constexpr RandomScore kRandomScoreMax = 4;

/**
 * Score random generator class.
 * Provides random score adjustments for varied play.
 * Corresponds to Ada's Score_Random package instantiation.
 */
class ScoreRandomGenerator {
public:
    /**
     * Default constructor - seeds with a default value.
     */
    ScoreRandomGenerator();

    /**
     * Constructor with explicit seed.
     *
     * @param seed The seed value for the random generator.
     */
    explicit ScoreRandomGenerator(std::uint64_t seed);

    /**
     * Reset the generator with a new seed.
     *
     * @param seed The new seed value.
     */
    void reset(std::uint64_t seed);

    /**
     * Reset the generator with a random device-based seed.
     */
    void reset();

    /**
     * Generate a random score adjustment.
     *
     * @return A random score in the range [-4, 4].
     */
    [[nodiscard]] RandomScore random();

private:
    std::mt19937_64 engine_;
    std::uniform_int_distribution<int> distribution_;
};

/**
 * Global score random generator.
 * Corresponds to Ada's Score_Seed variable.
 */
extern ScoreRandomGenerator g_score_seed;

/**
 * Global random mode flag.
 * When true, small random values are added to evaluations.
 * Corresponds to Ada's Random_Mode variable.
 */
extern bool g_random_mode;

} // namespace chess

#endif // ADACHESS_SCORE_HPP

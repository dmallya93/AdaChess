/**
 * AdaChess - Smart Chess Engine
 *
 * Score type implementation.
 * C++ port of the original Ada implementation.
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/score.hpp"

namespace chess {

// ============================================================================
// ScoreRandomGenerator Implementation
// ============================================================================

ScoreRandomGenerator::ScoreRandomGenerator()
    : engine_(std::random_device{}())
    , distribution_(kRandomScoreMin, kRandomScoreMax) {
}

ScoreRandomGenerator::ScoreRandomGenerator(std::uint64_t seed)
    : engine_(seed)
    , distribution_(kRandomScoreMin, kRandomScoreMax) {
}

void ScoreRandomGenerator::reset(std::uint64_t seed) {
    engine_.seed(seed);
}

void ScoreRandomGenerator::reset() {
    engine_.seed(std::random_device{}());
}

RandomScore ScoreRandomGenerator::random() {
    return static_cast<RandomScore>(distribution_(engine_));
}

// ============================================================================
// Global Variables
// ============================================================================

ScoreRandomGenerator g_score_seed{};
bool g_random_mode = false;

} // namespace chess

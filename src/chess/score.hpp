// ---------------------------------------------------------------------------
// AdaChess – Score type definitions
// Translated from: chess-score.ads
// ---------------------------------------------------------------------------
#pragma once

#include <random>

namespace chess {

// -----------------------------------------------------------------------
// Score_Type: evaluation score in centipawns.
// Ada: subtype Score_Type is Integer range -1_000_000 .. 1_000_000;
// -----------------------------------------------------------------------
using Score = int;

inline constexpr Score Min_Score =  -1'000'000;
inline constexpr Score Max_Score =   1'000'000;

/// Ada: Infinity := 900_900
inline constexpr Score Infinity  = 900'900;

// -----------------------------------------------------------------------
// Game scores
// -----------------------------------------------------------------------

/// Ada: Mate := 32767
inline constexpr Score Mate = 32'767;

/// Ada: Draw := 0
inline constexpr Score Draw = 0;

/// Ada: Is_Winning_Mate (Score >= Mate - 128)
[[nodiscard]] constexpr bool is_winning_mate(Score score) noexcept {
    return score >= Mate - 128;
}

/// Ada: Is_Losing_Mate (Score <= -Mate + 128)
[[nodiscard]] constexpr bool is_losing_mate(Score score) noexcept {
    return score <= -Mate + 128;
}

/// Ada: Is_Mate (Is_Winning_Mate or else Is_Losing_Mate)
[[nodiscard]] constexpr bool is_mate(Score score) noexcept {
    return is_winning_mate(score) || is_losing_mate(score);
}

// -----------------------------------------------------------------------
// Random Mode — adds a small random value to evaluation for play variety.
// Ada: subtype Random_Score_Type is Score_Type range -4 .. 4;
// Ada: package Score_Random is new Ada.Numerics.Discrete_Random (Random_Score_Type);
// -----------------------------------------------------------------------

inline constexpr Score Random_Score_Min = -4;
inline constexpr Score Random_Score_Max =  4;

/// Random score generator — wraps std::mt19937 with uniform int distribution.
/// Ada: Score_Seed : Score_Random.Generator;
inline std::mt19937 Score_Seed{}; // NOLINT(cert-msc51-cpp)

/// Generate a random score in the range [-4, 4].
[[nodiscard]] inline Score random_score() {
    std::uniform_int_distribution<Score> dist(Random_Score_Min, Random_Score_Max);
    return dist(Score_Seed);
}

/// Ada: Random_Mode : Boolean := False;
/// Flag telling the evaluation whether to add a small random value.
inline bool Random_Mode = false;

} // namespace chess

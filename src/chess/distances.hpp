// ---------------------------------------------------------------------------
// AdaChess – Distance type definitions
// Translated from: chess-distances.ads
// ---------------------------------------------------------------------------
#pragma once

namespace chess {

// -----------------------------------------------------------------------
// Distance_Type: number of moves a piece requires to reach a square.
// Ada: subtype Distance_Type is Natural range 0 .. 9;
// If a piece cannot reach a square (e.g., wrong-colored bishop), the
// special value Unreachable is used.
// -----------------------------------------------------------------------
using Distance = unsigned int;

inline constexpr Distance Max_Distance = 9;

/// Ada: No_Distance := 0
inline constexpr Distance No_Distance  = 0;

/// Ada: Unreachable := Distance_Type'Last (= 9)
inline constexpr Distance Unreachable  = Max_Distance;

} // namespace chess

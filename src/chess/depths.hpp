// ---------------------------------------------------------------------------
// AdaChess – Search depth type definitions
// Translated from: chess-depths.ads
// ---------------------------------------------------------------------------
#pragma once

namespace chess {

// -----------------------------------------------------------------------
// History_Depth_Type: how many half-moves from the start of the game.
// Ada: subtype History_Depth_Type is Natural range 0 .. 512;
// -----------------------------------------------------------------------
using HistoryDepth = unsigned int;

inline constexpr HistoryDepth Max_History_Depth = 512;

// -----------------------------------------------------------------------
// Depth_Type: how deep the engine searches while thinking.
// Ada: subtype Depth_Type is History_Depth_Type range 1 .. 256;
// -----------------------------------------------------------------------
using Depth = unsigned int;

inline constexpr Depth Min_Depth = 1;
inline constexpr Depth Max_Depth = 256;

/// Ada: Zero_Depth := Depth_Type'First (= 1)
inline constexpr Depth Zero_Depth         = Min_Depth;

/// Ada: Frontier_Depth := Zero_Depth + 1 (= 2)
inline constexpr Depth Frontier_Depth     = Zero_Depth + 1;

/// Ada: Pre_Frontier_Depth := Frontier_Depth + 1 (= 3)
inline constexpr Depth Pre_Frontier_Depth = Frontier_Depth + 1;

/// Ada: Horizon := Depth_Type'Last - 1 (= 255)
inline constexpr Depth Horizon            = Max_Depth - 1;

/// Ada: Pre_Horizon := Horizon - 2 (= 253)
inline constexpr Depth Pre_Horizon        = Horizon - 2;

} // namespace chess

/**
 * AdaChess - Smart Chess Engine
 *
 * Depth type definitions for search tree depth management.
 * C++ port of the original Ada implementation (chess-depths.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_DEPTHS_HPP
#define ADACHESS_DEPTHS_HPP

#include <cstdint>

namespace chess {

/**
 * History depth type - tracks half-moves from the beginning of the game.
 * Range: 0 to 512 (matches Ada's History_Depth_Type).
 *
 * A game longer than this value would overflow.
 */
using HistoryDepth = std::uint16_t;

/**
 * Maximum history depth value.
 */
inline constexpr HistoryDepth kMaxHistoryDepth = 512;

/**
 * Search depth type - tracks how deep we search while thinking.
 * Range: 1 to 256 (matches Ada's Depth_Type).
 *
 * It's very rare that we think deeper than 32 ply, but the type
 * supports up to 256 for move list management.
 */
using Depth = std::uint16_t;

/**
 * Minimum search depth value.
 */
inline constexpr Depth kMinDepth = 1;

/**
 * Maximum search depth value.
 */
inline constexpr Depth kMaxDepth = 256;

/**
 * Zero depth - the minimum depth value (equivalent to Depth_Type'First in Ada).
 * This is used as the base depth in search.
 */
inline constexpr Depth kZeroDepth = kMinDepth;

/**
 * Frontier depth - one ply above zero depth.
 * Corresponds to Ada's Frontier_Depth.
 */
inline constexpr Depth kFrontierDepth = kZeroDepth + 1;

/**
 * Pre-frontier depth - one ply above frontier depth.
 * Corresponds to Ada's Pre_Frontier_Depth.
 */
inline constexpr Depth kPreFrontierDepth = kFrontierDepth + 1;

/**
 * Horizon depth - maximum normal search depth (one less than max).
 * Corresponds to Ada's Horizon.
 */
inline constexpr Depth kHorizon = kMaxDepth - 1;

/**
 * Pre-horizon depth - two plies before horizon.
 * Corresponds to Ada's Pre_Horizon.
 */
inline constexpr Depth kPreHorizon = kHorizon - 2;

} // namespace chess

#endif // ADACHESS_DEPTHS_HPP

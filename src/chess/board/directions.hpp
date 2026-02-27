// ---------------------------------------------------------------------------
// AdaChess – Direction offsets for piece movement
// Translated from: chess-board-directions.ads
// ---------------------------------------------------------------------------
#pragma once

#include <array>

namespace chess::board {

// -----------------------------------------------------------------------
// Direction_Type: Integer range -21..+21
// Offsets added to a square index to get the target square.
// -----------------------------------------------------------------------
using Direction = int;

// Cardinal and diagonal directions
inline constexpr Direction North            = -10;
inline constexpr Direction North_North_East = -19;
inline constexpr Direction North_East       = -9;
inline constexpr Direction North_East_East  = -8;
inline constexpr Direction East             = +1;
inline constexpr Direction South_East_East  = +12;
inline constexpr Direction South_East       = +11;
inline constexpr Direction South_South_East = +21;
inline constexpr Direction South            = +10;
inline constexpr Direction South_South_West = +19;
inline constexpr Direction South_West       = +9;
inline constexpr Direction South_West_West  = +8;
inline constexpr Direction West             = -1;
inline constexpr Direction North_West_West  = -12;
inline constexpr Direction North_West       = -11;
inline constexpr Direction North_North_West = -21;
inline constexpr Direction No_Direction     = 0;

// Direction range limits (for array indexing).
inline constexpr Direction Direction_Min = -21;
inline constexpr Direction Direction_Max = +21;
inline constexpr int Direction_Range = Direction_Max - Direction_Min + 1; // 43

// -----------------------------------------------------------------------
// Piece-specific offset arrays.
// -----------------------------------------------------------------------

/// Knight: 8 L-shaped offsets
inline constexpr std::array<Direction, 8> Knight_Offsets = {{
    North_North_East, North_East_East,
    South_East_East,  South_South_East,
    South_South_West, South_West_West,
    North_West_West,  North_North_West
}};

/// Bishop: 4 diagonal offsets
inline constexpr std::array<Direction, 4> Bishop_Offsets = {{
    North_West, North_East, South_East, South_West
}};

/// Rook: 4 cardinal offsets
inline constexpr std::array<Direction, 4> Rook_Offsets = {{
    North, East, South, West
}};

/// Queen: 8 offsets (cardinal + diagonal)
inline constexpr std::array<Direction, 8> Queen_Offsets = {{
    North, North_East, East, South_East,
    South, South_West, West, North_West
}};

/// King: same 8 offsets as Queen (single step)
inline constexpr std::array<Direction, 8> King_Offsets = {{
    North, North_East, East, South_East,
    South, South_West, West, North_West
}};

} // namespace chess::board

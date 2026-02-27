// ---------------------------------------------------------------------------
// AdaChess – Precomputed attack pattern data
// Translated from: chess-board-attacks_data.ads
// ---------------------------------------------------------------------------
#pragma once

#include <array>
#include <functional>
#include "chess/board/board.hpp"
#include "chess/board/directions.hpp"

namespace chess::board {

// -----------------------------------------------------------------------
// Attack_Type: records a single attacker's origin square and piece type.
// -----------------------------------------------------------------------
struct Attack {
    Square origin = 0;
    Piece  piece  = Piece::Frame;
};

// -----------------------------------------------------------------------
// Attack_Collection_Type: up to 8 attackers.
// -----------------------------------------------------------------------
inline constexpr int Max_Attackers = 8;

struct AttackCollection {
    std::array<Attack, Max_Attackers> attacker{};
    int number_of_attackers = 0;
};

// -----------------------------------------------------------------------
// Directional attack functions.
// Each returns true if the given side attacks the square from that direction.
// -----------------------------------------------------------------------

// Sliding attacks (rook/queen for cardinal, bishop/queen for diagonal)
bool attacks_from_north      (const Board& board, Color side, Square sq);
bool attacks_from_south      (const Board& board, Color side, Square sq);
bool attacks_from_east       (const Board& board, Color side, Square sq);
bool attacks_from_west       (const Board& board, Color side, Square sq);
bool attacks_from_north_east (const Board& board, Color side, Square sq);
bool attacks_from_north_west (const Board& board, Color side, Square sq);
bool attacks_from_south_east (const Board& board, Color side, Square sq);
bool attacks_from_south_west (const Board& board, Color side, Square sq);

// Knight attacks (single step to knight-offset square)
bool attacks_from_north_north_east(const Board& board, Color side, Square sq);
bool attacks_from_north_east_east (const Board& board, Color side, Square sq);
bool attacks_from_south_east_east (const Board& board, Color side, Square sq);
bool attacks_from_south_south_east(const Board& board, Color side, Square sq);
bool attacks_from_south_south_west(const Board& board, Color side, Square sq);
bool attacks_from_south_west_west (const Board& board, Color side, Square sq);
bool attacks_from_north_west_west (const Board& board, Color side, Square sq);
bool attacks_from_north_north_west(const Board& board, Color side, Square sq);

/// Placeholder for unused direction indices.
bool attacks_from_placeholder(const Board& board, Color side, Square sq);

// -----------------------------------------------------------------------
// Function pointer type for directional attacks.
// Ada: type Attacks_Direction_Access is access function ...
// -----------------------------------------------------------------------
using AttacksDirFn = bool(*)(const Board&, Color, Square);

// -----------------------------------------------------------------------
// Attacks_To: maps Direction_Type index to the correct attack function.
// Indexed by (direction - Direction_Min) to handle negative indices.
// The array is initialized in attacks_data.cpp.
// -----------------------------------------------------------------------
extern std::array<AttacksDirFn, Direction_Range> Attacks_To;

/// Initialize the Attacks_To function pointer array.
/// Call this once at program startup.
void init_attacks_data();

} // namespace chess::board

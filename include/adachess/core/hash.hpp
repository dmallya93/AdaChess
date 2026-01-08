// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/square.hpp"
#include "adachess/core/types.hpp"
#include "adachess/core/history.hpp"
#include <array>
#include <cstdint>
#include <random>

namespace adachess {

// Forward declaration
class Chessboard;

// Zobrist hash tables for incremental hashing
// These are global (package-level in Ada) and initialized once
namespace zobrist {

// Hash table for pieces on squares
// [Piece][Square] -> hash value
extern std::array<std::array<Hash, 120>, 16> hash_pieces;

// Hash table for en passant squares
// [Square] -> hash value
extern std::array<Hash, 120> hash_en_passant;

// Hash table for castle rights
// Index: 0=none, 1=black queenside, 2=black kingside, 3=white queenside, 4=white kingside
extern std::array<Hash, 5> hash_castle;

// Hash value for side to move (XOR when switching sides)
extern Hash hash_side;

// Initialize all hash tables with random values
// Should be called once at program startup
// Uses a fixed seed for deterministic behavior (important for opening books)
void initialize_hash(uint32_t seed = 0x12345678);

} // namespace zobrist

} // namespace adachess

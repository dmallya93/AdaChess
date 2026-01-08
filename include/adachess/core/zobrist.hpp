//
// AdaChess - Smart Chess Engine
//
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// Email: adachess@gmail.com - Web Page: https://github.com/adachess/AdaChess
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef ADACHESS_CORE_ZOBRIST_HPP
#define ADACHESS_CORE_ZOBRIST_HPP

#include "adachess/core/types.hpp"
#include <array>
#include <cstdint>

namespace adachess {
namespace board {
struct Chessboard;
}  // namespace board
}  // namespace adachess

namespace adachess::core {

// ====== Zobrist Hash Tables ======
// Design Decision #5: Hash Storage Strategy
//
// Strategy: Global hash tables with fixed seed initialization
// - Hash tables are global variables initialized once at engine startup
// - Uses std::mt19937_64 with fixed seed (12345) for deterministic, reproducible hashes
// - Hash value is stored in Chessboard.zobrist_hash for better encapsulation
// - Incremental updates will be implemented in Task 9 (Play/Undo operations)
//
// Rationale:
// 1. Determinism: Fixed seed ensures same positions always hash to same value
// 2. Reproducibility: Critical for debugging, testing, and opening book matching
// 3. Performance: Global tables avoid per-board overhead, inline functions for updates
// 4. Simplicity: Matches Ada design, easier to migrate and verify correctness
//
// Hash Table Dimensions:
// - Hash_Pieces: 14 piece types × 120 board squares = 1,680 entries
// - Hash_En_Passant: 120 board squares (though only 16 are valid en passant squares)
// - Hash_Castle: 4 castle rights (White K-side, White Q-side, Black K-side, Black Q-side)
// - Hash_Side: Single value for side-to-move toggle

// Hash table for pieces on squares
// Index: [piece_type][square]
// Piece types: 0-13 (Empty=0, Frame=1, White pieces=2-7, Black pieces=8-13)
// Squares: 0-119 (10×12 mailbox)
extern std::array<std::array<uint64_t, 120>, 14> hash_pieces;

// Hash table for en passant squares
// Index: [square]
// Only squares on rank 3 (white) and rank 6 (black) are valid, but we allocate for all
extern std::array<uint64_t, 120> hash_en_passant;

// Hash table for castling rights
// Index: [0] = Black Queenside, [1] = Black Kingside, [2] = White Queenside, [3] = White Kingside
// Note: This matches Ada's Castle_Possibility enumeration order
extern std::array<uint64_t, 4> hash_castle;

// Hash value for side to move
// XOR this in when Black is to move (not XORed when White to move)
extern uint64_t hash_side;

// ====== Hash Initialization and Update Functions ======

// Initialize Zobrist hash tables with random values using fixed seed
// Must be called once at engine startup before any position hashing
// Uses std::mt19937_64 with seed 12345 for deterministic generation
// Also computes initial hash for the given chessboard
void initialize_hash(board::Chessboard& chessboard);

// Recompute full position hash from scratch
// Iterates over all pieces, castle rights, en passant, and side to move
// Stores result in chessboard.zobrist_hash
// Use this after setting up a position or for validation
void update_hash(board::Chessboard& chessboard);

// Optional debug validation (can be implemented later if needed)
// Recomputes hash and compares to stored value, returns true if match
// Useful for debugging incremental hash update logic
// bool validate_hash(const board::Chessboard& chessboard);

}  // namespace adachess::core

#endif  // ADACHESS_CORE_ZOBRIST_HPP

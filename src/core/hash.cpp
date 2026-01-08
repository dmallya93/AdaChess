// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#include "adachess/core/hash.hpp"
#include <random>

namespace adachess {
namespace zobrist {

// Define the global hash tables
std::array<std::array<Hash, 120>, 16> hash_pieces{};
std::array<Hash, 120> hash_en_passant{};
std::array<Hash, 5> hash_castle{};
Hash hash_side = 0;

void initialize_hash(uint32_t seed) {
    // Use a Mersenne Twister random number generator with fixed seed
    // for deterministic hash generation (important for opening books)
    std::mt19937_64 rng(seed);
    std::uniform_int_distribution<uint64_t> dist;

    // Initialize piece hashes
    // For each piece type (Empty through Black_King) and each square
    for (size_t piece = 0; piece < 16; ++piece) {
        for (size_t square = 0; square < 120; ++square) {
            hash_pieces[piece][square] = dist(rng);
        }
    }

    // Initialize en passant hashes
    for (size_t square = 0; square < 120; ++square) {
        hash_en_passant[square] = dist(rng);
    }

    // Initialize castle right hashes
    for (size_t i = 0; i < 5; ++i) {
        hash_castle[i] = dist(rng);
    }

    // Initialize side to move hash
    hash_side = dist(rng);
}

} // namespace zobrist
} // namespace adachess

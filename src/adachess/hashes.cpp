/**
 * AdaChess - Smart Chess Engine
 *
 * Hash type implementation.
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

#include "adachess/hashes.hpp"

namespace chess {

// ============================================================================
// HashRandomGenerator Implementation
// ============================================================================

HashRandomGenerator::HashRandomGenerator()
    : engine_(std::random_device{}()) {
}

HashRandomGenerator::HashRandomGenerator(std::uint64_t seed)
    : engine_(seed) {
}

void HashRandomGenerator::reset(std::uint64_t seed) {
    engine_.seed(seed);
}

void HashRandomGenerator::reset() {
    engine_.seed(std::random_device{}());
}

Hash HashRandomGenerator::random() {
    return engine_();
}

// ============================================================================
// Global Variables
// ============================================================================

HashRandomGenerator g_hash_seed{};

} // namespace chess

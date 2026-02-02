/**
 * AdaChess - Smart Chess Engine
 *
 * Hash type definitions for Zobrist hashing in transposition tables.
 * C++ port of the original Ada implementation (chess-hashes.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_HASHES_HPP
#define ADACHESS_HASHES_HPP

#include <cstdint>
#include <random>

namespace chess {

/**
 * Hash type for Zobrist hashing.
 * Uses a 64-bit unsigned integer (matches Ada's modular type 2^64).
 * Default value is 0.
 */
using Hash = std::uint64_t;

/**
 * Default/empty hash value.
 */
inline constexpr Hash kEmptyHash = 0;

/**
 * Hash random generator class.
 * Provides random hash values for Zobrist key initialization.
 * Corresponds to Ada's Hash_Random package instantiation.
 */
class HashRandomGenerator {
public:
    /**
     * Default constructor - seeds with a default value.
     */
    HashRandomGenerator();

    /**
     * Constructor with explicit seed.
     *
     * @param seed The seed value for the random generator.
     */
    explicit HashRandomGenerator(std::uint64_t seed);

    /**
     * Reset the generator with a new seed.
     * Corresponds to Ada's Reset procedure.
     *
     * @param seed The new seed value.
     */
    void reset(std::uint64_t seed);

    /**
     * Reset the generator with a random device-based seed.
     */
    void reset();

    /**
     * Generate a random hash value.
     * Corresponds to Ada's Random function.
     *
     * @return A random 64-bit hash value.
     */
    [[nodiscard]] Hash random();

private:
    std::mt19937_64 engine_;
};

/**
 * Global hash random generator (seed).
 * Corresponds to Ada's Seed variable.
 */
extern HashRandomGenerator g_hash_seed;

} // namespace chess

#endif // ADACHESS_HASHES_HPP

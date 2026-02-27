// ---------------------------------------------------------------------------
// AdaChess – Zobrist hash type definitions
// Translated from: chess-hashes.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <random>

namespace chess {

// -----------------------------------------------------------------------
// Hash_Type: 64-bit hash for Zobrist keys.
// Ada: type Hash_Type is mod 2 ** 64
//        with Size => 64, Default_Value => 0;
// -----------------------------------------------------------------------
using Hash = std::uint64_t;

// -----------------------------------------------------------------------
// Hash random number generator — wraps std::mt19937_64.
// Ada: package Hash_Random is new Ada.Numerics.Discrete_Random (Hash_Type);
//      Seed : Hash_Random.Generator;
// -----------------------------------------------------------------------

/// The global generator used for Zobrist key initialization.
inline std::mt19937_64 Hash_Seed{}; // NOLINT(cert-msc51-cpp)

/// Reset the hash random generator with a fresh seed.
/// Ada: procedure Reset (Seed : Generator) renames Hash_Random.Reset;
inline void hash_reset(std::mt19937_64& gen) {
    gen = std::mt19937_64{std::random_device{}()};
}

/// Generate a pseudo-random 64-bit hash value.
/// Ada: function Random (Seed : Generator) return Hash_Type renames Hash_Random.Random;
[[nodiscard]] inline Hash hash_random(std::mt19937_64& gen) {
    std::uniform_int_distribution<Hash> dist;
    return dist(gen);
}

} // namespace chess

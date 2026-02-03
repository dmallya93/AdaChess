/**
 * AdaChess - Smart Chess Engine
 *
 * Notation type definitions for chess move representation.
 * C++ port of the original Ada implementation (chess-notations.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_NOTATIONS_HPP
#define ADACHESS_NOTATIONS_HPP

#include <cstdint>

namespace chess {

// ============================================================================
// Notation Types
// ============================================================================

/**
 * Notation type for chess move representation.
 * Corresponds to Ada's Notation_Type enumeration.
 *
 * StandardAlgebraic: e4, Nf6, O-O, Qxc3+, e8=Q
 * LongAlgebraic:     e2-e4, Ng8-f6, O-O, Qa5xc3+, e7-e8=Q
 * PureAlgebraic:     e2e4, g8f6, e1c1, a5c3 (also called Winboard/coordinate notation)
 * ICCF:              5254, 7866, 5171 (numeric file+rank format)
 */
enum class NotationType : std::uint8_t {
    StandardAlgebraic = 0,  // SAN: e4, Nf6, O-O, Qxc3+, e8=Q
    LongAlgebraic = 1,      // LAN: e2-e4, Ng8-f6, Qa5xc3+
    PureAlgebraic = 2,      // Coordinate: e2e4, g8f6 (Winboard style)
    ICCF = 3                // Numeric: 5254, 7866 (ICCF standard)
};

/**
 * Total number of notation types.
 */
inline constexpr std::size_t kNumNotationTypes = 4;

/**
 * Global default notation setting.
 * Can be changed at runtime.
 */
inline NotationType g_default_notation = NotationType::StandardAlgebraic;

/**
 * Get the current default notation.
 * @return The current default notation type.
 */
[[nodiscard]] inline NotationType default_notation() noexcept {
    return g_default_notation;
}

/**
 * Set the default notation.
 * @param notation The new default notation type.
 */
inline void set_default_notation(NotationType notation) noexcept {
    g_default_notation = notation;
}

// ============================================================================
// Notation Utility Functions
// ============================================================================

/**
 * Convert a NotationType to its string representation.
 * @param notation The notation type.
 * @return A human-readable string describing the notation.
 */
[[nodiscard]] inline const char* notation_to_string(NotationType notation) noexcept {
    switch (notation) {
        case NotationType::StandardAlgebraic:
            return "Standard Algebraic (SAN)";
        case NotationType::LongAlgebraic:
            return "Long Algebraic (LAN)";
        case NotationType::PureAlgebraic:
            return "Pure Algebraic (Coordinate)";
        case NotationType::ICCF:
            return "ICCF (Numeric)";
    }
    return "Unknown";
}

} // namespace chess

#endif // ADACHESS_NOTATIONS_HPP

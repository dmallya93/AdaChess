// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/moves.hpp"
#include <cstdint>

namespace adachess {

// Hash type for Zobrist hashing
using Hash = uint64_t;

// History move structure
// Stores a move along with its hash and fifty-move counter
// Used for game history tracking and repetition detection
struct HistoryMove {
    Move move{};       // The move played
    Hash hash = 0;     // Position hash after the move
    uint8_t fifty = 0; // Fifty-move rule counter

    // Default constructor
    HistoryMove() = default;

    // Constructor with move
    explicit HistoryMove(const Move& mv)
        : move(mv), hash(0), fifty(0) {}

    // Constructor with all fields
    HistoryMove(const Move& mv, Hash h, uint8_t f)
        : move(mv), hash(h), fifty(f) {}
};

// Empty history move constant
constexpr HistoryMove EMPTY_HISTORY_MOVE{};

// Equality operator - compares only the move part (as in Ada)
inline bool operator==(const HistoryMove& left, const HistoryMove& right) {
    return left.move == right.move;
}

inline bool operator!=(const HistoryMove& left, const HistoryMove& right) {
    return !(left == right);
}

} // namespace adachess

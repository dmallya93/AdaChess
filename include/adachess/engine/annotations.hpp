// AdaChess - C++ Chess Engine
// Migrated from Ada source
// Copyright (C) 2013-2023 - Alessandro Iavicoli
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "adachess/board/moves.hpp"
#include <cstdint>

namespace adachess {

// Annotation type for move analysis
// Used for real-time move evaluation
enum class Annotation : uint8_t {
    None = 0,        // No annotation
    Blunder = 1,     // Blunder (major mistake)
    Mistake = 2,     // Mistake
    Dubious = 3,     // Dubious move (questionable)
    Interesting = 4, // Interesting move (leads to small improvement)
    Good = 5,        // Good move (valuable advantage)
    Brilliant = 6    // Brilliant move (amazing!)
};

// Annotated move structure
// Combines a move with its annotation
struct AnnotatedMove {
    Move move{};
    Annotation annotation = Annotation::None;

    // Default constructor
    AnnotatedMove() = default;

    // Constructor with move
    explicit AnnotatedMove(const Move& mv)
        : move(mv), annotation(Annotation::None) {}

    // Constructor with move and annotation
    AnnotatedMove(const Move& mv, Annotation annot)
        : move(mv), annotation(annot) {}
};

// Empty annotated move constant
constexpr AnnotatedMove EMPTY_ANNOTATED_MOVE{};

// Equality operators for AnnotatedMove
// Compares only the move part (as in Ada)
inline bool operator==(const AnnotatedMove& left, const AnnotatedMove& right) {
    return left.move == right.move;
}

inline bool operator!=(const AnnotatedMove& left, const AnnotatedMove& right) {
    return !(left == right);
}

// Mixed equality: Move and AnnotatedMove
inline bool operator==(const Move& left, const AnnotatedMove& right) {
    return left == right.move;
}

inline bool operator==(const AnnotatedMove& left, const Move& right) {
    return left.move == right;
}

inline bool operator!=(const Move& left, const AnnotatedMove& right) {
    return !(left == right);
}

inline bool operator!=(const AnnotatedMove& left, const Move& right) {
    return !(left == right);
}

} // namespace adachess

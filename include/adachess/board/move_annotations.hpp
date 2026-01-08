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

#ifndef ADACHESS_BOARD_MOVE_ANNOTATIONS_HPP
#define ADACHESS_BOARD_MOVE_ANNOTATIONS_HPP

#include "adachess/board/move.hpp"
#include <cstdint>

namespace adachess::board {

// ====== Annotation Types ======

// Annotation types for move analysis and real-time move evaluation
enum class Annotation_Type : uint8_t {
    None = 0,
    Blunder = 1,
    Mistake = 2,
    Dubious = 3,      // Note: Ada source has typo "Dobious"
    Interesting = 4,  // A move that leads to a small improvement
    Good = 5,         // A move that brings valuable advantages
    Brilliant = 6     // Amazing move!
};

// Annotated move structure wrapping Move_Type with annotation
struct Annotated_Move_Type {
    Move_Type move{Empty_Move};
    Annotation_Type annotation{Annotation_Type::None};
};

// Empty annotated move constant
constexpr Annotated_Move_Type Empty_Annotated_Move{
    Empty_Move,            // move
    Annotation_Type::None  // annotation
};

// ====== Annotated Move Comparison ======

// Equality operators for Annotated_Move_Type
// Note: Comparison is based on the move only, not the annotation
[[nodiscard]] inline bool operator==(const Annotated_Move_Type& left,
                                      const Annotated_Move_Type& right) noexcept {
    return left.move == right.move;
}

[[nodiscard]] inline bool operator!=(const Annotated_Move_Type& left,
                                      const Annotated_Move_Type& right) noexcept {
    return !(left == right);
}

// Mixed comparison: Move_Type vs Annotated_Move_Type
[[nodiscard]] inline bool operator==(const Move_Type& left,
                                      const Annotated_Move_Type& right) noexcept {
    return left == right.move;
}

[[nodiscard]] inline bool operator==(const Annotated_Move_Type& left,
                                      const Move_Type& right) noexcept {
    return left.move == right;
}

[[nodiscard]] inline bool operator!=(const Move_Type& left,
                                      const Annotated_Move_Type& right) noexcept {
    return !(left == right);
}

[[nodiscard]] inline bool operator!=(const Annotated_Move_Type& left,
                                      const Move_Type& right) noexcept {
    return !(left == right);
}

}  // namespace adachess::board

#endif  // ADACHESS_BOARD_MOVE_ANNOTATIONS_HPP

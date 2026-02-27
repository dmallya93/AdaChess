// ---------------------------------------------------------------------------
// AdaChess – Move annotation type definitions
// Translated from: chess-moves-annotations.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include "chess/moves.hpp"

namespace chess {

// -----------------------------------------------------------------------
// Annotation_Type
// Ada: (None, Blunder, Mistake, Dobious, Interesting, Good, Brilliant)
//      with Size => 3, Default_Value => None;
// Note: Ada source has a typo "Dobious"; corrected to "Dubious" here.
// -----------------------------------------------------------------------
enum class Annotation : std::uint8_t {
    None        = 0,
    Blunder     = 1,
    Mistake     = 2,
    Dubious     = 3,  // Ada: "Dobious" (typo corrected)
    Interesting = 4,  // A move that leads to a small improvement
    Good        = 5,  // A move that brings a valuable advantage
    Brilliant   = 6   // Amazing move!
};

// -----------------------------------------------------------------------
// Annotated_Move_Type (struct AnnotatedMove)
// Ada record: Move (Move_Type) + Annotation (Annotation_Type)
// -----------------------------------------------------------------------
struct AnnotatedMove {
    Move       move       = Empty_Move;
    Annotation annotation = Annotation::None;
};

// Ada: function "=" (Left, Right : in Annotated_Move_Type) return Boolean is
//        (Left.Move = Right.Move);
[[nodiscard]] constexpr bool operator==(const AnnotatedMove& lhs,
                                        const AnnotatedMove& rhs) noexcept {
    return lhs.move == rhs.move;
}

[[nodiscard]] constexpr bool operator!=(const AnnotatedMove& lhs,
                                        const AnnotatedMove& rhs) noexcept {
    return !(lhs == rhs);
}

// Ada: function "=" (Left : in Move_Type; Right : in Annotated_Move_Type) ...
[[nodiscard]] constexpr bool operator==(const Move& lhs,
                                        const AnnotatedMove& rhs) noexcept {
    return lhs == rhs.move;
}

[[nodiscard]] constexpr bool operator!=(const Move& lhs,
                                        const AnnotatedMove& rhs) noexcept {
    return !(lhs == rhs);
}

// Ada: function "=" (Left : in Annotated_Move_Type; Right : in Move_Type) ...
[[nodiscard]] constexpr bool operator==(const AnnotatedMove& lhs,
                                        const Move& rhs) noexcept {
    return lhs.move == rhs;
}

[[nodiscard]] constexpr bool operator!=(const AnnotatedMove& lhs,
                                        const Move& rhs) noexcept {
    return !(lhs == rhs);
}

// -----------------------------------------------------------------------
// Empty_Annotated_Move constant
// -----------------------------------------------------------------------
inline constexpr AnnotatedMove Empty_Annotated_Move = {
    Empty_Move,       // move
    Annotation::None  // annotation
};

} // namespace chess

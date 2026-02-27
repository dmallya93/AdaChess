// ---------------------------------------------------------------------------
// AdaChess – Notation type definitions
// Translated from: chess-notations.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>

namespace chess {

// -----------------------------------------------------------------------
// Notation_Type: supported algebraic notation formats.
// Ada: type Notation_Type is
//        (Standard_Algebraic, Long_Algebraic, Pure_Algebraic, ICCF)
//        with Size => 2;
// -----------------------------------------------------------------------
enum class Notation : std::uint8_t {
    Standard_Algebraic = 0, // e4 Nf6 O-O Qxc3+ e8=Q ...
    Long_Algebraic     = 1, // e2-e4 Ng8-f6 O-O Qa5xc3+ e7-e8=Q ...
    Pure_Algebraic     = 2, // e2e4 g8f6 e1c1 a5c3
    ICCF               = 3  // 5254 7866 5171
};

/// Ada: Default_Notation : Notation_Type := Standard_Algebraic;
inline Notation Default_Notation = Notation::Standard_Algebraic;

} // namespace chess

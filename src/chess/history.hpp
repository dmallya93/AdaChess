// ---------------------------------------------------------------------------
// AdaChess – History move type definitions
// Translated from: chess-history.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include "chess/moves.hpp"
#include "chess/hashes.hpp"

namespace chess {

// -----------------------------------------------------------------------
// History_Move_Type (struct HistoryMove)
// Ada record: Move (Move_Type), Hash (Hash_Type), Fifty (Natural)
// -----------------------------------------------------------------------
struct HistoryMove {
    Move         move  = Empty_Move;
    std::uint64_t hash  = 0;    // Ada: Hash_Type (mod 2**64)
    unsigned int fifty = 0;     // Ada: Natural (fifty-move rule counter)
};

// Ada: function "=" (Left, Right : in History_Move_Type) return Boolean is
//        (Left.Move = Right.Move);
// Override: only compare the Move fields.
[[nodiscard]] constexpr bool operator==(const HistoryMove& lhs,
                                        const HistoryMove& rhs) noexcept {
    return lhs.move == rhs.move;
}

[[nodiscard]] constexpr bool operator!=(const HistoryMove& lhs,
                                        const HistoryMove& rhs) noexcept {
    return !(lhs == rhs);
}

// -----------------------------------------------------------------------
// Empty_History_Move constant
// -----------------------------------------------------------------------
inline constexpr HistoryMove Empty_History_Move = {
    Empty_Move,  // move
    0,           // hash
    0            // fifty
};

} // namespace chess

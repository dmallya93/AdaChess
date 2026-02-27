// ---------------------------------------------------------------------------
// AdaChess – Player type definitions
// Translated from: chess-players.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>

namespace chess {

// -----------------------------------------------------------------------
// Player_Type: whether a side is controlled by a human or the engine.
// Ada: type Player_Type is (Human, Engine)
//        with Size => 2, Default_Value => Human;
// -----------------------------------------------------------------------
enum class Player : std::uint8_t {
    Human  = 0,
    Engine = 1
};

} // namespace chess

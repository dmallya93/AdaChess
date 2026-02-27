// ---------------------------------------------------------------------------
// AdaChess – Board coordinate functions
// Translated from: chess-board.adb
// ---------------------------------------------------------------------------
#include "chess/board/board.hpp"

namespace chess::board {

namespace {

// -----------------------------------------------------------------------
// Private coordinate lookup tables (from chess-board.ads private section).
// -----------------------------------------------------------------------

constexpr std::array<Coordinate, Board_Size> Files = {{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
}};

constexpr std::array<Coordinate, Board_Size> Diagonals = {{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  7,  6,  5,  4,  3,  2,  1,  0, -1,
    -1,  6,  5,  4,  3,  2,  1,  0,  8, -1,
    -1,  5,  4,  3,  2,  1,  0,  8,  9, -1,
    -1,  4,  3,  2,  1,  0,  8,  9, 10, -1,
    -1,  3,  2,  1,  0,  8,  9, 10, 11, -1,
    -1,  2,  1,  0,  8,  9, 10, 11, 12, -1,
    -1,  1,  0,  8,  9, 10, 11, 12, 13, -1,
    -1,  0,  8,  9, 10, 11, 12, 13, 14, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
}};

constexpr std::array<Coordinate, Board_Size> Anti_Diagonals = {{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1,  7,  8,  9, 10, 11, 12, 13, 14, -1,
    -1,  6,  7,  8,  9, 10, 11, 12, 13, -1,
    -1,  5,  6,  7,  8,  9, 10, 11, 12, -1,
    -1,  4,  5,  6,  7,  8,  9, 10, 11, -1,
    -1,  3,  4,  5,  6,  7,  8,  9, 10, -1,
    -1,  2,  3,  4,  5,  6,  7,  8,  9, -1,
    -1,  1,  2,  3,  4,  5,  6,  7,  8, -1,
    -1,  0,  1,  2,  3,  4,  5,  6,  7, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
}};

} // anonymous namespace

// -----------------------------------------------------------------------
// Coordinate functions — match Ada implementations in chess-board.adb.
// -----------------------------------------------------------------------

Coordinate file_of(Square sq) {
    // Ada uses the lookup table: return Files(Square);
    return Files[static_cast<std::size_t>(sq)];
}

Coordinate rank_of(Square sq) {
    // Ada uses formula: return 10 - (Square / 10);
    return 10 - (sq / 10);
}

Coordinate diagonal_of(Square sq) {
    return Diagonals[static_cast<std::size_t>(sq)];
}

Coordinate anti_diagonal_of(Square sq) {
    return Anti_Diagonals[static_cast<std::size_t>(sq)];
}

} // namespace chess::board

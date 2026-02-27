// ---------------------------------------------------------------------------
// AdaChess – Precomputed attack pattern data (implementation)
// Translated from: chess-board-attacks_data.adb
// ---------------------------------------------------------------------------
#include "chess/board/attacks_data.hpp"

namespace chess::board {

// -----------------------------------------------------------------------
// Helper: check if piece at target matches a sliding attacker of the given
// side along a cardinal direction (Rook or Queen).
// -----------------------------------------------------------------------
static bool is_rook_or_queen(Piece piece, Color side) {
    if (side == Color::White) {
        return piece == Piece::White_Rook || piece == Piece::White_Queen;
    } else {
        return piece == Piece::Black_Rook || piece == Piece::Black_Queen;
    }
}

// -----------------------------------------------------------------------
// Helper: check if piece at target matches a sliding attacker of the given
// side along a diagonal direction (Bishop or Queen).
// -----------------------------------------------------------------------
static bool is_bishop_or_queen(Piece piece, Color side) {
    if (side == Color::White) {
        return piece == Piece::White_Bishop || piece == Piece::White_Queen;
    } else {
        return piece == Piece::Black_Bishop || piece == Piece::Black_Queen;
    }
}

// -----------------------------------------------------------------------
// Helper: check if piece at square is a knight of the given side.
// -----------------------------------------------------------------------
static bool is_knight_of_side(Piece piece, Color side) {
    if (side == Color::White) {
        return piece == Piece::White_Knight;
    } else {
        return piece == Piece::Black_Knight;
    }
}

// -----------------------------------------------------------------------
// Generic sliding attack: slide in the given direction until a non-empty
// square is found, then check if it matches the expected piece type.
// -----------------------------------------------------------------------
template <bool IsDiagonal>
static bool sliding_attack(const Board& board, Color side, Square sq, Direction dir) {
    auto target = static_cast<std::size_t>(sq + dir);
    while (board[target] == Piece::Empty) {
        target = static_cast<std::size_t>(static_cast<int>(target) + dir);
    }
    Piece piece = board[target];
    if constexpr (IsDiagonal) {
        return is_bishop_or_queen(piece, side);
    } else {
        return is_rook_or_queen(piece, side);
    }
}

// -----------------------------------------------------------------------
// Cardinal sliding attacks (Rook / Queen)
// -----------------------------------------------------------------------

bool attacks_from_north(const Board& board, Color side, Square sq) {
    return sliding_attack<false>(board, side, sq, North);
}

bool attacks_from_south(const Board& board, Color side, Square sq) {
    return sliding_attack<false>(board, side, sq, South);
}

bool attacks_from_east(const Board& board, Color side, Square sq) {
    return sliding_attack<false>(board, side, sq, East);
}

bool attacks_from_west(const Board& board, Color side, Square sq) {
    return sliding_attack<false>(board, side, sq, West);
}

// -----------------------------------------------------------------------
// Diagonal sliding attacks (Bishop / Queen)
// -----------------------------------------------------------------------

bool attacks_from_north_east(const Board& board, Color side, Square sq) {
    return sliding_attack<true>(board, side, sq, North_East);
}

bool attacks_from_north_west(const Board& board, Color side, Square sq) {
    return sliding_attack<true>(board, side, sq, North_West);
}

bool attacks_from_south_east(const Board& board, Color side, Square sq) {
    return sliding_attack<true>(board, side, sq, South_East);
}

bool attacks_from_south_west(const Board& board, Color side, Square sq) {
    return sliding_attack<true>(board, side, sq, South_West);
}

// -----------------------------------------------------------------------
// Knight attacks — check the single square at the offset.
// Ada: each knight-offset function checks Board(Square) for knight.
// NOTE: In Ada, the parameter Square already has the offset applied
// (i.e., the function is called with the knight-offset square).
// -----------------------------------------------------------------------

bool attacks_from_north_north_east(const Board& board, Color side, Square sq) {
    return is_knight_of_side(board[static_cast<std::size_t>(sq)], side);
}

bool attacks_from_north_east_east(const Board& board, Color side, Square sq) {
    return is_knight_of_side(board[static_cast<std::size_t>(sq)], side);
}

bool attacks_from_south_east_east(const Board& board, Color side, Square sq) {
    return is_knight_of_side(board[static_cast<std::size_t>(sq)], side);
}

bool attacks_from_south_south_east(const Board& board, Color side, Square sq) {
    return is_knight_of_side(board[static_cast<std::size_t>(sq)], side);
}

bool attacks_from_south_south_west(const Board& board, Color side, Square sq) {
    return is_knight_of_side(board[static_cast<std::size_t>(sq)], side);
}

bool attacks_from_south_west_west(const Board& board, Color side, Square sq) {
    return is_knight_of_side(board[static_cast<std::size_t>(sq)], side);
}

bool attacks_from_north_west_west(const Board& board, Color side, Square sq) {
    return is_knight_of_side(board[static_cast<std::size_t>(sq)], side);
}

bool attacks_from_north_north_west(const Board& board, Color side, Square sq) {
    return is_knight_of_side(board[static_cast<std::size_t>(sq)], side);
}

// -----------------------------------------------------------------------
// Placeholder function for unused direction offsets.
// -----------------------------------------------------------------------
bool attacks_from_placeholder(const Board& /*board*/, Color /*side*/, Square /*sq*/) {
    return false;
}

// -----------------------------------------------------------------------
// Attacks_To function-pointer array.
// Initialized to placeholder, then populated by init_attacks_data().
// -----------------------------------------------------------------------
std::array<AttacksDirFn, Direction_Range> Attacks_To;

/// Convert a Direction value to an index in the Attacks_To array.
static constexpr std::size_t dir_index(Direction d) {
    return static_cast<std::size_t>(d - Direction_Min);
}

void init_attacks_data() {
    // Default all to placeholder
    Attacks_To.fill(attacks_from_placeholder);

    // Cardinal (Rook / Queen)
    Attacks_To[dir_index(North)] = attacks_from_north;
    Attacks_To[dir_index(South)] = attacks_from_south;
    Attacks_To[dir_index(East)]  = attacks_from_east;
    Attacks_To[dir_index(West)]  = attacks_from_west;

    // Diagonal (Bishop / Queen)
    Attacks_To[dir_index(North_East)] = attacks_from_north_east;
    Attacks_To[dir_index(North_West)] = attacks_from_north_west;
    Attacks_To[dir_index(South_East)] = attacks_from_south_east;
    Attacks_To[dir_index(South_West)] = attacks_from_south_west;

    // Knight offsets
    Attacks_To[dir_index(North_North_East)] = attacks_from_north_north_east;
    Attacks_To[dir_index(North_East_East)]  = attacks_from_north_east_east;
    Attacks_To[dir_index(South_East_East)]  = attacks_from_south_east_east;
    Attacks_To[dir_index(South_South_East)] = attacks_from_south_south_east;
    Attacks_To[dir_index(South_South_West)] = attacks_from_south_south_west;
    Attacks_To[dir_index(South_West_West)]  = attacks_from_south_west_west;
    Attacks_To[dir_index(North_West_West)]  = attacks_from_north_west_west;
    Attacks_To[dir_index(North_North_West)] = attacks_from_north_north_west;
}

} // namespace chess::board

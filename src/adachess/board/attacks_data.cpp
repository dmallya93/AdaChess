/**
 * AdaChess - Smart Chess Engine
 *
 * Attack data infrastructure implementation.
 * C++ port of the original Ada implementation (chess-board-attacks_data.adb and chess-engine.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/board/attacks_data.hpp"

namespace chess::board {

// ============================================================================
// Global Tables
// ============================================================================

// Direction table (initialized by preload_sliding_direction)
DirectionTable g_directions_table{};

// Attack dispatch table (initialized by initialize_attacks_dispatch_table)
AttackDispatchTable g_attacks_to{};

// ============================================================================
// Direction Table Initialization
// ============================================================================

namespace {

/**
 * Internal helper to compute the sliding direction between two squares.
 * Used during table initialization.
 *
 * @param origin The starting square.
 * @param destination The ending square.
 * @return The direction connecting the squares, or kNoDirection if not connected.
 */
Direction get_direction_impl(Square origin, Square destination) noexcept {
    // In our 10x12 mailbox layout:
    // - Lower indices are towards rank 8 (north)
    // - Higher indices are towards rank 1 (south)
    // - Indices increase by 1 going east (a->h)
    // - Indices increase by 10 going south (rank 8 -> rank 1)

    // Same file -> North or South
    if (file(origin) == file(destination)) {
        if (origin > destination) {
            // destination has lower index = moving north
            return kNorth;
        } else {
            // destination has higher index = moving south
            return kSouth;
        }
    }

    // Same rank -> East or West
    // Note: Ada convention - when origin < destination on same rank, return West
    // This matches the Ada behavior where the direction represents the line
    // connecting the squares, not necessarily the move direction
    if (rank(origin) == rank(destination)) {
        if (origin < destination) {
            return kWest;
        } else {
            return kEast;
        }
    }

    // Same diagonal (NE-SW diagonal, like a1-h8)
    // Moving NE: lower index and higher file = index decreases by 9 each step
    // Moving SW: higher index and lower file = index increases by 9 each step
    if (diagonal(origin) == diagonal(destination)) {
        if (origin > destination) {
            // destination has lower index = moving northeast
            return kNorthEast;
        } else {
            // destination has higher index = moving southwest
            return kSouthWest;
        }
    }

    // Same anti-diagonal (NW-SE diagonal, like a8-h1)
    // Moving NW: lower index and lower file = index decreases by 11 each step
    // Moving SE: higher index and higher file = index increases by 11 each step
    if (anti_diagonal(origin) == anti_diagonal(destination)) {
        if (origin > destination) {
            // destination has lower index = moving northwest
            return kNorthWest;
        } else {
            // destination has higher index = moving southeast
            return kSouthEast;
        }
    }

    // Not connected by a sliding direction
    return kNoDirection;
}

} // anonymous namespace

void preload_sliding_direction() {
    for (std::size_t origin = 0; origin < kBoardSize; ++origin) {
        for (std::size_t destination = 0; destination < kBoardSize; ++destination) {
            g_directions_table[origin][destination] =
                get_direction_impl(Square(static_cast<std::uint8_t>(origin)),
                                   Square(static_cast<std::uint8_t>(destination)));
        }
    }
}

// ============================================================================
// Sliding Piece Attack Detection Functions
// ============================================================================

bool attacks_from_north(const BoardArray& board, Color side, Square square) noexcept {
    Square target = square + kNorth;

    // Scan northward until we hit a non-empty square
    while (board[target.to_index()] == Piece::Empty) {
        target = target + kNorth;
    }

    const Piece piece = board[target.to_index()];

    // Check if it's a rook or queen of the specified side
    if (side == Color::White) {
        return piece == Piece::WhiteRook || piece == Piece::WhiteQueen;
    } else {
        return piece == Piece::BlackRook || piece == Piece::BlackQueen;
    }
}

bool attacks_from_south(const BoardArray& board, Color side, Square square) noexcept {
    Square target = square + kSouth;

    while (board[target.to_index()] == Piece::Empty) {
        target = target + kSouth;
    }

    const Piece piece = board[target.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteRook || piece == Piece::WhiteQueen;
    } else {
        return piece == Piece::BlackRook || piece == Piece::BlackQueen;
    }
}

bool attacks_from_east(const BoardArray& board, Color side, Square square) noexcept {
    Square target = square + kEast;

    while (board[target.to_index()] == Piece::Empty) {
        target = target + kEast;
    }

    const Piece piece = board[target.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteRook || piece == Piece::WhiteQueen;
    } else {
        return piece == Piece::BlackRook || piece == Piece::BlackQueen;
    }
}

bool attacks_from_west(const BoardArray& board, Color side, Square square) noexcept {
    Square target = square + kWest;

    while (board[target.to_index()] == Piece::Empty) {
        target = target + kWest;
    }

    const Piece piece = board[target.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteRook || piece == Piece::WhiteQueen;
    } else {
        return piece == Piece::BlackRook || piece == Piece::BlackQueen;
    }
}

bool attacks_from_north_east(const BoardArray& board, Color side, Square square) noexcept {
    Square target = square + kNorthEast;

    while (board[target.to_index()] == Piece::Empty) {
        target = target + kNorthEast;
    }

    const Piece piece = board[target.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteBishop || piece == Piece::WhiteQueen;
    } else {
        return piece == Piece::BlackBishop || piece == Piece::BlackQueen;
    }
}

bool attacks_from_north_west(const BoardArray& board, Color side, Square square) noexcept {
    Square target = square + kNorthWest;

    while (board[target.to_index()] == Piece::Empty) {
        target = target + kNorthWest;
    }

    const Piece piece = board[target.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteBishop || piece == Piece::WhiteQueen;
    } else {
        return piece == Piece::BlackBishop || piece == Piece::BlackQueen;
    }
}

bool attacks_from_south_east(const BoardArray& board, Color side, Square square) noexcept {
    Square target = square + kSouthEast;

    while (board[target.to_index()] == Piece::Empty) {
        target = target + kSouthEast;
    }

    const Piece piece = board[target.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteBishop || piece == Piece::WhiteQueen;
    } else {
        return piece == Piece::BlackBishop || piece == Piece::BlackQueen;
    }
}

bool attacks_from_south_west(const BoardArray& board, Color side, Square square) noexcept {
    Square target = square + kSouthWest;

    while (board[target.to_index()] == Piece::Empty) {
        target = target + kSouthWest;
    }

    const Piece piece = board[target.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteBishop || piece == Piece::WhiteQueen;
    } else {
        return piece == Piece::BlackBishop || piece == Piece::BlackQueen;
    }
}

// ============================================================================
// Knight Attack Detection Functions
// ============================================================================

bool attacks_from_north_north_east(const BoardArray& board, Color side, Square square) noexcept {
    const Piece piece = board[square.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteKnight;
    } else {
        return piece == Piece::BlackKnight;
    }
}

bool attacks_from_north_east_east(const BoardArray& board, Color side, Square square) noexcept {
    const Piece piece = board[square.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteKnight;
    } else {
        return piece == Piece::BlackKnight;
    }
}

bool attacks_from_south_east_east(const BoardArray& board, Color side, Square square) noexcept {
    const Piece piece = board[square.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteKnight;
    } else {
        return piece == Piece::BlackKnight;
    }
}

bool attacks_from_south_south_east(const BoardArray& board, Color side, Square square) noexcept {
    const Piece piece = board[square.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteKnight;
    } else {
        return piece == Piece::BlackKnight;
    }
}

bool attacks_from_south_south_west(const BoardArray& board, Color side, Square square) noexcept {
    const Piece piece = board[square.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteKnight;
    } else {
        return piece == Piece::BlackKnight;
    }
}

bool attacks_from_south_west_west(const BoardArray& board, Color side, Square square) noexcept {
    const Piece piece = board[square.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteKnight;
    } else {
        return piece == Piece::BlackKnight;
    }
}

bool attacks_from_north_west_west(const BoardArray& board, Color side, Square square) noexcept {
    const Piece piece = board[square.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteKnight;
    } else {
        return piece == Piece::BlackKnight;
    }
}

bool attacks_from_north_north_west(const BoardArray& board, Color side, Square square) noexcept {
    const Piece piece = board[square.to_index()];

    if (side == Color::White) {
        return piece == Piece::WhiteKnight;
    } else {
        return piece == Piece::BlackKnight;
    }
}

// ============================================================================
// Placeholder Function
// ============================================================================

bool attacks_from_placeholder(
    [[maybe_unused]] const BoardArray& board,
    [[maybe_unused]] Color side,
    [[maybe_unused]] Square square) noexcept {
    return false;
}

// ============================================================================
// Attack Dispatch Table Initialization
// ============================================================================

void initialize_attacks_dispatch_table() {
    // Initialize all entries to placeholder first
    for (auto& entry : g_attacks_to) {
        entry = attacks_from_placeholder;
    }

    // Map sliding directions to their functions
    g_attacks_to[static_cast<std::size_t>(kNorth + static_cast<int>(kDirectionOffset))] = attacks_from_north;
    g_attacks_to[static_cast<std::size_t>(kSouth + static_cast<int>(kDirectionOffset))] = attacks_from_south;
    g_attacks_to[static_cast<std::size_t>(kEast + static_cast<int>(kDirectionOffset))] = attacks_from_east;
    g_attacks_to[static_cast<std::size_t>(kWest + static_cast<int>(kDirectionOffset))] = attacks_from_west;
    g_attacks_to[static_cast<std::size_t>(kNorthEast + static_cast<int>(kDirectionOffset))] = attacks_from_north_east;
    g_attacks_to[static_cast<std::size_t>(kNorthWest + static_cast<int>(kDirectionOffset))] = attacks_from_north_west;
    g_attacks_to[static_cast<std::size_t>(kSouthEast + static_cast<int>(kDirectionOffset))] = attacks_from_south_east;
    g_attacks_to[static_cast<std::size_t>(kSouthWest + static_cast<int>(kDirectionOffset))] = attacks_from_south_west;

    // Map knight directions to their functions
    g_attacks_to[static_cast<std::size_t>(kNorthNorthEast + static_cast<int>(kDirectionOffset))] = attacks_from_north_north_east;
    g_attacks_to[static_cast<std::size_t>(kNorthEastEast + static_cast<int>(kDirectionOffset))] = attacks_from_north_east_east;
    g_attacks_to[static_cast<std::size_t>(kSouthEastEast + static_cast<int>(kDirectionOffset))] = attacks_from_south_east_east;
    g_attacks_to[static_cast<std::size_t>(kSouthSouthEast + static_cast<int>(kDirectionOffset))] = attacks_from_south_south_east;
    g_attacks_to[static_cast<std::size_t>(kSouthSouthWest + static_cast<int>(kDirectionOffset))] = attacks_from_south_south_west;
    g_attacks_to[static_cast<std::size_t>(kSouthWestWest + static_cast<int>(kDirectionOffset))] = attacks_from_south_west_west;
    g_attacks_to[static_cast<std::size_t>(kNorthWestWest + static_cast<int>(kDirectionOffset))] = attacks_from_north_west_west;
    g_attacks_to[static_cast<std::size_t>(kNorthNorthWest + static_cast<int>(kDirectionOffset))] = attacks_from_north_north_west;
}

} // namespace chess::board

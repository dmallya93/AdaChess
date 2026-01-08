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

#ifndef ADACHESS_BOARD_CHESSBOARD_HPP
#define ADACHESS_BOARD_CHESSBOARD_HPP

#include "adachess/board/move.hpp"
#include "adachess/board/square.hpp"
#include "adachess/core/types.hpp"
#include <array>
#include <cstdint>

namespace adachess::board {

// ====== History Move Type ======
// History_Move_Type struct tracks moves in the game history with hash and
// fifty-move counter
struct History_Move_Type {
    Move_Type move{Empty_Move};
    adachess::core::Hash hash{0};
    uint8_t fifty{0};
};

// Empty history move constant
constexpr History_Move_Type Empty_History_Move{
    Empty_Move,  // move
    0,           // hash
    0            // fifty
};

// History move equality operator - compares moves only
[[nodiscard]] inline bool operator==(const History_Move_Type& left,
                                      const History_Move_Type& right) noexcept {
    return left.move == right.move;
}

[[nodiscard]] inline bool operator!=(const History_Move_Type& left,
                                      const History_Move_Type& right) noexcept {
    return !(left == right);
}

// ====== Constants for Piece Tracking ======
constexpr uint8_t WHITE_PIECES_END = 16;
constexpr uint8_t BLACK_PIECES_OFFSET = 16;
constexpr uint8_t BLACK_PIECES_START = 17;
constexpr uint8_t BLACK_PIECES_END = 32;
constexpr adachess::core::Depth MAX_PLY = 128;

// ====== Chessboard Structure ======
struct Chessboard {
    // ===== Board Representation =====
    BoardArray board{};

    // ===== Piece Tracking =====
    // Pieces list: indices 1-16 for white, 17-32 for black
    std::array<Square, 33> pieces_list{};

    // Reverse lookup: square -> piece list index
    std::array<uint8_t, 120> piece_table{};

    // Piece counters (0-16 for each color)
    uint8_t white_pieces_counter{0};
    uint8_t black_pieces_counter{0};

    // Cached king positions for fast check detection
    Square white_king_position{0};
    Square black_king_position{0};

    // ===== Game State =====
    adachess::core::Color side_to_move{adachess::core::Color::White};
    adachess::core::Depth ply{0};
    adachess::core::History history_ply{0};

    // ===== Move Stack =====
    std::array<Move_Type, 4096> moves{};
    std::array<uint16_t, MAX_PLY> move_pointer{};

    // ===== History =====
    std::array<bool, MAX_PLY> white_castle_kingside{};
    std::array<bool, MAX_PLY> white_castle_queenside{};
    std::array<bool, MAX_PLY> black_castle_kingside{};
    std::array<bool, MAX_PLY> black_castle_queenside{};
    std::array<Square, MAX_PLY> en_passant_square{};
    std::array<uint8_t, MAX_PLY> fifty_move_counter{};

    // ===== Zobrist Hash =====
    adachess::core::Hash zobrist_hash{0};
};

// ====== Piece Management Operations ======
void add_white_piece(Chessboard& chessboard, Square square);
void add_black_piece(Chessboard& chessboard, Square square);
void delete_white_piece(Chessboard& chessboard, Square square);
void delete_black_piece(Chessboard& chessboard, Square square);
void update_white_piece(Chessboard& chessboard, Square from, Square to);
void update_black_piece(Chessboard& chessboard, Square from, Square to);

// ====== Board Initialization ======
void reset(Chessboard& chessboard);
void initialize(Chessboard& chessboard);

// ====== Move Registration ======
// Register a move with from/to squares only
void register_move(Chessboard& chessboard, Square from, Square to);

// Register a move with from/to squares and a flag
// If flag is Promotion, generates 4 moves (Queen, Rook, Bishop, Knight)
void register_move(Chessboard& chessboard, Square from, Square to, Flag_Type flag);

// Register a pre-constructed move
void register_move(Chessboard& chessboard, const Move_Type& move);

// Register a tactical move with from/to squares only
void register_tactical_move(Chessboard& chessboard, Square from, Square to);

// Register a tactical move with from/to squares and a flag
// If flag is Promotion, generates 4 moves (Queen, Rook, Bishop, Knight)
void register_tactical_move(Chessboard& chessboard, Square from, Square to, Flag_Type flag);

// Register a pre-constructed tactical move (only if it's tactical)
void register_tactical_move(Chessboard& chessboard, const Move_Type& move);

// ====== Helper Functions ======
[[nodiscard]] inline uint16_t moves_counter(const Chessboard& chessboard) noexcept {
    if (chessboard.ply == 0) {
        return 0;
    }
    return chessboard.move_pointer[chessboard.ply] - chessboard.move_pointer[chessboard.ply - 1];
}

}  // namespace adachess::board

#endif  // ADACHESS_BOARD_CHESSBOARD_HPP

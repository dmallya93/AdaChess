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

#ifndef ADACHESS_BOARD_MOVE_HPP
#define ADACHESS_BOARD_MOVE_HPP

#include "adachess/core/types.hpp"
#include <cstdint>

namespace adachess::board {

// Forward declaration for Square type
using Square = uint8_t;

// ====== Move Flag Types ======

// Helps to output moves in the correct way while using the algebraic notation
enum class Ambiguous_Flag_Type : uint8_t {
    Ambiguous_None = 0,
    Ambiguous_File = 1,
    Ambiguous_Rank = 2,
    Ambiguous_Both = 3
};

// Flags are information that will help the move generator to perform some operations faster
enum class Flag_Type : uint8_t {
    Standard_Move = 0,
    Pawn_Move_Two_Square = 1,
    Capture_En_Passant = 2,
    Promotion = 3,
    Castle = 4,
    Null_Move = 5,
    No_Move = 6
};

// Castle types for specific castle moves
enum class Castle_Type : uint8_t {
    No_Castle = 0,
    White_Kingside = 1,
    White_Queenside = 2,
    Black_Kingside = 3,
    Black_Queenside = 4
};

// Check types that can appear in a game
enum class Check_Type : uint8_t {
    No_Check = 0,
    Direct_Check = 1,
    Discovery_Check = 2,
    Double_Check = 3,
    Checkmate = 4,
    Unknown_Check = 5
};

// Helper operator: treat Check_Type as boolean (No_Check -> true, others -> false)
[[nodiscard]] constexpr inline bool operator!(Check_Type check) noexcept {
    return check == Check_Type::No_Check;
}

// ====== Move Structure ======

// This is the move description in the engine.
// Instances of this struct will be used more than other structured types in AdaChess.
// The smaller this struct, the faster the engine computation.
struct Move_Type {
    adachess::core::Piece piece{adachess::core::Piece::Empty};
    adachess::core::Piece captured{adachess::core::Piece::Empty};
    Square from{0};
    Square to{0};
    Flag_Type flag{Flag_Type::No_Move};
    adachess::core::Piece promotion{adachess::core::Piece::Empty};
    Check_Type check{Check_Type::No_Check};
    Ambiguous_Flag_Type ambiguous_flag{Ambiguous_Flag_Type::Ambiguous_None};
};

// Empty move constant
constexpr Move_Type Empty_Move{
    adachess::core::Piece::Empty,              // piece
    adachess::core::Piece::Empty,              // captured
    0,                                          // from
    0,                                          // to
    Flag_Type::No_Move,                        // flag
    adachess::core::Piece::Empty,              // promotion
    Check_Type::No_Check,                      // check
    Ambiguous_Flag_Type::Ambiguous_None        // ambiguous_flag
};

// ====== Move Comparison Operators ======

[[nodiscard]] inline bool operator==(const Move_Type& left, const Move_Type& right) noexcept {
    return left.from == right.from &&
           left.to == right.to &&
           left.piece == right.piece &&
           left.captured == right.captured &&
           left.promotion == right.promotion &&
           left.check == right.check &&
           left.flag == right.flag;
}

[[nodiscard]] inline bool operator!=(const Move_Type& left, const Move_Type& right) noexcept {
    return !(left == right);
}

// ====== Move Classification Helpers ======

[[nodiscard]] inline bool move_is_capture(const Move_Type& move) noexcept {
    return move.captured != adachess::core::Piece::Empty ||
           move.flag == Flag_Type::Capture_En_Passant;
}

[[nodiscard]] inline bool move_is_tactical(const Move_Type& move) noexcept {
    return move.check != Check_Type::No_Check ||
           move.captured != adachess::core::Piece::Empty ||
           move.flag == Flag_Type::Capture_En_Passant ||
           move.promotion != adachess::core::Piece::Empty ||
           move.flag == Flag_Type::Castle;
}

[[nodiscard]] inline bool move_is_quiet(const Move_Type& move) noexcept {
    return !move_is_tactical(move);
}

// ====== Move Annotation Types ======

enum class Annotation_Type : uint8_t {
    None = 0,
    Blunder = 1,
    Mistake = 2,
    Dubious = 3,       // Note: Ada source has typo "Dobious"
    Interesting = 4,   // A move that leads to a small improvement
    Good = 5,          // A move that brings valuable advantages
    Brilliant = 6      // Amazing move!
};

struct Annotated_Move_Type {
    Move_Type move{Empty_Move};
    Annotation_Type annotation{Annotation_Type::None};
};

// Empty annotated move constant
constexpr Annotated_Move_Type Empty_Annotated_Move{
    Empty_Move,                // move
    Annotation_Type::None      // annotation
};

// ====== Annotated Move Comparison Operators ======

[[nodiscard]] inline bool operator==(const Annotated_Move_Type& left,
                                      const Annotated_Move_Type& right) noexcept {
    return left.move == right.move;
}

[[nodiscard]] inline bool operator!=(const Annotated_Move_Type& left,
                                      const Annotated_Move_Type& right) noexcept {
    return !(left == right);
}

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

// ====== Move Registration Functions ======

// Forward declaration of Chessboard for Register_Move functions
struct Chessboard;

// Register a move with just from/to squares
void register_move(Chessboard& chessboard, Square from, Square to);

// Register a move with from/to squares and flag
// Note: If flag is Promotion, this will generate 4 moves (one for each promotion piece)
void register_move(Chessboard& chessboard, Square from, Square to, Flag_Type flag);

// Register a pre-constructed move
void register_move(Chessboard& chessboard, const Move_Type& move);

// Register a tactical move with just from/to squares (filters non-tactical)
void register_tactical_move(Chessboard& chessboard, Square from, Square to);

// Register a tactical move with from/to squares and flag (filters non-tactical)
void register_tactical_move(Chessboard& chessboard, Square from, Square to, Flag_Type flag);

// Register a pre-constructed tactical move (filters non-tactical)
void register_tactical_move(Chessboard& chessboard, const Move_Type& move);

}  // namespace adachess::board

#endif  // ADACHESS_BOARD_MOVE_HPP

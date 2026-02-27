// ---------------------------------------------------------------------------
// AdaChess – Board representation (10x12 mailbox)
// Translated from: chess-board.ads
// ---------------------------------------------------------------------------
#pragma once

#include <array>
#include <cstdint>
#include <string_view>
#include "chess/colors.hpp"
#include "chess/pieces.hpp"

namespace chess::board {

// -----------------------------------------------------------------------
// Square_Type: 0..119 index into the 10x12 mailbox board.
// Ada: subtype Square_Type is Natural range 0 .. 119;
// Design Decision #2: plain int with constexpr constants.
// -----------------------------------------------------------------------
using Square = int;
inline constexpr int Board_Size = 120;

// -----------------------------------------------------------------------
// Board_Type: Array of 120 Piece values.
// Ada: type Board_Type is array (Square_Type'Range) of Piece_Type;
// -----------------------------------------------------------------------
using Board = std::array<Piece, Board_Size>;

// -----------------------------------------------------------------------
// Square constants (A1–H8) — exact Ada values.
// The 10x12 layout:
//   x x x x x x x x x x     (indices   0– 9, row 0 = frame)
//   x x x x x x x x x x     (indices  10–19, row 1 = frame)
//   x a8 b8 ... h8 x          (indices  20–29, row 2 = rank 8)
//   ...
//   x a1 b1 ... h1 x          (indices  90–99, row 9 = rank 1)
//   x x x x x x x x x x     (indices 100–109, row 10 = frame)
//   x x x x x x x x x x     (indices 110–119, row 11 = frame)
// -----------------------------------------------------------------------

// Rank 8
inline constexpr Square A8 = 21, B8 = 22, C8 = 23, D8 = 24;
inline constexpr Square E8 = 25, F8 = 26, G8 = 27, H8 = 28;
// Rank 7
inline constexpr Square A7 = 31, B7 = 32, C7 = 33, D7 = 34;
inline constexpr Square E7 = 35, F7 = 36, G7 = 37, H7 = 38;
// Rank 6
inline constexpr Square A6 = 41, B6 = 42, C6 = 43, D6 = 44;
inline constexpr Square E6 = 45, F6 = 46, G6 = 47, H6 = 48;
// Rank 5
inline constexpr Square A5 = 51, B5 = 52, C5 = 53, D5 = 54;
inline constexpr Square E5 = 55, F5 = 56, G5 = 57, H5 = 58;
// Rank 4
inline constexpr Square A4 = 61, B4 = 62, C4 = 63, D4 = 64;
inline constexpr Square E4 = 65, F4 = 66, G4 = 67, H4 = 68;
// Rank 3
inline constexpr Square A3 = 71, B3 = 72, C3 = 73, D3 = 74;
inline constexpr Square E3 = 75, F3 = 76, G3 = 77, H3 = 78;
// Rank 2
inline constexpr Square A2 = 81, B2 = 82, C2 = 83, D2 = 84;
inline constexpr Square E2 = 85, F2 = 86, G2 = 87, H2 = 88;
// Rank 1
inline constexpr Square A1 = 91, B1 = 92, C1 = 93, D1 = 94;
inline constexpr Square E1 = 95, F1 = 96, G1 = 97, H1 = 98;

/// Special value for "no en passant target".
inline constexpr Square No_En_Passant = 0;

// -----------------------------------------------------------------------
// Frame detection
// Ada: Square_Is_Frame : Square_Flag_Type
// -----------------------------------------------------------------------
inline constexpr std::array<bool, Board_Size> Square_Is_Frame = {{
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  false, false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, false, true,
    true,  false, false, false, false, false, false, false, false, true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true,
    true,  true,  true,  true,  true,  true,  true,  true,  true,  true
}};

// -----------------------------------------------------------------------
// Coordinate_Type: Integer range -1..14
// -----------------------------------------------------------------------
using Coordinate = int;

/// Extract the file of the given square (0-based: A=0 .. H=7).
[[nodiscard]] Coordinate file_of(Square sq);
/// Extract the rank of the given square (1-based: 1..8).
[[nodiscard]] Coordinate rank_of(Square sq);
/// Extract the diagonal of the given square.
[[nodiscard]] Coordinate diagonal_of(Square sq);
/// Extract the anti-diagonal of the given square.
[[nodiscard]] Coordinate anti_diagonal_of(Square sq);

// Named rank/file constants
inline constexpr Coordinate Rank_1 = 1, Rank_2 = 2, Rank_3 = 3, Rank_4 = 4;
inline constexpr Coordinate Rank_5 = 5, Rank_6 = 6, Rank_7 = 7, Rank_8 = 8;
inline constexpr Coordinate File_A = 0, File_B = 1, File_C = 2, File_D = 3;
inline constexpr Coordinate File_E = 4, File_F = 5, File_G = 6, File_H = 7;

// -----------------------------------------------------------------------
// Square color lookup
// Ada: Color_Board : constant Color_Square_Table_Type
// -----------------------------------------------------------------------
inline constexpr std::array<Color, Board_Size> Color_Board = {{
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black,
    Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White, Color::Black, Color::White
}};

// -----------------------------------------------------------------------
// Square name table — maps each square index to its human-readable name.
// Ada: Pc_Sqr : constant Square_String_Representation_Type
// -----------------------------------------------------------------------
inline constexpr std::array<const char*, Board_Size> Pc_Sqr = {{
    "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ",
    "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ",
    "  ", "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", "  ",
    "  ", "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7", "  ",
    "  ", "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6", "  ",
    "  ", "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5", "  ",
    "  ", "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4", "  ",
    "  ", "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3", "  ",
    "  ", "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2", "  ",
    "  ", "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", "  ",
    "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ",
    "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  ", "  "
}};

} // namespace chess::board

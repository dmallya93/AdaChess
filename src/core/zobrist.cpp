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

#include "adachess/core/zobrist.hpp"
#include "adachess/board/chessboard.hpp"
#include <random>

namespace adachess::core {

// ====== Global Zobrist Hash Tables ======

std::array<std::array<uint64_t, 120>, 14> hash_pieces{};
std::array<uint64_t, 120> hash_en_passant{};
std::array<uint64_t, 4> hash_castle{};
uint64_t hash_side{0};

// ====== Hash Initialization ======

void initialize_hash(board::Chessboard& chessboard) {
    // Use fixed seed for deterministic, reproducible hash generation
    // This matches Ada's Reset(Seed) behavior
    constexpr uint64_t FIXED_SEED = 12345;
    std::mt19937_64 rng(FIXED_SEED);

    // Initialize hash table for pieces on squares
    // For each piece type (0-13) and each square (0-119)
    for (auto& piece_row : hash_pieces) {
        for (auto& hash_value : piece_row) {
            hash_value = rng();
        }
    }

    // Initialize hash table for en passant squares
    for (auto& hash_value : hash_en_passant) {
        hash_value = rng();
    }

    // Initialize hash table for castling rights
    // Order: [0]=Black Queenside, [1]=Black Kingside, [2]=White Queenside, [3]=White Kingside
    for (auto& hash_value : hash_castle) {
        hash_value = rng();
    }

    // Initialize hash value for side to move
    hash_side = rng();

    // Compute initial hash for the current board position
    update_hash(chessboard);
}

// ====== Hash Update ======

void update_hash(board::Chessboard& chessboard) {
    // Reset hash to 0
    uint64_t hash = 0;

    // Hash all white pieces
    // Iterate through white pieces list (indices 1-16)
    for (uint8_t i = 1; i <= chessboard.white_pieces_counter; ++i) {
        const uint8_t square = chessboard.pieces_list[i];
        if (square != 0) {
            const Piece piece = chessboard.board[square];
            hash ^= hash_pieces[static_cast<uint8_t>(piece)][square];
        }
    }

    // Hash all black pieces
    // Iterate through black pieces list (indices 17-32)
    const uint8_t black_pieces_start = 17;
    const uint8_t black_pieces_end = 16 + chessboard.black_pieces_counter;
    for (uint8_t i = black_pieces_start; i <= black_pieces_end; ++i) {
        const uint8_t square = chessboard.pieces_list[i];
        if (square != 0) {
            const Piece piece = chessboard.board[square];
            hash ^= hash_pieces[static_cast<uint8_t>(piece)][square];
        }
    }

    // Hash en passant square if available
    // Use history_ply to get current en passant square
    const uint8_t en_passant_square = chessboard.en_passant_square[chessboard.history_ply];
    if (en_passant_square != 0) {
        hash ^= hash_en_passant[en_passant_square];
    }

    // Hash castling rights
    // Check each castle right individually and XOR in if available
    // Use history_ply to get current castle rights
    if (chessboard.white_castle_kingside[chessboard.history_ply]) {
        hash ^= hash_castle[3];  // White Kingside
    }
    if (chessboard.white_castle_queenside[chessboard.history_ply]) {
        hash ^= hash_castle[2];  // White Queenside
    }
    if (chessboard.black_castle_kingside[chessboard.history_ply]) {
        hash ^= hash_castle[1];  // Black Kingside
    }
    if (chessboard.black_castle_queenside[chessboard.history_ply]) {
        hash ^= hash_castle[0];  // Black Queenside
    }

    // Hash side to move
    // XOR in hash_side if Black to move (White = 0, no XOR needed)
    if (chessboard.side_to_move == Color::Black) {
        hash ^= hash_side;
    }

    // Store computed hash in chessboard
    chessboard.zobrist_hash = hash;
}

}  // namespace adachess::core

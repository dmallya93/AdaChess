/**
 * AdaChess - Smart Chess Engine
 *
 * Board representation implementation.
 * C++ port of the original Ada implementation.
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/board/board.hpp"

#include <stdexcept>

namespace chess::board {

Square square_from_string(const std::string& str) {
    if (str.length() != 2) {
        throw std::invalid_argument("Invalid square string: must be 2 characters");
    }

    const char file_char = str[0];
    const char rank_char = str[1];

    // Validate file (a-h)
    if (file_char < 'a' || file_char > 'h') {
        throw std::invalid_argument("Invalid square string: file must be a-h");
    }

    // Validate rank (1-8)
    if (rank_char < '1' || rank_char > '8') {
        throw std::invalid_argument("Invalid square string: rank must be 1-8");
    }

    // Calculate the square index
    // In the 10x12 board:
    // - Rank 8 starts at index 21 (a8=21, b8=22, ..., h8=28)
    // - Each rank is 10 indices apart
    // - Rank 1 starts at index 91 (a1=91, b1=92, ..., h1=98)
    //
    // Formula: index = 91 - (rank - 1) * 10 + (file - 'a')
    // Where rank is 1-8 (with rank 1 at row index 9 from top)
    // and file is 0-7

    const int file_index = file_char - 'a';              // 0-7
    const int rank_index = rank_char - '1';              // 0-7 (rank 1 = 0, rank 8 = 7)

    // Rank 1 is at row 9 (indices 91-98), rank 8 is at row 2 (indices 21-28)
    // So: row = 9 - rank_index = 9 - (rank - 1)
    // And: index = row * 10 + 1 + file_index
    //            = (9 - rank_index) * 10 + 1 + file_index
    //            = 91 - rank_index * 10 + file_index

    const int index = 91 - rank_index * 10 + file_index;

    return Square(static_cast<std::uint8_t>(index));
}

} // namespace chess::board

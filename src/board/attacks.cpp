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

#include "adachess/board/attacks.hpp"

namespace adachess::board {

// Global attacks_to array (extern declaration in header)
std::array<AttackFunction, DIRECTION_ARRAY_SIZE> attacks_to;

void initialize_attacks_to() {
    // Initialize all entries to placeholder
    attacks_to.fill(attacks_from_placeholder);
    
    // Map valid directions to their attack functions
    // Index formula: direction + DIRECTION_OFFSET
    
    // Orthogonal directions (rook/queen)
    attacks_to[NORTH + DIRECTION_OFFSET] = attacks_from_north;
    attacks_to[SOUTH + DIRECTION_OFFSET] = attacks_from_south;
    attacks_to[EAST + DIRECTION_OFFSET] = attacks_from_east;
    attacks_to[WEST + DIRECTION_OFFSET] = attacks_from_west;
    
    // Diagonal directions (bishop/queen)
    attacks_to[NORTH_EAST + DIRECTION_OFFSET] = attacks_from_north_east;
    attacks_to[NORTH_WEST + DIRECTION_OFFSET] = attacks_from_north_west;
    attacks_to[SOUTH_EAST + DIRECTION_OFFSET] = attacks_from_south_east;
    attacks_to[SOUTH_WEST + DIRECTION_OFFSET] = attacks_from_south_west;
    
    // Knight directions
    attacks_to[NORTH_NORTH_EAST + DIRECTION_OFFSET] = attacks_from_north_north_east;
    attacks_to[NORTH_EAST_EAST + DIRECTION_OFFSET] = attacks_from_north_east_east;
    attacks_to[SOUTH_EAST_EAST + DIRECTION_OFFSET] = attacks_from_south_east_east;
    attacks_to[SOUTH_SOUTH_EAST + DIRECTION_OFFSET] = attacks_from_south_south_east;
    attacks_to[SOUTH_SOUTH_WEST + DIRECTION_OFFSET] = attacks_from_south_south_west;
    attacks_to[SOUTH_WEST_WEST + DIRECTION_OFFSET] = attacks_from_south_west_west;
    attacks_to[NORTH_WEST_WEST + DIRECTION_OFFSET] = attacks_from_north_west_west;
    attacks_to[NORTH_NORTH_WEST + DIRECTION_OFFSET] = attacks_from_north_north_west;
}

}  // namespace adachess::board

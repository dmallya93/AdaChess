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
//

#pragma once

#include <cstdint>

namespace adachess {

/// Color type representing White or Black side
/// Uses 2 bits to match Ada's representation
enum class Color : std::uint8_t {
  White = 0,
  Black = 1
};

/// Swap colors (Ada's "not" operator)
/// Returns Black if White, White if Black
[[nodiscard]] constexpr Color opposite(Color color) noexcept {
  return (color == Color::White) ? Color::Black : Color::White;
}

/// Alternative name for opposite function
[[nodiscard]] constexpr Color swap_color(Color color) noexcept {
  return opposite(color);
}

}  // namespace adachess

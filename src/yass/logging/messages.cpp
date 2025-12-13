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

#include "yass/logging/messages.hpp"

#include <iostream>

namespace yass::logging {

// Implementation of logging module with no-throw guarantee.
// See DESIGN_DECISIONS.md for the overall error handling strategy (Design Decision #4).
// This module provides console output for user feedback and diagnostics. All operations
// are marked noexcept to ensure logging never throws exceptions, maintaining the no-throw
// guarantee level of exception safety.

namespace {
// ANSI escape codes for colored terminal output
constexpr std::string_view ANSI_RESET = "\033[0m";
constexpr std::string_view ANSI_RED = "\033[31m";   // For errors
constexpr std::string_view ANSI_GREEN = "\033[32m"; // For success
} // namespace

void show_message(std::string_view message, message_type type) noexcept {
  // Select appropriate color code based on message type
  std::string_view color_code;
  switch (type) {
  case message_type::normal:
    color_code = ""; // No color for normal messages
    break;
  case message_type::error:
    color_code = ANSI_RED;
    break;
  case message_type::success:
    color_code = ANSI_GREEN;
    break;
  }

  // Output the message with color formatting if applicable
  // Format: [color_code]message[reset]newline
  if (!color_code.empty()) {
    std::cout << color_code;
  }
  std::cout << message;
  if (!color_code.empty()) {
    std::cout << ANSI_RESET;
  }
  std::cout << '\n';
}

} // namespace yass::logging

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

#ifndef YASS_LOGGING_MESSAGES_HPP
#define YASS_LOGGING_MESSAGES_HPP

#include <string_view>

namespace yass::logging {

// This module implements the logging foundation for the chess engine.
//
// Error Handling Strategy (Design Decision #4):
// All functions in this module provide the NO-THROW GUARANTEE - they are marked
// noexcept because logging failures should never interrupt program flow. This
// aligns with the error handling strategy documented in DESIGN_DECISIONS.md
// and implemented in yass/error.hpp, where logging is specifically designated
// as a no-throw operation.

// Message types for colored console output
// Maps to Ada's Messages_Types enum concept (not present in original AdaChess,
// but following the task specification for foundational logging support)
enum class message_type : unsigned char {
  normal, // No special formatting
  error,  // Red color for errors
  success // Green color for success messages
};

// Display a message to the console with optional color formatting
// Uses ANSI escape codes for colored output
//
// Parameters:
//   message - The message to display (passed by string_view to avoid copies)
//   type    - The message type determining color (defaults to error)
//
// Example:
//   show_message("Invalid move", message_type::error);
//   show_message("Search complete", message_type::success);
//   show_message("Info: using 64MB hash", message_type::normal);
void show_message(std::string_view message, message_type type = message_type::error) noexcept;

} // namespace yass::logging

#endif // YASS_LOGGING_MESSAGES_HPP

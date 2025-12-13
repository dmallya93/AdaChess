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

#ifndef YASS_ERROR_HPP
#define YASS_ERROR_HPP

#include <stdexcept>
#include <string>
#include <string_view>

// DESIGN DECISION #4: Error Handling and Exception Strategy
//
// Decision: Mixed approach - C++ exceptions for exceptional cases,
//           return codes/std::optional for expected failures
//
// Rationale:
// 1. Ada uses exceptions for error handling, making a C++ exception-based
//    approach the most natural translation for exceptional conditions
// 2. Chess engine operations have both:
//    - Exceptional errors: invalid FEN, corrupt opening book, memory allocation
//    - failure
//    - Expected failures: invalid move input, search timeout, pondering
//    interrupt
// 3. Mixed approach provides:
//    - Clear separation between programming errors and expected edge cases
//    - Strong exception safety guarantees (RAII ensures cleanup)
//    - Performance: no exception overhead for normal control flow (move
//    legality, search cutoffs)
//    - Maintainability: exceptions document truly exceptional conditions
//
// Guidelines:
// - Use EXCEPTIONS for:
//   * Programming errors (assertion failures, logic bugs)
//   * Resource failures (memory, file I/O, corrupt data)
//   * Unrecoverable errors (invalid FEN format, broken transposition table)
//   * Protocol violations (malformed commands from GUI)
//
// - Use RETURN CODES/std::optional for:
//   * Expected failures (invalid move notation, search stopped early)
//   * Validation results (move legality checks, input parsing)
//   * Optional results (transposition table lookups, opening book queries)
//   * Performance-critical paths where exceptions would add overhead
//
// - Exception Safety Levels:
//   * Basic guarantee: Resources are not leaked, objects are in valid state
//   * Strong guarantee: Operation succeeds or has no effect (rollback)
//   * No-throw guarantee: Operation cannot fail (marked noexcept)
//
// All custom exception types derive from std::exception hierarchy for
// consistent catching and standard what() interface.

namespace yass {

// Base exception class for all YASS-specific exceptions
// Provides consistent error reporting and categorization
class ChessException : public std::runtime_error {
public:
  explicit ChessException(const std::string& message) : std::runtime_error(message) {}
  explicit ChessException(std::string_view message) : std::runtime_error(std::string(message)) {}
};

// Exception for invalid position representations (e.g., malformed FEN)
class InvalidPositionError : public ChessException {
public:
  explicit InvalidPositionError(const std::string& message)
      : ChessException("Invalid position: " + message) {}
};

// Exception for invalid move representations or illegal moves
class InvalidMoveError : public ChessException {
public:
  explicit InvalidMoveError(const std::string& message)
      : ChessException("Invalid move: " + message) {}
};

// Exception for I/O failures (file access, network, etc.)
class IoError : public ChessException {
public:
  explicit IoError(const std::string& message) : ChessException("I/O error: " + message) {}
};

// Exception for protocol violations (malformed commands, invalid sequences)
class ProtocolError : public ChessException {
public:
  explicit ProtocolError(const std::string& message)
      : ChessException("Protocol error: " + message) {}
};

} // namespace yass

#endif // YASS_ERROR_HPP

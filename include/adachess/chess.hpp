/**
 * AdaChess - Smart Chess Engine
 *
 * Root namespace and exception types for the chess engine.
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

#ifndef ADACHESS_CHESS_HPP
#define ADACHESS_CHESS_HPP

#include <stdexcept>
#include <string>

namespace chess {

/**
 * Exception thrown when a feature is not yet implemented.
 * Corresponds to Ada's Not_Implemented exception.
 */
class NotImplemented : public std::logic_error {
public:
    explicit NotImplemented(const std::string& what_arg = "Feature not implemented")
        : std::logic_error(what_arg) {}

    explicit NotImplemented(const char* what_arg)
        : std::logic_error(what_arg) {}
};

} // namespace chess

#endif // ADACHESS_CHESS_HPP

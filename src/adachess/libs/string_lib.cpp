/**
 * AdaChess - Smart Chess Engine
 *
 * String manipulation utilities implementation.
 * C++ port of the original Ada implementation (libs/string_lib/string_lib.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/libs/string_lib.hpp"

#include <algorithm>

namespace string_lib {

std::string trim(std::string_view source, char delimiter, TrimSide side) {
    if (source.empty()) {
        return std::string{};
    }

    std::size_t from = 0;
    std::size_t to = source.length();

    // Trim left side
    if (side == TrimSide::Left || side == TrimSide::Both) {
        while (from < to && source[from] == delimiter) {
            ++from;
        }
    }

    // Trim right side
    if (side == TrimSide::Right || side == TrimSide::Both) {
        while (to > from && source[to - 1] == delimiter) {
            --to;
        }
    }

    return std::string(source.substr(from, to - from));
}

std::size_t index_of(std::string_view source, char delimiter) {
    for (std::size_t i = 0; i < source.length(); ++i) {
        if (source[i] == delimiter) {
            return i;
        }
    }
    throw DelimiterNotFound();
}

bool has_token(std::string_view source, char delimiter) {
    std::string trimmed = trim(source, delimiter, TrimSide::Both);
    for (char c : trimmed) {
        if (c != delimiter) {
            return true;
        }
    }
    return false;
}

std::size_t count_tokens(std::string_view source, char delimiter) {
    std::size_t tokens = 0;
    std::string current = trim(source, delimiter, TrimSide::Left);

    while (!current.empty()) {
        ++tokens;
        // Find the next delimiter
        std::size_t pos = 0;
        try {
            pos = index_of(current, delimiter);
            // Move past this token and trim leading delimiters
            current = trim(std::string_view(current).substr(pos), delimiter, TrimSide::Left);
        } catch (const DelimiterNotFound&) {
            // No more delimiters, we're done
            break;
        }
    }

    return tokens;
}

std::string extract_token(std::string_view source, char delimiter) {
    std::string input = trim(source, delimiter, TrimSide::Both);

    try {
        std::size_t pos = index_of(input, delimiter);
        return input.substr(0, pos);
    } catch (const DelimiterNotFound&) {
        // When the source has no delimiter, the entire source is the token
        return input;
    }
}

std::string extract_last_token(std::string_view source, char delimiter) {
    std::string input = trim(source, delimiter, TrimSide::Both);

    // Search from the end
    for (std::size_t i = input.length(); i > 0; --i) {
        if (input[i - 1] == delimiter) {
            return input.substr(i);
        }
    }

    // No delimiter found, the entire string is the token
    return input;
}

std::string extract_token_at(std::string_view source, std::size_t token_number, char delimiter) {
    if (token_number == 0) {
        throw NotEnoughTokens("Token number must be >= 1");
    }

    if (count_tokens(source, delimiter) < token_number) {
        throw NotEnoughTokens();
    }

    std::size_t tokens = 0;
    std::string current = trim(source, delimiter, TrimSide::Left);

    while (!current.empty()) {
        std::string token = extract_token(current, delimiter);
        ++tokens;

        if (tokens == token_number) {
            return token;
        }

        // Move past this token
        if (token.length() < current.length()) {
            current = trim(std::string_view(current).substr(token.length()),
                          delimiter, TrimSide::Left);
        } else {
            break;
        }
    }

    // Should never reach here due to the count check at the start
    throw TokenNotFound();
}

void replace(std::string& source, char from, char to) {
    for (char& c : source) {
        if (c == from) {
            c = to;
        }
    }
}

} // namespace string_lib

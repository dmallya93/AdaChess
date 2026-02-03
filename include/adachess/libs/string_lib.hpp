/**
 * AdaChess - Smart Chess Engine
 *
 * String manipulation utilities.
 * C++ port of the original Ada implementation (libs/string_lib/string_lib.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_LIBS_STRING_LIB_HPP
#define ADACHESS_LIBS_STRING_LIB_HPP

#include <string>
#include <string_view>
#include <stdexcept>
#include <cstddef>

namespace string_lib {

// ============================================================================
// Common Placeholders
// ============================================================================

/**
 * Whitespace character constant.
 */
inline constexpr char kWhitespace = ' ';

/**
 * Pipe character constant.
 */
inline constexpr char kPipe = '|';

/**
 * Semicolon character constant.
 */
inline constexpr char kSemicolon = ';';

/**
 * Dash character constant.
 */
inline constexpr char kDash = '-';

/**
 * Empty string constant.
 */
inline const std::string kEmptyString = "";

// ============================================================================
// Exceptions
// ============================================================================

/**
 * Exception thrown when a delimiter is not found in a string.
 */
class DelimiterNotFound : public std::runtime_error {
public:
    explicit DelimiterNotFound(const std::string& msg = "Delimiter not found")
        : std::runtime_error(msg) {}
};

/**
 * Exception thrown when a token is not found in a string.
 */
class TokenNotFound : public std::runtime_error {
public:
    explicit TokenNotFound(const std::string& msg = "Token not found")
        : std::runtime_error(msg) {}
};

/**
 * Exception thrown when there are not enough tokens in a string.
 */
class NotEnoughTokens : public std::runtime_error {
public:
    explicit NotEnoughTokens(const std::string& msg = "Not enough tokens")
        : std::runtime_error(msg) {}
};

// ============================================================================
// Trim Types
// ============================================================================

/**
 * Specifies which side(s) of a string to trim.
 */
enum class TrimSide {
    Left,   ///< Trim from the left (beginning) only
    Right,  ///< Trim from the right (end) only
    Both    ///< Trim from both sides
};

// ============================================================================
// String Utility Functions
// ============================================================================

/**
 * Count the number of tokens in a string separated by a delimiter.
 * Multiple consecutive delimiters are treated as a single delimiter.
 * An empty input or input with only delimiters returns 0.
 * An input with no delimiter is a single token.
 *
 * @param source The string to parse.
 * @param delimiter The character that separates tokens.
 * @return The number of tokens found.
 */
[[nodiscard]] std::size_t count_tokens(std::string_view source, char delimiter);

/**
 * Check if the source string contains at least one token.
 * A token is a non-delimiter sequence.
 *
 * @param source The input string to parse.
 * @param delimiter The delimiter character (default: whitespace).
 * @return True if at least one token exists, false otherwise.
 */
[[nodiscard]] bool has_token(std::string_view source, char delimiter = kWhitespace);

/**
 * Extract the first token from a string.
 * The input is trimmed before extraction. If no delimiter is found,
 * the entire trimmed string is returned as the token.
 *
 * @param source The input string to parse.
 * @param delimiter The character that delimits tokens (default: whitespace).
 * @return The first token found.
 * @pre has_token(source, delimiter) must be true.
 * @post Result length > 0.
 */
[[nodiscard]] std::string extract_token(std::string_view source, char delimiter = kWhitespace);

/**
 * Extract the last token from a string.
 * Similar to extract_token but searches from the end.
 *
 * @param source The input string to parse.
 * @param delimiter The character that delimits tokens (default: whitespace).
 * @return The last token found.
 * @pre has_token(source, delimiter) must be true.
 * @post Result length > 0.
 */
[[nodiscard]] std::string extract_last_token(std::string_view source, char delimiter = kWhitespace);

/**
 * Extract the Nth token from a string.
 * Tokens are numbered starting from 1.
 *
 * @param source The input string to be parsed.
 * @param token_number The 1-based index of the token to extract.
 * @param delimiter The character that separates tokens.
 * @return The token at the specified position.
 * @throws NotEnoughTokens if token_number exceeds the token count.
 */
[[nodiscard]] std::string extract_token_at(std::string_view source,
                                            std::size_t token_number,
                                            char delimiter);

/**
 * Remove delimiter characters from the specified side(s) of a string.
 *
 * @param source The input string to trim.
 * @param delimiter The character to remove (default: whitespace).
 * @param side Which side(s) to trim (default: Both).
 * @return The trimmed string.
 */
[[nodiscard]] std::string trim(std::string_view source,
                               char delimiter = kWhitespace,
                               TrimSide side = TrimSide::Both);

/**
 * Find the index of the first occurrence of a delimiter in a string.
 *
 * @param source The input string to search.
 * @param delimiter The character to find.
 * @return The 0-based index of the first delimiter found.
 * @throws DelimiterNotFound if the delimiter is not found.
 */
[[nodiscard]] std::size_t index_of(std::string_view source, char delimiter);

/**
 * Replace all occurrences of a character with another character.
 *
 * @param source The string to modify (in place).
 * @param from The character to replace.
 * @param to The replacement character.
 */
void replace(std::string& source, char from, char to);

/**
 * Check if a string is empty or contains only whitespace.
 *
 * @param source The string to test.
 * @return True if the string is empty or contains only spaces.
 */
[[nodiscard]] inline bool is_empty(std::string_view source) noexcept {
    return trim(source, kWhitespace, TrimSide::Both).empty();
}

} // namespace string_lib

#endif // ADACHESS_LIBS_STRING_LIB_HPP

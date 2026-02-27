// ---------------------------------------------------------------------------
// AdaChess – String utility library
// Translated from: string_lib.ads
// ---------------------------------------------------------------------------
#pragma once

#include <string>
#include <string_view>
#include <stdexcept>

namespace util::strings {

// -----------------------------------------------------------------------
// Common delimiter constants
// -----------------------------------------------------------------------
inline constexpr char Whitespace = ' ';
inline constexpr char Pipe       = '|';
inline constexpr char Semicolon  = ';';
inline constexpr char Dash       = '-';

inline const std::string Empty_String; // NOLINT

// -----------------------------------------------------------------------
// Trim side
// -----------------------------------------------------------------------
enum class TrimSide { Left, Right, Both };

// -----------------------------------------------------------------------
// Exception types
// -----------------------------------------------------------------------
class DelimiterNotFound : public std::runtime_error {
public:
    DelimiterNotFound() : std::runtime_error("Delimiter not found") {}
};

class TokenNotFound : public std::runtime_error {
public:
    TokenNotFound() : std::runtime_error("Token not found") {}
};

class NotEnoughTokens : public std::runtime_error {
public:
    NotEnoughTokens() : std::runtime_error("Not enough tokens") {}
};

// -----------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------

/// Count the number of tokens separated by delimiter.
/// Multiple consecutive delimiters are treated as a single separator.
[[nodiscard]] int count_tokens(std::string_view source, char delimiter);

/// Returns true if the source contains at least one non-delimiter token.
[[nodiscard]] bool has_token(std::string_view source, char delimiter = Whitespace);

/// Extract the first token from source. The source is first trimmed of
/// the delimiter on both sides. If no delimiter is found, the entire
/// trimmed string is the token.
[[nodiscard]] std::string extract_token(std::string_view source, char delimiter = Whitespace);

/// Extract the Nth token (1-based) from source.
/// @throws NotEnoughTokens if token_number exceeds the token count.
[[nodiscard]] std::string extract_token_at(std::string_view source, int token_number, char delimiter);

/// Extract the last token from source.
[[nodiscard]] std::string extract_last_token(std::string_view source, char delimiter = Whitespace);

/// Trim delimiters from the left, right, or both sides of source.
[[nodiscard]] std::string_view trim(std::string_view source,
                                     char delimiter = Whitespace,
                                     TrimSide side = TrimSide::Both);

/// Find the index (0-based) of the first occurrence of delimiter.
/// @throws DelimiterNotFound if delimiter is not present.
[[nodiscard]] std::size_t index_of(std::string_view source, char delimiter);

/// Replace all occurrences of ch with replace_with in source.
void replace(std::string& source, char ch, char replace_with);

/// Returns true if the trimmed source is empty.
[[nodiscard]] bool is_empty(std::string_view source);

} // namespace util::strings

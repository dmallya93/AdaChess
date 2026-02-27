// ---------------------------------------------------------------------------
// AdaChess – String utility library (implementation)
// Translated from: string_lib.adb
// ---------------------------------------------------------------------------
#include "util/string_lib.hpp"

namespace util::strings {

// -----------------------------------------------------------------------
// trim — remove leading/trailing delimiter characters.
// -----------------------------------------------------------------------
std::string_view trim(std::string_view source, char delimiter, TrimSide side) {
    auto from = source.begin();
    auto to   = source.end();

    if (side == TrimSide::Left || side == TrimSide::Both) {
        while (from != to && *from == delimiter) {
            ++from;
        }
    }
    if (side == TrimSide::Right || side == TrimSide::Both) {
        while (to != from && *(to - 1) == delimiter) {
            --to;
        }
    }
    return std::string_view(from, static_cast<std::size_t>(to - from));
}

// -----------------------------------------------------------------------
// index_of — find first occurrence of delimiter.
// -----------------------------------------------------------------------
std::size_t index_of(std::string_view source, char delimiter) {
    for (std::size_t i = 0; i < source.size(); ++i) {
        if (source[i] == delimiter) {
            return i;
        }
    }
    throw DelimiterNotFound();
}

// -----------------------------------------------------------------------
// is_empty — true if trimmed source has length 0.
// -----------------------------------------------------------------------
bool is_empty(std::string_view source) {
    return trim(source, Whitespace, TrimSide::Both).empty();
}

// -----------------------------------------------------------------------
// has_token — true if source contains at least one non-delimiter char.
// -----------------------------------------------------------------------
bool has_token(std::string_view source, char delimiter) {
    auto trimmed = trim(source, delimiter, TrimSide::Both);
    for (char c : trimmed) {
        if (c != delimiter) return true;
    }
    return false;
}

// -----------------------------------------------------------------------
// count_tokens — count delimiter-separated tokens (consecutive delims
// are treated as one).
// -----------------------------------------------------------------------
int count_tokens(std::string_view source, char delimiter) {
    int tokens = 0;
    auto sv = source;

    while (true) {
        // Trim leading delimiters
        sv = trim(sv, delimiter, TrimSide::Left);
        if (sv.empty()) break;
        tokens++;
        // Find next delimiter
        auto pos = sv.find(delimiter);
        if (pos == std::string_view::npos) break;
        sv = sv.substr(pos);
    }

    return tokens;
}

// -----------------------------------------------------------------------
// extract_token — get first token.
// -----------------------------------------------------------------------
std::string extract_token(std::string_view source, char delimiter) {
    auto input = trim(source, delimiter, TrimSide::Both);
    try {
        auto pos = index_of(input, delimiter);
        return std::string(input.substr(0, pos));
    } catch (const DelimiterNotFound&) {
        // When source has no delimiter, the entire source is the token.
        return std::string(input);
    }
}

// -----------------------------------------------------------------------
// extract_last_token — get last token.
// -----------------------------------------------------------------------
std::string extract_last_token(std::string_view source, char delimiter) {
    auto input = trim(source, delimiter, TrimSide::Both);
    for (auto i = input.size(); i > 0; --i) {
        if (input[i - 1] == delimiter) {
            return std::string(input.substr(i));
        }
    }
    // No delimiter found — the entire input is the only token.
    return std::string(input);
}

// -----------------------------------------------------------------------
// extract_token_at — get the Nth token (1-based).
// -----------------------------------------------------------------------
std::string extract_token_at(std::string_view source, int token_number, char delimiter) {
    if (count_tokens(source, delimiter) < token_number) {
        throw NotEnoughTokens();
    }

    auto sv = source;
    int tokens = 0;

    while (true) {
        sv = trim(sv, delimiter, TrimSide::Left);
        if (sv.empty()) break;

        auto tok = extract_token(sv, delimiter);
        tokens++;
        if (tokens == token_number) {
            return tok;
        }
        // Advance past this token
        sv = sv.substr(tok.size());
    }

    // Should not reach here due to count check above.
    throw TokenNotFound();
}

// -----------------------------------------------------------------------
// replace — replace all occurrences of ch with replace_with.
// -----------------------------------------------------------------------
void replace(std::string& source, char ch, char replace_with) {
    for (auto& c : source) {
        if (c == ch) {
            c = replace_with;
        }
    }
}

} // namespace util::strings

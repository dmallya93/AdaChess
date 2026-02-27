// ---------------------------------------------------------------------------
// AdaChess – Unit tests for string_lib utilities
// ---------------------------------------------------------------------------
#include <gtest/gtest.h>

#include "util/string_lib.hpp"

using namespace util::strings;

// -----------------------------------------------------------------------
// trim tests
// -----------------------------------------------------------------------
TEST(StringLibTest, TrimBoth) {
    EXPECT_EQ(trim("  hello  "), "hello");
}

TEST(StringLibTest, TrimLeft) {
    EXPECT_EQ(trim("  hello  ", ' ', TrimSide::Left), "hello  ");
}

TEST(StringLibTest, TrimRight) {
    EXPECT_EQ(trim("  hello  ", ' ', TrimSide::Right), "  hello");
}

TEST(StringLibTest, TrimEmpty) {
    EXPECT_EQ(trim("   "), "");
}

TEST(StringLibTest, TrimNoOp) {
    EXPECT_EQ(trim("hello"), "hello");
}

TEST(StringLibTest, TrimCustomDelimiter) {
    EXPECT_EQ(trim("--hello--", '-'), "hello");
}

// -----------------------------------------------------------------------
// is_empty tests
// -----------------------------------------------------------------------
TEST(StringLibTest, IsEmpty) {
    EXPECT_TRUE(is_empty(""));
    EXPECT_TRUE(is_empty("   "));
    EXPECT_FALSE(is_empty("hello"));
    EXPECT_FALSE(is_empty("  x  "));
}

// -----------------------------------------------------------------------
// has_token tests
// -----------------------------------------------------------------------
TEST(StringLibTest, HasToken) {
    EXPECT_TRUE(has_token("hello world"));
    EXPECT_TRUE(has_token("  hello  "));
    EXPECT_FALSE(has_token("   "));
    EXPECT_FALSE(has_token(""));
}

// -----------------------------------------------------------------------
// count_tokens tests
// -----------------------------------------------------------------------
TEST(StringLibTest, CountTokens) {
    EXPECT_EQ(count_tokens("hello world", ' '), 2);
    EXPECT_EQ(count_tokens("  hello  world  ", ' '), 2);
    EXPECT_EQ(count_tokens("hello", ' '), 1);
    EXPECT_EQ(count_tokens("   ", ' '), 0);
    EXPECT_EQ(count_tokens("a b c d", ' '), 4);
}

// -----------------------------------------------------------------------
// extract_token tests
// -----------------------------------------------------------------------
TEST(StringLibTest, ExtractToken) {
    EXPECT_EQ(extract_token("hello world"), "hello");
    EXPECT_EQ(extract_token("  hello  world  "), "hello");
    EXPECT_EQ(extract_token("single"), "single");
}

// -----------------------------------------------------------------------
// extract_last_token tests
// -----------------------------------------------------------------------
TEST(StringLibTest, ExtractLastToken) {
    EXPECT_EQ(extract_last_token("hello world"), "world");
    EXPECT_EQ(extract_last_token("  hello  world  "), "world");
    EXPECT_EQ(extract_last_token("single"), "single");
}

// -----------------------------------------------------------------------
// extract_token_at tests
// -----------------------------------------------------------------------
TEST(StringLibTest, ExtractTokenAt) {
    EXPECT_EQ(extract_token_at("a b c d", 1, ' '), "a");
    EXPECT_EQ(extract_token_at("a b c d", 2, ' '), "b");
    EXPECT_EQ(extract_token_at("a b c d", 3, ' '), "c");
    EXPECT_EQ(extract_token_at("a b c d", 4, ' '), "d");
}

TEST(StringLibTest, ExtractTokenAtThrows) {
    EXPECT_THROW(extract_token_at("a b", 3, ' '), NotEnoughTokens);
}

// -----------------------------------------------------------------------
// index_of tests
// -----------------------------------------------------------------------
TEST(StringLibTest, IndexOf) {
    EXPECT_EQ(index_of("hello world", ' '), 5u);
    EXPECT_EQ(index_of("abc", 'b'), 1u);
}

TEST(StringLibTest, IndexOfThrows) {
    EXPECT_THROW((void)index_of("hello", 'x'), DelimiterNotFound);
}

// -----------------------------------------------------------------------
// replace tests
// -----------------------------------------------------------------------
TEST(StringLibTest, Replace) {
    std::string s = "a-b-c";
    replace(s, '-', '_');
    EXPECT_EQ(s, "a_b_c");
}

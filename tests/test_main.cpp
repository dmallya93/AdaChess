#include <gtest/gtest.h>

// Placeholder test to verify Google Test integration
TEST(PlaceholderTest, BasicAssertion) {
  EXPECT_EQ(1 + 1, 2);
  EXPECT_TRUE(true);
}

// Test that demonstrates test framework is working
TEST(PlaceholderTest, StringComparison) {
  std::string expected = "AdaChess";
  std::string actual = "AdaChess";
  EXPECT_EQ(expected, actual);
}

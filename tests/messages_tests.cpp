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

#include <gtest/gtest.h>

#include <sstream>
#include <string>

namespace yass::logging::test {

// Helper class to capture stdout for testing
class StdoutCapture {
public:
  StdoutCapture() : old_buf_(std::cout.rdbuf(buffer_.rdbuf())) {}

  ~StdoutCapture() { std::cout.rdbuf(old_buf_); }

  StdoutCapture(const StdoutCapture&) = delete;
  StdoutCapture& operator=(const StdoutCapture&) = delete;
  StdoutCapture(StdoutCapture&&) = delete;
  StdoutCapture& operator=(StdoutCapture&&) = delete;

  std::string get_output() { return buffer_.str(); }

  void clear() {
    buffer_.str("");
    buffer_.clear();
  }

private:
  std::stringstream buffer_;
  std::streambuf* old_buf_;
};

// Test that normal message type produces output without ANSI codes
TEST(MessagesTest, NormalMessageNoColorCodes) {
  StdoutCapture capture;

  show_message("Test normal message", message_type::normal);

  std::string output = capture.get_output();
  EXPECT_EQ(output, "Test normal message\n");
  EXPECT_EQ(output.find("\033["), std::string::npos)
      << "Normal messages should not contain ANSI escape codes";
}

// Test that error message type produces red color codes
TEST(MessagesTest, ErrorMessageHasRedColorCode) {
  StdoutCapture capture;

  show_message("Test error message", message_type::error);

  std::string output = capture.get_output();

  // Should contain red ANSI code (ESC[31m)
  EXPECT_NE(output.find("\033[31m"), std::string::npos)
      << "Error messages should contain red color code";

  // Should contain reset code (ESC[0m)
  EXPECT_NE(output.find("\033[0m"), std::string::npos)
      << "Error messages should contain reset code";

  // Should contain the message text
  EXPECT_NE(output.find("Test error message"), std::string::npos);

  // Should end with newline
  EXPECT_EQ(output.back(), '\n');
}

// Test that success message type produces green color codes
TEST(MessagesTest, SuccessMessageHasGreenColorCode) {
  StdoutCapture capture;

  show_message("Test success message", message_type::success);

  std::string output = capture.get_output();

  // Should contain green ANSI code (ESC[32m)
  EXPECT_NE(output.find("\033[32m"), std::string::npos)
      << "Success messages should contain green color code";

  // Should contain reset code (ESC[0m)
  EXPECT_NE(output.find("\033[0m"), std::string::npos)
      << "Success messages should contain reset code";

  // Should contain the message text
  EXPECT_NE(output.find("Test success message"), std::string::npos);

  // Should end with newline
  EXPECT_EQ(output.back(), '\n');
}

// Test that default parameter uses error type (red)
TEST(MessagesTest, DefaultParameterIsError) {
  StdoutCapture capture;

  show_message("Default type message"); // No type specified

  std::string output = capture.get_output();

  // Should contain red ANSI code (ESC[31m) - error is the default
  EXPECT_NE(output.find("\033[31m"), std::string::npos)
      << "Default message type should be error (red)";

  // Should contain reset code (ESC[0m)
  EXPECT_NE(output.find("\033[0m"), std::string::npos);
}

// Test messages with special characters
TEST(MessagesTest, SpecialCharactersHandled) {
  StdoutCapture capture;

  show_message("Message with special chars: !@#$%^&*()", message_type::normal);

  std::string output = capture.get_output();
  EXPECT_NE(output.find("!@#$%^&*()"), std::string::npos);
}

// Test empty message
TEST(MessagesTest, EmptyMessageHandled) {
  StdoutCapture capture;

  show_message("", message_type::normal);

  std::string output = capture.get_output();
  EXPECT_EQ(output, "\n"); // Should just be a newline
}

// Test very long message
TEST(MessagesTest, LongMessageHandled) {
  StdoutCapture capture;

  std::string long_message(1000, 'A');
  show_message(long_message, message_type::normal);

  std::string output = capture.get_output();
  EXPECT_NE(output.find(long_message), std::string::npos);
  EXPECT_EQ(output.back(), '\n');
}

// Test that all message types end with newline
TEST(MessagesTest, AllTypesEndWithNewline) {
  StdoutCapture capture;

  show_message("Normal", message_type::normal);
  std::string output = capture.get_output();
  EXPECT_EQ(output.back(), '\n');
  capture.clear();

  show_message("Error", message_type::error);
  output = capture.get_output();
  EXPECT_EQ(output.back(), '\n');
  capture.clear();

  show_message("Success", message_type::success);
  output = capture.get_output();
  EXPECT_EQ(output.back(), '\n');
}

// Test message ordering
TEST(MessagesTest, MultipleMessagesInOrder) {
  StdoutCapture capture;

  show_message("First", message_type::normal);
  show_message("Second", message_type::error);
  show_message("Third", message_type::success);

  std::string output = capture.get_output();

  // Check that messages appear in order
  size_t first_pos = output.find("First");
  size_t second_pos = output.find("Second");
  size_t third_pos = output.find("Third");

  EXPECT_NE(first_pos, std::string::npos);
  EXPECT_NE(second_pos, std::string::npos);
  EXPECT_NE(third_pos, std::string::npos);

  EXPECT_LT(first_pos, second_pos);
  EXPECT_LT(second_pos, third_pos);
}

} // namespace yass::logging::test

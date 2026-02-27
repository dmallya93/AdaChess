// ---------------------------------------------------------------------------
// AdaChess – Unit tests for protocol command parser
// ---------------------------------------------------------------------------
#include <gtest/gtest.h>

#include "chess/protocols/protocols.hpp"

using namespace chess::protocols;

// -----------------------------------------------------------------------
// Standard command parsing
// -----------------------------------------------------------------------
TEST(ProtocolTest, QuitCommand) {
    EXPECT_EQ(parse_input("quit"), ProtocolCommand::Quit);
    EXPECT_EQ(parse_input("QUIT"), ProtocolCommand::Quit);
    EXPECT_EQ(parse_input("Quit"), ProtocolCommand::Quit);
}

TEST(ProtocolTest, GoCommand) {
    EXPECT_EQ(parse_input("go"), ProtocolCommand::Go);
    EXPECT_EQ(parse_input("GO"), ProtocolCommand::Go);
}

TEST(ProtocolTest, XboardCommand) {
    EXPECT_EQ(parse_input("xboard"), ProtocolCommand::Xboard);
}

TEST(ProtocolTest, UciCommand) {
    EXPECT_EQ(parse_input("uci"), ProtocolCommand::Uci);
}

TEST(ProtocolTest, ForceCommand) {
    EXPECT_EQ(parse_input("force"), ProtocolCommand::Force);
}

TEST(ProtocolTest, PerftCommand) {
    EXPECT_EQ(parse_input("perft"), ProtocolCommand::Perft);
}

TEST(ProtocolTest, DisplayCommand) {
    EXPECT_EQ(parse_input("display"), ProtocolCommand::Display);
}

TEST(ProtocolTest, UsageCommand) {
    EXPECT_EQ(parse_input("usage"), ProtocolCommand::Usage);
}

TEST(ProtocolTest, LevelCommand) {
    EXPECT_EQ(parse_input("level"), ProtocolCommand::Level);
}

TEST(ProtocolTest, MemoryCommand) {
    EXPECT_EQ(parse_input("memory"), ProtocolCommand::Memory);
}

TEST(ProtocolTest, PostCommand) {
    EXPECT_EQ(parse_input("post"), ProtocolCommand::Post);
}

TEST(ProtocolTest, NopostCommand) {
    EXPECT_EQ(parse_input("nopost"), ProtocolCommand::Nopost);
}

// -----------------------------------------------------------------------
// Special case command parsing
// -----------------------------------------------------------------------
TEST(ProtocolTest, NewCommand) {
    // "new" maps to New_Game (not a direct enum 'Image match)
    EXPECT_EQ(parse_input("new"), ProtocolCommand::New_Game);
    EXPECT_EQ(parse_input("NEW"), ProtocolCommand::New_Game);
}

TEST(ProtocolTest, QuestionMarkCommand) {
    EXPECT_EQ(parse_input("?"), ProtocolCommand::Question_Mark);
}

TEST(ProtocolTest, ExitCommand) {
    EXPECT_EQ(parse_input("exit"), ProtocolCommand::Exit_Command);
    EXPECT_EQ(parse_input("EXIT"), ProtocolCommand::Exit_Command);
}

TEST(ProtocolTest, EmptyInput) {
    EXPECT_EQ(parse_input(""), ProtocolCommand::Noop);
}

// -----------------------------------------------------------------------
// Unknown commands
// -----------------------------------------------------------------------
TEST(ProtocolTest, UnknownCommand) {
    EXPECT_EQ(parse_input("gobbledygook"), ProtocolCommand::Unknown);
    EXPECT_EQ(parse_input("e2e4"), ProtocolCommand::Unknown);
    EXPECT_EQ(parse_input("Nf3"), ProtocolCommand::Unknown);
}

// -----------------------------------------------------------------------
// Internal commands
// -----------------------------------------------------------------------
TEST(ProtocolTest, InternalCommands) {
    EXPECT_EQ(parse_input("bench"), ProtocolCommand::Bench);
    EXPECT_EQ(parse_input("benchmark"), ProtocolCommand::Benchmark);
    EXPECT_EQ(parse_input("eval"), ProtocolCommand::Eval);
    EXPECT_EQ(parse_input("evalall"), ProtocolCommand::Evalall);
    EXPECT_EQ(parse_input("seetest"), ProtocolCommand::Seetest);
    EXPECT_EQ(parse_input("fensave"), ProtocolCommand::Fensave);
    EXPECT_EQ(parse_input("invert"), ProtocolCommand::Invert);
    EXPECT_EQ(parse_input("mirror"), ProtocolCommand::Mirror);
    EXPECT_EQ(parse_input("multipv"), ProtocolCommand::Multipv);
    EXPECT_EQ(parse_input("divide"), ProtocolCommand::Divide);
}

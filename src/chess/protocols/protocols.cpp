// ---------------------------------------------------------------------------
// AdaChess – Protocol command parser (implementation)
// Translated from: chess-protocols.adb
// ---------------------------------------------------------------------------
#include "chess/protocols/protocols.hpp"

#include <algorithm>
#include <cctype>
#include <unordered_map>

namespace chess::protocols {

namespace {

/// Convert string to uppercase.
std::string to_upper(std::string_view sv) {
    std::string result(sv);
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return result;
}

// -----------------------------------------------------------------------
// Command string → enum mapping.
//
// The Ada implementation iterates over all Protocol_Command_Type values
// and compares Uppercase(Input) with Uppercase(Command'Image). We replicate
// that with an explicit lookup table. Special cases that don't match their
// 'Image (like NEW, ?, EXIT) are handled separately below.
// -----------------------------------------------------------------------
struct CommandEntry {
    const char* name;
    ProtocolCommand cmd;
};

// This table contains the uppercase image of each enum value that CAN be
// matched via the standard 'Image comparison loop in Ada.
const std::unordered_map<std::string, ProtocolCommand>& command_map() {
    static const std::unordered_map<std::string, ProtocolCommand> map = {
        {"UNKNOWN",          ProtocolCommand::Unknown},
        {"NOOP",             ProtocolCommand::Noop},
        {"XBOARD",           ProtocolCommand::Xboard},
        {"NEW_GAME",         ProtocolCommand::New_Game},
        {"PROTOVER",         ProtocolCommand::Protover},
        {"ACCEPTED",         ProtocolCommand::Accepted},
        {"REJECTED",         ProtocolCommand::Rejected},
        {"VARIANT",          ProtocolCommand::Variant},
        {"QUIT",             ProtocolCommand::Quit},
        {"RANDOM",           ProtocolCommand::Random},
        {"FORCE",            ProtocolCommand::Force},
        {"GO",               ProtocolCommand::Go},
        {"PLAYOTHER",        ProtocolCommand::Playother},
        {"WHITE",            ProtocolCommand::White},
        {"BLACK",            ProtocolCommand::Black},
        {"LEVEL",            ProtocolCommand::Level},
        {"ST",               ProtocolCommand::St},
        {"SD",               ProtocolCommand::Sd},
        {"NPS",              ProtocolCommand::Nps},
        {"TIME",             ProtocolCommand::Time},
        {"OTIM",             ProtocolCommand::Otim},
        {"MOVE",             ProtocolCommand::Move},
        {"USERMOVE",         ProtocolCommand::Usermove},
        {"QUESTION_MARK",    ProtocolCommand::Question_Mark},
        {"PING",             ProtocolCommand::Ping},
        {"DRAW",             ProtocolCommand::Draw},
        {"RESULT",           ProtocolCommand::Result},
        {"SETBOARD",         ProtocolCommand::Setboard},
        {"EDIT",             ProtocolCommand::Edit},
        {"HINT",             ProtocolCommand::Hint},
        {"BK",               ProtocolCommand::Bk},
        {"UNDO",             ProtocolCommand::Undo},
        {"REMOVE",           ProtocolCommand::Remove},
        {"HARD",             ProtocolCommand::Hard},
        {"EASY",             ProtocolCommand::Easy},
        {"POST",             ProtocolCommand::Post},
        {"NOPOST",           ProtocolCommand::Nopost},
        {"ANALYZE",          ProtocolCommand::Analyze},
        {"NAME",             ProtocolCommand::Name},
        {"RATING",           ProtocolCommand::Rating},
        {"ICS",              ProtocolCommand::Ics},
        {"COMPUTER",         ProtocolCommand::Computer},
        {"PAUSE",            ProtocolCommand::Pause},
        {"RESUME",           ProtocolCommand::Resume},
        {"MEMORY",           ProtocolCommand::Memory},
        {"CORES",            ProtocolCommand::Cores},
        {"EGTPATH",          ProtocolCommand::Egtpath},
        {"OPTION",           ProtocolCommand::Option},
        {"UCI",              ProtocolCommand::Uci},
        {"DEBUG",            ProtocolCommand::Debug},
        {"ISREADY",          ProtocolCommand::Isready},
        {"SETOPTION",        ProtocolCommand::Setoption},
        {"REGISTER",         ProtocolCommand::Register},
        {"UCINEWGAME",       ProtocolCommand::Ucinewgame},
        {"POSITION",         ProtocolCommand::Position},
        {"STOP",             ProtocolCommand::Stop},
        {"PONDERHIT",        ProtocolCommand::Ponderhit},
        {"RUNEPDTESTSUITE",  ProtocolCommand::RunEpdTestSuite},
        {"MULTIPV",          ProtocolCommand::Multipv},
        {"PERFT",            ProtocolCommand::Perft},
        {"DIVIDE",           ProtocolCommand::Divide},
        {"BENCH",            ProtocolCommand::Bench},
        {"BENCHMARK",        ProtocolCommand::Benchmark},
        {"MOVES",            ProtocolCommand::Moves},
        {"SORT",             ProtocolCommand::Sort},
        {"GAME",             ProtocolCommand::Game},
        {"EVAL",             ProtocolCommand::Eval},
        {"EVALALL",          ProtocolCommand::Evalall},
        {"SEETEST",          ProtocolCommand::Seetest},
        {"SETEPD",           ProtocolCommand::Setepd},
        {"TUNE",             ProtocolCommand::Tune},
        {"FENSAVE",          ProtocolCommand::Fensave},
        {"INVERT",           ProtocolCommand::Invert},
        {"MIRROR",           ProtocolCommand::Mirror},
        {"DISPLAY",          ProtocolCommand::Display},
        {"EXIT_COMMAND",     ProtocolCommand::Exit_Command},
        {"USAGE",            ProtocolCommand::Usage}
    };
    return map;
}

} // anonymous namespace

// -----------------------------------------------------------------------
// parse_input — converts a string command to Protocol_Command_Type.
//
// Ada logic:
//   1. Uppercase the input and compare against each enum 'Image.
//   2. If no match: check for special cases "NEW", "", "?", "EXIT".
//   3. Otherwise: return Unknown.
// -----------------------------------------------------------------------
ProtocolCommand parse_input(std::string_view input) {
    // Empty input → Noop
    if (input.empty()) {
        return ProtocolCommand::Noop;
    }

    std::string command = to_upper(input);

    // Try the main lookup table first
    const auto& map = command_map();
    auto it = map.find(command);
    if (it != map.end()) {
        return it->second;
    }

    // Special cases not matching enum 'Image
    if (command == "NEW") {
        return ProtocolCommand::New_Game;
    }
    if (command == "?") {
        return ProtocolCommand::Question_Mark;
    }
    if (command == "EXIT") {
        return ProtocolCommand::Exit_Command;
    }

    return ProtocolCommand::Unknown;
}

} // namespace chess::protocols

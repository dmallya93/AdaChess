// ---------------------------------------------------------------------------
// AdaChess – Protocol command definitions and parser
// Translated from: chess-protocols.ads
// ---------------------------------------------------------------------------
#pragma once

#include <cstdint>
#include <string>
#include <string_view>

namespace chess::protocols {

// -----------------------------------------------------------------------
// Communication_Protocol_Type
// Ada: (No_Gui_Connection, Winboard, Universal_Chess_Interface) with Size => 2
// -----------------------------------------------------------------------
enum class CommunicationProtocol : std::uint8_t {
    No_Gui_Connection         = 0,
    Winboard                  = 1,
    Universal_Chess_Interface = 2
};

// -----------------------------------------------------------------------
// Winboard_Mode_Type
// Ada: (Play_White, Play_Black, Forcemode, Analyze) with Size => 2
// -----------------------------------------------------------------------
enum class WinboardMode : std::uint8_t {
    Play_White = 0,
    Play_Black = 1,
    Forcemode  = 2,
    Analyze    = 3
};

// -----------------------------------------------------------------------
// Protocol_Command_Type — 95+ commands.
// Ada: type Protocol_Command_Type is (Unknown, Noop, Xboard, ...)
// -----------------------------------------------------------------------
enum class ProtocolCommand : std::uint8_t {
    Unknown = 0,
    Noop,
    // Winboard related commands
    Xboard,
    New_Game,
    Protover,
    Accepted,
    Rejected,
    Variant,
    Quit,
    Random,
    Force,
    Go,
    Playother,
    White,
    Black,
    Level,
    St,
    Sd,
    Nps,
    Time,
    Otim,
    Move,
    Usermove,
    Question_Mark,
    Ping,
    Draw,
    Result,
    Setboard,
    Edit,
    Hint,
    Bk,
    Undo,
    Remove,
    Hard,
    Easy,
    Post,
    Nopost,
    Analyze,
    Name,
    Rating,
    Ics,
    Computer,
    Pause,
    Resume,
    Memory,
    Cores,
    Egtpath,
    Option,
    // UCI protocol commands
    Uci,
    Debug,
    Isready,
    Setoption,
    Register,
    Ucinewgame,
    Position,
    Stop,
    Ponderhit,
    // Internal commands
    RunEpdTestSuite,
    Multipv,
    Perft,
    Divide,
    Bench,
    Benchmark,
    Moves,
    Sort,
    Game,
    Eval,
    Evalall,
    Seetest,
    Setepd,
    Tune,
    Fensave,
    Invert,
    Mirror,
    Display,
    Exit_Command,
    Usage
};

// -----------------------------------------------------------------------
// Parse input string to a protocol command.
// Ada: function Parse_Input (Input : in String) return Protocol_Command_Type;
// -----------------------------------------------------------------------
[[nodiscard]] ProtocolCommand parse_input(std::string_view input);

// -----------------------------------------------------------------------
// Global state (matching Ada package-level variables)
// -----------------------------------------------------------------------

/// Whether to show thinking output (principal variation).
inline bool Principal_Variation_Post = true;

/// The currently active communication protocol.
inline CommunicationProtocol Communication_Protocol = CommunicationProtocol::No_Gui_Connection;

} // namespace chess::protocols

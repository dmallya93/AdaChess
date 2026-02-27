// ---------------------------------------------------------------------------
// AdaChess – Main entry point (partial migration)
// Translated from: adachess.adb
//
// This milestone implements:
//   - Command-line argument parsing
//   - Engine info display and prompt
//   - Basic command-read-dispatch loop (quit, exit, usage, display)
//   - Graceful exit
//
// Deferred to later milestones:
//   - Thinking task (std::jthread)
//   - Full game loop, move handling, engine integration
//   - FEN/PGN I/O
//   - Opening book loading
//   - Clock integration
// ---------------------------------------------------------------------------

#include <iostream>
#include <string>
#include <cstdlib>

#include "chess/chess.hpp"
#include "chess/colors.hpp"
#include "chess/info.hpp"
#include "chess/notations.hpp"
#include "chess/score.hpp"
#include "chess/protocols/protocols.hpp"
#include "chess/board/board.hpp"
#include "chess/board/attacks_data.hpp"
#include "util/string_lib.hpp"
#include "util/multiprocessor.hpp"

// ---------------------------------------------------------------------------
// EngineConfig — runtime configuration populated from CLI arguments.
// ---------------------------------------------------------------------------
struct EngineConfig {
    chess::Notation notation       = chess::Notation::Standard_Algebraic;
    std::size_t     hash_mb        = 256;
    std::string     opening_book_path;
    unsigned        multi_pv       = 1;
    bool            resign_enabled = false;
    bool            print_logo     = false;
};

// ---------------------------------------------------------------------------
// System_Info — display CPU count.
// ---------------------------------------------------------------------------
static void system_info() {
    std::cout << "\nNumber of CPU detected: "
              << util::multiprocessor::number_of_cpus() << "\n\n";
}

// ---------------------------------------------------------------------------
// Prompt — print the interactive prompt.
// ---------------------------------------------------------------------------
static void prompt() {
    std::cout << "AdaChess v." << chess::Version() << " => " << std::flush;
}

// ---------------------------------------------------------------------------
// Usage — print help text.
// ---------------------------------------------------------------------------
static void usage() {
    auto name    = chess::Engine_Name();
    auto author  = chess::Author();
    auto version = chess::Version();
    auto email   = chess::E_Mail();
    auto site    = chess::Site();

    std::cout << "\n"
              << name << " is a chess engine written by " << author << ".\n"
              << "Current release is n." << version << "\n\n"
              << "Visit " << site << " for informations on how to connect " << name << "\n"
              << "to an external GUI (like Winboard or Arena).\n"
              << "Write to " << email << " for comments and report bug reports!\n\n"
              << "Enjoy, share, have fun!\n\n";
}

// ---------------------------------------------------------------------------
// Engine_Info — one-line engine identification.
// ---------------------------------------------------------------------------
static std::string engine_info() {
    return std::string(chess::Engine_Name()) + " " +
           std::string(chess::Version()) + " - " +
           std::string(chess::Engine_Motto());
}

// ---------------------------------------------------------------------------
// parse_args — parse command-line arguments into EngineConfig.
// ---------------------------------------------------------------------------
static EngineConfig parse_args(int argc, char* argv[]) {
    EngineConfig config;
    int i = 1;
    while (i < argc) {
        std::string arg = argv[i];

        if (arg == "-n" || arg == "--notation") {
            if (i + 1 < argc) {
                std::string val = argv[i + 1];
                if (val == "winboard") {
                    config.notation = chess::Notation::Pure_Algebraic;
                } else if (val == "san") {
                    config.notation = chess::Notation::Standard_Algebraic;
                } else if (val == "lan") {
                    config.notation = chess::Notation::Long_Algebraic;
                } else if (val == "iccf") {
                    config.notation = chess::Notation::ICCF;
                } else {
                    std::cout << "Notation " << val << " is not available in AdaChess\n";
                }
                ++i;
            }
        } else if (arg == "-m" || arg == "--hash") {
            if (i + 1 < argc) {
                try {
                    config.hash_mb = static_cast<std::size_t>(std::stoi(argv[i + 1]));
                    std::cout << "Reserve memory for hashing: " << config.hash_mb << "\n";
                } catch (...) {
                    std::cout << "Invalid hash size\n";
                }
                ++i;
            }
        } else if (arg == "--system-info") {
            system_info();
        } else if (arg == "--easter-egg") {
            config.print_logo = true;
        } else if (arg == "--opening-book") {
            if (i + 1 < argc) {
                config.opening_book_path = argv[i + 1];
                std::cout << "Adding opening book: " << config.opening_book_path << "\n";
                ++i;
            }
        } else if (arg == "--multi-pv") {
            if (i + 1 < argc) {
                try {
                    config.multi_pv = static_cast<unsigned>(std::stoi(argv[i + 1]));
                    std::cout << "Using " << config.multi_pv << " PV lines\n";
                } catch (...) {
                    std::cout << "Invalid multi-pv value\n";
                }
                ++i;
            }
        } else if (arg == "--resign") {
            config.resign_enabled = true;
        } else if (arg == "--help" || arg == "-h") {
            usage();
            std::exit(0);
        }

        ++i;
    }
    return config;
}

// ---------------------------------------------------------------------------
// extract_command — get the first whitespace-delimited token from input.
// ---------------------------------------------------------------------------
static std::string extract_command(const std::string& source) {
    return util::strings::extract_token(source, ' ');
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    using namespace chess::protocols;

    // Print engine identification
    std::cout << engine_info() << "\n";

    // Initialize attack data tables
    chess::board::init_attacks_data();

    // Parse command-line arguments
    EngineConfig config = parse_args(argc, argv);

    // Apply notation from config
    chess::Default_Notation = config.notation;

    // Print console logo if requested
    if (config.print_logo) {
        std::cout << "\n\n" << chess::Console_Logo() << "\n\n";
    }

    // -----------
    // Main loop
    // -----------
    std::string line;

    while (true) {
        // Prompt (only when not connected to a GUI)
        if (Communication_Protocol == CommunicationProtocol::No_Gui_Connection) {
            prompt();
        }

        // Read input
        if (!std::getline(std::cin, line)) {
            break; // EOF
        }

        // Skip empty input
        if (util::strings::is_empty(line)) {
            continue;
        }

        // Parse the command
        std::string cmd_str = extract_command(line);
        ProtocolCommand command = parse_input(cmd_str);

        // ---------------------------------------------------------------
        // Dispatch recognized commands.
        // Full dispatch is deferred to Milestone 5 — here we handle only
        // the essential commands for a functional CLI scaffold.
        // ---------------------------------------------------------------
        switch (command) {
            case ProtocolCommand::Quit:
            case ProtocolCommand::Exit_Command:
                goto done;

            case ProtocolCommand::Usage:
                usage();
                break;

            case ProtocolCommand::Display:
                // Full board display is deferred (needs Chess.IO.Consoles).
                std::cout << "[Display command recognized — board display not yet implemented]\n";
                break;

            case ProtocolCommand::Xboard:
                Communication_Protocol = CommunicationProtocol::Winboard;
                std::cout << "\n" << std::flush;
                break;

            case ProtocolCommand::Uci:
                Communication_Protocol = CommunicationProtocol::Universal_Chess_Interface;
                std::cout << "\n" << std::flush;
                break;

            case ProtocolCommand::Protover:
                std::cout << "feature myname=\"AdaChess\"\n"
                          << "feature memory=1\n"
                          << "feature colors=0\n"
                          << "feature setboard=1\n"
                          << "feature sigint=0\n"
                          << "feature sigterm=0\n"
                          << "feature usermove=0\n"
                          << "feature random=1\n"
                          << "feature option=\"Resign -check 0\"\n"
                          << "feature san=0\n" << std::flush
                          << "feature done=1\n" << std::flush;
                break;

            case ProtocolCommand::Accepted:
                // Feedback from Winboard — ignore.
                break;

            case ProtocolCommand::Post:
                Principal_Variation_Post = true;
                break;

            case ProtocolCommand::Nopost:
                Principal_Variation_Post = false;
                break;

            case ProtocolCommand::Noop:
                break;

            default:
                // Commands not yet implemented in this milestone
                std::cout << "Command not yet implemented in this build\n";
                break;
        }
    }

done:
    std::cout << "Thanks for playing with AdaChess!\n";
    return 0;
}

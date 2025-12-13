#include <cstdlib>
#include <iostream>
#include <string>

namespace {

constexpr const char* PROJECT_NAME = "AdaChess";
constexpr const char* VERSION = "1.0.0";

void print_version() {
  std::cout << PROJECT_NAME << " " << VERSION << '\n';
}

void print_usage() {
  std::cout << "\nUsage: " << PROJECT_NAME << " [command]\n\n"
            << "Available commands:\n"
            << "  help       - Display this help message\n"
            << "  version    - Display version information\n"
            << "  create     - Create a new project (not yet implemented)\n"
            << "  createnow  - Create project immediately (not yet implemented)\n"
            << "  build      - Build the project (not yet implemented)\n"
            << "  server     - Start development server (not yet implemented)\n"
            << "  createfile - Create a new file (not yet implemented)\n"
            << "  license    - Display license information (not yet implemented)\n"
            << "  readme     - Display README (not yet implemented)\n"
            << "  quit       - Exit the program\n"
            << '\n';
}

void print_prompt() {
  std::cout << PROJECT_NAME << " v." << VERSION << " => " << std::flush;
}

enum class Command : std::uint8_t {
  Noop,
  Quit,
  Help,
  Version,
  Create,
  CreateNow,
  Build,
  Server,
  CreateFile,
  License,
  Readme,
  Unknown
};

Command parse_command(const std::string& input) {
  if (input.empty()) {
    return Command::Noop;
  }

  // Extract first token
  auto space_pos = input.find(' ');
  std::string cmd = input.substr(0, space_pos);

  // Match commands
  if (cmd == "quit" || cmd == "exit") {
    return Command::Quit;
  }
  if (cmd == "help") {
    return Command::Help;
  }
  if (cmd == "version") {
    return Command::Version;
  }
  if (cmd == "create") {
    return Command::Create;
  }
  if (cmd == "createnow") {
    return Command::CreateNow;
  }
  if (cmd == "build") {
    return Command::Build;
  }
  if (cmd == "server") {
    return Command::Server;
  }
  if (cmd == "createfile") {
    return Command::CreateFile;
  }
  if (cmd == "license") {
    return Command::License;
  }
  if (cmd == "readme") {
    return Command::Readme;
  }

  return Command::Unknown;
}

} // namespace

int main(int argc, char** argv) {
  // Print project info
  print_version();

  // Parse command line arguments
  // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
  for (char** arg = argv + 1; arg != argv + argc; ++arg) {
    std::string arg_str = *arg;
    if (arg_str == "--help" || arg_str == "-h") {
      print_usage();
      return EXIT_SUCCESS;
    }
    if (arg_str == "--version" || arg_str == "-v") {
      print_version();
      return EXIT_SUCCESS;
    }
    // Other command-line options will be handled in future tasks
  }

  // Main command loop
  std::string input;
  bool running = true;

  while (running) {
    print_prompt();

    if (!std::getline(std::cin, input)) {
      // EOF reached
      break;
    }

    Command cmd = parse_command(input);

    switch (cmd) {
    case Command::Noop:
      // Empty input, do nothing
      break;

    case Command::Quit:
      running = false;
      std::cout << "Goodbye!\n";
      break;

    case Command::Help:
      print_usage();
      break;

    case Command::Version:
      print_version();
      break;

    case Command::Create:
      std::cout << "[Create project - not yet implemented]\n";
      break;

    case Command::CreateNow:
      std::cout << "[Create project now - not yet implemented]\n";
      break;

    case Command::Build:
      std::cout << "[Build project - not yet implemented]\n";
      break;

    case Command::Server:
      std::cout << "[Start server - not yet implemented]\n";
      break;

    case Command::CreateFile:
      std::cout << "[Create file - not yet implemented]\n";
      break;

    case Command::License:
      std::cout << "[Display license - not yet implemented]\n";
      break;

    case Command::Readme:
      std::cout << "[Display README - not yet implemented]\n";
      break;

    case Command::Unknown:
      std::cout << "Unknown command: " << input << '\n';
      std::cout << "Type 'help' for available commands.\n";
      break;
    }

    std::cout << std::flush;
  }

  return EXIT_SUCCESS;
}

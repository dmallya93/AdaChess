# AdaChess - C++ Migration

Modern C++20 chess engine migrated from Ada, preserving safety-critical features while leveraging C++ performance and ecosystem.

## Overview

This is a modernization project migrating the AdaChess engine from Ada to C++20. The project maintains Ada's strong type safety and reliability principles while utilizing modern C++ practices.

**Source Repository**: https://github.com/dmallya93/AdaChess.git
**Migration Started**: Wed Jan 7 20:51:24 UTC 2026

## Current Status

**Milestone 1: Core Types, Board Representation, and Basic Move Generation**
- ✅ **Task 1**: Core Type System Foundation (Complete)
  - Color, Piece, Score, Depth, Node, Hash, Pin types
  - 49 unit tests passing
  - Header-only implementation

## Building

### Prerequisites

- CMake 3.20+
- GCC 11+ or Clang 20+ (C++20 support required)
- Google Test (for unit tests)

### Build Instructions

```bash
# Configure
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
make -j4

# Run tests
./adachess_tests
```

### Build Options

- `ENABLE_SANITIZERS`: Enable AddressSanitizer and UBSan (default: ON for Debug)
- `ENABLE_CLANG_TIDY`: Enable clang-tidy static analysis (default: OFF)
- `WARNINGS_AS_ERRORS`: Treat warnings as errors (default: OFF)

## Project Structure

```
include/adachess/core/  - Core type definitions (header-only)
src/core/              - Implementation files (when needed)
tests/                 - Unit tests
```

## Architecture

- **Namespace**: `adachess`
- **Module Structure**:
  - `adachess::core` - Core types and utilities
  - `adachess::board` - Board representation (planned)
  - `adachess::engine` - Search and evaluation (planned)
  - `adachess::io` - FEN/PGN parsing (planned)
  - `adachess::protocols` - Winboard/Xboard support (planned)

## Design Principles

- Modern C++20 with strong type safety
- Zero-cost abstractions where possible
- Header-only core for maximum inlining
- Comprehensive unit testing
- Static analysis integration (clang-tidy)
- Memory safety through RAII and smart pointers

## License

GNU General Public License v3.0 or later

Copyright (C) 2013-2023 - Alessandro Iavicoli
Email: adachess@gmail.com

# AdaChess - C++ Chess Engine

C++ migration of the AdaChess engine, preserving safety-critical features while leveraging modern C++20 capabilities.

**Source**: https://github.com/dmallya93/AdaChess.git
**Original Author**: Alessandro Iavicoli
**License**: GPL-3.0-or-later

## Project Status

This is an early-stage migration from Ada to C++. Currently implementing Milestone 1: Core Types, Board Representation, and Basic Move Generation.

### Completed Tasks

- **Task 5**: Move Structures and Registration System
  - Move_Type structure with all fields and enums
  - Move comparison operators and helper functions
  - Move annotation types (Annotation_Type, Annotated_Move_Type)
  - History_Move_Type using real Move_Type
  - Register_Move procedures (3 overloads)
  - Register_Tactical_Move procedures (3 overloads)
  - Comprehensive unit tests (21 tests, all passing)

## Building

### Requirements

- CMake 3.20+
- C++20 compatible compiler (GCC 11+, Clang 12+)
- Google Test (for unit tests)

### Build Instructions

```bash
# Configure
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Build
cmake --build . --parallel

# Run tests
./adachess_tests
```

### Build Options

- `ENABLE_SANITIZERS` (default: ON in Debug): Enable AddressSanitizer and UndefinedBehaviorSanitizer
- `ENABLE_CLANG_TIDY` (default: OFF): Enable clang-tidy static analysis
- `WARNINGS_AS_ERRORS` (default: OFF): Treat compiler warnings as errors

## Project Structure

```
AdaChess/
├── include/adachess/      # Public headers
│   ├── board/             # Board representation, moves
│   ├── core/              # Core types, history
│   └── engine/            # Engine components (annotations, registration)
├── src/                   # Implementation files
│   ├── board/
│   └── engine/
├── tests/                 # Unit tests
└── build/                 # Build directory (gitignored)
```

## Design Decisions

### Move Structure

The `Move` struct is the fundamental unit of chess computation, instantiated billions of times during search. Key design points:

- **Size**: Target ≤16 bytes for cache efficiency (current: check with `sizeof(Move)`)
- **Fields**: piece, captured, from, to, flag, promotion, check, ambiguous
- **Enums**: MoveFlag, CheckType, AmbiguousFlag as enum class for type safety

### Move Registration System

Provides three overloads for flexibility:
1. `register_move(board, from, to)` - Simple move creation
2. `register_move(board, from, to, flag)` - With flags (auto-expands promotions to 4 moves)
3. `register_move(board, move)` - Pre-constructed move with validation

The system automatically:
- Validates move legality (king safety check)
- Detects check types (placeholder stubs for now)
- Adds disambiguation flags for notation
- Filters tactical vs quiet moves

## Testing

Current test coverage:
- Move structure tests (8 tests)
- Annotation tests (5 tests)
- History move tests (3 tests)
- Chessboard initialization (1 test)
- Move registration tests (4 tests)

All tests pass with AddressSanitizer and UndefinedBehaviorSanitizer enabled.

## License

GPL-3.0-or-later (inherited from Ada source)

Copyright (C) 2013-2023 - Alessandro Iavicoli

# Design Decisions

This document records major design decisions made during the Ada to C++ migration of the chess engine.

## Design Decision #1: Configuration Storage and Parsing Strategy

**Decision Date:** Task #3 - Configuration Module

**Decision:** Custom C++ containers (std::unordered_map) with manual INI parsing

### Rationale

1. **Simplicity**: INI format is simple enough that a manual parser is straightforward and adds zero dependencies
2. **Performance**: Direct map access without JSON overhead
3. **Type Safety**: Separate containers for different tag types (simple vs. table) provides compile-time clarity
4. **No External Dependencies**: Keeps the core configuration module dependency-free

### Implementation Details

- `site_config` struct: Plain C++ struct with typed fields
- Simple tags: `std::unordered_map<std::string, std::string>`
- Table tags: `std::unordered_map<std::string, std::vector<std::string>>`
- Excluded files: `std::vector<std::string>`
- Manual line-by-line parsing with string splitting on '='

### Alternatives Considered

- **nlohmann::json**: Adds external dependency, overkill for simple INI format
- **boost::property_tree**: Heavy dependency, not worth it for basic key=value parsing
- **inih library**: Adds dependency, minimal benefit over manual parsing
- **std::variant approach**: More complex, unnecessary for our use case

## Design Decision #2: String Handling and Path Management

**Decision Date:** Task #3 - Configuration Module

**Decision:** std::string + std::filesystem with std::string_view for read-only parameters

### Rationale

1. **Standard Library**: Uses only C++17/20 standard library features
2. **Cross-Platform**: std::filesystem handles path separators automatically
3. **Performance**: std::string_view avoids copies for read-only operations
4. **Clarity**: std::filesystem::path makes intent explicit for path operations

### Implementation Details

- Storage: `std::string` for owned strings and paths
- Parameters: `const std::string&` or `std::string_view` for read-only
- Path operations: Convert to `std::filesystem::path` for normalization
- Path normalization: Use `std::filesystem::absolute()` and `std::filesystem::canonical()`

### Alternatives Considered

- **std::filesystem::path everywhere**: More type-safe but verbose, conversion overhead
- **Strong typedef wrappers**: Adds complexity without significant benefit
- **Raw C strings**: Not idiomatic C++, loses RAII benefits

## Design Decision #4: Error Handling and Exception Strategy

**Decision Date:** Task #2 - Messages Module and Error Handling Foundation

**Decision:** Mixed approach - C++ exceptions for exceptional cases, return codes/std::optional for expected failures

### Rationale

1. **Ada Compatibility**: Ada uses exceptions for error handling, making a C++ exception-based approach the most natural translation for exceptional conditions.

2. **Chess Engine Context**: Chess engine operations have both:
   - **Exceptional errors**: invalid FEN, corrupt opening book, memory allocation failure
   - **Expected failures**: invalid move input, search timeout, pondering interrupt

3. **Benefits of Mixed Approach**:
   - Clear separation between programming errors and expected edge cases
   - Strong exception safety guarantees (RAII ensures cleanup)
   - Performance: no exception overhead for normal control flow (move legality, search cutoffs)
   - Maintainability: exceptions document truly exceptional conditions

### Guidelines

#### Use EXCEPTIONS for:
- Programming errors (assertion failures, logic bugs)
- Resource failures (memory, file I/O, corrupt data)
- Unrecoverable errors (invalid FEN format, broken transposition table)
- Protocol violations (malformed commands from GUI)

#### Use RETURN CODES/std::optional for:
- Expected failures (invalid move notation, search stopped early)
- Validation results (move legality checks, input parsing)
- Optional results (transposition table lookups, opening book queries)
- Performance-critical paths where exceptions would add overhead

### Exception Safety Levels

All code should target one of these three levels:

1. **Basic guarantee**: Resources are not leaked, objects are in valid state
2. **Strong guarantee**: Operation succeeds or has no effect (rollback)
3. **No-throw guarantee**: Operation cannot fail (marked `noexcept`)

### Exception Hierarchy

All custom exception types derive from `std::exception` hierarchy for consistent catching and standard `what()` interface.

Implemented in `include/yass/error.hpp`:

```cpp
namespace yass {
  class ChessException : public std::runtime_error { ... };
  class InvalidPositionError : public ChessException { ... };
  class InvalidMoveError : public ChessException { ... };
  class IoError : public ChessException { ... };
  class ProtocolError : public ChessException { ... };
}
```

### Application to Modules

- **Logging Module** (`yass::logging`): Provides no-throw guarantee. All functions marked `noexcept` as logging failures should never interrupt program flow.
- **I/O Modules** (future): Will throw `IoError` for file access failures, `InvalidPositionError` for malformed FEN.
- **Engine Core** (future): Will use return codes for move validation, exceptions for internal consistency errors.
- **Protocol Handlers** (future): Will throw `ProtocolError` for malformed commands.

### References

- Exception safety guarantees: Herb Sutter's "Guru of the Week" articles
- C++ Core Guidelines: E.1-E.31 (Error handling)
- Implementation: `include/yass/error.hpp`

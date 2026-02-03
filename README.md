# AdaChess - Smart Chess Engine

A C++ chess engine, ported from the original Ada implementation.

## Requirements

- **CMake** >= 3.20
- **C++17 compliant compiler**:
  - GCC >= 9
  - Clang >= 10
  - MSVC 2019 or later

## Building

### Linux / macOS

```bash
# Configure the build (Release mode)
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build the project
cmake --build build

# Run the executable
./build/bin/adachess
```

### Debug Build

```bash
# Configure with debug flags
cmake -B build -DCMAKE_BUILD_TYPE=Debug

# Build
cmake --build build
```

### Windows (Visual Studio)

```powershell
# Configure
cmake -B build -G "Visual Studio 16 2019"

# Build (Release)
cmake --build build --config Release

# Run
.\build\bin\Release\adachess.exe
```

## Build Options

| Option | Default | Description |
|--------|---------|-------------|
| `ADACHESS_BUILD_TESTS` | ON | Build unit tests |
| `CMAKE_BUILD_TYPE` | Release | Build type (Debug, Release, RelWithDebInfo) |

## Project Structure

```
AdaChess/
├── CMakeLists.txt          # Top-level build configuration
├── cmake/                  # CMake modules and toolchain files
├── include/
│   └── adachess/           # Public header files
├── src/
│   ├── adachess/           # Engine implementation
│   └── main/
│       └── main.cpp        # Main entry point
└── tests/                  # Unit tests (future)
```

## License

See the original AdaChess project for license information.

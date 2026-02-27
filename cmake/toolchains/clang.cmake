# ---------------------------------------------------------------------------
# AdaChess – Clang Toolchain File (Clang >= 12)
# ---------------------------------------------------------------------------
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/clang.cmake ..
# ---------------------------------------------------------------------------

set(CMAKE_C_COMPILER   clang)
set(CMAKE_CXX_COMPILER clang++)

# Ensure we get at least Clang 12 for solid C++20 support
# (Checked at configure time by CompilerWarnings.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

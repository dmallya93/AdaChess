# ---------------------------------------------------------------------------
# AdaChess – GCC Toolchain File (GCC >= 11)
# ---------------------------------------------------------------------------
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/gcc.cmake ..
# ---------------------------------------------------------------------------

set(CMAKE_C_COMPILER   gcc)
set(CMAKE_CXX_COMPILER g++)

# Ensure we get at least GCC 11 for solid C++20 support
# (Checked at configure time by CompilerWarnings.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

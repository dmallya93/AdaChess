# ---------------------------------------------------------------------------
# AdaChess – MSVC Toolchain File (Visual Studio 2022+)
# ---------------------------------------------------------------------------
# Usage:
#   cmake -DCMAKE_TOOLCHAIN_FILE=cmake/toolchains/msvc.cmake ..
#
# This toolchain is designed for use with the Visual Studio generator or
# Ninja on Windows with the MSVC compiler (cl.exe).
# ---------------------------------------------------------------------------

set(CMAKE_C_COMPILER   cl)
set(CMAKE_CXX_COMPILER cl)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Use the dynamic CRT by default (/MD in Release, /MDd in Debug)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

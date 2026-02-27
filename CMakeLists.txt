cmake_minimum_required(VERSION 3.22)

project(AdaChess
    VERSION 4.0.0
    DESCRIPTION "AdaChess - Smart Chess Engine (C++20 port)"
    LANGUAGES CXX
)

# ---------------------------------------------------------------------------
# Global C++ standard
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------
option(ADACHESS_BUILD_TESTS    "Build unit and integration tests"  ON)
option(ADACHESS_ENABLE_LTO     "Enable link-time optimization"     OFF)
option(ADACHESS_STRICT_WARNINGS "Enable strict compiler warnings"  ON)
option(ADACHESS_PROFILE_BUILD  "Enable profiling instrumentation"  OFF)

# ---------------------------------------------------------------------------
# Include cmake/ helper modules
# ---------------------------------------------------------------------------
# Compiler version checks and warning flags (cmake/CompilerWarnings.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/CompilerWarnings.cmake)

# Custom build types: Speed, Profile (cmake/BuildTypes.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/BuildTypes.cmake)

# LTO support (cmake/LTO.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LTO.cmake)

# ---------------------------------------------------------------------------
# LTO
# ---------------------------------------------------------------------------
if(ADACHESS_ENABLE_LTO)
    adachess_enable_lto()
endif()

# ---------------------------------------------------------------------------
# Precondition/postcondition macros
# ---------------------------------------------------------------------------
# In Debug/Profile builds: expand to assert()
# In Release/Speed builds: expand to nothing (safe default)
# These are defined via compile definitions on the core library target.

# ---------------------------------------------------------------------------
# Core library: adachess_core
# ---------------------------------------------------------------------------
add_library(adachess_core STATIC
    # chess/ foundational types (header-only, but we include .cpp files for any impl)
    src/chess/board/board.cpp
    src/chess/board/attacks_data.cpp
    src/chess/protocols/protocols.cpp
    src/util/string_lib.cpp
    src/util/timers.cpp
    src/util/multiprocessor.cpp
)

target_include_directories(adachess_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(adachess_core PUBLIC cxx_std_20)

# Apply strict warnings if enabled (via cmake/CompilerWarnings.cmake)
if(ADACHESS_STRICT_WARNINGS)
    adachess_set_warnings(adachess_core)
endif()

# Platform-specific link libraries
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(adachess_core PUBLIC pthread)
endif()

# ---------------------------------------------------------------------------
# Main executable: adachess
# ---------------------------------------------------------------------------
add_executable(adachess
    src/main.cpp
)

target_link_libraries(adachess PRIVATE adachess_core)

# Apply strict warnings if enabled (via cmake/CompilerWarnings.cmake)
if(ADACHESS_STRICT_WARNINGS)
    adachess_set_warnings(adachess)
endif()

# ---------------------------------------------------------------------------
# Tests (Google Test via FetchContent)
# ---------------------------------------------------------------------------
if(ADACHESS_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.15.2
    )
    # Prevent overriding parent project compiler/linker settings on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()
    add_subdirectory(tests)
endif()

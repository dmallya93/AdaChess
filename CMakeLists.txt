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
# Compiler warnings
# ---------------------------------------------------------------------------
if(ADACHESS_STRICT_WARNINGS)
    if(MSVC)
        add_compile_options(/W4 /WX)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic -Werror)
    endif()
endif()

# ---------------------------------------------------------------------------
# Build-type specific flags (beyond CMake defaults)
# ---------------------------------------------------------------------------
# CMake provides: Debug (-g), Release (-O3 -DNDEBUG), RelWithDebInfo, MinSizeRel.
# We add custom configurations Speed and Profile.

# Speed: aggressive optimization
set(CMAKE_CXX_FLAGS_SPEED "-O3 -DNDEBUG" CACHE STRING "Flags for Speed build")
if(NOT MSVC)
    string(APPEND CMAKE_CXX_FLAGS_SPEED " -finline-functions -funroll-loops")
endif()

# Profile: optimized with debug info for profiling
set(CMAKE_CXX_FLAGS_PROFILE "-O2 -g -DNDEBUG" CACHE STRING "Flags for Profile build")
if(NOT MSVC)
    string(APPEND CMAKE_CXX_FLAGS_PROFILE " -fno-omit-frame-pointer")
endif()

# Register custom build types so IDEs and generators recognize them
get_property(_is_multi GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(_is_multi)
    if(NOT "Speed" IN_LIST CMAKE_CONFIGURATION_TYPES)
        list(APPEND CMAKE_CONFIGURATION_TYPES Speed Profile)
    endif()
else()
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        Debug Release Speed Profile RelWithDebInfo MinSizeRel)
endif()

# ---------------------------------------------------------------------------
# LTO support
# ---------------------------------------------------------------------------
if(ADACHESS_ENABLE_LTO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT _lto_supported OUTPUT _lto_output)
    if(_lto_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    else()
        message(WARNING "LTO not supported: ${_lto_output}")
    endif()
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

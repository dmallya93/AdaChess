# AdaChess source build configuration

# ============================================================================
# Core Library - Foundation Types
# ============================================================================

# Core type system sources (colors, pieces, score, hashes, etc.)
set(ADACHESS_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/pieces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/score.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/hashes.cpp
)

# Core type system headers
set(ADACHESS_CORE_HEADERS
    ${CMAKE_SOURCE_DIR}/include/adachess/chess.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/colors.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/pieces.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/score.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/depths.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/nodes.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/distances.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/hashes.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/pins.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/history.hpp
)

# Create the core library (static library with implementation files)
add_library(adachess_core STATIC
    ${ADACHESS_CORE_SOURCES}
    ${ADACHESS_CORE_HEADERS}
)

target_include_directories(adachess_core PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

# Set C++17 requirement for the library
target_compile_features(adachess_core PUBLIC cxx_std_17)

# ============================================================================
# Future Library Targets (to be added in subsequent milestones)
# ============================================================================
# - adachess_io: I/O utilities (FEN, PGN, console display)
# - adachess_protocols: Protocol handlers (Winboard/Xboard)
# - adachess_libs: Utility libraries (string_lib, timers)

# ============================================================================
# Main Executable
# ============================================================================
add_subdirectory(main)

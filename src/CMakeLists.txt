# AdaChess source build configuration

# ============================================================================
# Core Library - Foundation Types
# ============================================================================

# Core type system sources (colors, pieces, score, hashes, board, moves, io, engine, etc.)
# Note: io.cpp is included in core since it only depends on core types (Move, Piece, etc.)
# and engine.cpp uses io.hpp for move_to_string conversions.
set(ADACHESS_CORE_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/pieces.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/score.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/hashes.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/board/board.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/board/attacks_data.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/moves.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/io/io.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/engine/engine.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/engine/perfts.cpp
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
    ${CMAKE_SOURCE_DIR}/include/adachess/moves.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/moves/annotations.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/notations.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/players.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/io/io.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/board/board.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/board/directions.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/board/attacks_data.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/engine/engine.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/engine/perfts.hpp
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
# Utility Libraries (string_lib, timers)
# ============================================================================

set(ADACHESS_LIBS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/libs/string_lib.cpp
)

set(ADACHESS_LIBS_HEADERS
    ${CMAKE_SOURCE_DIR}/include/adachess/libs/string_lib.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/libs/timers/timers.hpp
)

add_library(adachess_libs STATIC
    ${ADACHESS_LIBS_SOURCES}
    ${ADACHESS_LIBS_HEADERS}
)

target_include_directories(adachess_libs PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_compile_features(adachess_libs PUBLIC cxx_std_17)

# ============================================================================
# I/O Library (console display, FEN, PGN, move notation)
# ============================================================================

set(ADACHESS_IO_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/adachess/io/consoles.cpp
)

set(ADACHESS_IO_HEADERS
    ${CMAKE_SOURCE_DIR}/include/adachess/io/consoles.hpp
    ${CMAKE_SOURCE_DIR}/include/adachess/info.hpp
)

add_library(adachess_io STATIC
    ${ADACHESS_IO_SOURCES}
    ${ADACHESS_IO_HEADERS}
)

target_include_directories(adachess_io PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)

target_link_libraries(adachess_io PUBLIC
    adachess_core
)

target_compile_features(adachess_io PUBLIC cxx_std_17)

# ============================================================================
# Future Library Targets (to be added in subsequent milestones)
# ============================================================================
# - adachess_protocols: Protocol handlers (Winboard/Xboard)

# ============================================================================
# Main Executable
# ============================================================================
add_subdirectory(main)

# AdaChess source build configuration

# ============================================================================
# Core Library (placeholder for future implementation)
# ============================================================================

# Future library targets will be added here:
# - adachess_core: Core chess engine (board, move generation, evaluation)
# - adachess_io: I/O utilities (FEN, PGN, console display)
# - adachess_protocols: Protocol handlers (Winboard/Xboard)
# - adachess_libs: Utility libraries (string_lib, timers)

# For now, we create an INTERFACE library as a placeholder
add_library(adachess_core INTERFACE)
target_include_directories(adachess_core INTERFACE
    ${CMAKE_SOURCE_DIR}/include
)

# ============================================================================
# Main Executable
# ============================================================================
add_subdirectory(main)

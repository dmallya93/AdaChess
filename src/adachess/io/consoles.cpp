/**
 * AdaChess - Smart Chess Engine
 *
 * Console display implementation for rendering the chessboard.
 * C++ port of the original Ada implementation (chess-io-consoles.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/io/consoles.hpp"

#include <iostream>
#include <memory>

#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/board/board.hpp"
#include "adachess/engine/engine.hpp"

namespace chess::io::consoles {

// ============================================================================
// Global Render Engine
// ============================================================================

namespace {

/**
 * The currently selected render engine.
 * Defaults to SimpleConsoleRenderEngine.
 */
std::unique_ptr<RenderEngine> g_selected_render_engine = 
    std::make_unique<SimpleConsoleRenderEngine>();

} // anonymous namespace

RenderEngine& get_render_engine() {
    return *g_selected_render_engine;
}

void set_render_engine(std::unique_ptr<RenderEngine> engine) {
    if (engine) {
        g_selected_render_engine = std::move(engine);
    }
}

void reset_render_engine() {
    g_selected_render_engine = std::make_unique<SimpleConsoleRenderEngine>();
}

// ============================================================================
// SimpleConsoleRenderEngine Implementation
// ============================================================================

void SimpleConsoleRenderEngine::render(const engine::Chessboard& chessboard) const {
    // Render the board in the 10x12 mailbox format
    // The playable squares are at indices:
    //   21-28 (rank 8), 31-38 (rank 7), ..., 91-98 (rank 1)
    //
    // We iterate through the board array and:
    // - Skip frame squares
    // - Print piece symbols for non-empty squares
    // - Print '.' for empty squares
    // - Add rank numbers and file labels

    board::Coordinate row = 8;  // Start at rank 8
    
    std::cout << std::endl;

    for (std::size_t i = 0; i < board::kBoardSize; ++i) {
        Piece piece = chessboard.square.at_index(i);
        
        // Skip frame squares (don't output anything for them)
        if (piece != Piece::Frame) {
            if (piece == Piece::Empty) {
                std::cout << '.';
            } else {
                std::cout << piece_symbol(piece);
            }
            std::cout << ' ';
        }

        // Handle end-of-row markers
        // In the 10x12 mailbox:
        //   Row ends at indices 9, 19, 29, 39, 49, 59, 69, 79, 89, 99, 109, 119
        //   But we only print rank numbers for playable rows (20-99)
        switch (i) {
            case 9:
            case 19:
            case 109:
            case 119:
                // Frame rows - just newline
                std::cout << std::endl;
                break;
            case 29:
            case 39:
            case 49:
            case 59:
            case 69:
            case 79:
            case 89:
            case 99:
                // Playable rows - print rank number and newline
                std::cout << " " << static_cast<int>(row);
                row = static_cast<board::Coordinate>(row - 1);
                std::cout << std::endl;
                break;
            default:
                // Not end of row - do nothing
                break;
        }
    }

    // Print file labels
    std::cout << std::endl;
    std::cout << "  a b c d e f g h" << std::endl;
    std::cout << std::endl;

    // Print en passant square if available
    if (chessboard.en_passant[chessboard.history_ply] != board::kNoEnPassant) {
        std::cout << "En passant: " 
                  << board::square_to_string(chessboard.en_passant[chessboard.history_ply]) 
                  << std::endl;
        std::cout << std::endl;
    }

    // Print check status
    // Note: The Ada code uses Attacks() function which we don't have yet
    // For now, we'll skip this feature and add it when attack detection is available
    // This is a minor deviation from the original Ada implementation
    // TODO: Add check detection when Attacks() function is available
    
    std::cout << std::endl;
}

// ============================================================================
// Convenience Function
// ============================================================================

void display_on_console(const engine::Chessboard& chessboard) {
    g_selected_render_engine->render(chessboard);
}

} // namespace chess::io::consoles

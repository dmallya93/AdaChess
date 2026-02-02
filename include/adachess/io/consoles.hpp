/**
 * AdaChess - Smart Chess Engine
 *
 * Console display for rendering the chessboard.
 * C++ port of the original Ada implementation (chess-io-consoles.ads).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_IO_CONSOLES_HPP
#define ADACHESS_IO_CONSOLES_HPP

#include <memory>

// Forward declaration to avoid circular dependency
namespace chess::engine {
class Chessboard;
}

namespace chess::io::consoles {

// ============================================================================
// Render Interface
// ============================================================================

/**
 * Abstract interface for rendering a chessboard.
 * This allows for different rendering implementations (console, GUI, etc.).
 */
class RenderEngine {
public:
    /**
     * Virtual destructor for proper cleanup of derived classes.
     */
    virtual ~RenderEngine() = default;

    /**
     * Render the chessboard.
     * @param chessboard The chessboard to render.
     */
    virtual void render(const engine::Chessboard& chessboard) const = 0;

protected:
    // Protected constructors - this is an abstract base class
    RenderEngine() = default;
    RenderEngine(const RenderEngine&) = default;
    RenderEngine& operator=(const RenderEngine&) = default;
    RenderEngine(RenderEngine&&) = default;
    RenderEngine& operator=(RenderEngine&&) = default;
};

// ============================================================================
// Console Render Engine
// ============================================================================

/**
 * Abstract base class for console-based renderers.
 * Provides a common base for different console rendering styles.
 */
class ConsoleRenderEngine : public RenderEngine {
public:
    ~ConsoleRenderEngine() override = default;

protected:
    ConsoleRenderEngine() = default;
};

/**
 * Simple console render engine that displays the board as ASCII.
 * 
 * Uses the following format:
 * - Piece symbols: P, N, B, R, Q, K for white; p, n, b, r, q, k for black
 * - Empty squares: . (dot)
 * - File labels: a-h
 * - Rank labels: 1-8
 * 
 * White pieces are on the bottom (ranks 1-2), black on top (ranks 7-8).
 */
class SimpleConsoleRenderEngine : public ConsoleRenderEngine {
public:
    SimpleConsoleRenderEngine() = default;
    ~SimpleConsoleRenderEngine() override = default;

    /**
     * Render the chessboard to the console.
     * @param chessboard The chessboard to display.
     */
    void render(const engine::Chessboard& chessboard) const override;
};

// ============================================================================
// Global Render Engine Selection
// ============================================================================

/**
 * Get the currently selected render engine.
 * @return Reference to the current render engine.
 */
[[nodiscard]] RenderEngine& get_render_engine();

/**
 * Set a custom render engine.
 * @param engine Unique pointer to the new render engine.
 */
void set_render_engine(std::unique_ptr<RenderEngine> engine);

/**
 * Reset to the default simple console render engine.
 */
void reset_render_engine();

// ============================================================================
// Convenience Function
// ============================================================================

/**
 * Display the chessboard on the console using the currently selected render engine.
 * This is the main entry point for displaying the board.
 * 
 * @param chessboard The chessboard to display.
 */
void display_on_console(const engine::Chessboard& chessboard);

} // namespace chess::io::consoles

#endif // ADACHESS_IO_CONSOLES_HPP

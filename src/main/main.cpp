/**
 * AdaChess - Smart Chess Engine
 *
 * Main entry point for the AdaChess chess engine.
 * C++ port of the original Ada implementation.
 *
 * Copyright (c) AdaChess Project
 */

#include <iostream>

// Include all core type headers to verify they compile
#include "adachess/chess.hpp"
#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/score.hpp"
#include "adachess/depths.hpp"
#include "adachess/nodes.hpp"
#include "adachess/distances.hpp"
#include "adachess/hashes.hpp"
#include "adachess/pins.hpp"
#include "adachess/history.hpp"

int main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) {
    std::cout << "AdaChess - Smart Chess Engine" << std::endl;

    // Basic type verification
    static_assert(chess::flip(chess::Color::White) == chess::Color::Black,
                  "Color flip test failed");
    static_assert(chess::is_white_piece(chess::Piece::WhiteKing),
                  "White piece test failed");
    static_assert(chess::is_black_piece(chess::Piece::BlackKing),
                  "Black piece test failed");
    static_assert(chess::is_mate(chess::kMate), "Mate detection test failed");
    static_assert(!chess::is_mate(chess::kDraw), "Draw mate test failed");
    static_assert(chess::kZeroDepth == 1, "Zero depth test failed");
    static_assert(chess::kUnreachable == 9, "Unreachable distance test failed");
    static_assert(chess::kEmptyHash == 0, "Empty hash test failed");
    static_assert(chess::is_not_pinned(chess::Pin::NoPin), "Pin test failed");

    return 0;
}

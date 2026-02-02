/**
 * AdaChess - Smart Chess Engine
 *
 * Chessboard state and basic initialization.
 * C++ port of the original Ada implementation (chess-engine.ads/adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#ifndef ADACHESS_ENGINE_ENGINE_HPP
#define ADACHESS_ENGINE_ENGINE_HPP

#include <array>
#include <cstdint>
#include <cassert>
#include <iostream>

#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/board/board.hpp"
#include "adachess/board/directions.hpp"
#include "adachess/moves.hpp"
#include "adachess/depths.hpp"
#include "adachess/history.hpp"
#include "adachess/hashes.hpp"
#include "adachess/nodes.hpp"

namespace chess::engine {

// ============================================================================
// Constants
// ============================================================================

/**
 * Maximum number of pieces per side (16 for standard chess).
 */
inline constexpr std::size_t kMaxPiecesPerSide = 16;

/**
 * Total piece slots in the piece list (32 for both sides).
 */
inline constexpr std::size_t kTotalPieceSlots = 32;

/**
 * Range indices for white pieces in the piece list (1-16).
 * Note: Using 1-based indexing to match Ada implementation.
 */
inline constexpr std::size_t kWhitePiecesStart = 1;
inline constexpr std::size_t kWhitePiecesEnd = 16;

/**
 * Range indices for black pieces in the piece list (17-32).
 */
inline constexpr std::size_t kBlackPiecesStart = 17;
inline constexpr std::size_t kBlackPiecesEnd = 32;

/**
 * Start index offset for black pieces (16).
 */
inline constexpr std::size_t kBlackPiecesStartIndex = 16;

/**
 * Maximum size of the move stack.
 * High value needed for loading PGN books with long games.
 */
inline constexpr std::size_t kMoveStackSize = 4096;

/**
 * Fifty-move counter maximum value (100 half-moves = 50 full moves).
 */
inline constexpr std::uint8_t kFiftyMax = 100;

// ============================================================================
// Type Definitions
// ============================================================================

/**
 * Pieces list type - array storing square indices for each piece.
 * Index 0 is unused; indices 1-16 for white, 17-32 for black.
 */
using PiecesList = std::array<std::uint8_t, kTotalPieceSlots + 1>;

/**
 * Piece table type - maps squares to piece list indices.
 * 0 means no piece at that square.
 */
using PieceTable = std::array<std::uint8_t, board::kBoardSize>;

/**
 * Castle history type - tracks castling rights for each ply.
 */
using CastleHistory = std::array<bool, kMaxHistoryDepth + 1>;

/**
 * En passant history type - tracks en passant square for each ply.
 */
using EnPassantHistory = std::array<board::Square, kMaxHistoryDepth + 1>;

/**
 * Moves history list type - tracks game history.
 */
using MovesHistoryList = std::array<HistoryMove, kMaxHistoryDepth + 1>;

/**
 * Move stack type - stores generated moves.
 */
using MoveStack = std::array<Move, kMoveStackSize>;

/**
 * Move stack pointer type - stores first move index for each ply.
 */
using MoveStackPointer = std::array<std::size_t, kMaxDepth + 1>;

/**
 * Fifty-move counter type.
 */
using FiftyCounter = std::uint8_t;

// ============================================================================
// Zobrist Hashing Types and Data
// ============================================================================

/**
 * Hash table for pieces at squares.
 * Indexed by [piece][square].
 */
using HashPiecesTable = std::array<std::array<Hash, board::kBoardSize>, kNumPieceValues>;

/**
 * Hash table for en passant squares.
 */
using HashEnPassantTable = std::array<Hash, board::kBoardSize>;

/**
 * Number of castle possibilities (4: white kingside, white queenside,
 * black kingside, black queenside).
 */
inline constexpr std::size_t kCastlePossibilities = 4;

/**
 * Hash table for castling rights.
 */
using HashCastleTable = std::array<Hash, kCastlePossibilities>;

/**
 * Global hash tables for Zobrist hashing.
 * These are initialized once at startup.
 */
extern HashPiecesTable g_hash_pieces;
extern HashEnPassantTable g_hash_en_passant;
extern HashCastleTable g_hash_castle;
extern Hash g_hash_side;

/**
 * Global position hash value.
 */
extern Hash g_hash;

/**
 * Global search node counter.
 */
extern Node g_search_nodes;

/**
 * Global quiescence node counter.
 */
extern Node g_qnodes;

// ============================================================================
// Chessboard Class
// ============================================================================

/**
 * Chessboard class representing the complete state of a chess game.
 *
 * This class contains:
 * - Board array (piece positions)
 * - Ply counter (current search depth)
 * - Piece tracking (pieces list and piece table)
 * - King positions
 * - Side to move
 * - Castling rights history
 * - En passant history
 * - Move stack for generated moves
 * - Game history for undo
 * - Fifty-move counter
 */
class Chessboard {
public:
    // ========================================================================
    // Public Data Members
    // ========================================================================

    /**
     * The board array containing pieces at each square.
     */
    board::Board square;

    /**
     * Current search ply/depth.
     */
    Depth ply{kMinDepth};

    /**
     * List of piece positions.
     * Index 0 unused; 1-16 white pieces; 17-32 black pieces.
     */
    PiecesList pieces_list{};

    /**
     * Table mapping squares to piece list indices.
     * 0 means no piece at that square.
     */
    PieceTable piece_table{};

    /**
     * Number of white pieces on the board.
     */
    PieceCounter white_pieces_counter{0};

    /**
     * Position of the white king.
     */
    board::Square white_king_position{};

    /**
     * Number of black pieces on the board.
     */
    PieceCounter black_pieces_counter{0};

    /**
     * Position of the black king.
     */
    board::Square black_king_position{};

    /**
     * Which side is to move.
     */
    Color side_to_move{Color::White};

    /**
     * White queenside castling rights history.
     */
    CastleHistory white_castle_queenside{};

    /**
     * White kingside castling rights history.
     */
    CastleHistory white_castle_kingside{};

    /**
     * Black queenside castling rights history.
     */
    CastleHistory black_castle_queenside{};

    /**
     * Black kingside castling rights history.
     */
    CastleHistory black_castle_kingside{};

    /**
     * En passant square history.
     */
    EnPassantHistory en_passant{};

    /**
     * Stack of generated moves.
     */
    MoveStack moves_stack{};

    /**
     * Pointer to first move at each ply.
     */
    MoveStackPointer moves_pointer{};

    /**
     * Current history ply (game ply, not search ply).
     */
    HistoryDepth history_ply{0};

    /**
     * Game move history for undo and repetition detection.
     */
    MovesHistoryList moves_history{};

    /**
     * Fifty-move rule counter.
     */
    FiftyCounter fifty{0};

    /**
     * Flag indicating if validity test is required for the next move.
     * Set when move generator cannot guarantee legality.
     */
    bool force_validity_test{false};

    // ========================================================================
    // Constructors
    // ========================================================================

    /**
     * Default constructor creates an empty chessboard.
     */
    Chessboard() = default;

    // ========================================================================
    // Initialization Methods
    // ========================================================================

    /**
     * Reset the chessboard to an empty state.
     * Sets all values to their "zero" state, clears the board.
     */
    void reset();

    /**
     * Initialize the chessboard to the standard starting position.
     * Resets counters, places pieces, and sets castling rights.
     */
    void initialize();

    // ========================================================================
    // Piece Tracking Methods
    // ========================================================================

    /**
     * Add a white piece to the piece list.
     *
     * @param sq The square where the piece is located.
     */
    void add_white_piece(board::Square sq);

    /**
     * Add a black piece to the piece list.
     *
     * @param sq The square where the piece is located.
     */
    void add_black_piece(board::Square sq);

    /**
     * Delete a white piece from the piece list.
     *
     * @param sq The square where the piece was located.
     */
    void delete_white_piece(board::Square sq);

    /**
     * Delete a black piece from the piece list.
     *
     * @param sq The square where the piece was located.
     */
    void delete_black_piece(board::Square sq);

    /**
     * Update a white piece's position in the piece list.
     *
     * @param from The origin square.
     * @param to The destination square.
     */
    void update_white_piece(board::Square from, board::Square to);

    /**
     * Update a black piece's position in the piece list.
     *
     * @param from The origin square.
     * @param to The destination square.
     */
    void update_black_piece(board::Square from, board::Square to);

    // ========================================================================
    // Hash Methods
    // ========================================================================

    /**
     * Initialize Zobrist hash tables with random values.
     * Should be called once at startup.
     */
    void initialize_hash();

    /**
     * Recalculate the hash value for the current position.
     */
    void update_hash();

    // ========================================================================
    // Move Stack Methods
    // ========================================================================

    /**
     * Clear the move list for the current ply.
     */
    void clear_moves_list();

    /**
     * Get the number of legal moves at a specific ply.
     *
     * @param ply_level The ply to count moves at.
     * @return The number of legal moves.
     */
    [[nodiscard]] std::size_t moves_counter(Depth ply_level) const;

    // ========================================================================
    // Debug/Display Methods
    // ========================================================================

    /**
     * Display the piece table for debugging.
     */
    void display_piece_table() const;
};

// ============================================================================
// Utility Functions
// ============================================================================

/**
 * Initialize the global Zobrist hash tables.
 * Called once at startup.
 */
void initialize_zobrist_tables();

} // namespace chess::engine

#endif // ADACHESS_ENGINE_ENGINE_HPP

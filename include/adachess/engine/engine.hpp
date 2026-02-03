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
#include <optional>

#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/board/board.hpp"
#include "adachess/board/directions.hpp"
#include "adachess/board/attacks_data.hpp"
#include "adachess/moves.hpp"
#include "adachess/depths.hpp"
#include "adachess/history.hpp"
#include "adachess/hashes.hpp"
#include "adachess/nodes.hpp"
#include "adachess/notations.hpp"

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

    // ========================================================================
    // Attack Detection Methods
    // ========================================================================

    /**
     * Check if a side attacks a specific square.
     * Detects if any piece of the given side can reach that square.
     *
     * @param side The side to check for attacks.
     * @param sq The target square.
     * @return true if any piece of the side attacks the square.
     */
    [[nodiscard]] bool attacks(Color side, board::Square sq) const;

    /**
     * Collect all attackers of a square from a specific side.
     *
     * @param side The attacking side.
     * @param sq The target square.
     * @param only_one If true, stop after finding one attacker.
     * @return Collection of attackers found.
     */
    [[nodiscard]] board::AttackCollection attacking_square(
        Color side, board::Square sq, bool only_one) const;

    /**
     * Find all defenders of a square (pieces not pinned).
     *
     * @param side The defending side.
     * @param sq The target square.
     * @return Collection of unpinned defenders.
     */
    [[nodiscard]] board::AttackCollection defending_square(
        Color side, board::Square sq) const;

    // ========================================================================
    // Check Detection Methods
    // ========================================================================

    /**
     * Check if the king of the specified side is in check.
     *
     * @param side The side whose king to check.
     * @return true if the king is in check.
     */
    [[nodiscard]] bool has_king_in_check(Color side) const;

    /**
     * Check if the white king is in check.
     * @return true if white king is under attack.
     */
    [[nodiscard]] bool white_has_king_in_check();

    /**
     * Check if the black king is in check.
     * @return true if black king is under attack.
     */
    [[nodiscard]] bool black_has_king_in_check();

    // ========================================================================
    // Pin Detection Methods
    // ========================================================================

    /**
     * Detect the absolute pin direction for a piece on a square.
     * The piece must not be a king.
     *
     * @param sq The square where the piece is located.
     * @return The direction of the pin, or kNoDirection if not pinned.
     */
    [[nodiscard]] board::Direction absolute_pin_direction(board::Square sq) const;

    /**
     * Detect if a piece is absolutely pinned (from opponent's perspective).
     *
     * @param sq The square where the piece is located.
     * @return The direction of the pin, or kNoDirection if not pinned.
     */
    [[nodiscard]] board::Direction piece_is_absolute_pinned(board::Square sq) const;

    // ========================================================================
    // Legality Testing Methods
    // ========================================================================

    /**
     * Check if a move would leave the own king in check.
     *
     * @param move The move to test.
     * @return true if the move would be illegal (king left in check).
     */
    [[nodiscard]] bool move_leaves_king_in_check(const Move& move);

    /**
     * Determine if/how a move checks the opponent's king.
     *
     * @param move The move to analyze.
     * @return The type of check delivered.
     */
    [[nodiscard]] CheckType move_checks_opponent_king(const Move& move);

    /**
     * Check if the king can escape from check.
     *
     * @param type_of_check The current check type.
     * @param the_move Optional move being tested (for checkmate detection during
     *                 move generation when move hasn't been stored in history yet).
     *                 If nullopt, uses last_move_made() to get the attacking move info.
     * @return true if at least one legal move exists.
     */
    [[nodiscard]] bool king_has_escapes(CheckType type_of_check,
                                         const std::optional<Move>& the_move = std::nullopt);

    // ========================================================================
    // Move Registration Methods
    // ========================================================================

    /**
     * Register a move from source to destination.
     * Creates a standard move and adds it to the stack.
     *
     * @param from Source square.
     * @param to Destination square.
     */
    void register_move(board::Square from, board::Square to);

    /**
     * Register a move with a specific flag.
     * Handles promotions by generating all 4 promotion moves.
     *
     * @param from Source square.
     * @param to Destination square.
     * @param flag Move flag.
     */
    void register_move(board::Square from, board::Square to, MoveFlag flag);

    /**
     * Register a fully constructed move.
     * Performs legality test and adds check information.
     *
     * @param move The move to register.
     */
    void register_move(const Move& move);

    /**
     * Register a tactical move (only if it's tactical).
     *
     * @param from Source square.
     * @param to Destination square.
     */
    void register_tactical_move(board::Square from, board::Square to);

    /**
     * Register a tactical move with flag (only if it's tactical).
     *
     * @param from Source square.
     * @param to Destination square.
     * @param flag Move flag.
     */
    void register_tactical_move(board::Square from, board::Square to, MoveFlag flag);

    /**
     * Register a tactical move (only if it's tactical).
     *
     * @param move The move to register.
     */
    void register_tactical_move(const Move& move);

    // ========================================================================
    // Move Generation Methods
    // ========================================================================

    /**
     * Generate all legal moves from the current position.
     * Populates the move stack at the current ply.
     */
    void generate_moves();

    /**
     * Generate legal moves when the king is in check.
     * Optimized for check evasion scenarios.
     */
    void generate_check_evasion();

    /**
     * Generate tactical moves (captures, checks, promotions).
     * Used for quiescence search.
     */
    void generate_tactical_moves();

    /**
     * Generate capture moves only.
     */
    void generate_captures();

    /**
     * Generate moves to a specific target square.
     * Used for static exchange evaluation.
     *
     * @param target The target square.
     */
    void generate_see_moves(board::Square target);

    /**
     * Generate SEE check evasion moves to a target square.
     *
     * @param see_target The SEE target square.
     */
    void generate_see_check_evasion(board::Square see_target);

    // ========================================================================
    // Play/Undo Move Methods
    // ========================================================================

    /**
     * Apply a move to the board.
     * Updates all state: board array, piece tracking, castling rights,
     * en passant, fifty-move counter, ply, side to move, and hash.
     *
     * @param move The move to apply.
     */
    void play(const Move& move);

    /**
     * Undo the last move.
     * Restores all state from the move history.
     */
    void undo();

    /**
     * Play a null move (pass).
     * Used in null-move pruning. Switches side without moving pieces.
     */
    void play_null_move();

    /**
     * Undo a null move.
     * Reverses the null move operation.
     */
    void undo_null_move();

    /**
     * Play a move for check detection (lightweight version).
     * Skips fifty-move and some castling updates.
     * Used for mate detection where full state isn't needed.
     *
     * @param move The move to apply.
     */
    void play_check_move(const Move& move);

    /**
     * Undo a check move (lightweight version).
     * Reverses play_check_move.
     */
    void undo_check_move();

    /**
     * Play a move for SEE (static exchange evaluation).
     * Minimal updates for SEE calculations.
     *
     * @param move The move to apply.
     */
    void play_see_move(const Move& move);

    /**
     * Undo a SEE move.
     * Reverses play_see_move.
     */
    void undo_see_move();

    // ========================================================================
    // Move History Methods
    // ========================================================================

    /**
     * Get the last move made on the chessboard.
     *
     * @return The last move, or an empty move if no moves played.
     */
    [[nodiscard]] Move last_move_made() const;

    // ========================================================================
    // Ambiguous Notation Detection
    // ========================================================================

    /**
     * Detect if a move requires disambiguation in algebraic notation.
     *
     * @param move The move to check.
     * @return The type of disambiguation required.
     */
    [[nodiscard]] AmbiguousFlag detect_ambiguous_move_notation(const Move& move);

    // ========================================================================
    // Move Parsing Methods
    // ========================================================================

    /**
     * Parse a move from a string in any supported notation.
     *
     * The function generates all legal moves for the current position and
     * attempts to match the input string against each move converted to
     * all supported notation formats. This allows the parser to accept
     * any notation without needing to determine the format first.
     *
     * @param input The move string to parse (e.g., "e4", "Nf3", "e2e4", "5254").
     * @return The matching legal move, or kEmptyMove if no match found.
     */
    [[nodiscard]] Move parse_move(const std::string& input);

    // ========================================================================
    // Move List Methods
    // ========================================================================

    /**
     * Print all legal moves for the current position to the output stream.
     *
     * Generates legal moves (if not already generated) and prints each move
     * in the specified notation, separated by spaces.
     *
     * @param notation The notation format to use (default: current default).
     * @param out The output stream (default: std::cout).
     */
    void print_moves_list(
        NotationType notation = default_notation(),
        std::ostream& out = std::cout);
};

// ============================================================================
// Move Classification Functions
// ============================================================================

/**
 * Check if a move is tactical (check, capture, promotion, castle).
 * Matches Ada's Move_Is_Tactical function.
 *
 * @param move The move to check.
 * @return true if the move is tactical.
 */
[[nodiscard]] inline bool move_is_tactical(const Move& move) noexcept {
    return move.check != CheckType::NoCheck ||
           move.captured != Piece::Empty ||
           move.flag == MoveFlag::CaptureEnPassant ||
           move.promotion != Piece::Empty ||
           move.flag == MoveFlag::Castle;
}

/**
 * Check if a move is quiet (not tactical).
 * Matches Ada's Move_Is_Quiet function.
 *
 * @param move The move to check.
 * @return true if the move is quiet.
 */
[[nodiscard]] inline bool move_is_quiet(const Move& move) noexcept {
    return !move_is_tactical(move);
}

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

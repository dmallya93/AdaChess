/**
 * AdaChess - Smart Chess Engine
 *
 * Static evaluation implementation - full position evaluation.
 * C++ port of the original Ada implementation (chess-engine-evaluations-static_evaluations.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/engine/evaluations/static_evaluations.hpp"
#include "adachess/engine/engine.hpp"
#include "adachess/board/directions.hpp"
#include "adachess/board/attacks_data.hpp"

#include <algorithm>
#include <cassert>
#include <cstdlib>

namespace chess::engine::evaluations {

using namespace chess::board;

// ============================================================================
// Global Variables
// ============================================================================

std::array<std::array<Distance, kBoardSize>, kBoardSize> g_knight_distance_table{};

// ============================================================================
// Local Data Structures for Evaluation Context
// ============================================================================

namespace {

// Sentinel value for piece location arrays
inline constexpr std::uint8_t kNotPresent = 0;

// Material count per piece type
std::array<Score, kNumPieceValues> g_material{};

// Piece location arrays
std::array<Square, 17> g_white_piece_location{};
std::array<Square, 17> g_black_piece_location{};

// Pawn structure arrays (indexed by file 0-7)
std::array<std::uint8_t, 8> g_white_pawn_files{};
std::array<std::uint8_t, 8> g_black_pawn_files{};

// Most advanced pawn on each file (rank values)
std::array<Coordinate, 8> g_white_pawn_ranks{};  // Highest white pawn rank per file
std::array<Coordinate, 8> g_black_pawn_ranks{};  // Lowest black pawn rank per file

// Passed pawn tracking
std::array<Coordinate, 8> g_white_passed_pawn{};  // Rank of white passed pawn
std::array<Coordinate, 8> g_black_passed_pawn{};  // Rank of black passed pawn

// Pawn structure details
struct PawnStructure {
    std::array<bool, 8> doubled{};      // Doubled pawns on file
    std::array<bool, 8> isolated{};     // Isolated pawn on file
    std::array<bool, 8> weak{};         // Weak pawn on file
    std::array<bool, 8> backwards{};    // Backward pawn on file
    std::array<bool, 8> connected{};    // Connected pawn on file
    std::array<bool, 8> hanging{};      // Hanging pawn on file
    Score islands{0};                   // Number of pawn islands

    void reset() {
        doubled.fill(false);
        isolated.fill(false);
        weak.fill(false);
        backwards.fill(false);
        connected.fill(false);
        hanging.fill(false);
        islands = 0;
    }
};

PawnStructure g_white_pawn_structure{};
PawnStructure g_black_pawn_structure{};

// Game phase value (0 = endgame, 100 = opening)
Score g_game_phase{100};

// Current endgame pattern
EndgamePattern g_endgame_pattern{EndgamePattern::NoPattern};

// Pointer to the chessboard being evaluated
const Chessboard* g_chessboard{nullptr};

// ============================================================================
// Helper Functions
// ============================================================================

/**
 * Apply tapered evaluation between opening and endgame scores.
 */
[[nodiscard]] Score tapered_score(const TaperedScore& score) noexcept {
    return (score.opening * g_game_phase + score.endgame * (100 - g_game_phase)) / 100;
}

/**
 * Get file index from square (returns -1 for frame squares).
 */
[[nodiscard]] inline Coordinate get_file(Square sq) noexcept {
    return file(sq);
}

/**
 * Get rank from square (returns -1 for frame squares).
 */
[[nodiscard]] inline Coordinate get_rank(Square sq) noexcept {
    return rank(sq);
}

// Note: find_sliding_direction is provided by board/attacks_data.hpp

/**
 * Detect absolute pin for a piece.
 * Returns the direction of the pin, or kNoDirection if not pinned.
 */
[[nodiscard]] Direction piece_is_hanging(Square sq) noexcept {
    assert(g_chessboard != nullptr);

    const Piece piece = g_chessboard->square[sq];
    if (!is_chess_piece(piece)) {
        return kNoDirection;
    }

    const Color pc_color = chess::piece_color(piece);
    const Square king_pos = (pc_color == Color::White)
                              ? g_chessboard->white_king_position
                              : g_chessboard->black_king_position;

    const Direction dir = chess::board::find_sliding_direction(sq, king_pos);
    if (dir == kNoDirection) {
        return kNoDirection;
    }

    // Check if there's a clear path to king
    Square target = sq + dir;
    while (target != king_pos) {
        if (g_chessboard->square[target] != Piece::Empty) {
            return kNoDirection;  // Something blocks the path
        }
        target = target + dir;
    }

    // Check if there's an attacker in the opposite direction
    const Direction opposite_dir = opposite(dir);
    target = sq + opposite_dir;
    Piece blocker = g_chessboard->square[target];

    while (blocker == Piece::Empty) {
        target = target + opposite_dir;
        blocker = g_chessboard->square[target];
    }

    if (blocker == Piece::Frame) {
        return kNoDirection;
    }

    // Check if it's an enemy slider that can attack along this direction
    const bool is_enemy = (pc_color == Color::White) ? is_black_piece(blocker) : is_white_piece(blocker);
    if (!is_enemy) {
        return kNoDirection;
    }

    // Check if the enemy piece can attack along this direction
    const bool is_diag = is_diagonal(dir);
    if (is_diag) {
        if (is_bishop(blocker) || is_queen(blocker)) {
            return dir;
        }
    } else {
        if (is_rook(blocker) || is_queen(blocker)) {
            return dir;
        }
    }

    return kNoDirection;
}

/**
 * Collect pawn and piece location information.
 */
void collect_information() noexcept {
    assert(g_chessboard != nullptr);

    // Reset arrays
    g_material.fill(0);
    g_white_piece_location.fill(Square{kNotPresent});
    g_black_piece_location.fill(Square{kNotPresent});
    g_white_pawn_files.fill(0);
    g_black_pawn_files.fill(0);
    g_white_pawn_ranks.fill(kRank1);  // Default: no white pawn found
    g_black_pawn_ranks.fill(kRank8);  // Default: no black pawn found
    g_white_passed_pawn.fill(kRank1);
    g_black_passed_pawn.fill(kRank8);
    g_white_pawn_structure.reset();
    g_black_pawn_structure.reset();

    std::size_t white_idx = 1;
    std::size_t black_idx = 1;

    // Scan all squares
    for (std::uint8_t idx = 0; idx < kBoardSize; ++idx) {
        const Square sq{idx};
        if (is_frame(sq)) continue;

        const Piece piece = g_chessboard->square[sq];
        if (!is_chess_piece(piece)) continue;

        // Count material
        g_material[to_index(piece)]++;

        // Track piece locations
        if (is_white_piece(piece)) {
            if (white_idx < g_white_piece_location.size()) {
                g_white_piece_location[white_idx++] = sq;
            }
            if (piece == Piece::WhitePawn) {
                const auto f = get_file(sq);
                const auto r = get_rank(sq);
                if (f >= 0 && f < 8) {
                    g_white_pawn_files[static_cast<std::size_t>(f)]++;
                    if (r > g_white_pawn_ranks[static_cast<std::size_t>(f)]) {
                        g_white_pawn_ranks[static_cast<std::size_t>(f)] = r;
                    }
                }
            }
        } else {
            if (black_idx < g_black_piece_location.size()) {
                g_black_piece_location[black_idx++] = sq;
            }
            if (piece == Piece::BlackPawn) {
                const auto f = get_file(sq);
                const auto r = get_rank(sq);
                if (f >= 0 && f < 8) {
                    g_black_pawn_files[static_cast<std::size_t>(f)]++;
                    if (r < g_black_pawn_ranks[static_cast<std::size_t>(f)]) {
                        g_black_pawn_ranks[static_cast<std::size_t>(f)] = r;
                    }
                }
            }
        }
    }

    // Analyze pawn structure
    for (std::size_t f = 0; f < 8; ++f) {
        // Doubled pawns
        if (g_white_pawn_files[f] > 1) {
            g_white_pawn_structure.doubled[f] = true;
        }
        if (g_black_pawn_files[f] > 1) {
            g_black_pawn_structure.doubled[f] = true;
        }

        // Isolated pawns
        const bool white_left = (f > 0) && (g_white_pawn_files[f - 1] > 0);
        const bool white_right = (f < 7) && (g_white_pawn_files[f + 1] > 0);
        const bool black_left = (f > 0) && (g_black_pawn_files[f - 1] > 0);
        const bool black_right = (f < 7) && (g_black_pawn_files[f + 1] > 0);

        if (g_white_pawn_files[f] > 0 && !white_left && !white_right) {
            g_white_pawn_structure.isolated[f] = true;
        }
        if (g_black_pawn_files[f] > 0 && !black_left && !black_right) {
            g_black_pawn_structure.isolated[f] = true;
        }

        // Connected pawns
        if (g_white_pawn_files[f] > 0 && (white_left || white_right)) {
            g_white_pawn_structure.connected[f] = true;
        }
        if (g_black_pawn_files[f] > 0 && (black_left || black_right)) {
            g_black_pawn_structure.connected[f] = true;
        }

        // Calculate pawn islands
        if (g_white_pawn_files[f] > 0 && (f == 0 || g_white_pawn_files[f - 1] == 0)) {
            g_white_pawn_structure.islands++;
        }
        if (g_black_pawn_files[f] > 0 && (f == 0 || g_black_pawn_files[f - 1] == 0)) {
            g_black_pawn_structure.islands++;
        }
    }

    // Detect passed pawns
    for (std::size_t f = 0; f < 8; ++f) {
        // White passed pawns: no black pawns on same or adjacent files in front
        if (g_white_pawn_files[f] > 0) {
            bool is_passed = true;
            const auto pawn_rank = g_white_pawn_ranks[f];

            // Check same file
            if (g_black_pawn_files[f] > 0 && g_black_pawn_ranks[f] > pawn_rank) {
                is_passed = false;
            }
            // Check left file
            if (is_passed && f > 0 && g_black_pawn_files[f - 1] > 0) {
                if (g_black_pawn_ranks[f - 1] > pawn_rank) {
                    is_passed = false;
                }
            }
            // Check right file
            if (is_passed && f < 7 && g_black_pawn_files[f + 1] > 0) {
                if (g_black_pawn_ranks[f + 1] > pawn_rank) {
                    is_passed = false;
                }
            }

            if (is_passed) {
                g_white_passed_pawn[f] = pawn_rank;
            }
        }

        // Black passed pawns
        if (g_black_pawn_files[f] > 0) {
            bool is_passed = true;
            const auto pawn_rank = g_black_pawn_ranks[f];

            // Check same file
            if (g_white_pawn_files[f] > 0 && g_white_pawn_ranks[f] < pawn_rank) {
                is_passed = false;
            }
            // Check left file
            if (is_passed && f > 0 && g_white_pawn_files[f - 1] > 0) {
                if (g_white_pawn_ranks[f - 1] < pawn_rank) {
                    is_passed = false;
                }
            }
            // Check right file
            if (is_passed && f < 7 && g_white_pawn_files[f + 1] > 0) {
                if (g_white_pawn_ranks[f + 1] < pawn_rank) {
                    is_passed = false;
                }
            }

            if (is_passed) {
                g_black_passed_pawn[f] = pawn_rank;
            }
        }
    }
}

/**
 * Check for insufficient material (automatic draw).
 */
[[nodiscard]] bool insufficient_material() noexcept {
    // K vs K
    if (g_endgame_pattern == EndgamePattern::KK) {
        return true;
    }

    // K+B vs K or K vs K+B
    if (g_endgame_pattern == EndgamePattern::KB_K || g_endgame_pattern == EndgamePattern::K_KB) {
        return true;
    }

    // K+N vs K or K vs K+N
    if (g_endgame_pattern == EndgamePattern::KN_K || g_endgame_pattern == EndgamePattern::K_KN) {
        return true;
    }

    // K+B vs K+B (same colored bishops)
    if (g_endgame_pattern == EndgamePattern::KB_KB) {
        // Find bishop squares and check colors
        Square white_bishop_sq{0};
        Square black_bishop_sq{0};

        for (std::size_t i = 1; i < g_white_piece_location.size() && g_white_piece_location[i].index() != kNotPresent; ++i) {
            if (g_chessboard->square[g_white_piece_location[i]] == Piece::WhiteBishop) {
                white_bishop_sq = g_white_piece_location[i];
                break;
            }
        }

        for (std::size_t i = 1; i < g_black_piece_location.size() && g_black_piece_location[i].index() != kNotPresent; ++i) {
            if (g_chessboard->square[g_black_piece_location[i]] == Piece::BlackBishop) {
                black_bishop_sq = g_black_piece_location[i];
                break;
            }
        }

        if (square_color(white_bishop_sq) == square_color(black_bishop_sq)) {
            return true;
        }
    }

    return false;
}

/**
 * Detect endgame pattern based on material.
 */
[[nodiscard]] EndgamePattern detect_endgame_pattern() noexcept {
    const auto white_pawns = g_material[to_index(Piece::WhitePawn)];
    const auto white_knights = g_material[to_index(Piece::WhiteKnight)];
    const auto white_bishops = g_material[to_index(Piece::WhiteBishop)];
    const auto white_rooks = g_material[to_index(Piece::WhiteRook)];
    const auto white_queens = g_material[to_index(Piece::WhiteQueen)];

    const auto black_pawns = g_material[to_index(Piece::BlackPawn)];
    const auto black_knights = g_material[to_index(Piece::BlackKnight)];
    const auto black_bishops = g_material[to_index(Piece::BlackBishop)];
    const auto black_rooks = g_material[to_index(Piece::BlackRook)];
    const auto black_queens = g_material[to_index(Piece::BlackQueen)];

    const auto white_pieces = white_pawns + white_knights + white_bishops + white_rooks + white_queens;
    const auto black_pieces = black_pawns + black_knights + black_bishops + black_rooks + black_queens;

    // K vs K
    if (white_pieces == 0 && black_pieces == 0) {
        return EndgamePattern::KK;
    }

    // KQ vs K
    if (white_queens == 1 && white_pieces == 1 && black_pieces == 0) {
        return EndgamePattern::KQ_K;
    }
    if (black_queens == 1 && black_pieces == 1 && white_pieces == 0) {
        return EndgamePattern::K_KQ;
    }

    // KR vs K
    if (white_rooks == 1 && white_pieces == 1 && black_pieces == 0) {
        return EndgamePattern::KR_K;
    }
    if (black_rooks == 1 && black_pieces == 1 && white_pieces == 0) {
        return EndgamePattern::K_KR;
    }

    // KB vs K
    if (white_bishops == 1 && white_pieces == 1 && black_pieces == 0) {
        return EndgamePattern::KB_K;
    }
    if (black_bishops == 1 && black_pieces == 1 && white_pieces == 0) {
        return EndgamePattern::K_KB;
    }

    // KN vs K
    if (white_knights == 1 && white_pieces == 1 && black_pieces == 0) {
        return EndgamePattern::KN_K;
    }
    if (black_knights == 1 && black_pieces == 1 && white_pieces == 0) {
        return EndgamePattern::K_KN;
    }

    // KP vs K
    if (white_pawns == 1 && white_pieces == 1 && black_pieces == 0) {
        return EndgamePattern::KP_K;
    }
    if (black_pawns == 1 && black_pieces == 1 && white_pieces == 0) {
        return EndgamePattern::K_KP;
    }

    // KB vs KB
    if (white_bishops == 1 && white_pieces == 1 && black_bishops == 1 && black_pieces == 1) {
        return EndgamePattern::KB_KB;
    }

    // KN vs KN
    if (white_knights == 1 && white_pieces == 1 && black_knights == 1 && black_pieces == 1) {
        return EndgamePattern::KN_KN;
    }

    // KR vs KR
    if (white_rooks == 1 && white_pieces == 1 && black_rooks == 1 && black_pieces == 1) {
        return EndgamePattern::KR_KR;
    }

    // KP vs KP
    if (white_pieces == white_pawns && black_pieces == black_pawns &&
        white_pawns > 0 && black_pawns > 0) {
        return EndgamePattern::KP_KP;
    }

    return EndgamePattern::NoPattern;
}

/**
 * Calculate attacks on king zone.
 */
[[nodiscard]] Score attacks_king_zone(Color side) noexcept {
    assert(g_chessboard != nullptr);

    const Square king_pos = (side == Color::White)
                              ? g_chessboard->white_king_position
                              : g_chessboard->black_king_position;

    const auto& enemy_pieces = (side == Color::White)
                                 ? g_black_piece_location
                                 : g_white_piece_location;

    Score attack_count = 0;
    Score attack_value = 0;

    // Check attacks from enemy pieces
    for (std::size_t i = 1; i < enemy_pieces.size() && enemy_pieces[i].index() != kNotPresent; ++i) {
        const Square sq = enemy_pieces[i];
        const Piece piece = g_chessboard->square[sq];

        // Skip pawns and kings
        if (is_pawn(piece) || is_king(piece)) continue;

        // Check if piece attacks king zone (squares around the king)
        for (int dr = -1; dr <= 1; ++dr) {
            for (int df = -1; df <= 1; ++df) {
                const Square target = king_pos + (dr * kSouth + df * kEast);
                if (is_frame(target)) continue;

                // Simple attack detection
                const Direction dir = chess::board::find_sliding_direction(sq, target);
                if (dir != kNoDirection) {
                    // For sliding pieces, check if path is clear
                    if (is_sliding_piece(piece)) {
                        bool clear = true;
                        Square check_sq = sq + dir;
                        while (check_sq != target) {
                            if (g_chessboard->square[check_sq] != Piece::Empty) {
                                clear = false;
                                break;
                            }
                            check_sq = check_sq + dir;
                        }
                        if (clear) {
                            attack_count++;
                            attack_value += kKingAttackValue[to_index(piece)];
                        }
                    }
                }

                // Knight attacks
                if (is_knight(piece)) {
                    for (const auto offset : kKnightOffsets) {
                        if (sq + offset == target) {
                            attack_count++;
                            attack_value += kKingAttackValue[to_index(piece)];
                            break;
                        }
                    }
                }
            }
        }
    }

    // Scale by attack weight
    const auto attack_idx = std::min(static_cast<std::size_t>(attack_count), kAttackWeight.size() - 1);
    return (attack_value * kAttackWeight[attack_idx]) / kAttackingKingFactor;
}

// ============================================================================
// Material Evaluation
// ============================================================================

[[nodiscard]] Score evaluate_white_material() noexcept {
    TaperedScore score{0, 0};

    score += static_cast<int>(g_material[to_index(Piece::WhitePawn)]) * kPawnScore;
    score += static_cast<int>(g_material[to_index(Piece::WhiteKnight)]) * kKnightScore;
    score += static_cast<int>(g_material[to_index(Piece::WhiteBishop)]) * kBishopScore;
    score += static_cast<int>(g_material[to_index(Piece::WhiteRook)]) * kRookScore;
    score += static_cast<int>(g_material[to_index(Piece::WhiteQueen)]) * kQueenScore;

    // Bishop pair bonus
    if (g_material[to_index(Piece::WhiteBishop)] >= 2) {
        score += kBishopPairBonus;
    }

    return tapered_score(score);
}

[[nodiscard]] Score evaluate_black_material() noexcept {
    TaperedScore score{0, 0};

    score += static_cast<int>(g_material[to_index(Piece::BlackPawn)]) * kPawnScore;
    score += static_cast<int>(g_material[to_index(Piece::BlackKnight)]) * kKnightScore;
    score += static_cast<int>(g_material[to_index(Piece::BlackBishop)]) * kBishopScore;
    score += static_cast<int>(g_material[to_index(Piece::BlackRook)]) * kRookScore;
    score += static_cast<int>(g_material[to_index(Piece::BlackQueen)]) * kQueenScore;

    // Bishop pair bonus
    if (g_material[to_index(Piece::BlackBishop)] >= 2) {
        score += kBishopPairBonus;
    }

    return tapered_score(score);
}

// ============================================================================
// Pawn Structure Evaluation
// ============================================================================

[[nodiscard]] Score evaluate_white_pawn_structure() noexcept {
    TaperedScore score{0, 0};

    for (std::size_t f = 0; f < 8; ++f) {
        if (g_white_pawn_files[f] == 0) continue;

        // Doubled pawns
        if (g_white_pawn_structure.doubled[f]) {
            score += kDoubledPawnPenalty;
        }

        // Isolated pawns
        if (g_white_pawn_structure.isolated[f]) {
            if (f == static_cast<std::size_t>(kFileD)) {
                score += kIsolaniPawnPenalty;  // Isolated queen pawn
            } else {
                score += kIsolatedPawnPenalty;
            }
        }

        // Connected pawns
        if (g_white_pawn_structure.connected[f]) {
            score += kConnectedPawnBonus;
        }

        // Passed pawns
        if (g_white_passed_pawn[f] > kRank1) {
            const auto rank_idx = static_cast<std::size_t>(g_white_passed_pawn[f] - kRank2);
            if (rank_idx < 6) {
                score.opening += kWhitePassedPawnBonus[0][rank_idx];
                score.endgame += kWhitePassedPawnBonus[1][rank_idx];
            }
        }
    }

    // Pawn islands penalty
    if (g_white_pawn_structure.islands > 1) {
        score += static_cast<int>(g_white_pawn_structure.islands - 1) * kPawnIslandPenaltyFactor;
    }

    return tapered_score(score);
}

[[nodiscard]] Score evaluate_black_pawn_structure() noexcept {
    TaperedScore score{0, 0};

    for (std::size_t f = 0; f < 8; ++f) {
        if (g_black_pawn_files[f] == 0) continue;

        // Doubled pawns
        if (g_black_pawn_structure.doubled[f]) {
            score += kDoubledPawnPenalty;
        }

        // Isolated pawns
        if (g_black_pawn_structure.isolated[f]) {
            if (f == static_cast<std::size_t>(kFileD)) {
                score += kIsolaniPawnPenalty;
            } else {
                score += kIsolatedPawnPenalty;
            }
        }

        // Connected pawns
        if (g_black_pawn_structure.connected[f]) {
            score += kConnectedPawnBonus;
        }

        // Passed pawns
        if (g_black_passed_pawn[f] < kRank8) {
            const auto rank_idx = static_cast<std::size_t>(kRank7 - g_black_passed_pawn[f]);
            if (rank_idx < 6) {
                score.opening += kBlackPassedPawnBonus[0][rank_idx];
                score.endgame += kBlackPassedPawnBonus[1][rank_idx];
            }
        }
    }

    // Pawn islands penalty
    if (g_black_pawn_structure.islands > 1) {
        score += static_cast<int>(g_black_pawn_structure.islands - 1) * kPawnIslandPenaltyFactor;
    }

    return tapered_score(score);
}

// ============================================================================
// Mobility Evaluation
// ============================================================================

[[nodiscard]] Score evaluate_white_mobility() noexcept {
    assert(g_chessboard != nullptr);

    TaperedScore score{0, 0};

    for (std::size_t i = 1; i < g_white_piece_location.size() && g_white_piece_location[i].index() != kNotPresent; ++i) {
        const Square sq = g_white_piece_location[i];
        const Piece piece = g_chessboard->square[sq];

        switch (piece) {
            case Piece::WhitePawn:
            case Piece::WhiteKing:
                // Pawns and kings evaluated separately
                break;

            case Piece::WhiteKnight: {
                const Direction hanging_dir = piece_is_hanging(sq);
                Score mobility = -kKnightMobilityPenalty;

                for (const auto offset : kKnightOffsets) {
                    if (hanging_dir == kNoDirection || hanging_dir == offset || hanging_dir == -offset) {
                        const Square target = sq + offset;
                        const Piece blocker = g_chessboard->square[target];
                        mobility += kWhiteMobilityUnit[to_index(blocker)];
                    }
                }

                score += mobility * kKnightMobilityBonus;
                score.opening += kKnightPositionTable[sq.to_index()];
                break;
            }

            case Piece::WhiteBishop: {
                const Direction hanging_dir = piece_is_hanging(sq);
                Score mobility = -kBishopMobilityPenalty;

                for (const auto offset : kBishopOffsets) {
                    if (hanging_dir == kNoDirection || hanging_dir == offset || hanging_dir == -offset) {
                        Square target = sq + offset;
                        Piece blocker = g_chessboard->square[target];

                        while (blocker == Piece::Empty || blocker == Piece::WhiteQueen) {
                            mobility += kWhiteMobilityUnit[to_index(blocker)];
                            target = target + offset;
                            blocker = g_chessboard->square[target];
                        }
                        mobility += kWhiteMobilityUnit[to_index(blocker)];
                    }
                }

                score += mobility * kBishopMobilityBonus;
                break;
            }

            case Piece::WhiteRook: {
                const Direction hanging_dir = piece_is_hanging(sq);
                Score mobility = -kRookMobilityPenalty;

                for (const auto offset : kRookOffsets) {
                    if (hanging_dir == kNoDirection || hanging_dir == offset || hanging_dir == -offset) {
                        Square target = sq + offset;
                        Piece blocker = g_chessboard->square[target];

                        while (blocker == Piece::Empty || blocker == Piece::WhiteRook) {
                            mobility += kWhiteMobilityUnit[to_index(blocker)];
                            target = target + offset;
                            blocker = g_chessboard->square[target];
                        }
                        mobility += kWhiteMobilityUnit[to_index(blocker)];
                    }
                }

                score += mobility * kRookMobilityBonus;

                // Rook on (semi)open file
                const auto rook_file = get_file(sq);
                const auto rook_rank = get_rank(sq);

                if (rook_file >= 0 && rook_file < 8) {
                    const auto f = static_cast<std::size_t>(rook_file);
                    if (g_white_pawn_files[f] == 0) {
                        if (g_black_pawn_files[f] == 0) {
                            score.opening += kRookOnOpenFile;
                        } else {
                            score.opening += kRookOnSemiOpenFile;
                            if (g_black_pawn_structure.backwards[f]) {
                                score.opening += kRookOnSemiOpenWithBackwardPawn;
                            }
                        }
                    }
                }

                // Rook on 7th rank
                if (rook_rank == kRank7 && get_rank(g_chessboard->black_king_position) == kRank8) {
                    score += kRookOn7thRank;
                }

                // Tarrash rule (rook behind passed pawn)
                if (rook_file >= 0 && rook_file < 8) {
                    const auto f = static_cast<std::size_t>(rook_file);
                    if (g_white_passed_pawn[f] > kRank1 && rook_rank < g_white_passed_pawn[f]) {
                        score.endgame += kTarrashRule;
                    }
                    if (g_black_passed_pawn[f] < kRank8 && rook_rank < g_black_passed_pawn[f]) {
                        score.endgame += kTarrashRule;
                    }
                }
                break;
            }

            case Piece::WhiteQueen: {
                const Direction hanging_dir = piece_is_hanging(sq);
                Score mobility = -kQueenMobilityPenalty;

                for (const auto offset : kQueenOffsets) {
                    if (hanging_dir == kNoDirection || hanging_dir == offset || hanging_dir == -offset) {
                        Square target = sq + offset;
                        Piece blocker = g_chessboard->square[target];

                        while (blocker == Piece::Empty || blocker == Piece::WhiteBishop) {
                            mobility += kWhiteMobilityUnit[to_index(blocker)];
                            target = target + offset;
                            blocker = g_chessboard->square[target];
                        }
                        mobility += kWhiteMobilityUnit[to_index(blocker)];
                    }
                }

                score += mobility * kQueenMobilityBonus;

                // Queen on 7th rank
                if (get_rank(sq) == kRank7 && get_rank(g_chessboard->black_king_position) == kRank8) {
                    score += kQueenOn7thRank;
                }
                break;
            }

            default:
                break;
        }
    }

    // Fianchetto bonus
    if (g_chessboard->square[G2] == Piece::WhiteBishop) {
        score.opening += kFianchetto;
    }
    if (g_chessboard->square[B2] == Piece::WhiteBishop) {
        score.opening += kFianchetto;
    }

    // Trapped bishop
    if (g_chessboard->square[A7] == Piece::WhiteBishop && g_chessboard->square[B6] == Piece::BlackPawn) {
        score -= kTrappedBishop;
    } else if (g_chessboard->square[B8] == Piece::WhiteBishop && g_chessboard->square[C7] == Piece::BlackPawn) {
        score -= kTrappedBishop;
    }

    if (g_chessboard->square[H7] == Piece::WhiteBishop && g_chessboard->square[G6] == Piece::BlackPawn) {
        score -= kTrappedBishop;
    } else if (g_chessboard->square[G8] == Piece::WhiteBishop && g_chessboard->square[F7] == Piece::BlackPawn) {
        score -= kTrappedBishop;
    }

    if (g_chessboard->square[A6] == Piece::WhiteBishop && g_chessboard->square[B5] == Piece::BlackPawn) {
        score -= kTrappedBishop / 2;
    } else if (g_chessboard->square[H6] == Piece::WhiteBishop && g_chessboard->square[G5] == Piece::BlackPawn) {
        score -= kTrappedBishop / 2;
    }

    // Blocked bishop
    if (g_chessboard->square[C1] == Piece::WhiteBishop &&
        g_chessboard->square[D2] == Piece::WhitePawn &&
        g_chessboard->square[D3] != Piece::Empty) {
        score.opening -= kBlockedBishop;
    }

    if (g_chessboard->square[F1] == Piece::WhiteBishop &&
        g_chessboard->square[E2] == Piece::WhitePawn &&
        g_chessboard->square[E3] != Piece::Empty) {
        score.opening -= kBlockedBishop;
    }

    // Blocked rook
    const Square king_pos = g_chessboard->white_king_position;
    if (king_pos == C1 || king_pos == B1) {
        if (g_chessboard->square[A1] == Piece::WhiteRook ||
            g_chessboard->square[A2] == Piece::WhiteRook ||
            g_chessboard->square[B1] == Piece::WhiteRook) {
            score.opening -= kBlockedRook;
        }
    }

    if (king_pos == F1 || king_pos == G1) {
        if (g_chessboard->square[H1] == Piece::WhiteRook ||
            g_chessboard->square[H2] == Piece::WhiteRook ||
            g_chessboard->square[G1] == Piece::WhiteRook) {
            score.opening -= kBlockedRook;
        }
    }

    // Exposing queen
    if (g_chessboard->square[D1] != Piece::WhiteQueen) {
        if (g_chessboard->square[B1] == Piece::WhiteKnight) {
            score.opening -= kExposingQueen;
        }
        if (g_chessboard->square[C1] == Piece::WhiteBishop) {
            score.opening -= kExposingQueen;
        }
        if (g_chessboard->square[F1] == Piece::WhiteBishop) {
            score.opening -= kExposingQueen;
        }
        if (g_chessboard->square[G1] == Piece::WhiteKnight) {
            score.opening -= kExposingQueen;
        }
    }

    return tapered_score(score);
}

[[nodiscard]] Score evaluate_black_mobility() noexcept {
    assert(g_chessboard != nullptr);

    TaperedScore score{0, 0};

    for (std::size_t i = 1; i < g_black_piece_location.size() && g_black_piece_location[i].index() != kNotPresent; ++i) {
        const Square sq = g_black_piece_location[i];
        const Piece piece = g_chessboard->square[sq];

        switch (piece) {
            case Piece::BlackPawn:
            case Piece::BlackKing:
                break;

            case Piece::BlackKnight: {
                const Direction hanging_dir = piece_is_hanging(sq);
                Score mobility = -kKnightMobilityPenalty;

                for (const auto offset : kKnightOffsets) {
                    if (hanging_dir == kNoDirection || hanging_dir == offset || hanging_dir == -offset) {
                        const Square target = sq + offset;
                        const Piece blocker = g_chessboard->square[target];
                        mobility += kBlackMobilityUnit[to_index(blocker)];
                    }
                }

                score += mobility * kKnightMobilityBonus;
                score.opening += kKnightPositionTable[sq.to_index()];
                break;
            }

            case Piece::BlackBishop: {
                const Direction hanging_dir = piece_is_hanging(sq);
                Score mobility = -kBishopMobilityPenalty;

                for (const auto offset : kBishopOffsets) {
                    if (hanging_dir == kNoDirection || hanging_dir == offset || hanging_dir == -offset) {
                        Square target = sq + offset;
                        Piece blocker = g_chessboard->square[target];

                        while (blocker == Piece::Empty || blocker == Piece::BlackQueen) {
                            mobility += kBlackMobilityUnit[to_index(blocker)];
                            target = target + offset;
                            blocker = g_chessboard->square[target];
                        }
                        mobility += kBlackMobilityUnit[to_index(blocker)];
                    }
                }

                score += mobility * kBishopMobilityBonus;
                break;
            }

            case Piece::BlackRook: {
                const Direction hanging_dir = piece_is_hanging(sq);
                Score mobility = -kRookMobilityPenalty;

                for (const auto offset : kRookOffsets) {
                    if (hanging_dir == kNoDirection || hanging_dir == offset || hanging_dir == -offset) {
                        Square target = sq + offset;
                        Piece blocker = g_chessboard->square[target];

                        while (blocker == Piece::Empty || blocker == Piece::BlackRook) {
                            mobility += kBlackMobilityUnit[to_index(blocker)];
                            target = target + offset;
                            blocker = g_chessboard->square[target];
                        }
                        mobility += kBlackMobilityUnit[to_index(blocker)];
                    }
                }

                score += mobility * kRookMobilityBonus;

                // Rook on (semi)open file
                const auto rook_file = get_file(sq);
                const auto rook_rank = get_rank(sq);

                if (rook_file >= 0 && rook_file < 8) {
                    const auto f = static_cast<std::size_t>(rook_file);
                    if (g_black_pawn_files[f] == 0) {
                        if (g_white_pawn_files[f] == 0) {
                            score.opening += kRookOnOpenFile;
                        } else {
                            score.opening += kRookOnSemiOpenFile;
                            if (g_white_pawn_structure.backwards[f]) {
                                score.opening += kRookOnSemiOpenWithBackwardPawn;
                            }
                        }
                    }
                }

                // Rook on 2nd rank
                if (rook_rank == kRank2 && get_rank(g_chessboard->white_king_position) == kRank1) {
                    score += kRookOn7thRank;  // Same bonus as 7th rank for white
                }

                // Tarrash rule
                if (rook_file >= 0 && rook_file < 8) {
                    const auto f = static_cast<std::size_t>(rook_file);
                    if (g_black_passed_pawn[f] < kRank8 && rook_rank > g_black_passed_pawn[f]) {
                        score.endgame += kTarrashRule;
                    }
                    if (g_white_passed_pawn[f] > kRank1 && rook_rank > g_white_passed_pawn[f]) {
                        score.endgame += kTarrashRule;
                    }
                }
                break;
            }

            case Piece::BlackQueen: {
                const Direction hanging_dir = piece_is_hanging(sq);
                Score mobility = -kQueenMobilityPenalty;

                for (const auto offset : kQueenOffsets) {
                    if (hanging_dir == kNoDirection || hanging_dir == offset || hanging_dir == -offset) {
                        Square target = sq + offset;
                        Piece blocker = g_chessboard->square[target];

                        while (blocker == Piece::Empty || blocker == Piece::BlackBishop) {
                            mobility += kBlackMobilityUnit[to_index(blocker)];
                            target = target + offset;
                            blocker = g_chessboard->square[target];
                        }
                        mobility += kBlackMobilityUnit[to_index(blocker)];
                    }
                }

                score += mobility * kQueenMobilityBonus;

                // Queen on 2nd rank
                if (get_rank(sq) == kRank2 && get_rank(g_chessboard->white_king_position) == kRank1) {
                    score += kQueenOn7thRank;
                }
                break;
            }

            default:
                break;
        }
    }

    // Fianchetto bonus
    if (g_chessboard->square[G7] == Piece::BlackBishop) {
        score.opening += kFianchetto;
    }
    if (g_chessboard->square[B7] == Piece::BlackBishop) {
        score.opening += kFianchetto;
    }

    // Trapped bishop
    if (g_chessboard->square[A2] == Piece::BlackBishop && g_chessboard->square[B3] == Piece::WhitePawn) {
        score -= kTrappedBishop;
    } else if (g_chessboard->square[B1] == Piece::BlackBishop && g_chessboard->square[C2] == Piece::WhitePawn) {
        score -= kTrappedBishop;
    }

    if (g_chessboard->square[H2] == Piece::BlackBishop && g_chessboard->square[G3] == Piece::WhitePawn) {
        score -= kTrappedBishop;
    } else if (g_chessboard->square[G1] == Piece::BlackBishop && g_chessboard->square[F2] == Piece::WhitePawn) {
        score -= kTrappedBishop;
    }

    if (g_chessboard->square[A3] == Piece::BlackBishop && g_chessboard->square[B4] == Piece::WhitePawn) {
        score -= kTrappedBishop / 2;
    } else if (g_chessboard->square[H3] == Piece::BlackBishop && g_chessboard->square[G4] == Piece::WhitePawn) {
        score -= kTrappedBishop / 2;
    }

    // Blocked bishop
    if (g_chessboard->square[C8] == Piece::BlackBishop &&
        g_chessboard->square[D7] == Piece::BlackPawn &&
        g_chessboard->square[D6] != Piece::Empty) {
        score.opening -= kBlockedBishop;
    }

    if (g_chessboard->square[F8] == Piece::BlackBishop &&
        g_chessboard->square[E7] == Piece::BlackPawn &&
        g_chessboard->square[E6] != Piece::Empty) {
        score.opening -= kBlockedBishop;
    }

    // Blocked rook
    const Square king_pos = g_chessboard->black_king_position;
    if (king_pos == C8 || king_pos == B8) {
        if (g_chessboard->square[A8] == Piece::BlackRook ||
            g_chessboard->square[A7] == Piece::BlackRook ||
            g_chessboard->square[B8] == Piece::BlackRook) {
            score.opening -= kBlockedRook;
        }
    }

    if (king_pos == F8 || king_pos == G8) {
        if (g_chessboard->square[H8] == Piece::BlackRook ||
            g_chessboard->square[H7] == Piece::BlackRook ||
            g_chessboard->square[G8] == Piece::BlackRook) {
            score.opening -= kBlockedRook;
        }
    }

    // Exposing queen
    if (g_chessboard->square[D8] != Piece::BlackQueen) {
        if (g_chessboard->square[B8] == Piece::BlackKnight) {
            score.opening -= kExposingQueen;
        }
        if (g_chessboard->square[C8] == Piece::BlackBishop) {
            score.opening -= kExposingQueen;
        }
        if (g_chessboard->square[F8] == Piece::BlackBishop) {
            score.opening -= kExposingQueen;
        }
        if (g_chessboard->square[G8] == Piece::BlackKnight) {
            score.opening -= kExposingQueen;
        }
    }

    return tapered_score(score);
}

// ============================================================================
// King Safety Evaluation
// ============================================================================

[[nodiscard]] Score evaluate_white_king_safety() noexcept {
    assert(g_chessboard != nullptr);

    TaperedScore score{0, 0};
    const Square king_pos = g_chessboard->white_king_position;
    const auto king_file = get_file(king_pos);

    // Position table score
    score.opening += kWhiteKingPositionTable[king_pos.to_index()];

    // Pawn shelter
    auto shelter = [](Square sq) -> Score {
        const Square stop = sq + kNorth;
        const Square telestop = stop + kNorth;

        if (g_chessboard->square[sq] == Piece::WhitePawn) {
            return kPawnShelter;
        } else if (g_chessboard->square[stop] == Piece::WhitePawn) {
            return kPawnShelterPenalty;
        } else if (!is_frame(telestop) && g_chessboard->square[telestop] == Piece::WhitePawn) {
            return kPawnShelterDefuse;
        }
        return 0;
    };

    if (king_file == kFileA || king_file == kFileB || king_file == kFileC) {
        score.opening += shelter(A2) + shelter(B2) + shelter(C2);
    } else if (king_file == kFileF || king_file == kFileG || king_file == kFileH) {
        score.opening += shelter(F2) + shelter(G2) + shelter(H2);
    }

    // Open file near king
    if (king_file >= 0 && king_file < 8) {
        const auto f = static_cast<std::size_t>(king_file);

        if (f > 0 && g_white_pawn_files[f - 1] == 0) {
            score.opening -= kOpenFileNearKing;
        }
        if (g_white_pawn_files[f] == 0) {
            score.opening -= kOpenFileInFrontOfKing;
        }
        if (f < 7 && g_white_pawn_files[f + 1] == 0) {
            score.opening -= kOpenFileNearKing;
        }

        // Semi-open file near king
        if (f > 0 && g_black_pawn_files[f - 1] == 0) {
            score.opening -= kSemiOpenFileNearKing;
        }
        if (g_black_pawn_files[f] == 0) {
            score.opening -= kSemiOpenFileInFrontOfKing;
        }
        if (f < 7 && g_black_pawn_files[f + 1] == 0) {
            score.opening -= kSemiOpenFileNearKing;
        }
    }

    // Pawn storm
    auto storm = [](std::size_t file_idx) -> Score {
        const auto r = g_black_pawn_ranks[file_idx];
        switch (r) {
            case kRank2:
            case kRank3:
                return kPawnStormSevereThreat;
            case kRank4:
                return kPawnStormThreat;
            case kRank5:
                return kPawnStormAlert;
            default:
                return 0;
        }
    };

    if (king_file == kFileA || king_file == kFileB || king_file == kFileC) {
        score.opening -= storm(0) + storm(1) + storm(2);
    } else if (king_file == kFileF || king_file == kFileG || king_file == kFileH) {
        score.opening -= storm(5) + storm(6) + storm(7);
    }

    // Attacking king zone
    score.opening -= attacks_king_zone(Color::White);

    // Endgame king position
    score.endgame += kKingEndGamePositionTable[king_pos.to_index()];

    return tapered_score(score);
}

[[nodiscard]] Score evaluate_black_king_safety() noexcept {
    assert(g_chessboard != nullptr);

    TaperedScore score{0, 0};
    const Square king_pos = g_chessboard->black_king_position;
    const auto king_file = get_file(king_pos);

    // Position table score
    score.opening += kBlackKingPositionTable[king_pos.to_index()];

    // Pawn shelter
    auto shelter = [](Square sq) -> Score {
        const Square stop = sq + kSouth;
        const Square telestop = stop + kSouth;

        if (g_chessboard->square[sq] == Piece::BlackPawn) {
            return kPawnShelter;
        } else if (g_chessboard->square[stop] == Piece::BlackPawn) {
            return kPawnShelterPenalty;
        } else if (!is_frame(telestop) && g_chessboard->square[telestop] == Piece::BlackPawn) {
            return kPawnShelterDefuse;
        }
        return 0;
    };

    if (king_file == kFileA || king_file == kFileB || king_file == kFileC) {
        score.opening += shelter(A7) + shelter(B7) + shelter(C7);
    } else if (king_file == kFileF || king_file == kFileG || king_file == kFileH) {
        score.opening += shelter(F7) + shelter(G7) + shelter(H7);
    }

    // Open file near king
    if (king_file >= 0 && king_file < 8) {
        const auto f = static_cast<std::size_t>(king_file);

        if (f > 0 && g_black_pawn_files[f - 1] == 0) {
            score.opening -= kOpenFileNearKing;
        }
        if (g_black_pawn_files[f] == 0) {
            score.opening -= kOpenFileInFrontOfKing;
        }
        if (f < 7 && g_black_pawn_files[f + 1] == 0) {
            score.opening -= kOpenFileNearKing;
        }

        // Semi-open file near king
        if (f > 0 && g_white_pawn_files[f - 1] == 0) {
            score.opening -= kSemiOpenFileNearKing;
        }
        if (g_white_pawn_files[f] == 0) {
            score.opening -= kSemiOpenFileInFrontOfKing;
        }
        if (f < 7 && g_white_pawn_files[f + 1] == 0) {
            score.opening -= kSemiOpenFileNearKing;
        }
    }

    // Pawn storm
    auto storm = [](std::size_t file_idx) -> Score {
        const auto r = g_white_pawn_ranks[file_idx];
        switch (r) {
            case kRank7:
            case kRank6:
                return kPawnStormSevereThreat;
            case kRank5:
                return kPawnStormThreat;
            case kRank4:
                return kPawnStormAlert;
            default:
                return 0;
        }
    };

    if (king_file == kFileA || king_file == kFileB || king_file == kFileC) {
        score.opening -= storm(0) + storm(1) + storm(2);
    } else if (king_file == kFileF || king_file == kFileG || king_file == kFileH) {
        score.opening -= storm(5) + storm(6) + storm(7);
    }

    // Attacking king zone
    score.opening -= attacks_king_zone(Color::Black);

    // Endgame king position
    score.endgame += kKingEndGamePositionTable[king_pos.to_index()];

    return tapered_score(score);
}

}  // anonymous namespace

// ============================================================================
// Public Functions
// ============================================================================

void initialize_evaluation_engine() {
    fill_knight_distance_table();
}

Evaluation evaluate(const Chessboard& chessboard) {
    g_chessboard = &chessboard;

    collect_information();

    g_endgame_pattern = detect_endgame_pattern();

    if (insufficient_material()) {
        return Evaluation{0, MatchStatus::DrawByInsufficientMaterial};
    }

    // Calculate game phase
    {
        Score pawn_phase = 0 * (g_material[to_index(Piece::WhitePawn)] + g_material[to_index(Piece::BlackPawn)]);
        Score knight_phase = 4 * (g_material[to_index(Piece::WhiteKnight)] + g_material[to_index(Piece::BlackKnight)]);
        Score bishop_phase = 4 * (g_material[to_index(Piece::WhiteBishop)] + g_material[to_index(Piece::BlackBishop)]);
        Score rook_phase = 9 * (g_material[to_index(Piece::WhiteRook)] + g_material[to_index(Piece::BlackRook)]);
        Score queen_phase = 16 * (g_material[to_index(Piece::WhiteQueen)] + g_material[to_index(Piece::BlackQueen)]);

        Score phase = pawn_phase + knight_phase + bishop_phase + rook_phase + queen_phase;

        if (phase > 100) {
            phase = 100;
        }

        g_game_phase = phase;
    }

    Score white_material = evaluate_white_material();
    Score black_material = evaluate_black_material();

    Score white_pawns_structure = evaluate_white_pawn_structure();
    Score black_pawns_structure = evaluate_black_pawn_structure();

    Score white_mobility = evaluate_white_mobility();
    Score black_mobility = evaluate_black_mobility();

    Score white_king_safety = evaluate_white_king_safety();
    Score black_king_safety = evaluate_black_king_safety();

    Score white_score = white_material + white_pawns_structure + white_mobility + white_king_safety;
    Score black_score = black_material + black_pawns_structure + black_mobility + black_king_safety;

    // Tempo bonus
    if (chessboard.side_to_move == Color::White) {
        white_score += kTempoBonus;
    } else {
        black_score += kTempoBonus;
    }

    // Random mode bias
    RandomScore bias = 0;
    if (g_random_mode) {
        TaperedScore random_tapered{g_score_seed.random(), 0};
        bias = static_cast<RandomScore>(tapered_score(random_tapered));
    }

    Score final_score;
    if (chessboard.side_to_move == Color::White) {
        final_score = white_score - black_score + bias;
    } else {
        final_score = black_score - white_score + bias;
    }

    return Evaluation{final_score, MatchStatus::InProgress};
}

// ============================================================================
// Distance Functions
// ============================================================================

void fill_knight_distance_table() {
    // Initialize all distances to unreachable
    for (auto& row : g_knight_distance_table) {
        row.fill(kUnreachable);
    }

    // Use Dijkstra-like algorithm to fill the table
    enum class VisitStatus : std::uint8_t { NotVisited, InVisit, Complete };

    std::array<std::array<VisitStatus, kBoardSize>, kBoardSize> visited{};

    std::function<Distance(Square, Square)> dijkstra = [&](Square from, Square to) -> Distance {
        visited[from.to_index()][from.to_index()] = VisitStatus::Complete;
        g_knight_distance_table[from.to_index()][from.to_index()] = kNoDistance;

        if (from == to) {
            return kNoDistance;
        }

        if (visited[from.to_index()][to.to_index()] == VisitStatus::Complete) {
            return g_knight_distance_table[from.to_index()][to.to_index()];
        }

        if (visited[to.to_index()][from.to_index()] == VisitStatus::Complete) {
            return g_knight_distance_table[to.to_index()][from.to_index()];
        }

        // Mark adjacent squares (knight moves)
        for (const auto offset : kKnightOffsets) {
            Square target = from + offset;
            if (!is_frame(target)) {
                visited[from.to_index()][target.to_index()] = VisitStatus::Complete;
                g_knight_distance_table[from.to_index()][target.to_index()] = 1;
            }
        }

        visited[from.to_index()][to.to_index()] = VisitStatus::InVisit;

        Distance shortest = kUnreachable;

        for (const auto offset : kKnightOffsets) {
            Square target = from + offset;
            if (!is_frame(target)) {
                Distance d;
                switch (visited[target.to_index()][to.to_index()]) {
                    case VisitStatus::Complete:
                        d = static_cast<Distance>(1 + g_knight_distance_table[target.to_index()][to.to_index()]);
                        break;
                    case VisitStatus::InVisit:
                        d = kUnreachable - 1;  // Skip, already searching this path
                        break;
                    case VisitStatus::NotVisited:
                        d = static_cast<Distance>(1 + dijkstra(target, to));
                        visited[target.to_index()][to.to_index()] = VisitStatus::Complete;
                        break;
                }
                if (d < shortest) {
                    shortest = d;
                }
            }
        }

        g_knight_distance_table[from.to_index()][to.to_index()] = shortest;
        return shortest;
    };

    // Fill table for all square pairs
    for (std::uint8_t sq_idx = 0; sq_idx < kBoardSize; ++sq_idx) {
        Square sq{sq_idx};
        if (!is_frame(sq)) {
            for (std::uint8_t target_idx = 0; target_idx < kBoardSize; ++target_idx) {
                Square target{target_idx};
                if (!is_frame(target)) {
                    // Reset visited for each pair
                    for (auto& row : visited) {
                        row.fill(VisitStatus::NotVisited);
                    }
                    g_knight_distance_table[sq_idx][target_idx] = dijkstra(sq, target);
                }
            }
        }
    }
}

Distance raw_distance(Piece piece, Square square, Square target) {
    assert(!is_frame(square));
    assert(!is_frame(target));
    assert(piece != Piece::Frame && piece != Piece::Empty);

    const auto x1 = get_rank(square);
    const auto y1 = get_file(square);
    const auto x2 = get_rank(target);
    const auto y2 = get_file(target);

    auto max_val = [](int v1, int v2) -> int {
        return (v1 > v2) ? v1 : v2;
    };

    switch (piece) {
        case Piece::WhiteKing:
        case Piece::BlackKing:
            return static_cast<Distance>(max_val(std::abs(x1 - x2), std::abs(y1 - y2)));

        case Piece::WhitePawn:
            if (y1 == y2 && square.index() > target.index()) {
                return static_cast<Distance>(x2 - x1);
            }
            return kUnreachable;

        case Piece::BlackPawn:
            if (y1 == y2 && square.index() < target.index()) {
                return static_cast<Distance>(x1 - x2);
            }
            return kUnreachable;

        case Piece::WhiteBishop:
        case Piece::BlackBishop:
            if (square_color(square) == square_color(target)) {
                if (diagonal(square) == diagonal(target) ||
                    anti_diagonal(square) == anti_diagonal(target)) {
                    return 1;
                }
                return 2;
            }
            return kUnreachable;

        case Piece::WhiteRook:
        case Piece::BlackRook:
            if (x1 == x2 || y1 == y2) {
                return 1;
            }
            return 2;

        case Piece::WhiteQueen:
        case Piece::BlackQueen:
            if (x1 == x2 || y1 == y2 ||
                diagonal(square) == diagonal(target) ||
                anti_diagonal(square) == anti_diagonal(target)) {
                return 1;
            }
            return 2;

        case Piece::WhiteKnight:
        case Piece::BlackKnight:
            return g_knight_distance_table[square.to_index()][target.to_index()];

        default:
            break;
    }

    return kUnreachable;
}

Distance distance(const Chessboard& chessboard, Piece piece, Square square, Square target) {
    const Color side_to_move = chessboard.side_to_move;
    bool gain_tempo = false;

    if (side_to_move == Color::White) {
        gain_tempo = is_black_piece(piece);
    } else {
        gain_tempo = is_white_piece(piece);
    }

    Distance d = raw_distance(piece, square, target);
    if (d != kUnreachable && gain_tempo) {
        d = static_cast<Distance>(d + 1);
    }

    return d;
}

DistanceMatch race(const Chessboard& chessboard,
                   Piece piece, Square square,
                   Piece opponent, Square opponent_square,
                   Square target) {
    const Distance d1 = distance(chessboard, piece, square, target);
    const Distance d2 = distance(chessboard, opponent, opponent_square, target);

    constexpr Distance kReachableMax = 8;

    if (d1 == kUnreachable) {
        if (d2 == kUnreachable) {
            return DistanceMatch::NotReachable;
        }
        return DistanceMatch::Loser;
    }

    if (d1 <= kReachableMax) {
        if (d2 == kUnreachable) {
            return DistanceMatch::Winner;
        }

        if (d1 == d2) {
            return DistanceMatch::Equals;
        } else if (d1 < d2) {
            return DistanceMatch::Closer;
        } else {
            return DistanceMatch::Farther;
        }
    }

    return DistanceMatch::NotReachable;
}

} // namespace chess::engine::evaluations

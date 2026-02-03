/**
 * AdaChess - Smart Chess Engine
 *
 * Static Exchange Evaluation (SEE) implementation.
 * C++ port of the original Ada implementation (chess-engine-see.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/engine/see.hpp"
#include "adachess/engine/engine.hpp"
#include "adachess/board/board.hpp"
#include "adachess/board/directions.hpp"

#include <cassert>
#include <algorithm>

namespace chess::engine::see {

namespace {

/**
 * Check if an attacker is not pinned.
 * A king cannot be pinned, so this returns true for kings.
 *
 * @param chessboard The chess position.
 * @param square The square where the attacker is located.
 * @return true if the attacker is not absolutely pinned.
 */
[[nodiscard]] bool attacker_is_not_pinned(
    const Chessboard& chessboard,
    board::Square square
) noexcept {
    Piece attacker = chessboard.square[square];

    // King cannot be pinned
    assert(!is_king(attacker) && "Bug! King cannot be pinned");

    return chessboard.piece_is_absolute_pinned(square) == board::kNoDirection;
}

/**
 * Find and remove the weakest attacker from an attack collection.
 *
 * Searches through the attackers to find the one with the lowest piece value,
 * removes it from the collection, and returns it.
 *
 * @param attack_data The attack collection to search (modified in place).
 * @return The weakest attacker found.
 */
[[nodiscard]] board::AttackType next_weaker_attacker(
    board::AttackCollection& attack_data
) noexcept {
    assert(attack_data.number_of_attackers > 0 &&
           "No attacker found to be processed in the capture sequence");

    // Find the weakest attacker from the attack collection
    std::size_t weakest_pivot = 0;  // Index of weaker attacker
    Score weakest_attacker_score = kInfinity;

    for (std::size_t i = 0; i < attack_data.number_of_attackers; ++i) {
        Score attacker_score = piece_see_value(attack_data.attackers[i].piece);
        if (attacker_score < weakest_attacker_score) {
            weakest_pivot = i;
            weakest_attacker_score = attacker_score;
        }
        // Exit early if we found a pawn (lowest value piece that can attack)
        if (is_pawn(attack_data.attackers[i].piece)) {
            break;
        }
    }

    // Save the weakest attacker to return
    board::AttackType weakest_attacker = attack_data.attackers[weakest_pivot];

    // "Delete" the attacker by moving the last element to its position
    attack_data.attackers[weakest_pivot] =
        attack_data.attackers[attack_data.number_of_attackers - 1];
    attack_data.attackers[attack_data.number_of_attackers - 1] =
        board::AttackType{board::Square(0), Piece::Empty};
    attack_data.number_of_attackers -= 1;

    return weakest_attacker;
}

/**
 * Calculate the SEE score for a capture sequence.
 *
 * Recursive function that evaluates the exchange on a target square
 * by finding the weakest attacker and simulating the capture sequence.
 *
 * @param chessboard The chess position (modified temporarily).
 * @param capturing The piece being captured.
 * @param side The side that is about to capture.
 * @param target_square The square where the exchange takes place.
 * @return The score of the exchange from the perspective of the capturing side.
 */
[[nodiscard]] Score see_score(
    Chessboard& chessboard,
    Piece capturing,
    Color side,
    board::Square target_square
) noexcept {
    // Build a list of all pieces of 'side' defending the target square
    board::AttackCollection attack_data =
        chessboard.defending_square(side, target_square);

    Score score = 0;
    Color opponent_side = flip(side);
    Score captured_score = piece_see_value(capturing);

    while (attack_data.number_of_attackers > 0) {
        board::AttackType attacker = next_weaker_attacker(attack_data);

        // Process the attacker if it's a king or if it's not pinned
        if (is_king(attacker.piece) || attacker_is_not_pinned(chessboard, attacker.origin)) {
            // Temporarily remove the attacker from the board
            chessboard.square[attacker.origin] = Piece::Empty;

            // Recursively evaluate the exchange from opponent's perspective
            score = captured_score - see_score(chessboard, attacker.piece, opponent_side, target_square);

            // Restore the attacker
            chessboard.square[attacker.origin] = attacker.piece;

            // Handle pawn promotion bonus
            if (attacker.piece == Piece::WhitePawn &&
                board::rank(target_square) == board::kRank8) {
                score = score + kPromotionScore - kPawnScore;
            } else if (attacker.piece == Piece::BlackPawn &&
                       board::rank(target_square) == board::kRank1) {
                score = score + kPromotionScore - kPawnScore;
            }

            // Pruning: if we're losing and the attacker is at least as valuable
            // as what we captured, stop - further captures only make it worse
            if (score < 0 && piece_see_value(attacker.piece) >= captured_score) {
                score = 0;
            }

            // Stop conditions
            if (piece_see_value(attacker.piece) <= captured_score) {
                break;
            }
            if (is_king(attacker.piece)) {
                break;
            }
        }
    }

    return score;
}

} // anonymous namespace

// ============================================================================
// Public Functions
// ============================================================================

SeeScore static_exchange_evaluation_score(
    Chessboard& chessboard,
    const Move& move
) {
    SeeScore sequence_score = 0;

    // Determine the side to move based on the moving piece
    Color side_to_move = is_white_piece(move.piece) ? Color::White : Color::Black;
    Color opponent_side = flip(side_to_move);

    // Note: If a move delivers a discovery check, the moving piece cannot be
    // taken back.
    if (move.check == CheckType::DiscoveryCheck ||
        move.check == CheckType::DoubleCheck ||
        move.check == CheckType::Checkmate) {
        return piece_see_value(move.captured);
    }

    // Calculate initial gain from the move
    if (move.captured != Piece::Empty) {
        sequence_score = piece_see_value(move.captured);
    } else if (move.flag == MoveFlag::CaptureEnPassant) {
        sequence_score = piece_see_value(Piece::WhitePawn);  // Pawn value
    }

    // If it's a promotion, add the promotion bonus
    if (move.promotion != Piece::Empty) {
        return sequence_score + piece_see_value(move.promotion) - kPawnScore;
    }

    // Temporarily remove the moving piece and evaluate recaptures
    chessboard.square[move.from] = Piece::Empty;
    sequence_score = sequence_score - see_score(chessboard, move.piece, opponent_side, move.to);
    chessboard.square[move.from] = move.piece;

    return sequence_score;
}

CaptureResult static_exchange_evaluation(
    Chessboard& chessboard,
    const Move& move
) {
    Score sequence_score = static_exchange_evaluation_score(chessboard, move);

    if (sequence_score == 0) {
        return CaptureResult::Equal;
    } else if (sequence_score > 0) {
        return CaptureResult::Winning;
    } else {
        return CaptureResult::Losing;
    }
}

Score static_recapture_exchange_evaluation(
    Chessboard& chessboard,
    const Move& recapture_move,
    const Move& previous_move
) {
    Score score = static_exchange_evaluation_score(chessboard, recapture_move);
    return score - piece_see_value(previous_move.captured);
}

} // namespace chess::engine::see

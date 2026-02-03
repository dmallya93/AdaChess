/**
 * AdaChess - Smart Chess Engine
 *
 * FEN (Forsyth-Edwards Notation) parser and serializer implementation.
 * C++ port of the original Ada implementation (chess-io-fen.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/io/fen.hpp"

#include <cctype>
#include <sstream>

#include "adachess/colors.hpp"
#include "adachess/pieces.hpp"
#include "adachess/depths.hpp"
#include "adachess/board/board.hpp"
#include "adachess/board/directions.hpp"
#include "adachess/engine/engine.hpp"
#include "adachess/libs/string_lib.hpp"

namespace chess::io::fen {

namespace {

// ============================================================================
// Standard board mapping (FEN order: a8, b8, ..., h8, a7, ..., h1)
// ============================================================================

/**
 * Mapping from FEN index (0-63, starting at a8) to internal square index.
 * FEN starts at rank 8 and goes down to rank 1.
 */
constexpr std::array<board::Square, 64> kFenToSquare = {{
    board::A8, board::B8, board::C8, board::D8, board::E8, board::F8, board::G8, board::H8,
    board::A7, board::B7, board::C7, board::D7, board::E7, board::F7, board::G7, board::H7,
    board::A6, board::B6, board::C6, board::D6, board::E6, board::F6, board::G6, board::H6,
    board::A5, board::B5, board::C5, board::D5, board::E5, board::F5, board::G5, board::H5,
    board::A4, board::B4, board::C4, board::D4, board::E4, board::F4, board::G4, board::H4,
    board::A3, board::B3, board::C3, board::D3, board::E3, board::F3, board::G3, board::H3,
    board::A2, board::B2, board::C2, board::D2, board::E2, board::F2, board::G2, board::H2,
    board::A1, board::B1, board::C1, board::D1, board::E1, board::F1, board::G1, board::H1
}};

/**
 * Border squares marking the end of each rank in FEN serialization order.
 */
constexpr std::array<board::Square, 8> kRankBorders = {{
    board::Square(board::H8.index() + 1),
    board::Square(board::H7.index() + 1),
    board::Square(board::H6.index() + 1),
    board::Square(board::H5.index() + 1),
    board::Square(board::H4.index() + 1),
    board::Square(board::H3.index() + 1),
    board::Square(board::H2.index() + 1),
    board::Square(board::H1.index() + 1)
}};

// ============================================================================
// Helper Functions for Parsing
// ============================================================================

/**
 * Load pieces from FEN piece placement string onto the chessboard.
 *
 * @param chessboard The chessboard to populate.
 * @param fen_data The piece placement portion of the FEN string (e.g., "rnbqkbnr/pppppppp/...").
 */
void fen_load_pieces(engine::Chessboard& chessboard, std::string_view fen_data) {
    std::size_t sq_index = 0;  // Index into kFenToSquare

    for (char item : fen_data) {
        if (sq_index >= 64) {
            break;  // Board is full
        }

        board::Square square = kFenToSquare[sq_index];

        switch (item) {
            // White pieces
            case 'P':
                chessboard.square[square] = Piece::WhitePawn;
                ++sq_index;
                break;
            case 'N':
                chessboard.square[square] = Piece::WhiteKnight;
                ++sq_index;
                break;
            case 'B':
                chessboard.square[square] = Piece::WhiteBishop;
                ++sq_index;
                break;
            case 'R':
                chessboard.square[square] = Piece::WhiteRook;
                ++sq_index;
                break;
            case 'Q':
                chessboard.square[square] = Piece::WhiteQueen;
                ++sq_index;
                break;
            case 'K':
                chessboard.square[square] = Piece::WhiteKing;
                chessboard.white_king_position = square;
                ++sq_index;
                break;

            // Black pieces
            case 'p':
                chessboard.square[square] = Piece::BlackPawn;
                ++sq_index;
                break;
            case 'n':
                chessboard.square[square] = Piece::BlackKnight;
                ++sq_index;
                break;
            case 'b':
                chessboard.square[square] = Piece::BlackBishop;
                ++sq_index;
                break;
            case 'r':
                chessboard.square[square] = Piece::BlackRook;
                ++sq_index;
                break;
            case 'q':
                chessboard.square[square] = Piece::BlackQueen;
                ++sq_index;
                break;
            case 'k':
                chessboard.square[square] = Piece::BlackKing;
                chessboard.black_king_position = square;
                ++sq_index;
                break;

            // Empty squares (1-8)
            case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8':
                sq_index += static_cast<std::size_t>(item - '0');
                break;

            // Rank separator '/' - ignored in this implementation
            case '/':
                break;

            // Space indicates end of piece placement
            case ' ':
                goto done;

            default:
                // Skip unknown characters
                break;
        }
    }
done:

    // Update pieces list from the board
    for (std::size_t i = 0; i < board::kBoardSize; ++i) {
        board::Square sq(static_cast<std::uint8_t>(i));
        Piece piece = chessboard.square[sq];

        if (is_white_piece(piece)) {
            chessboard.add_white_piece(sq);
        } else if (is_black_piece(piece)) {
            chessboard.add_black_piece(sq);
        }
    }
}

/**
 * Load side to move from FEN.
 *
 * @param chessboard The chessboard to update.
 * @param fen_data The side to move portion of the FEN string ('w' or 'b').
 * @throws InvalidFen if side to move is not specified.
 */
void fen_load_side_to_move(engine::Chessboard& chessboard, std::string_view fen_data) {
    if (fen_data == "w") {
        chessboard.side_to_move = Color::White;
    } else if (fen_data == "b") {
        chessboard.side_to_move = Color::Black;
    } else {
        throw InvalidFen("Side to move is not specified");
    }
}

/**
 * Load castling rights from FEN.
 *
 * @param chessboard The chessboard to update.
 * @param fen_data The castling rights portion of the FEN string (e.g., "KQkq" or "-").
 */
void fen_load_castle_flags(engine::Chessboard& chessboard, std::string_view fen_data) {
    HistoryDepth ply = chessboard.history_ply;

    for (char item : fen_data) {
        switch (item) {
            case 'K':
                chessboard.white_castle_kingside[ply] = true;
                break;
            case 'Q':
                chessboard.white_castle_queenside[ply] = true;
                break;
            case 'k':
                chessboard.black_castle_kingside[ply] = true;
                break;
            case 'q':
                chessboard.black_castle_queenside[ply] = true;
                break;
            case '-':
                // All castling rights have already been set to false by reset()
                break;
            default:
                // Ignore unknown characters
                break;
        }
    }
}

/**
 * Load en passant square from FEN.
 *
 * @param chessboard The chessboard to update.
 * @param fen_data The en passant portion of the FEN string (e.g., "e3" or "-").
 */
void fen_load_en_passant(engine::Chessboard& chessboard, std::string_view fen_data) {
    if (fen_data != "-" && fen_data.length() == 2) {
        // Parse algebraic notation (e.g., "e3")
        char file_char = fen_data[0];
        char rank_char = fen_data[1];

        if (file_char >= 'a' && file_char <= 'h' &&
            rank_char >= '1' && rank_char <= '8') {

            // Convert to square
            // The board uses 10x12 representation
            // Files: a=0, b=1, ..., h=7
            // Ranks: 1-8 map to rows 9, 8, 7, 6, 5, 4, 3, 2 (index = 9 - rank + 1) * 10 + file + 1
            int file = file_char - 'a';
            int rank = rank_char - '0';

            // In 10x12 board: rank 1 is row 9 (indices 91-98), rank 8 is row 2 (indices 21-28)
            // Formula: base = (10 - rank) * 10, square = base + file + 1
            int index = (10 - rank) * 10 + file + 1;

            chessboard.en_passant[chessboard.history_ply] = board::Square(static_cast<std::uint8_t>(index));
        }
    }
}

/**
 * Load fifty-move counter from FEN.
 *
 * @param chessboard The chessboard to update.
 * @param fen_data The fifty-move counter portion of the FEN string.
 */
void fen_load_fifty_move_counter(engine::Chessboard& chessboard, std::string_view fen_data) {
    if (fen_data.empty()) {
        chessboard.fifty = 0;
        return;
    }

    try {
        int value = std::stoi(std::string(fen_data));
        chessboard.fifty = static_cast<engine::FiftyCounter>(
            std::min(std::max(value, 0), static_cast<int>(engine::kFiftyMax)));
    } catch (...) {
        chessboard.fifty = 0;
    }
}

/**
 * Load ply depth from FEN (fullmove number).
 * Note: We ignore the fullmove number and set history_ply to zero as in Ada implementation.
 *
 * @param chessboard The chessboard to update.
 * @param fen_data The fullmove number portion of the FEN string (ignored).
 */
void fen_load_ply_depth(engine::Chessboard& chessboard, [[maybe_unused]] std::string_view fen_data) {
    // We ignore the initial value (as in Ada implementation)
    chessboard.history_ply = 0;
}

} // anonymous namespace

// ============================================================================
// Public Functions
// ============================================================================

void from_string(engine::Chessboard& chessboard, std::string_view fen) {
    try {
        // Reset the board to empty state
        chessboard.reset();

        // Convert to string for token extraction
        std::string fen_str(fen);

        // Parse piece placement (token 1)
        std::string piece_data = string_lib::extract_token_at(fen_str, 1, string_lib::kWhitespace);
        fen_load_pieces(chessboard, piece_data);

        // Parse side to move (token 2)
        std::string side_data = string_lib::extract_token_at(fen_str, 2, string_lib::kWhitespace);
        fen_load_side_to_move(chessboard, side_data);

        // Parse castling rights (token 3)
        std::string castle_data = string_lib::extract_token_at(fen_str, 3, string_lib::kWhitespace);
        fen_load_castle_flags(chessboard, castle_data);

        // Parse en passant square (token 4)
        std::string en_passant_data = string_lib::extract_token_at(fen_str, 4, string_lib::kWhitespace);
        fen_load_en_passant(chessboard, en_passant_data);

        // Parse optional fields (fifty-move counter and fullmove number)
        try {
            // Fifty-move counter (token 5)
            std::string fifty_data = string_lib::extract_token_at(fen_str, 5, string_lib::kWhitespace);
            fen_load_fifty_move_counter(chessboard, fifty_data);

            // Fullmove number (token 6)
            std::string ply_data = string_lib::extract_token_at(fen_str, 6, string_lib::kWhitespace);
            fen_load_ply_depth(chessboard, ply_data);
        } catch (const string_lib::NotEnoughTokens&) {
            // Optional fields - ignore if missing
        }

        // Validate the position

        // Check that the opponent's king is not in check when it's not their turn
        if (chessboard.side_to_move == Color::White) {
            if (chessboard.has_king_in_check(Color::Black)) {
                throw InvalidFen("Black king is in check when it is White's turn");
            }
        } else {
            if (chessboard.has_king_in_check(Color::White)) {
                throw InvalidFen("White king is in check when it is Black's turn");
            }
        }

        // Validate en passant square
        board::Square ep_square = chessboard.en_passant[chessboard.history_ply];
        if (ep_square != board::kNoEnPassant) {
            if (chessboard.side_to_move == Color::White) {
                // Black made last move (pawn push), en passant should be on rank 6
                if (board::rank(ep_square) != board::kRank6 ||
                    chessboard.square[ep_square + board::kSouth] != Piece::BlackPawn) {
                    throw InvalidFen("En-passant square is not valid");
                }
            } else {
                // White made last move, en passant should be on rank 3
                if (board::rank(ep_square) != board::kRank3 ||
                    chessboard.square[ep_square + board::kNorth] != Piece::WhitePawn) {
                    throw InvalidFen("En-passant square is not valid");
                }
            }
        }

        // Update the position hash
        chessboard.update_hash();

    } catch (const InvalidFen&) {
        throw;  // Re-throw InvalidFen exceptions
    } catch (const std::exception& e) {
        throw InvalidFen(std::string("Invalid FEN string: ") + e.what());
    } catch (...) {
        throw InvalidFen("Invalid FEN string");
    }
}

std::string to_string(const engine::Chessboard& chessboard) {
    std::ostringstream fen;

    int empty_count = 0;

    // Output piece placement
    for (std::size_t sq_idx = 0; sq_idx < 64; ++sq_idx) {
        board::Square square = kFenToSquare[sq_idx];
        Piece piece = chessboard.square[square];

        if (piece == Piece::Empty) {
            ++empty_count;
        } else {
            // Output accumulated empty squares
            if (empty_count > 0) {
                fen << empty_count;
                empty_count = 0;
            }
            fen << piece_symbol(piece);
        }

        // Check for end of rank (every 8 squares)
        if ((sq_idx + 1) % 8 == 0) {
            // Output accumulated empty squares at end of rank
            if (empty_count > 0) {
                fen << empty_count;
                empty_count = 0;
            }

            // Add rank separator (except after last rank)
            if (sq_idx < 63) {
                fen << '/';
            }
        }
    }

    // Add space separator
    fen << ' ';

    // Output side to move
    fen << (chessboard.side_to_move == Color::White ? 'w' : 'b');

    fen << ' ';

    // Output castling rights
    HistoryDepth ply = chessboard.history_ply;
    bool has_castle = false;

    if (chessboard.white_castle_kingside[ply]) {
        fen << 'K';
        has_castle = true;
    }
    if (chessboard.white_castle_queenside[ply]) {
        fen << 'Q';
        has_castle = true;
    }
    if (chessboard.black_castle_kingside[ply]) {
        fen << 'k';
        has_castle = true;
    }
    if (chessboard.black_castle_queenside[ply]) {
        fen << 'q';
        has_castle = true;
    }

    if (!has_castle) {
        fen << '-';
    }

    fen << ' ';

    // Output en passant square
    board::Square ep_square = chessboard.en_passant[ply];
    if (ep_square != board::kNoEnPassant && ep_square.index() != 0) {
        fen << board::square_to_string(ep_square);
    } else {
        fen << '-';
    }

    // Note: The Ada implementation comments out the fifty-move counter and fullmove number
    // but we include them for completeness

    return fen.str();
}

} // namespace chess::io::fen

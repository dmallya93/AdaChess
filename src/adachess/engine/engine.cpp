/**
 * AdaChess - Smart Chess Engine
 *
 * Chessboard state and basic initialization implementation.
 * C++ port of the original Ada implementation (chess-engine.adb).
 *
 * Original Copyright (C) 2013-2023 - Alessandro Iavicoli
 * C++ Port Copyright (C) AdaChess Project
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 */

#include "adachess/engine/engine.hpp"
#include "adachess/board/attacks_data.hpp"
#include "adachess/io/io.hpp"

#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace chess::engine {

// ============================================================================
// Global Variables
// ============================================================================

HashPiecesTable g_hash_pieces{};
HashEnPassantTable g_hash_en_passant{};
HashCastleTable g_hash_castle{};
Hash g_hash_side{0};
Hash g_hash{0};

Node g_search_nodes{0};
Node g_qnodes{0};

// ============================================================================
// Zobrist Hash Initialization
// ============================================================================

void initialize_zobrist_tables() {
    // Reset the global hash seed
    g_hash_seed.reset();

    // Initialize hash values for pieces at each square
    for (std::size_t piece = 0; piece < kNumPieceValues; ++piece) {
        for (std::size_t sq = 0; sq < board::kBoardSize; ++sq) {
            g_hash_pieces[piece][sq] = g_hash_seed.random();
        }
    }

    // Initialize hash values for en passant squares
    for (std::size_t sq = 0; sq < board::kBoardSize; ++sq) {
        g_hash_en_passant[sq] = g_hash_seed.random();
    }

    // Initialize hash values for castling rights
    for (std::size_t castle = 0; castle < kCastlePossibilities; ++castle) {
        g_hash_castle[castle] = g_hash_seed.random();
    }

    // Initialize hash value for side to move
    g_hash_side = g_hash_seed.random();
}

// ============================================================================
// Chessboard Methods - Initialization
// ============================================================================

void Chessboard::reset() {
    // Set side to move
    side_to_move = Color::White;

    // Clear the board - set all squares to Frame first
    for (std::size_t i = 0; i < board::kBoardSize; ++i) {
        square.at_index(i) = Piece::Frame;
    }

    // Set playable squares to Empty
    // The 10x12 mailbox has playable squares at specific indices
    // Rows 2-9 (indices 20-99), columns 1-8 (indices x1-x8)
    for (std::size_t rank = 2; rank <= 9; ++rank) {
        for (std::size_t file = 1; file <= 8; ++file) {
            std::size_t index = rank * 10 + file;
            square.at_index(index) = Piece::Empty;
        }
    }

    // Clear piece lists
    pieces_list.fill(0);
    piece_table.fill(0);

    // Reset piece counters
    white_pieces_counter = 0;
    black_pieces_counter = 0;

    // Reset king positions
    white_king_position = board::Square(0);
    black_king_position = board::Square(0);

    // Clear castling rights
    white_castle_queenside.fill(false);
    white_castle_kingside.fill(false);
    black_castle_queenside.fill(false);
    black_castle_kingside.fill(false);

    // Initialize move pointers to 1 (1-based indexing from Ada)
    moves_pointer.fill(1);

    // Clear move stack
    for (auto& move : moves_stack) {
        move = kEmptyMove;
    }

    // Reset ply counters
    ply = kMinDepth;
    history_ply = 0;

    // Clear move history
    for (auto& hist : moves_history) {
        hist = kEmptyHistoryMove;
    }

    // Clear en passant
    for (auto& ep : en_passant) {
        ep = board::kNoEnPassant;
    }

    // Reset fifty-move counter
    fifty = 0;

    // Reset validity test flag
    force_validity_test = false;

    // Reset global hash
    g_hash = 0;
}

void Chessboard::initialize() {
    // First reset the board
    reset();

    // Place pieces in starting position
    // Black pieces (rank 8 - indices 21-28)
    square[board::A8] = Piece::BlackRook;
    square[board::B8] = Piece::BlackKnight;
    square[board::C8] = Piece::BlackBishop;
    square[board::D8] = Piece::BlackQueen;
    square[board::E8] = Piece::BlackKing;
    square[board::F8] = Piece::BlackBishop;
    square[board::G8] = Piece::BlackKnight;
    square[board::H8] = Piece::BlackRook;

    // Black pawns (rank 7 - indices 31-38)
    square[board::A7] = Piece::BlackPawn;
    square[board::B7] = Piece::BlackPawn;
    square[board::C7] = Piece::BlackPawn;
    square[board::D7] = Piece::BlackPawn;
    square[board::E7] = Piece::BlackPawn;
    square[board::F7] = Piece::BlackPawn;
    square[board::G7] = Piece::BlackPawn;
    square[board::H7] = Piece::BlackPawn;

    // White pawns (rank 2 - indices 81-88)
    square[board::A2] = Piece::WhitePawn;
    square[board::B2] = Piece::WhitePawn;
    square[board::C2] = Piece::WhitePawn;
    square[board::D2] = Piece::WhitePawn;
    square[board::E2] = Piece::WhitePawn;
    square[board::F2] = Piece::WhitePawn;
    square[board::G2] = Piece::WhitePawn;
    square[board::H2] = Piece::WhitePawn;

    // White pieces (rank 1 - indices 91-98)
    square[board::A1] = Piece::WhiteRook;
    square[board::B1] = Piece::WhiteKnight;
    square[board::C1] = Piece::WhiteBishop;
    square[board::D1] = Piece::WhiteQueen;
    square[board::E1] = Piece::WhiteKing;
    square[board::F1] = Piece::WhiteBishop;
    square[board::G1] = Piece::WhiteKnight;
    square[board::H1] = Piece::WhiteRook;

    // Initialize the array of pieces for both sides
    // White pieces - rank 1
    add_white_piece(board::A1);
    add_white_piece(board::B1);
    add_white_piece(board::C1);
    add_white_piece(board::D1);
    add_white_piece(board::E1);
    add_white_piece(board::F1);
    add_white_piece(board::G1);
    add_white_piece(board::H1);
    // White pieces - rank 2
    add_white_piece(board::A2);
    add_white_piece(board::B2);
    add_white_piece(board::C2);
    add_white_piece(board::D2);
    add_white_piece(board::E2);
    add_white_piece(board::F2);
    add_white_piece(board::G2);
    add_white_piece(board::H2);

    // Black pieces - rank 8
    add_black_piece(board::A8);
    add_black_piece(board::B8);
    add_black_piece(board::C8);
    add_black_piece(board::D8);
    add_black_piece(board::E8);
    add_black_piece(board::F8);
    add_black_piece(board::G8);
    add_black_piece(board::H8);
    // Black pieces - rank 7
    add_black_piece(board::A7);
    add_black_piece(board::B7);
    add_black_piece(board::C7);
    add_black_piece(board::D7);
    add_black_piece(board::E7);
    add_black_piece(board::F7);
    add_black_piece(board::G7);
    add_black_piece(board::H7);

    // Set king positions
    white_king_position = board::E1;
    black_king_position = board::E8;

    // Set castling rights to true for both sides
    white_castle_queenside.fill(true);
    white_castle_kingside.fill(true);
    black_castle_queenside.fill(true);
    black_castle_kingside.fill(true);

    // Set side to move
    side_to_move = Color::White;

    // Reset search nodes
    g_search_nodes = 0;

    // Initialize Zobrist hash
    initialize_hash();
}

// ============================================================================
// Chessboard Methods - Piece Tracking
// ============================================================================

void Chessboard::add_white_piece(board::Square sq) {
    assert(white_pieces_counter < kMaxPiecesPerSide && "White pieces are more than allowed");

    white_pieces_counter++;
    pieces_list[white_pieces_counter] = sq.index();
    piece_table[sq.to_index()] = static_cast<std::uint8_t>(white_pieces_counter);
}

void Chessboard::add_black_piece(board::Square sq) {
    assert(black_pieces_counter < kMaxPiecesPerSide && "Black pieces are more than allowed");

    black_pieces_counter++;
    pieces_list[black_pieces_counter + kBlackPiecesStartIndex] = sq.index();
    piece_table[sq.to_index()] = static_cast<std::uint8_t>(black_pieces_counter + kBlackPiecesStartIndex);
}

void Chessboard::delete_white_piece(board::Square sq) {
    std::uint8_t index = piece_table[sq.to_index()];

    // Move the last white piece to fill the gap
    pieces_list[index] = pieces_list[white_pieces_counter];
    pieces_list[white_pieces_counter] = 0;
    white_pieces_counter--;

    // Clear the piece table entry for the deleted piece
    piece_table[sq.to_index()] = 0;

    // Update the piece table for the moved piece
    if (index <= white_pieces_counter) {
        piece_table[pieces_list[index]] = index;
    }
}

void Chessboard::delete_black_piece(board::Square sq) {
    std::uint8_t index = piece_table[sq.to_index()];

    // Move the last black piece to fill the gap
    pieces_list[index] = pieces_list[kBlackPiecesStartIndex + black_pieces_counter];
    pieces_list[kBlackPiecesStartIndex + black_pieces_counter] = 0;
    black_pieces_counter--;

    // Clear the piece table entry for the deleted piece
    piece_table[sq.to_index()] = 0;

    // Update the piece table for the moved piece
    if (index <= kBlackPiecesStartIndex + black_pieces_counter) {
        piece_table[pieces_list[index]] = index;
    }
}

void Chessboard::update_white_piece(board::Square from, board::Square to) {
    std::uint8_t index = piece_table[from.to_index()];
    assert(index > 0 && "Invalid white index value");

    piece_table[from.to_index()] = 0;
    piece_table[to.to_index()] = index;
    pieces_list[index] = to.index();
}

void Chessboard::update_black_piece(board::Square from, board::Square to) {
    std::uint8_t index = piece_table[from.to_index()];
    assert(index > 0 && "Invalid black index value");

    piece_table[from.to_index()] = 0;
    piece_table[to.to_index()] = index;
    pieces_list[index] = to.index();
}

// ============================================================================
// Chessboard Methods - Hash
// ============================================================================

void Chessboard::initialize_hash() {
    // Initialize global Zobrist tables
    initialize_zobrist_tables();

    // Calculate initial hash value
    update_hash();
}

void Chessboard::update_hash() {
    g_hash = 0;

    // Hash piece information for white pieces
    for (std::size_t i = 1; i <= white_pieces_counter; ++i) {
        std::uint8_t sq = pieces_list[i];
        if (sq == 0) break;
        Piece piece = square.at_index(sq);
        g_hash ^= g_hash_pieces[to_index(piece)][sq];
    }

    // Hash piece information for black pieces
    for (std::size_t i = kBlackPiecesStart; i <= kBlackPiecesStartIndex + black_pieces_counter; ++i) {
        std::uint8_t sq = pieces_list[i];
        if (sq == 0) break;
        Piece piece = square.at_index(sq);
        g_hash ^= g_hash_pieces[to_index(piece)][sq];
    }

    // Hash en passant square
    if (en_passant[history_ply] != board::kNoEnPassant) {
        g_hash ^= g_hash_en_passant[en_passant[history_ply].to_index()];
    }

    // Hash castling rights
    // Index 3: White kingside
    if (white_castle_kingside[history_ply]) {
        g_hash ^= g_hash_castle[3];
    }
    // Index 2: White queenside
    if (white_castle_queenside[history_ply]) {
        g_hash ^= g_hash_castle[2];
    }
    // Index 1: Black kingside
    if (black_castle_kingside[history_ply]) {
        g_hash ^= g_hash_castle[1];
    }
    // Index 0: Black queenside
    if (black_castle_queenside[history_ply]) {
        g_hash ^= g_hash_castle[0];
    }

    // Hash side to move
    if (side_to_move == Color::Black) {
        g_hash ^= g_hash_side;
    }
}

// ============================================================================
// Chessboard Methods - Move Stack
// ============================================================================

void Chessboard::clear_moves_list() {
    assert(ply <= kHorizon && "Ply exceeds horizon");
    moves_pointer[ply + 1] = moves_pointer[ply];
}

std::size_t Chessboard::moves_counter(Depth ply_level) const {
    assert(ply_level <= kHorizon && "Ply exceeds horizon");
    return moves_pointer[ply_level + 1] - moves_pointer[ply_level];
}

// ============================================================================
// Chessboard Methods - Debug/Display
// ============================================================================

void Chessboard::display_piece_table() const {
    std::cout << std::endl;

    // Display white pieces
    std::cout << "White pieces: ";
    for (std::size_t i = 1; i <= white_pieces_counter; ++i) {
        std::uint8_t sq = pieces_list[i];
        if (sq == 0) break;
        Piece piece = square.at_index(sq);
        std::cout << piece_symbol(piece) << board::square_to_string(board::Square(sq)) << " ";
    }
    std::cout << std::endl;

    // Display black pieces
    std::cout << "Black pieces: ";
    for (std::size_t i = kBlackPiecesStart; i <= kBlackPiecesStartIndex + black_pieces_counter; ++i) {
        std::uint8_t sq = pieces_list[i];
        if (sq == 0) break;
        Piece piece = square.at_index(sq);
        std::cout << piece_symbol(piece) << board::square_to_string(board::Square(sq)) << " ";
    }
    std::cout << std::endl << std::endl;

    // Display the piece table
    int row = 8;
    for (std::size_t i = 0; i < board::kBoardSize; ++i) {
        Piece piece = square.at_index(i);
        if (piece != Piece::Frame) {
            if (piece_table[i] == 0) {
                std::cout << " .";
            } else {
                if (piece_table[i] < 10) {
                    std::cout << " ";
                }
                std::cout << static_cast<int>(piece_table[i]);
            }
        }
        std::cout << " ";

        // End of row handling
        if (i == 9 || i == 19 || i == 29 || i == 39 || i == 49 ||
            i == 59 || i == 69 || i == 79 || i == 89 || i == 99 || i == 109) {
            if (i >= 20 && i <= 109) {
                std::cout << row;
                row--;
            }
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    std::cout << "   a  b  c  d  e  f  g  h" << std::endl;
    std::cout << std::endl;
}

// ============================================================================
// Chessboard Methods - Attack Detection
// ============================================================================

bool Chessboard::attacks(Color side, board::Square sq) const {
    board::Square target;

    if (side == Color::Black) {
        // Look for bishop/queen/king attacks
        for (auto offset : board::kBishopOffsets) {
            target = sq + offset;
            if (square[target] == Piece::BlackKing) {
                return true;
            }
            while (square[target] == Piece::Empty) {
                target = target + offset;
            }
            if (square[target] == Piece::BlackQueen ||
                square[target] == Piece::BlackBishop) {
                return true;
            }
        }

        // Look for rook/queen/king attacks
        for (auto offset : board::kRookOffsets) {
            target = sq + offset;
            if (square[target] == Piece::BlackKing) {
                return true;
            }
            while (square[target] == Piece::Empty) {
                target = target + offset;
            }
            if (square[target] == Piece::BlackQueen ||
                square[target] == Piece::BlackRook) {
                return true;
            }
        }

        // Look for knight attacks
        for (auto offset : board::kKnightOffsets) {
            target = sq + offset;
            if (square[target] == Piece::BlackKnight) {
                return true;
            }
        }

        // Look for pawn attacks
        if (square[sq + board::kNorthWest] == Piece::BlackPawn) {
            return true;
        }
        if (square[sq + board::kNorthEast] == Piece::BlackPawn) {
            return true;
        }

    } else { // Side = White
        // Look for bishop/queen/king attacks
        for (auto offset : board::kBishopOffsets) {
            target = sq + offset;
            if (square[target] == Piece::WhiteKing) {
                return true;
            }
            while (square[target] == Piece::Empty) {
                target = target + offset;
            }
            if (square[target] == Piece::WhiteQueen ||
                square[target] == Piece::WhiteBishop) {
                return true;
            }
        }

        // Look for rook/queen/king attacks
        for (auto offset : board::kRookOffsets) {
            target = sq + offset;
            if (square[target] == Piece::WhiteKing) {
                return true;
            }
            while (square[target] == Piece::Empty) {
                target = target + offset;
            }
            if (square[target] == Piece::WhiteQueen ||
                square[target] == Piece::WhiteRook) {
                return true;
            }
        }

        // Look for knight attacks
        for (auto offset : board::kKnightOffsets) {
            target = sq + offset;
            if (square[target] == Piece::WhiteKnight) {
                return true;
            }
        }

        // Look for pawn attacks
        if (square[sq + board::kSouthEast] == Piece::WhitePawn) {
            return true;
        }
        if (square[sq + board::kSouthWest] == Piece::WhitePawn) {
            return true;
        }
    }

    return false;
}

board::AttackCollection Chessboard::attacking_square(
    Color side, board::Square sq, bool only_one) const {

    board::AttackCollection atk{};
    atk.number_of_attackers = 0;
    board::Square target;

    auto register_attack = [&atk, this](board::Square origin) {
        atk.attackers[atk.number_of_attackers].origin = origin;
        atk.attackers[atk.number_of_attackers].piece = square[origin];
        atk.number_of_attackers++;
    };

    if (side == Color::Black) {
        // Look for bishop/queen/king attacks
        for (auto offset : board::kBishopOffsets) {
            target = sq + offset;
            if (square[target] == Piece::BlackKing) {
                register_attack(target);
                if (only_one) return atk;
            }
            while (square[target] == Piece::Empty) {
                target = target + offset;
            }
            if (square[target] == Piece::BlackQueen ||
                square[target] == Piece::BlackBishop) {
                register_attack(target);
                if (only_one) return atk;
            }
        }

        // Look for rook/queen/king attacks
        for (auto offset : board::kRookOffsets) {
            target = sq + offset;
            if (square[target] == Piece::BlackKing) {
                register_attack(target);
                if (only_one) return atk;
            }
            while (square[target] == Piece::Empty) {
                target = target + offset;
            }
            if (square[target] == Piece::BlackQueen ||
                square[target] == Piece::BlackRook) {
                register_attack(target);
                if (only_one) return atk;
            }
        }

        // Look for knight attacks
        for (auto offset : board::kKnightOffsets) {
            target = sq + offset;
            if (square[target] == Piece::BlackKnight) {
                register_attack(target);
                if (only_one) return atk;
            }
        }

        // Look for pawn attacks
        target = sq + board::kNorthWest;
        if (square[target] == Piece::BlackPawn) {
            register_attack(target);
            if (only_one) return atk;
        }
        target = sq + board::kNorthEast;
        if (square[target] == Piece::BlackPawn) {
            register_attack(target);
            if (only_one) return atk;
        }

    } else { // Side = White
        // Look for bishop/queen/king attacks
        for (auto offset : board::kBishopOffsets) {
            target = sq + offset;
            if (square[target] == Piece::WhiteKing) {
                register_attack(target);
                if (only_one) return atk;
            }
            while (square[target] == Piece::Empty) {
                target = target + offset;
            }
            if (square[target] == Piece::WhiteQueen ||
                square[target] == Piece::WhiteBishop) {
                register_attack(target);
                if (only_one) return atk;
            }
        }

        // Look for rook/queen/king attacks
        for (auto offset : board::kRookOffsets) {
            target = sq + offset;
            if (square[target] == Piece::WhiteKing) {
                register_attack(target);
                if (only_one) return atk;
            }
            while (square[target] == Piece::Empty) {
                target = target + offset;
            }
            if (square[target] == Piece::WhiteQueen ||
                square[target] == Piece::WhiteRook) {
                register_attack(target);
                if (only_one) return atk;
            }
        }

        // Look for knight attacks
        for (auto offset : board::kKnightOffsets) {
            target = sq + offset;
            if (square[target] == Piece::WhiteKnight) {
                register_attack(target);
                if (only_one) return atk;
            }
        }

        // Look for pawn attacks
        target = sq + board::kSouthEast;
        if (square[target] == Piece::WhitePawn) {
            register_attack(target);
            if (only_one) return atk;
        }
        target = sq + board::kSouthWest;
        if (square[target] == Piece::WhitePawn) {
            register_attack(target);
            if (only_one) return atk;
        }
    }

    return atk;
}

board::AttackCollection Chessboard::defending_square(Color side, board::Square sq) const {
    board::AttackCollection attacking_pieces = attacking_square(side, sq, false);
    board::AttackCollection atk{};
    atk.number_of_attackers = 0;

    for (std::size_t i = 0; i < attacking_pieces.number_of_attackers; ++i) {
        Piece piece = attacking_pieces.attackers[i].piece;
        board::Square origin = attacking_pieces.attackers[i].origin;

        // Keep kings and non-pinned pieces
        if (is_king(piece) || piece_is_absolute_pinned(origin) == board::kNoDirection) {
            atk.attackers[atk.number_of_attackers].origin = origin;
            atk.attackers[atk.number_of_attackers].piece = piece;
            atk.number_of_attackers++;
        }
    }

    return atk;
}

// ============================================================================
// Chessboard Methods - Check Detection
// ============================================================================

bool Chessboard::has_king_in_check(Color side) const {
    if (side == Color::White) {
        return attacks(Color::Black, white_king_position);
    } else {
        return attacks(Color::White, black_king_position);
    }
}

bool Chessboard::white_has_king_in_check() {
    return attacks(Color::Black, white_king_position);
}

bool Chessboard::black_has_king_in_check() {
    return attacks(Color::White, black_king_position);
}

// ============================================================================
// Chessboard Methods - Pin Detection
// ============================================================================

board::Direction Chessboard::absolute_pin_direction(board::Square sq) const {
    assert(sq != white_king_position && "Called absolute_pin_direction for White King");
    assert(sq != black_king_position && "Called absolute_pin_direction for Black King");

    Color opponent_side;
    board::Square king_position;

    if (side_to_move == Color::White) {
        opponent_side = Color::Black;
        king_position = white_king_position;
    } else {
        opponent_side = Color::White;
        king_position = black_king_position;
    }

    // Detect the direction in which to look for attackers
    board::Direction direction = board::find_sliding_direction(sq, king_position);

    // Check if there are other pieces between the king and the square
    if (direction != board::kNoDirection) {
        board::Square check_sq = sq;
        while (true) {
            check_sq = check_sq - direction;
            if (check_sq == king_position) break;
            if (square[check_sq] != Piece::Empty) {
                return board::kNoDirection;
            }
        }
    }

    // If opponent attacks the king along this direction, the piece is pinned
    if (direction != board::kNoDirection) {
        board::AttackFunction attack_fn = board::get_attack_function(direction);
        if (attack_fn(square.data(), opponent_side, sq)) {
            return direction;
        }
    }

    return board::kNoDirection;
}

board::Direction Chessboard::piece_is_absolute_pinned(board::Square sq) const {
    assert(sq != white_king_position && "Called piece_is_absolute_pinned for White King");
    assert(sq != black_king_position && "Called piece_is_absolute_pinned for Black King");

    Color piece_side = is_white_piece(square[sq]) ? Color::White : Color::Black;
    Color opponent_side = (piece_side == Color::White) ? Color::Black : Color::White;
    board::Square king_position = (piece_side == Color::White) ? white_king_position : black_king_position;

    // Detect the direction in which to look for attackers
    board::Direction direction = board::find_sliding_direction(sq, king_position);

    // Check if there are other pieces between the king and the square
    if (direction != board::kNoDirection) {
        board::Square check_sq = sq;
        while (true) {
            check_sq = check_sq - direction;
            if (check_sq == king_position) break;
            if (square[check_sq] != Piece::Empty) {
                return board::kNoDirection;
            }
        }
    }

    // If opponent attacks the king along this direction, the piece is pinned
    if (direction != board::kNoDirection) {
        board::AttackFunction attack_fn = board::get_attack_function(direction);
        if (attack_fn(square.data(), opponent_side, sq)) {
            return direction;
        }
    }

    return board::kNoDirection;
}

// ============================================================================
// Chessboard Methods - Legality Testing
// ============================================================================

bool Chessboard::move_leaves_king_in_check(const Move& move) {
    if (force_validity_test || move.flag == MoveFlag::CaptureEnPassant) {
        // Need to make the move and check
        // Create a temporary chessboard state - we'll modify and restore
        Piece from_piece = square[move.from];
        Piece to_piece = square[move.to];
        board::Square original_white_king = white_king_position;
        board::Square original_black_king = black_king_position;

        // Make the move temporarily
        square[move.to] = move.promotion != Piece::Empty ? move.promotion : move.piece;
        square[move.from] = Piece::Empty;

        // Handle en passant capture
        board::Square ep_captured{0};
        Piece ep_piece = Piece::Empty;
        if (move.flag == MoveFlag::CaptureEnPassant) {
            if (is_white_piece(move.piece)) {
                ep_captured = move.to + board::kSouth;
            } else {
                ep_captured = move.to + board::kNorth;
            }
            ep_piece = square[ep_captured];
            square[ep_captured] = Piece::Empty;
        }

        // Handle castling
        if (move.flag == MoveFlag::Castle) {
            if (move.to == board::G1) {
                square[board::F1] = Piece::WhiteRook;
                square[board::H1] = Piece::Empty;
            } else if (move.to == board::C1) {
                square[board::D1] = Piece::WhiteRook;
                square[board::A1] = Piece::Empty;
            } else if (move.to == board::G8) {
                square[board::F8] = Piece::BlackRook;
                square[board::H8] = Piece::Empty;
            } else if (move.to == board::C8) {
                square[board::D8] = Piece::BlackRook;
                square[board::A8] = Piece::Empty;
            }
        }

        // Update king position if needed
        if (move.piece == Piece::WhiteKing) {
            white_king_position = move.to;
        } else if (move.piece == Piece::BlackKing) {
            black_king_position = move.to;
        }

        // Check if king is in check
        bool in_check = false;
        if (side_to_move == Color::White) {
            in_check = attacks(Color::Black, white_king_position);
        } else {
            in_check = attacks(Color::White, black_king_position);
        }

        // Restore the position
        square[move.from] = from_piece;
        square[move.to] = to_piece;

        if (move.flag == MoveFlag::CaptureEnPassant) {
            square[ep_captured] = ep_piece;
        }

        if (move.flag == MoveFlag::Castle) {
            if (move.to == board::G1) {
                square[board::H1] = Piece::WhiteRook;
                square[board::F1] = Piece::Empty;
            } else if (move.to == board::C1) {
                square[board::A1] = Piece::WhiteRook;
                square[board::D1] = Piece::Empty;
            } else if (move.to == board::G8) {
                square[board::H8] = Piece::BlackRook;
                square[board::F8] = Piece::Empty;
            } else if (move.to == board::C8) {
                square[board::A8] = Piece::BlackRook;
                square[board::D8] = Piece::Empty;
            }
        }

        white_king_position = original_white_king;
        black_king_position = original_black_king;

        return in_check;

    } else if (move.from == white_king_position) {
        return attacks(Color::Black, move.to);

    } else if (move.from == black_king_position) {
        return attacks(Color::White, move.to);
    }

    return false;
}

CheckType Chessboard::move_checks_opponent_king(const Move& move) {
    CheckType type_of_check = CheckType::NoCheck;
    Color side = side_to_move;
    board::Square king_position = (side == Color::White) ? black_king_position : white_king_position;

    // Direct Check - Knight
    if (is_knight(move.piece) || is_knight(move.promotion)) {
        for (auto offset : board::kKnightOffsets) {
            if (move.to + offset == king_position) {
                type_of_check = CheckType::DirectCheck;
                break;
            }
        }
    }
    // Direct Check - Pawn
    else if (move.piece == Piece::WhitePawn && move.promotion == Piece::Empty) {
        if (move.to + board::kNorthWest == black_king_position ||
            move.to + board::kNorthEast == king_position) {
            type_of_check = CheckType::DirectCheck;
        }
    }
    else if (move.piece == Piece::BlackPawn && move.promotion == Piece::Empty) {
        if (move.to + board::kSouthEast == king_position ||
            move.to + board::kSouthWest == white_king_position) {
            type_of_check = CheckType::DirectCheck;
        }
    }
    // Direct Check - Sliding pieces
    else {
        board::Direction direction = board::find_sliding_direction(move.to, king_position);

        if (direction != board::kNoDirection) {
            // Temporarily make the move to check
            Piece from_piece = square[move.from];
            Piece to_piece = square[move.to];

            square[move.from] = Piece::Empty;
            square[move.to] = (move.promotion != Piece::Empty) ? move.promotion : move.piece;

            board::AttackFunction attack_fn = board::get_attack_function(direction);
            if (attack_fn(square.data(), side, king_position)) {
                type_of_check = CheckType::DirectCheck;
            }

            square[move.from] = from_piece;
            square[move.to] = to_piece;
        }
    }

    // Discovery Check and Double Check
    board::Square ep_captured{0};
    Piece ep_piece = Piece::Empty;

    if (move.flag == MoveFlag::CaptureEnPassant) {
        ep_captured = is_white_piece(move.piece) ? move.to + board::kSouth : move.to + board::kNorth;
        ep_piece = square[ep_captured];
        square[ep_captured] = Piece::Empty;
    }

    board::Direction discovery_direction = board::find_sliding_direction(move.from, king_position);

    // For pawn and king, check if moving along the same discovery line
    if (is_pawn(move.piece) || is_king(move.piece)) {
        if (discovery_direction == board::find_sliding_direction(move.to, king_position)) {
            discovery_direction = board::kNoDirection;
        }
    }

    if (discovery_direction != board::kNoDirection) {
        Piece original_piece = square[move.from];
        square[move.from] = Piece::Empty;

        board::AttackFunction attack_fn = board::get_attack_function(discovery_direction);
        if (attack_fn(square.data(), side, king_position)) {
            type_of_check = (type_of_check == CheckType::NoCheck) ?
                CheckType::DiscoveryCheck : CheckType::DoubleCheck;
        }

        square[move.from] = original_piece;
    }

    // En-passant discovery check
    if (move.flag == MoveFlag::CaptureEnPassant) {
        board::Direction ep_direction = board::find_sliding_direction(ep_captured, king_position);
        if (ep_direction != discovery_direction &&
            ep_direction != board::find_sliding_direction(move.to, king_position)) {
            board::AttackFunction attack_fn = board::get_attack_function(ep_direction);
            if (attack_fn(square.data(), side, king_position)) {
                type_of_check = (type_of_check == CheckType::NoCheck) ?
                    CheckType::DiscoveryCheck : CheckType::DoubleCheck;
            }
        }
        square[ep_captured] = ep_piece;
    }

    // Castle check
    if (type_of_check == CheckType::NoCheck && move.flag == MoveFlag::Castle) {
        Piece orig_g = square[move.to];
        Piece orig_d = Piece::Empty;
        Piece orig_e = square[move.from];

        if (move.from == board::E1 && move.to == board::G1) {
            square[board::G1] = Piece::WhiteKing;
            square[board::E1] = Piece::Empty;
            square[board::F1] = Piece::WhiteRook;
            if (board::attacks_from_south(square.data(), Color::White, black_king_position) ||
                board::attacks_from_east(square.data(), Color::White, black_king_position)) {
                type_of_check = CheckType::DirectCheck;
            }
            square[board::F1] = Piece::Empty;
            square[board::G1] = orig_g;
            square[board::E1] = orig_e;
        }
        else if (move.from == board::E1 && move.to == board::C1) {
            orig_d = square[board::D1];
            square[board::D1] = Piece::WhiteRook;
            square[board::E1] = Piece::Empty;
            square[board::C1] = Piece::WhiteKing;
            if (board::attacks_from_south(square.data(), Color::White, black_king_position) ||
                board::attacks_from_west(square.data(), Color::White, black_king_position)) {
                type_of_check = CheckType::DirectCheck;
            }
            square[board::D1] = orig_d;
            square[board::E1] = orig_e;
            square[board::C1] = Piece::Empty;
        }
        else if (move.from == board::E8 && move.to == board::G8) {
            square[board::G8] = Piece::BlackKing;
            square[board::E8] = Piece::Empty;
            square[board::F8] = Piece::BlackRook;
            if (board::attacks_from_north(square.data(), Color::Black, white_king_position) ||
                board::attacks_from_east(square.data(), Color::Black, white_king_position)) {
                type_of_check = CheckType::DirectCheck;
            }
            square[board::E8] = Piece::BlackKing;
            square[board::G8] = Piece::Empty;
            square[board::F8] = Piece::Empty;
        }
        else if (move.from == board::E8 && move.to == board::C8) {
            square[board::E8] = Piece::Empty;
            square[board::C8] = Piece::BlackKing;
            square[board::D8] = Piece::BlackRook;
            if (board::attacks_from_north(square.data(), Color::Black, white_king_position) ||
                board::attacks_from_west(square.data(), Color::Black, white_king_position)) {
                type_of_check = CheckType::DirectCheck;
            }
            square[board::E8] = Piece::BlackKing;
            square[board::D8] = Piece::Empty;
            square[board::C8] = Piece::Empty;
        }
    }

    // Check for checkmate
    if (type_of_check != CheckType::NoCheck) {
        // Temporarily make the move to check for mate
        // Save state
        Piece from_piece = square[move.from];
        Piece to_piece = square[move.to];
        board::Square original_white_king = white_king_position;
        board::Square original_black_king = black_king_position;
        Color original_side = side_to_move;

        // Make move
        square[move.to] = (move.promotion != Piece::Empty) ? move.promotion : move.piece;
        square[move.from] = Piece::Empty;

        if (move.flag == MoveFlag::Castle) {
            if (move.to == board::G1) {
                square[board::F1] = Piece::WhiteRook;
                square[board::H1] = Piece::Empty;
            } else if (move.to == board::C1) {
                square[board::D1] = Piece::WhiteRook;
                square[board::A1] = Piece::Empty;
            } else if (move.to == board::G8) {
                square[board::F8] = Piece::BlackRook;
                square[board::H8] = Piece::Empty;
            } else if (move.to == board::C8) {
                square[board::D8] = Piece::BlackRook;
                square[board::A8] = Piece::Empty;
            }
        }

        if (move.flag == MoveFlag::CaptureEnPassant) {
            board::Square captured = is_white_piece(move.piece) ?
                move.to + board::kSouth : move.to + board::kNorth;
            square[captured] = Piece::Empty;
        }

        if (move.piece == Piece::WhiteKing) white_king_position = move.to;
        if (move.piece == Piece::BlackKing) black_king_position = move.to;

        side_to_move = flip(side_to_move);

        // Check for escapes
        if (!king_has_escapes(type_of_check)) {
            type_of_check = CheckType::Checkmate;
        }

        // Restore state
        side_to_move = original_side;
        white_king_position = original_white_king;
        black_king_position = original_black_king;
        square[move.from] = from_piece;
        square[move.to] = to_piece;

        if (move.flag == MoveFlag::Castle) {
            if (move.to == board::G1) {
                square[board::H1] = Piece::WhiteRook;
                square[board::F1] = Piece::Empty;
            } else if (move.to == board::C1) {
                square[board::A1] = Piece::WhiteRook;
                square[board::D1] = Piece::Empty;
            } else if (move.to == board::G8) {
                square[board::H8] = Piece::BlackRook;
                square[board::F8] = Piece::Empty;
            } else if (move.to == board::C8) {
                square[board::A8] = Piece::BlackRook;
                square[board::D8] = Piece::Empty;
            }
        }

        if (move.flag == MoveFlag::CaptureEnPassant) {
            board::Square captured = is_white_piece(move.piece) ?
                move.to + board::kSouth : move.to + board::kNorth;
            square[captured] = is_white_piece(move.piece) ? Piece::BlackPawn : Piece::WhitePawn;
        }
    }

    return type_of_check;
}

bool Chessboard::king_has_escapes(CheckType type_of_check) {
    board::Square target;
    board::AttackCollection attack_to_the_king;
    Piece attacker = Piece::Empty;
    board::Square origin{0};
    board::Direction direction = board::kNoDirection;
    bool sliding_attack = false;
    board::Square sq{0};

    if (side_to_move == Color::White) {
        // King escapes
        square[white_king_position] = Piece::Empty;

        for (auto offset : board::kKingOffsets) {
            target = white_king_position + offset;
            if (square[target] == Piece::Empty || is_black_piece(square[target])) {
                if (!attacks(Color::Black, target)) {
                    square[white_king_position] = Piece::WhiteKing;
                    return true;
                }
            }
        }

        square[white_king_position] = Piece::WhiteKing;

        if (type_of_check == CheckType::DoubleCheck) {
            return false;
        }

        // Obtain attack information
        if (type_of_check == CheckType::DiscoveryCheck) {
            attack_to_the_king = this->attacking_square(Color::Black, white_king_position, true);
            assert(attack_to_the_king.number_of_attackers == 1);
            attacker = attack_to_the_king.attackers[0].piece;
            origin = attack_to_the_king.attackers[0].origin;
        } else if (type_of_check == CheckType::UnknownCheck) {
            attack_to_the_king = this->attacking_square(Color::Black, white_king_position, false);
            assert(attack_to_the_king.number_of_attackers >= 1 && attack_to_the_king.number_of_attackers <= 2);
            if (attack_to_the_king.number_of_attackers == 2) {
                return false;
            }
            attacker = attack_to_the_king.attackers[0].piece;
            origin = attack_to_the_king.attackers[0].origin;
        } else {
            Move last = last_move_made();
            if (last.flag == MoveFlag::Castle) {
                attacker = Piece::BlackRook;
                origin = (last.to == board::G8) ? board::F8 : board::D8;
            } else {
                attacker = (last.promotion != Piece::Empty) ? last.promotion : last.piece;
                origin = last.to;
            }
        }

        direction = (attacker == Piece::BlackBishop || attacker == Piece::BlackRook || attacker == Piece::BlackQueen) ?
            board::find_sliding_direction(origin, white_king_position) : board::kNoDirection;
        sliding_attack = (direction != board::kNoDirection);

        // Neutralize the attack with pawn captures
        sq = origin + board::kSouthEast;
        if (square[sq] == Piece::WhitePawn && absolute_pin_direction(sq) == board::kNoDirection) {
            return true;
        }
        sq = origin + board::kSouthWest;
        if (square[sq] == Piece::WhitePawn && absolute_pin_direction(sq) == board::kNoDirection) {
            return true;
        }

        // En-passant
        if (en_passant[history_ply] != board::kNoEnPassant) {
            sq = en_passant[history_ply] + board::kSouthWest;
            if (square[sq] == Piece::WhitePawn) return true;
            sq = en_passant[history_ply] + board::kSouthEast;
            if (square[sq] == Piece::WhitePawn) return true;
        }

        // Other pieces can block
        target = origin;
        while (target != white_king_position) {
            // Bishop/queen blocks
            for (auto offset : board::kBishopOffsets) {
                sq = target + offset;
                while (square[sq] == Piece::Empty) {
                    sq = sq + offset;
                }
                if (square[sq] == Piece::WhiteBishop || square[sq] == Piece::WhiteQueen) {
                    if (absolute_pin_direction(sq) == board::kNoDirection) {
                        return true;
                    }
                }
            }
            // Rook/queen blocks
            for (auto offset : board::kRookOffsets) {
                sq = target + offset;
                while (square[sq] == Piece::Empty) {
                    sq = sq + offset;
                }
                if (square[sq] == Piece::WhiteRook || square[sq] == Piece::WhiteQueen) {
                    if (absolute_pin_direction(sq) == board::kNoDirection) {
                        return true;
                    }
                }
            }
            // Knight blocks
            for (auto offset : board::kKnightOffsets) {
                if (square[target + offset] == Piece::WhiteKnight) {
                    if (absolute_pin_direction(target + offset) == board::kNoDirection) {
                        return true;
                    }
                }
            }
            // Pawn blocks
            if (target != origin) {
                sq = target + board::kSouth;
                if (square[sq] == Piece::WhitePawn && absolute_pin_direction(sq) == board::kNoDirection) {
                    return true;
                }
                if (square[sq] == Piece::Empty) {
                    sq = target + board::kSouth + board::kSouth;
                    if (square[sq] == Piece::WhitePawn &&
                        board::rank(sq) == board::rank(board::A2) &&
                        absolute_pin_direction(sq) == board::kNoDirection) {
                        return true;
                    }
                }
            }

            target = target - direction;
            if (!sliding_attack) break;
        }

    } else { // Black to move
        // King escapes
        square[black_king_position] = Piece::Empty;

        for (auto offset : board::kKingOffsets) {
            target = black_king_position + offset;
            if (square[target] == Piece::Empty || is_white_piece(square[target])) {
                if (!attacks(Color::White, target)) {
                    square[black_king_position] = Piece::BlackKing;
                    return true;
                }
            }
        }

        square[black_king_position] = Piece::BlackKing;

        if (type_of_check == CheckType::DoubleCheck) {
            return false;
        }

        // Obtain attack information
        if (type_of_check == CheckType::DiscoveryCheck) {
            attack_to_the_king = this->attacking_square(Color::White, black_king_position, true);
            assert(attack_to_the_king.number_of_attackers == 1);
            attacker = attack_to_the_king.attackers[0].piece;
            origin = attack_to_the_king.attackers[0].origin;
        } else if (type_of_check == CheckType::UnknownCheck) {
            attack_to_the_king = this->attacking_square(Color::White, black_king_position, false);
            assert(attack_to_the_king.number_of_attackers >= 1 && attack_to_the_king.number_of_attackers <= 2);
            if (attack_to_the_king.number_of_attackers == 2) {
                return false;
            }
            attacker = attack_to_the_king.attackers[0].piece;
            origin = attack_to_the_king.attackers[0].origin;
        } else {
            Move last = last_move_made();
            if (last.flag == MoveFlag::Castle) {
                attacker = Piece::WhiteRook;
                origin = (last.to == board::G1) ? board::F1 : board::D1;
            } else {
                attacker = (last.promotion != Piece::Empty) ? last.promotion : last.piece;
                origin = last.to;
            }
        }

        direction = (attacker == Piece::WhiteBishop || attacker == Piece::WhiteRook || attacker == Piece::WhiteQueen) ?
            board::find_sliding_direction(origin, black_king_position) : board::kNoDirection;
        sliding_attack = (direction != board::kNoDirection);

        // Neutralize the attack with pawn captures
        sq = origin + board::kNorthEast;
        if (square[sq] == Piece::BlackPawn && absolute_pin_direction(sq) == board::kNoDirection) {
            return true;
        }
        sq = origin + board::kNorthWest;
        if (square[sq] == Piece::BlackPawn && absolute_pin_direction(sq) == board::kNoDirection) {
            return true;
        }

        // En-passant
        if (en_passant[history_ply] != board::kNoEnPassant) {
            sq = en_passant[history_ply] + board::kNorthWest;
            if (square[sq] == Piece::BlackPawn) return true;
            sq = en_passant[history_ply] + board::kNorthEast;
            if (square[sq] == Piece::BlackPawn) return true;
        }

        // Other pieces can block
        target = origin;
        while (target != black_king_position) {
            // Bishop/queen blocks
            for (auto offset : board::kBishopOffsets) {
                sq = target + offset;
                while (square[sq] == Piece::Empty) {
                    sq = sq + offset;
                }
                if (square[sq] == Piece::BlackBishop || square[sq] == Piece::BlackQueen) {
                    if (absolute_pin_direction(sq) == board::kNoDirection) {
                        return true;
                    }
                }
            }
            // Rook/queen blocks
            for (auto offset : board::kRookOffsets) {
                sq = target + offset;
                while (square[sq] == Piece::Empty) {
                    sq = sq + offset;
                }
                if (square[sq] == Piece::BlackRook || square[sq] == Piece::BlackQueen) {
                    if (absolute_pin_direction(sq) == board::kNoDirection) {
                        return true;
                    }
                }
            }
            // Knight blocks
            for (auto offset : board::kKnightOffsets) {
                if (square[target + offset] == Piece::BlackKnight) {
                    if (absolute_pin_direction(target + offset) == board::kNoDirection) {
                        return true;
                    }
                }
            }
            // Pawn blocks
            if (target != origin) {
                sq = target + board::kNorth;
                if (square[sq] == Piece::BlackPawn && absolute_pin_direction(sq) == board::kNoDirection) {
                    return true;
                }
                if (square[sq] == Piece::Empty) {
                    sq = target + board::kNorth + board::kNorth;
                    if (square[sq] == Piece::BlackPawn &&
                        board::rank(sq) == board::rank(board::A7) &&
                        absolute_pin_direction(sq) == board::kNoDirection) {
                        return true;
                    }
                }
            }

            target = target - direction;
            if (!sliding_attack) break;
        }
    }

    return false;
}

// ============================================================================
// Chessboard Methods - Move Registration
// ============================================================================

void Chessboard::register_move(board::Square from, board::Square to) {
    Move move{};
    move.piece = square[from];
    move.captured = square[to];
    move.from = from;
    move.to = to;
    move.flag = MoveFlag::Standard;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous_flag = AmbiguousFlag::None;
    register_move(move);
}

void Chessboard::register_move(board::Square from, board::Square to, MoveFlag flag) {
    Move move{};
    move.piece = square[from];
    move.captured = square[to];
    move.from = from;
    move.to = to;
    move.flag = flag;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous_flag = AmbiguousFlag::None;

    if (flag == MoveFlag::Promotion) {
        if (side_to_move == Color::White) {
            for (Piece promoted : {Piece::WhiteQueen, Piece::WhiteRook, Piece::WhiteBishop, Piece::WhiteKnight}) {
                move.promotion = promoted;
                register_move(move);
            }
        } else {
            for (Piece promoted : {Piece::BlackQueen, Piece::BlackRook, Piece::BlackBishop, Piece::BlackKnight}) {
                move.promotion = promoted;
                register_move(move);
            }
        }
    } else {
        register_move(move);
    }
}

void Chessboard::register_move(const Move& move) {
    assert(moves_pointer[ply + 1] < kMoveStackSize);

    // Legality test
    if (move_leaves_king_in_check(move)) {
        return;
    }

    Move the_move = move;
    the_move.check = move_checks_opponent_king(move);
    the_move.ambiguous_flag = detect_ambiguous_move_notation(the_move);

    moves_stack[moves_pointer[ply + 1]] = the_move;
    moves_pointer[ply + 1]++;
}

void Chessboard::register_tactical_move(board::Square from, board::Square to) {
    Move move{};
    move.piece = square[from];
    move.captured = square[to];
    move.from = from;
    move.to = to;
    move.flag = MoveFlag::Standard;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous_flag = AmbiguousFlag::None;
    register_tactical_move(move);
}

void Chessboard::register_tactical_move(board::Square from, board::Square to, MoveFlag flag) {
    Move move{};
    move.piece = square[from];
    move.captured = square[to];
    move.from = from;
    move.to = to;
    move.flag = flag;
    move.promotion = Piece::Empty;
    move.check = CheckType::NoCheck;
    move.ambiguous_flag = AmbiguousFlag::None;

    if (flag == MoveFlag::Promotion) {
        if (side_to_move == Color::White) {
            for (Piece promoted : {Piece::WhiteQueen, Piece::WhiteRook, Piece::WhiteBishop, Piece::WhiteKnight}) {
                move.promotion = promoted;
                register_tactical_move(move);
            }
        } else {
            for (Piece promoted : {Piece::BlackQueen, Piece::BlackRook, Piece::BlackBishop, Piece::BlackKnight}) {
                move.promotion = promoted;
                register_tactical_move(move);
            }
        }
    } else {
        register_tactical_move(move);
    }
}

void Chessboard::register_tactical_move(const Move& move) {
    assert(moves_pointer[ply + 1] < kMoveStackSize);

    // Legality test
    if (move_leaves_king_in_check(move)) {
        return;
    }

    Move the_move = move;
    the_move.check = move_checks_opponent_king(move);
    the_move.ambiguous_flag = AmbiguousFlag::None;

    if (move_is_tactical(the_move)) {
        the_move.ambiguous_flag = detect_ambiguous_move_notation(the_move);
        moves_stack[moves_pointer[ply + 1]] = the_move;
        moves_pointer[ply + 1]++;
    }
}

// ============================================================================
// Chessboard Methods - Move Generation
// ============================================================================

void Chessboard::generate_moves() {
    Piece piece;
    board::Square target;
    board::Direction pin_direction = board::kNoDirection;
    board::Square ep_square = en_passant[history_ply];

    force_validity_test = false;
    clear_moves_list();

    if (side_to_move == Color::White) {
        if (white_has_king_in_check()) {
            generate_check_evasion();
            return;
        }

        // Iterate through white pieces
        for (std::size_t i = 1; i <= white_pieces_counter; ++i) {
            std::uint8_t sq_idx = pieces_list[i];
            if (sq_idx == 0) break;
            board::Square sq{sq_idx};
            piece = square[sq];

            pin_direction = (piece == Piece::WhiteKing) ?
                board::kNoDirection : absolute_pin_direction(sq);

            switch (piece) {
                case Piece::WhitePawn: {
                    target = sq + board::kNorth;
                    if (pin_direction == board::kNoDirection ||
                        pin_direction == board::kNorth ||
                        pin_direction == board::kSouth) {
                        if (square[target] == Piece::Empty) {
                            if (target >= board::A8 && target <= board::H8) {
                                register_move(sq, target, MoveFlag::Promotion);
                            } else {
                                register_move(sq, target);
                            }
                            if (sq >= board::A2 && sq <= board::H2) {
                                target = target + board::kNorth;
                                if (square[target] == Piece::Empty) {
                                    register_move(sq, target, MoveFlag::PawnMoveTwoSquares);
                                }
                            }
                        }
                    }
                    // Captures
                    if (pin_direction == board::kNoDirection || pin_direction == board::kNorthWest) {
                        if (board::file(sq) != board::kFileA) {
                            target = sq + board::kNorthWest;
                            if (is_black_piece(square[target])) {
                                if (target >= board::A8 && target <= board::H8) {
                                    register_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kNorthEast) {
                        if (board::file(sq) != board::kFileH) {
                            target = sq + board::kNorthEast;
                            if (is_black_piece(square[target])) {
                                if (target >= board::A8 && target <= board::H8) {
                                    register_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteKnight: {
                    if (pin_direction == board::kNoDirection) {
                        for (auto offset : board::kKnightOffsets) {
                            target = sq + offset;
                            if (square[target] == Piece::Empty || is_black_piece(square[target])) {
                                register_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteBishop: {
                    for (auto offset : board::kBishopOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_move(sq, target);
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteRook: {
                    for (auto offset : board::kRookOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_move(sq, target);
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteQueen: {
                    for (auto offset : board::kQueenOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_move(sq, target);
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteKing: {
                    for (auto offset : board::kKingOffsets) {
                        target = sq + offset;
                        if (square[target] == Piece::Empty || is_black_piece(square[target])) {
                            register_move(sq, target);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

        // Castle moves
        if (!force_validity_test) {
            if (white_castle_kingside[history_ply]) {
                if (square[board::F1] == Piece::Empty && square[board::G1] == Piece::Empty) {
                    if (!attacks(Color::Black, board::F1) && !attacks(Color::Black, board::G1)) {
                        register_move(board::E1, board::G1, MoveFlag::Castle);
                    }
                }
            }
            if (white_castle_queenside[history_ply]) {
                if (square[board::D1] == Piece::Empty && square[board::C1] == Piece::Empty &&
                    square[board::B1] == Piece::Empty) {
                    if (!attacks(Color::Black, board::D1) && !attacks(Color::Black, board::C1)) {
                        register_move(board::E1, board::C1, MoveFlag::Castle);
                    }
                }
            }
        }

    } else { // Black to move
        if (black_has_king_in_check()) {
            generate_check_evasion();
            return;
        }

        // Iterate through black pieces
        for (std::size_t i = kBlackPiecesStart; i <= kBlackPiecesStartIndex + black_pieces_counter; ++i) {
            std::uint8_t sq_idx = pieces_list[i];
            if (sq_idx == 0) break;
            board::Square sq{sq_idx};
            piece = square[sq];

            pin_direction = (piece == Piece::BlackKing) ?
                board::kNoDirection : absolute_pin_direction(sq);

            switch (piece) {
                case Piece::BlackPawn: {
                    target = sq + board::kSouth;
                    if (pin_direction == board::kNoDirection ||
                        pin_direction == board::kSouth ||
                        pin_direction == board::kNorth) {
                        if (square[target] == Piece::Empty) {
                            if (target >= board::A1 && target <= board::H1) {
                                register_move(sq, target, MoveFlag::Promotion);
                            } else {
                                register_move(sq, target);
                            }
                            if (sq >= board::A7 && sq <= board::H7) {
                                target = target + board::kSouth;
                                if (square[target] == Piece::Empty) {
                                    register_move(sq, target, MoveFlag::PawnMoveTwoSquares);
                                }
                            }
                        }
                    }
                    // Captures
                    if (pin_direction == board::kNoDirection || pin_direction == board::kSouthEast) {
                        if (board::file(sq) != board::kFileH) {
                            target = sq + board::kSouthEast;
                            if (is_white_piece(square[target])) {
                                if (target >= board::A1 && target <= board::H1) {
                                    register_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kSouthWest) {
                        if (board::file(sq) != board::kFileA) {
                            target = sq + board::kSouthWest;
                            if (is_white_piece(square[target])) {
                                if (target >= board::A1 && target <= board::H1) {
                                    register_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackKnight: {
                    if (pin_direction == board::kNoDirection) {
                        for (auto offset : board::kKnightOffsets) {
                            target = sq + offset;
                            if (square[target] == Piece::Empty || is_white_piece(square[target])) {
                                register_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackBishop: {
                    for (auto offset : board::kBishopOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_move(sq, target);
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackRook: {
                    for (auto offset : board::kRookOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_move(sq, target);
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackQueen: {
                    for (auto offset : board::kQueenOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_move(sq, target);
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackKing: {
                    for (auto offset : board::kKingOffsets) {
                        target = sq + offset;
                        if (square[target] == Piece::Empty || is_white_piece(square[target])) {
                            register_move(sq, target);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

        // Castle moves
        if (!force_validity_test) {
            if (black_castle_kingside[history_ply]) {
                if (square[board::F8] == Piece::Empty && square[board::G8] == Piece::Empty) {
                    if (!attacks(Color::White, board::F8) && !attacks(Color::White, board::G8)) {
                        register_move(board::E8, board::G8, MoveFlag::Castle);
                    }
                }
            }
            if (black_castle_queenside[history_ply]) {
                if (square[board::D8] == Piece::Empty && square[board::C8] == Piece::Empty &&
                    square[board::B8] == Piece::Empty) {
                    if (!attacks(Color::White, board::D8) && !attacks(Color::White, board::C8)) {
                        register_move(board::E8, board::C8, MoveFlag::Castle);
                    }
                }
            }
        }
    }
}

void Chessboard::generate_check_evasion() {
    board::Square target{0};
    Move last = last_move_made();
    CheckType type_of_check = last.check;
    board::AttackCollection attack_to_the_king;
    Piece attacker = Piece::Empty;
    board::Square origin{0};
    board::Direction direction = board::kNoDirection;
    bool sliding_attack = false;
    board::Square sq{0};

    clear_moves_list();

    if (type_of_check == CheckType::Checkmate) {
        return;
    }

    if (side_to_move == Color::White) {
        // King escapes
        force_validity_test = true;

        for (auto offset : board::kKingOffsets) {
            target = white_king_position + offset;
            if (square[target] == Piece::Empty || is_black_piece(square[target])) {
                register_move(white_king_position, target);
            }
        }

        force_validity_test = false;

        if (type_of_check == CheckType::DoubleCheck) {
            return;
        }

        // Obtain attack information
        if (type_of_check == CheckType::DiscoveryCheck) {
            attack_to_the_king = this->attacking_square(Color::Black, white_king_position, true);
            assert(attack_to_the_king.number_of_attackers == 1);
            attacker = attack_to_the_king.attackers[0].piece;
            origin = attack_to_the_king.attackers[0].origin;
        } else if (type_of_check == CheckType::UnknownCheck) {
            attack_to_the_king = this->attacking_square(Color::Black, white_king_position, false);
            assert(attack_to_the_king.number_of_attackers >= 1 && attack_to_the_king.number_of_attackers <= 2);
            if (attack_to_the_king.number_of_attackers == 2) {
                return;
            }
            attacker = attack_to_the_king.attackers[0].piece;
            origin = attack_to_the_king.attackers[0].origin;
        } else {
            if (last.flag == MoveFlag::Castle) {
                attacker = Piece::BlackRook;
                origin = (last.to == board::G8) ? board::F8 : board::D8;
            } else {
                attacker = (last.promotion != Piece::Empty) ? last.promotion : last.piece;
                origin = last.to;
            }
        }

        direction = (attacker == Piece::BlackBishop || attacker == Piece::BlackRook || attacker == Piece::BlackQueen) ?
            board::find_sliding_direction(origin, white_king_position) : board::kNoDirection;
        sliding_attack = (direction != board::kNoDirection);

        // Neutralize the attack - Pawn captures
        sq = origin + board::kSouthEast;
        if (square[sq] == Piece::WhitePawn && absolute_pin_direction(sq) == board::kNoDirection) {
            if (origin >= board::A8 && origin <= board::H8) {
                register_move(sq, origin, MoveFlag::Promotion);
            } else {
                register_move(sq, origin);
            }
        }
        sq = origin + board::kSouthWest;
        if (square[sq] == Piece::WhitePawn && absolute_pin_direction(sq) == board::kNoDirection) {
            if (origin >= board::A8 && origin <= board::H8) {
                register_move(sq, origin, MoveFlag::Promotion);
            } else {
                register_move(sq, origin);
            }
        }

        // En-passant
        if (en_passant[history_ply] != board::kNoEnPassant) {
            sq = en_passant[history_ply] + board::kSouthWest;
            if (square[sq] == Piece::WhitePawn) {
                register_move(sq, sq + board::kNorthEast, MoveFlag::CaptureEnPassant);
            }
            sq = en_passant[history_ply] + board::kSouthEast;
            if (square[sq] == Piece::WhitePawn) {
                register_move(sq, sq + board::kNorthWest, MoveFlag::CaptureEnPassant);
            }
        }

        // Other pieces block or capture
        target = origin;
        while (target != white_king_position) {
            // Bishop/queen blocks
            for (auto offset : board::kBishopOffsets) {
                sq = target + offset;
                while (square[sq] == Piece::Empty) {
                    sq = sq + offset;
                }
                if (square[sq] == Piece::WhiteBishop || square[sq] == Piece::WhiteQueen) {
                    if (absolute_pin_direction(sq) == board::kNoDirection) {
                        register_move(sq, target);
                    }
                }
            }
            // Rook/queen blocks
            for (auto offset : board::kRookOffsets) {
                sq = target + offset;
                while (square[sq] == Piece::Empty) {
                    sq = sq + offset;
                }
                if (square[sq] == Piece::WhiteRook || square[sq] == Piece::WhiteQueen) {
                    if (absolute_pin_direction(sq) == board::kNoDirection) {
                        register_move(sq, target);
                    }
                }
            }
            // Knight blocks
            for (auto offset : board::kKnightOffsets) {
                if (square[target + offset] == Piece::WhiteKnight) {
                    if (absolute_pin_direction(target + offset) == board::kNoDirection) {
                        register_move(target + offset, target);
                    }
                }
            }
            // Pawn blocks
            if (target != origin) {
                sq = target + board::kSouth;
                if (square[sq] == Piece::WhitePawn && absolute_pin_direction(sq) == board::kNoDirection) {
                    if (target >= board::A8 && target <= board::H8) {
                        register_move(sq, target, MoveFlag::Promotion);
                    } else {
                        register_move(sq, target);
                    }
                }
                if (square[sq] == Piece::Empty) {
                    sq = target + board::kSouth + board::kSouth;
                    if (square[sq] == Piece::WhitePawn &&
                        board::rank(sq) == board::rank(board::A2) &&
                        absolute_pin_direction(sq) == board::kNoDirection) {
                        register_move(sq, target, MoveFlag::PawnMoveTwoSquares);
                    }
                }
            }

            target = target - direction;
            if (!sliding_attack) break;
        }

    } else { // Black to move
        // King escapes
        force_validity_test = true;

        for (auto offset : board::kKingOffsets) {
            target = black_king_position + offset;
            if (square[target] == Piece::Empty || is_white_piece(square[target])) {
                register_move(black_king_position, target);
            }
        }

        force_validity_test = false;

        if (type_of_check == CheckType::DoubleCheck) {
            return;
        }

        // Obtain attack information
        if (type_of_check == CheckType::DiscoveryCheck) {
            attack_to_the_king = this->attacking_square(Color::White, black_king_position, true);
            assert(attack_to_the_king.number_of_attackers == 1);
            attacker = attack_to_the_king.attackers[0].piece;
            origin = attack_to_the_king.attackers[0].origin;
        } else if (type_of_check == CheckType::UnknownCheck) {
            attack_to_the_king = this->attacking_square(Color::White, black_king_position, false);
            assert(attack_to_the_king.number_of_attackers >= 1 && attack_to_the_king.number_of_attackers <= 2);
            if (attack_to_the_king.number_of_attackers == 2) {
                return;
            }
            attacker = attack_to_the_king.attackers[0].piece;
            origin = attack_to_the_king.attackers[0].origin;
        } else {
            if (last.flag == MoveFlag::Castle) {
                attacker = Piece::WhiteRook;
                origin = (last.to == board::G1) ? board::F1 : board::D1;
            } else {
                attacker = (last.promotion != Piece::Empty) ? last.promotion : last.piece;
                origin = last.to;
            }
        }

        direction = (attacker == Piece::WhiteBishop || attacker == Piece::WhiteRook || attacker == Piece::WhiteQueen) ?
            board::find_sliding_direction(origin, black_king_position) : board::kNoDirection;
        sliding_attack = (direction != board::kNoDirection);

        // Neutralize the attack - Pawn captures
        sq = origin + board::kNorthEast;
        if (square[sq] == Piece::BlackPawn && absolute_pin_direction(sq) == board::kNoDirection) {
            if (origin >= board::A1 && origin <= board::H1) {
                register_move(sq, origin, MoveFlag::Promotion);
            } else {
                register_move(sq, origin);
            }
        }
        sq = origin + board::kNorthWest;
        if (square[sq] == Piece::BlackPawn && absolute_pin_direction(sq) == board::kNoDirection) {
            if (origin >= board::A1 && origin <= board::H1) {
                register_move(sq, origin, MoveFlag::Promotion);
            } else {
                register_move(sq, origin);
            }
        }

        // En-passant
        if (en_passant[history_ply] != board::kNoEnPassant) {
            sq = en_passant[history_ply] + board::kNorthWest;
            if (square[sq] == Piece::BlackPawn) {
                register_move(sq, sq + board::kSouthEast, MoveFlag::CaptureEnPassant);
            }
            sq = en_passant[history_ply] + board::kNorthEast;
            if (square[sq] == Piece::BlackPawn) {
                register_move(sq, sq + board::kSouthWest, MoveFlag::CaptureEnPassant);
            }
        }

        // Other pieces block or capture
        target = origin;
        while (target != black_king_position) {
            // Bishop/queen blocks
            for (auto offset : board::kBishopOffsets) {
                sq = target + offset;
                while (square[sq] == Piece::Empty) {
                    sq = sq + offset;
                }
                if (square[sq] == Piece::BlackBishop || square[sq] == Piece::BlackQueen) {
                    if (absolute_pin_direction(sq) == board::kNoDirection) {
                        register_move(sq, target);
                    }
                }
            }
            // Rook/queen blocks
            for (auto offset : board::kRookOffsets) {
                sq = target + offset;
                while (square[sq] == Piece::Empty) {
                    sq = sq + offset;
                }
                if (square[sq] == Piece::BlackRook || square[sq] == Piece::BlackQueen) {
                    if (absolute_pin_direction(sq) == board::kNoDirection) {
                        register_move(sq, target);
                    }
                }
            }
            // Knight blocks
            for (auto offset : board::kKnightOffsets) {
                if (square[target + offset] == Piece::BlackKnight) {
                    if (absolute_pin_direction(target + offset) == board::kNoDirection) {
                        register_move(target + offset, target);
                    }
                }
            }
            // Pawn blocks
            if (target != origin) {
                sq = target + board::kNorth;
                if (square[sq] == Piece::BlackPawn && absolute_pin_direction(sq) == board::kNoDirection) {
                    if (target >= board::A1 && target <= board::H1) {
                        register_move(sq, target, MoveFlag::Promotion);
                    } else {
                        register_move(sq, target);
                    }
                }
                if (square[sq] == Piece::Empty) {
                    sq = target + board::kNorth + board::kNorth;
                    if (square[sq] == Piece::BlackPawn &&
                        board::rank(sq) == board::rank(board::A7) &&
                        absolute_pin_direction(sq) == board::kNoDirection) {
                        register_move(sq, target, MoveFlag::PawnMoveTwoSquares);
                    }
                }
            }

            target = target - direction;
            if (!sliding_attack) break;
        }
    }
}

void Chessboard::generate_tactical_moves() {
    Piece piece;
    board::Square target;
    board::Direction pin_direction = board::kNoDirection;
    board::Square ep_square = en_passant[history_ply];

    force_validity_test = false;
    clear_moves_list();

    if (side_to_move == Color::White) {
        if (white_has_king_in_check()) {
            generate_check_evasion();
            return;
        }

        for (std::size_t i = 1; i <= white_pieces_counter; ++i) {
            std::uint8_t sq_idx = pieces_list[i];
            if (sq_idx == 0) break;
            board::Square sq{sq_idx};
            piece = square[sq];

            pin_direction = (piece == Piece::WhiteKing) ?
                board::kNoDirection : absolute_pin_direction(sq);

            switch (piece) {
                case Piece::WhitePawn: {
                    target = sq + board::kNorth;
                    if (pin_direction == board::kNoDirection ||
                        pin_direction == board::kNorth ||
                        pin_direction == board::kSouth) {
                        if (square[target] == Piece::Empty) {
                            if (target >= board::A8 && target <= board::H8) {
                                register_tactical_move(sq, target, MoveFlag::Promotion);
                            } else {
                                register_tactical_move(sq, target);
                            }
                            if (sq >= board::A2 && sq <= board::H2) {
                                target = target + board::kNorth;
                                if (square[target] == Piece::Empty) {
                                    register_tactical_move(sq, target, MoveFlag::PawnMoveTwoSquares);
                                }
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kNorthWest) {
                        if (board::file(sq) != board::kFileA) {
                            target = sq + board::kNorthWest;
                            if (is_black_piece(square[target])) {
                                if (target >= board::A8 && target <= board::H8) {
                                    register_tactical_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_tactical_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kNorthEast) {
                        if (board::file(sq) != board::kFileH) {
                            target = sq + board::kNorthEast;
                            if (is_black_piece(square[target])) {
                                if (target >= board::A8 && target <= board::H8) {
                                    register_tactical_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_tactical_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_tactical_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteKnight: {
                    if (pin_direction == board::kNoDirection) {
                        for (auto offset : board::kKnightOffsets) {
                            target = sq + offset;
                            if (square[target] == Piece::Empty || is_black_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteBishop: {
                    for (auto offset : board::kBishopOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_tactical_move(sq, target);
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteRook: {
                    for (auto offset : board::kRookOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_tactical_move(sq, target);
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteQueen: {
                    for (auto offset : board::kQueenOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_tactical_move(sq, target);
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteKing: {
                    for (auto offset : board::kKingOffsets) {
                        target = sq + offset;
                        if (square[target] == Piece::Empty || is_black_piece(square[target])) {
                            register_tactical_move(sq, target);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

        // Castle moves
        if (!force_validity_test) {
            if (white_castle_kingside[history_ply]) {
                if (square[board::F1] == Piece::Empty && square[board::G1] == Piece::Empty) {
                    if (!attacks(Color::Black, board::F1) && !attacks(Color::Black, board::G1)) {
                        register_tactical_move(board::E1, board::G1, MoveFlag::Castle);
                    }
                }
            }
            if (white_castle_queenside[history_ply]) {
                if (square[board::D1] == Piece::Empty && square[board::C1] == Piece::Empty &&
                    square[board::B1] == Piece::Empty) {
                    if (!attacks(Color::Black, board::D1) && !attacks(Color::Black, board::C1)) {
                        register_tactical_move(board::E1, board::C1, MoveFlag::Castle);
                    }
                }
            }
        }

    } else { // Black
        if (black_has_king_in_check()) {
            generate_check_evasion();
            return;
        }

        for (std::size_t i = kBlackPiecesStart; i <= kBlackPiecesStartIndex + black_pieces_counter; ++i) {
            std::uint8_t sq_idx = pieces_list[i];
            if (sq_idx == 0) break;
            board::Square sq{sq_idx};
            piece = square[sq];

            pin_direction = (piece == Piece::BlackKing) ?
                board::kNoDirection : absolute_pin_direction(sq);

            switch (piece) {
                case Piece::BlackPawn: {
                    target = sq + board::kSouth;
                    if (pin_direction == board::kNoDirection ||
                        pin_direction == board::kSouth ||
                        pin_direction == board::kNorth) {
                        if (square[target] == Piece::Empty) {
                            if (target >= board::A1 && target <= board::H1) {
                                register_tactical_move(sq, target, MoveFlag::Promotion);
                            } else {
                                register_tactical_move(sq, target);
                            }
                            if (sq >= board::A7 && sq <= board::H7) {
                                target = target + board::kSouth;
                                if (square[target] == Piece::Empty) {
                                    register_tactical_move(sq, target, MoveFlag::PawnMoveTwoSquares);
                                }
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kSouthEast) {
                        if (board::file(sq) != board::kFileH) {
                            target = sq + board::kSouthEast;
                            if (is_white_piece(square[target])) {
                                if (target >= board::A1 && target <= board::H1) {
                                    register_tactical_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_tactical_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_tactical_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kSouthWest) {
                        if (board::file(sq) != board::kFileA) {
                            target = sq + board::kSouthWest;
                            if (is_white_piece(square[target])) {
                                if (target >= board::A1 && target <= board::H1) {
                                    register_tactical_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_tactical_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_tactical_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackKnight: {
                    if (pin_direction == board::kNoDirection) {
                        for (auto offset : board::kKnightOffsets) {
                            target = sq + offset;
                            if (square[target] == Piece::Empty || is_white_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackBishop: {
                    for (auto offset : board::kBishopOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_tactical_move(sq, target);
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackRook: {
                    for (auto offset : board::kRookOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_tactical_move(sq, target);
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackQueen: {
                    for (auto offset : board::kQueenOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                register_tactical_move(sq, target);
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackKing: {
                    for (auto offset : board::kKingOffsets) {
                        target = sq + offset;
                        if (square[target] == Piece::Empty || is_white_piece(square[target])) {
                            register_tactical_move(sq, target);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

        // Castle moves
        if (!force_validity_test) {
            if (black_castle_kingside[history_ply]) {
                if (square[board::F8] == Piece::Empty && square[board::G8] == Piece::Empty) {
                    if (!attacks(Color::White, board::F8) && !attacks(Color::White, board::G8)) {
                        register_tactical_move(board::E8, board::G8, MoveFlag::Castle);
                    }
                }
            }
            if (black_castle_queenside[history_ply]) {
                if (square[board::D8] == Piece::Empty && square[board::C8] == Piece::Empty &&
                    square[board::B8] == Piece::Empty) {
                    if (!attacks(Color::White, board::D8) && !attacks(Color::White, board::C8)) {
                        register_tactical_move(board::E8, board::C8, MoveFlag::Castle);
                    }
                }
            }
        }
    }
}

void Chessboard::generate_captures() {
    Piece piece;
    board::Square target;
    board::Direction pin_direction = board::kNoDirection;
    board::Square ep_square = en_passant[history_ply];

    force_validity_test = false;
    clear_moves_list();

    if (side_to_move == Color::White) {
        if (white_has_king_in_check()) {
            generate_check_evasion();
            return;
        }

        for (std::size_t i = 1; i <= white_pieces_counter; ++i) {
            std::uint8_t sq_idx = pieces_list[i];
            if (sq_idx == 0) break;
            board::Square sq{sq_idx};
            piece = square[sq];

            pin_direction = (piece == Piece::WhiteKing) ?
                board::kNoDirection : absolute_pin_direction(sq);

            switch (piece) {
                case Piece::WhitePawn: {
                    if (pin_direction == board::kNoDirection || pin_direction == board::kNorthWest) {
                        if (board::file(sq) != board::kFileA) {
                            target = sq + board::kNorthWest;
                            if (is_black_piece(square[target])) {
                                if (target >= board::A8 && target <= board::H8) {
                                    register_tactical_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_tactical_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kNorthEast) {
                        if (board::file(sq) != board::kFileH) {
                            target = sq + board::kNorthEast;
                            if (is_black_piece(square[target])) {
                                if (target >= board::A8 && target <= board::H8) {
                                    register_tactical_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_tactical_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_tactical_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteKnight: {
                    if (pin_direction == board::kNoDirection) {
                        for (auto offset : board::kKnightOffsets) {
                            target = sq + offset;
                            if (is_black_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteBishop: {
                    for (auto offset : board::kBishopOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteRook: {
                    for (auto offset : board::kRookOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteQueen: {
                    for (auto offset : board::kQueenOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                target = target + offset;
                            }
                            if (is_black_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteKing: {
                    for (auto offset : board::kKingOffsets) {
                        target = sq + offset;
                        if (is_black_piece(square[target])) {
                            register_tactical_move(sq, target);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

    } else { // Black
        if (black_has_king_in_check()) {
            generate_check_evasion();
            return;
        }

        for (std::size_t i = kBlackPiecesStart; i <= kBlackPiecesStartIndex + black_pieces_counter; ++i) {
            std::uint8_t sq_idx = pieces_list[i];
            if (sq_idx == 0) break;
            board::Square sq{sq_idx};
            piece = square[sq];

            pin_direction = (piece == Piece::BlackKing) ?
                board::kNoDirection : absolute_pin_direction(sq);

            switch (piece) {
                case Piece::BlackPawn: {
                    if (pin_direction == board::kNoDirection || pin_direction == board::kSouthEast) {
                        if (board::file(sq) != board::kFileH) {
                            target = sq + board::kSouthEast;
                            if (is_white_piece(square[target])) {
                                if (target >= board::A1 && target <= board::H1) {
                                    register_tactical_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_tactical_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_tactical_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kSouthWest) {
                        if (board::file(sq) != board::kFileA) {
                            target = sq + board::kSouthWest;
                            if (is_white_piece(square[target])) {
                                if (target >= board::A1 && target <= board::H1) {
                                    register_tactical_move(sq, target, MoveFlag::Promotion);
                                } else {
                                    register_tactical_move(sq, target);
                                }
                            } else if (target == ep_square) {
                                register_tactical_move(sq, target, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackKnight: {
                    if (pin_direction == board::kNoDirection) {
                        for (auto offset : board::kKnightOffsets) {
                            target = sq + offset;
                            if (is_white_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackBishop: {
                    for (auto offset : board::kBishopOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackRook: {
                    for (auto offset : board::kRookOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackQueen: {
                    for (auto offset : board::kQueenOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            target = sq + offset;
                            while (square[target] == Piece::Empty) {
                                target = target + offset;
                            }
                            if (is_white_piece(square[target])) {
                                register_tactical_move(sq, target);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackKing: {
                    for (auto offset : board::kKingOffsets) {
                        target = sq + offset;
                        if (is_white_piece(square[target])) {
                            register_tactical_move(sq, target);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

void Chessboard::generate_see_moves(board::Square target_sq) {
    assert(square[target_sq] != Piece::Frame);

    board::Square to_sq;
    Piece piece;
    board::Direction pin_direction = board::kNoDirection;

    clear_moves_list();
    force_validity_test = false;

    if (side_to_move == Color::White) {
        if (white_has_king_in_check()) {
            generate_see_check_evasion(target_sq);
            return;
        }

        for (std::size_t i = 1; i <= white_pieces_counter; ++i) {
            std::uint8_t sq_idx = pieces_list[i];
            if (sq_idx == 0) break;
            board::Square sq{sq_idx};
            piece = square[sq];

            pin_direction = (piece == Piece::WhiteKing) ?
                board::kNoDirection : absolute_pin_direction(sq);

            switch (piece) {
                case Piece::WhitePawn: {
                    if (pin_direction == board::kNoDirection || pin_direction == board::kNorthWest) {
                        if (board::file(sq) != board::kFileA) {
                            if (sq + board::kNorthWest == target_sq) {
                                if (target_sq >= board::A8 && target_sq <= board::H8) {
                                    register_move(sq, target_sq, MoveFlag::Promotion);
                                } else {
                                    register_move(sq, target_sq);
                                }
                            } else if (target_sq == en_passant[history_ply]) {
                                register_move(sq, target_sq, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kNorthEast) {
                        if (board::file(sq) != board::kFileH) {
                            if (sq + board::kNorthEast == target_sq) {
                                if (target_sq >= board::A8 && target_sq <= board::H8) {
                                    register_move(sq, target_sq, MoveFlag::Promotion);
                                } else {
                                    register_move(sq, target_sq);
                                }
                            } else if (target_sq == en_passant[history_ply]) {
                                register_move(sq, target_sq, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteKnight: {
                    if (pin_direction == board::kNoDirection) {
                        for (auto offset : board::kKnightOffsets) {
                            if (sq + offset == target_sq) {
                                register_move(sq, target_sq);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteBishop: {
                    for (auto offset : board::kBishopOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            to_sq = sq + offset;
                            while (square[to_sq] == Piece::Empty) {
                                to_sq = to_sq + offset;
                            }
                            if (to_sq == target_sq) {
                                register_move(sq, target_sq);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteRook: {
                    for (auto offset : board::kRookOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            to_sq = sq + offset;
                            while (square[to_sq] == Piece::Empty) {
                                to_sq = to_sq + offset;
                            }
                            if (to_sq == target_sq) {
                                register_move(sq, target_sq);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteQueen: {
                    for (auto offset : board::kQueenOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            to_sq = sq + offset;
                            while (square[to_sq] == Piece::Empty) {
                                to_sq = to_sq + offset;
                            }
                            if (to_sq == target_sq) {
                                register_move(sq, target_sq);
                            }
                        }
                    }
                    break;
                }
                case Piece::WhiteKing: {
                    for (auto offset : board::kKingOffsets) {
                        if (sq + offset == target_sq) {
                            register_move(sq, target_sq);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }

    } else { // Black
        if (black_has_king_in_check()) {
            generate_see_check_evasion(target_sq);
            return;
        }

        for (std::size_t i = kBlackPiecesStart; i <= kBlackPiecesStartIndex + black_pieces_counter; ++i) {
            std::uint8_t sq_idx = pieces_list[i];
            if (sq_idx == 0) break;
            board::Square sq{sq_idx};
            piece = square[sq];

            pin_direction = (piece == Piece::BlackKing) ?
                board::kNoDirection : absolute_pin_direction(sq);

            switch (piece) {
                case Piece::BlackPawn: {
                    if (pin_direction == board::kNoDirection || pin_direction == board::kSouthEast) {
                        if (board::file(sq) != board::kFileH) {
                            if (target_sq == sq + board::kSouthEast) {
                                if (target_sq >= board::A1 && target_sq <= board::H1) {
                                    register_move(sq, target_sq, MoveFlag::Promotion);
                                } else {
                                    register_move(sq, target_sq);
                                }
                            } else if (target_sq == en_passant[history_ply]) {
                                register_move(sq, target_sq, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    if (pin_direction == board::kNoDirection || pin_direction == board::kSouthWest) {
                        if (board::file(sq) != board::kFileA) {
                            if (target_sq == sq + board::kSouthWest) {
                                if (target_sq >= board::A1 && target_sq <= board::H1) {
                                    register_move(sq, target_sq, MoveFlag::Promotion);
                                } else {
                                    register_move(sq, target_sq);
                                }
                            } else if (target_sq == en_passant[history_ply]) {
                                register_move(sq, target_sq, MoveFlag::CaptureEnPassant);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackKnight: {
                    if (pin_direction == board::kNoDirection) {
                        for (auto offset : board::kKnightOffsets) {
                            if (target_sq == sq + offset) {
                                register_move(sq, target_sq);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackBishop: {
                    for (auto offset : board::kBishopOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            to_sq = sq + offset;
                            while (square[to_sq] == Piece::Empty) {
                                to_sq = to_sq + offset;
                            }
                            if (to_sq == target_sq) {
                                register_move(sq, target_sq);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackRook: {
                    for (auto offset : board::kRookOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            to_sq = sq + offset;
                            while (square[to_sq] == Piece::Empty) {
                                to_sq = to_sq + offset;
                            }
                            if (to_sq == target_sq) {
                                register_move(sq, target_sq);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackQueen: {
                    for (auto offset : board::kQueenOffsets) {
                        if (pin_direction == board::kNoDirection ||
                            pin_direction == offset ||
                            pin_direction == -offset) {
                            to_sq = sq + offset;
                            while (square[to_sq] == Piece::Empty) {
                                to_sq = to_sq + offset;
                            }
                            if (to_sq == target_sq) {
                                register_move(sq, target_sq);
                            }
                        }
                    }
                    break;
                }
                case Piece::BlackKing: {
                    for (auto offset : board::kKingOffsets) {
                        if (target_sq == sq + offset) {
                            register_move(sq, target_sq);
                        }
                    }
                    break;
                }
                default:
                    break;
            }
        }
    }
}

void Chessboard::generate_see_check_evasion(board::Square see_target) {
    assert(square[see_target] != Piece::Frame);

    board::Square target{0};
    Move last = last_move_made();
    CheckType type_of_check = last.check;
    board::AttackCollection attack_to_the_king;
    board::Square origin = see_target;
    board::Square sq{0};

    clear_moves_list();

    if (type_of_check == CheckType::DoubleCheck || type_of_check == CheckType::Checkmate) {
        return;
    }

    if (side_to_move == Color::White) {
        // King escapes to target only
        force_validity_test = true;

        for (auto offset : board::kKingOffsets) {
            target = white_king_position + offset;
            if (target == see_target) {
                register_move(white_king_position, target);
            }
        }

        force_validity_test = false;

        if (type_of_check == CheckType::DiscoveryCheck) {
            return;
        }

        // Get attack origin
        if (type_of_check == CheckType::UnknownCheck) {
            attack_to_the_king = this->attacking_square(Color::Black, white_king_position, false);
            assert(attack_to_the_king.number_of_attackers >= 1 && attack_to_the_king.number_of_attackers <= 2);
            if (attack_to_the_king.number_of_attackers == 2) {
                return;
            }
            origin = attack_to_the_king.attackers[0].origin;
        } else {
            if (last.flag == MoveFlag::Castle) {
                origin = (last.to == board::G8) ? board::F8 : board::D8;
            } else {
                origin = last.to;
            }
        }

        // Pawn captures attacker
        sq = origin + board::kSouthEast;
        if (square[sq] == Piece::WhitePawn && absolute_pin_direction(sq) == board::kNoDirection) {
            if (origin >= board::A8 && origin <= board::H8) {
                register_move(sq, origin, MoveFlag::Promotion);
            } else {
                register_move(sq, origin);
            }
        }
        sq = origin + board::kSouthWest;
        if (square[sq] == Piece::WhitePawn && absolute_pin_direction(sq) == board::kNoDirection) {
            if (origin >= board::A8 && origin <= board::H8) {
                register_move(sq, origin, MoveFlag::Promotion);
            } else {
                register_move(sq, origin);
            }
        }

        // En-passant
        if (en_passant[history_ply] != board::kNoEnPassant) {
            sq = en_passant[history_ply] + board::kSouthWest;
            if (square[sq] == Piece::WhitePawn) {
                register_move(sq, sq + board::kNorthEast, MoveFlag::CaptureEnPassant);
            }
            sq = en_passant[history_ply] + board::kSouthEast;
            if (square[sq] == Piece::WhitePawn) {
                register_move(sq, sq + board::kNorthWest, MoveFlag::CaptureEnPassant);
            }
        }

        // Other pieces capture attacker
        target = origin;
        for (auto offset : board::kBishopOffsets) {
            sq = target + offset;
            while (square[sq] == Piece::Empty) {
                sq = sq + offset;
            }
            if (square[sq] == Piece::WhiteBishop || square[sq] == Piece::WhiteQueen) {
                if (absolute_pin_direction(sq) == board::kNoDirection) {
                    register_move(sq, target);
                }
            }
        }
        for (auto offset : board::kRookOffsets) {
            sq = target + offset;
            while (square[sq] == Piece::Empty) {
                sq = sq + offset;
            }
            if (square[sq] == Piece::WhiteRook || square[sq] == Piece::WhiteQueen) {
                if (absolute_pin_direction(sq) == board::kNoDirection) {
                    register_move(sq, target);
                }
            }
        }
        for (auto offset : board::kKnightOffsets) {
            if (square[target + offset] == Piece::WhiteKnight) {
                if (absolute_pin_direction(target + offset) == board::kNoDirection) {
                    register_move(target + offset, target);
                }
            }
        }

    } else { // Black
        // King escapes to target only
        force_validity_test = true;

        for (auto offset : board::kKingOffsets) {
            target = black_king_position + offset;
            if (target == see_target) {
                register_move(black_king_position, target);
            }
        }

        force_validity_test = false;

        if (type_of_check == CheckType::DiscoveryCheck) {
            return;
        }

        // Get attack origin
        if (type_of_check == CheckType::UnknownCheck) {
            attack_to_the_king = this->attacking_square(Color::White, black_king_position, false);
            assert(attack_to_the_king.number_of_attackers >= 1 && attack_to_the_king.number_of_attackers <= 2);
            if (attack_to_the_king.number_of_attackers == 2) {
                return;
            }
            origin = attack_to_the_king.attackers[0].origin;
        } else {
            if (last.flag == MoveFlag::Castle) {
                origin = (last.to == board::G1) ? board::F1 : board::D1;
            } else {
                origin = last.to;
            }
        }

        // Pawn captures attacker
        sq = origin + board::kNorthEast;
        if (square[sq] == Piece::BlackPawn && absolute_pin_direction(sq) == board::kNoDirection) {
            if (origin >= board::A1 && origin <= board::H1) {
                register_move(sq, origin, MoveFlag::Promotion);
            } else {
                register_move(sq, origin);
            }
        }
        sq = origin + board::kNorthWest;
        if (square[sq] == Piece::BlackPawn && absolute_pin_direction(sq) == board::kNoDirection) {
            if (origin >= board::A1 && origin <= board::H1) {
                register_move(sq, origin, MoveFlag::Promotion);
            } else {
                register_move(sq, origin);
            }
        }

        // En-passant
        if (en_passant[history_ply] != board::kNoEnPassant) {
            sq = en_passant[history_ply] + board::kNorthWest;
            if (square[sq] == Piece::BlackPawn) {
                register_move(sq, sq + board::kSouthEast, MoveFlag::CaptureEnPassant);
            }
            sq = en_passant[history_ply] + board::kNorthEast;
            if (square[sq] == Piece::BlackPawn) {
                register_move(sq, sq + board::kSouthWest, MoveFlag::CaptureEnPassant);
            }
        }

        // Other pieces capture attacker
        target = origin;
        for (auto offset : board::kBishopOffsets) {
            sq = target + offset;
            while (square[sq] == Piece::Empty) {
                sq = sq + offset;
            }
            if (square[sq] == Piece::BlackBishop || square[sq] == Piece::BlackQueen) {
                if (absolute_pin_direction(sq) == board::kNoDirection) {
                    register_move(sq, target);
                }
            }
        }
        for (auto offset : board::kRookOffsets) {
            sq = target + offset;
            while (square[sq] == Piece::Empty) {
                sq = sq + offset;
            }
            if (square[sq] == Piece::BlackRook || square[sq] == Piece::BlackQueen) {
                if (absolute_pin_direction(sq) == board::kNoDirection) {
                    register_move(sq, target);
                }
            }
        }
        for (auto offset : board::kKnightOffsets) {
            if (square[target + offset] == Piece::BlackKnight) {
                if (absolute_pin_direction(target + offset) == board::kNoDirection) {
                    register_move(target + offset, target);
                }
            }
        }
    }
}

// ============================================================================
// Chessboard Methods - Move History
// ============================================================================

Move Chessboard::last_move_made() const {
    if (history_ply == 0) {
        // No move played yet - check for check from FEN
        Move unknown_move = kEmptyMove;
        if (has_king_in_check(side_to_move)) {
            unknown_move.check = CheckType::UnknownCheck;
        }
        return unknown_move;
    } else {
        return moves_history[history_ply - 1].move;
    }
}

// ============================================================================
// Chessboard Methods - Ambiguous Notation
// ============================================================================

AmbiguousFlag Chessboard::detect_ambiguous_move_notation(const Move& move) {
    // For simplicity, this implementation uses a basic approach
    // Full implementation would require checking notation settings

    Piece piece = move.piece;
    board::Square sq = move.from;
    board::Square target;
    board::Square slide_target;
    board::Direction abs_pin_offset = board::kNoDirection;
    bool rank_disambiguation_required = false;
    bool file_disambiguation_required = false;
    bool disaligned_disambiguation_required = false;

    switch (piece) {
        case Piece::WhiteKnight:
        case Piece::BlackKnight:
            for (auto offset : board::kKnightOffsets) {
                target = move.to + offset;
                if (target != sq && square[target] == piece &&
                    absolute_pin_direction(target) == board::kNoDirection) {
                    if (board::rank(sq) == board::rank(target)) {
                        file_disambiguation_required = true;
                    } else if (board::file(sq) == board::file(target)) {
                        rank_disambiguation_required = true;
                    } else {
                        disaligned_disambiguation_required = true;
                    }
                }
            }
            break;

        case Piece::WhiteBishop:
        case Piece::BlackBishop:
            slide_target = move.to;
            for (auto offset : board::kBishopOffsets) {
                target = slide_target + offset;
                while (square[target] == Piece::Empty) {
                    target = target + offset;
                }
                if (target != sq && square[target] == piece) {
                    abs_pin_offset = absolute_pin_direction(target);
                    if (abs_pin_offset == board::kNoDirection ||
                        abs_pin_offset == offset ||
                        abs_pin_offset == -offset) {
                        if (board::rank(target) == board::rank(sq)) {
                            file_disambiguation_required = true;
                        } else if (board::file(target) == board::file(sq)) {
                            rank_disambiguation_required = true;
                        } else {
                            disaligned_disambiguation_required = true;
                        }
                    }
                }
            }
            break;

        case Piece::WhiteRook:
        case Piece::BlackRook:
            slide_target = move.to;
            for (auto offset : board::kRookOffsets) {
                target = slide_target + offset;
                while (square[target] == Piece::Empty) {
                    target = target + offset;
                }
                if (target != sq && square[target] == piece) {
                    abs_pin_offset = absolute_pin_direction(target);
                    if (abs_pin_offset == board::kNoDirection ||
                        abs_pin_offset == offset ||
                        abs_pin_offset == -offset) {
                        if (board::rank(target) == board::rank(sq)) {
                            file_disambiguation_required = true;
                        } else if (board::file(target) == board::file(sq)) {
                            rank_disambiguation_required = true;
                        } else {
                            disaligned_disambiguation_required = true;
                        }
                    }
                }
            }
            break;

        case Piece::WhiteQueen:
        case Piece::BlackQueen:
            slide_target = move.to;
            for (auto offset : board::kQueenOffsets) {
                target = slide_target + offset;
                while (square[target] == Piece::Empty) {
                    target = target + offset;
                }
                if (target != sq && square[target] == piece) {
                    abs_pin_offset = absolute_pin_direction(target);
                    if (abs_pin_offset == board::kNoDirection ||
                        abs_pin_offset == offset ||
                        abs_pin_offset == -offset) {
                        if (board::rank(target) == board::rank(sq)) {
                            file_disambiguation_required = true;
                        } else if (board::file(target) == board::file(sq)) {
                            rank_disambiguation_required = true;
                        } else {
                            disaligned_disambiguation_required = true;
                        }
                    }
                }
            }
            break;

        default:
            break;
    }

    if (file_disambiguation_required && rank_disambiguation_required) {
        return AmbiguousFlag::Both;
    }

    if (disaligned_disambiguation_required) {
        if (rank_disambiguation_required) {
            return AmbiguousFlag::Rank;
        } else {
            return AmbiguousFlag::File;
        }
    }

    if (file_disambiguation_required) {
        return AmbiguousFlag::File;
    } else if (rank_disambiguation_required) {
        return AmbiguousFlag::Rank;
    }

    return AmbiguousFlag::None;
}

// ============================================================================
// Move Parsing Implementation
// ============================================================================

namespace {

/**
 * Convert a string to lowercase for case-insensitive comparison.
 */
[[nodiscard]] std::string to_lower_string(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return result;
}

} // anonymous namespace

Move Chessboard::parse_move(const std::string& input) {
    // First, generate all legal moves for the current position
    generate_moves();

    // Convert input to lowercase for comparison
    std::string input_lower = to_lower_string(input);

    // Iterate through all legal moves at current ply
    for (std::size_t i = moves_pointer[ply]; i < moves_pointer[ply + 1]; ++i) {
        Move& move = moves_stack[i];

        // Try to match against each notation format
        for (std::size_t notation_idx = 0; notation_idx < kNumNotationTypes; ++notation_idx) {
            NotationType notation = static_cast<NotationType>(notation_idx);

            // Convert move to string in this notation
            std::string move_str = io::move_to_string(move, notation);

            // Convert to lowercase for case-insensitive comparison
            std::string move_str_lower = to_lower_string(move_str);

            // Compare (case-insensitive)
            if (move_str_lower == input_lower) {
                return move;
            }
        }
    }

    // No match found - return empty move
    return kEmptyMove;
}

// ============================================================================
// Move List Printing Implementation
// ============================================================================

void Chessboard::print_moves_list(NotationType notation, std::ostream& out) {
    // Generate legal moves if needed
    generate_moves();

    // Iterate through all legal moves at current ply
    for (std::size_t i = moves_pointer[ply]; i < moves_pointer[ply + 1]; ++i) {
        const Move& move = moves_stack[i];

        // Print move with separator
        io::print_move(move, notation, out);
        out << " ";
    }

    // Print newline at end
    out << "\n";
}

// ============================================================================
// Play/Undo Move Implementation
// ============================================================================

void Chessboard::play(const Move& move) {
    board::Square from = move.from;
    board::Square to = move.to;
    Piece piece = move.piece;
    Piece captured = move.captured;
    Piece promote = move.promotion;
    MoveFlag flag = move.flag;

    // Store data to undo this move
    HistoryMove history_data(move, g_hash, fifty);

    // Prepare data for the next ply
    en_passant[history_ply + 1] = board::kNoEnPassant;
    fifty = fifty + 1;
    white_castle_kingside[history_ply + 1] = white_castle_kingside[history_ply];
    white_castle_queenside[history_ply + 1] = white_castle_queenside[history_ply];
    black_castle_kingside[history_ply + 1] = black_castle_kingside[history_ply];
    black_castle_queenside[history_ply + 1] = black_castle_queenside[history_ply];

    // Handle castle
    if (flag == MoveFlag::Castle) {
        board::Square rook_from, rook_to;
        if (to == board::G1) {
            // White kingside castle
            rook_from = board::H1;
            rook_to = board::F1;
            update_white_piece(board::H1, board::F1);
            white_castle_kingside[history_ply + 1] = false;
        } else if (to == board::C1) {
            // White queenside castle
            rook_from = board::A1;
            rook_to = board::D1;
            update_white_piece(board::A1, board::D1);
            white_castle_queenside[history_ply + 1] = false;
        } else if (to == board::G8) {
            // Black kingside castle
            rook_from = board::H8;
            rook_to = board::F8;
            update_black_piece(board::H8, board::F8);
            black_castle_kingside[history_ply + 1] = false;
        } else if (to == board::C8) {
            // Black queenside castle
            rook_from = board::A8;
            rook_to = board::D8;
            update_black_piece(board::A8, board::D8);
            black_castle_queenside[history_ply + 1] = false;
        } else {
            // Invalid castle move - should not happen
            assert(false && "Invalid castle destination");
            return;
        }
        square[rook_to] = square[rook_from];
        square[rook_from] = Piece::Empty;
    }

    // Detect the en-passant square, if any
    if (flag == MoveFlag::PawnMoveTwoSquares) {
        if (side_to_move == Color::White) {
            en_passant[history_ply + 1] = to + board::kSouth;
        } else {
            en_passant[history_ply + 1] = to + board::kNorth;
        }
    }

    // Remove the captured piece, if any
    if (captured != Piece::Empty) {
        if (side_to_move == Color::White) {
            delete_black_piece(to);
        } else {
            delete_white_piece(to);
        }
    } else if (flag == MoveFlag::CaptureEnPassant) {
        fifty = 0;
        if (side_to_move == Color::White) {
            delete_black_piece(to + board::kSouth);
            square[to + board::kSouth] = Piece::Empty;
        } else {
            delete_white_piece(to + board::kNorth);
            square[to + board::kNorth] = Piece::Empty;
        }
    }

    // Move the piece in the square
    square[to] = square[from];
    square[from] = Piece::Empty;

    // Update the other data involved into the move
    if (side_to_move == Color::White) {
        update_white_piece(from, to);

        if (square[to] == Piece::WhiteKing) {
            white_king_position = to;
            white_castle_kingside[history_ply + 1] = false;
            white_castle_queenside[history_ply + 1] = false;
        }

        if (flag == MoveFlag::Promotion) {
            if (is_black_piece(square[to])) {
                delete_black_piece(to);
            }
            square[to] = promote;
        }
    } else {
        update_black_piece(from, to);

        if (square[to] == Piece::BlackKing) {
            black_king_position = to;
            black_castle_kingside[history_ply + 1] = false;
            black_castle_queenside[history_ply + 1] = false;
        }

        if (flag == MoveFlag::Promotion) {
            if (is_white_piece(square[to])) {
                delete_white_piece(to);
            }
            square[to] = promote;
        }
    }

    // Fifty moves counter
    if (is_pawn(piece) || captured != Piece::Empty) {
        fifty = 0;
    }

    // Castle rights - update if rook/king squares are involved
    if (from == board::A1 || to == board::A1) {
        white_castle_queenside[history_ply + 1] = false;
    }
    if (from == board::H1 || to == board::H1) {
        white_castle_kingside[history_ply + 1] = false;
    }
    if (from == board::A8 || to == board::A8) {
        black_castle_queenside[history_ply + 1] = false;
    }
    if (from == board::H8 || to == board::H8) {
        black_castle_kingside[history_ply + 1] = false;
    }

    // Finally, set up the data for the next move
    side_to_move = flip(side_to_move);
    moves_history[history_ply] = history_data;
    ply = ply + 1;
    history_ply = history_ply + 1;

    update_hash();
}

void Chessboard::undo() {
    ply = ply - 1;
    history_ply = history_ply - 1;
    HistoryMove hmove = moves_history[history_ply];
    fifty = static_cast<FiftyCounter>(hmove.fifty);

    side_to_move = flip(side_to_move);

    board::Square from = hmove.move.from;
    board::Square to = hmove.move.to;
    MoveFlag flag = hmove.move.flag;
    Piece captured = hmove.move.captured;
    g_hash = hmove.hash;

    // Handle castle undo
    if (flag == MoveFlag::Castle) {
        board::Square rook_from, rook_to;
        if (to == board::G1) {
            // White kingside castle
            rook_from = board::F1;
            rook_to = board::H1;
            update_white_piece(rook_from, rook_to);
        } else if (to == board::C1) {
            // White queenside castle
            rook_from = board::D1;
            rook_to = board::A1;
            update_white_piece(rook_from, rook_to);
        } else if (to == board::G8) {
            // Black kingside castle
            rook_from = board::F8;
            rook_to = board::H8;
            update_black_piece(rook_from, rook_to);
        } else if (to == board::C8) {
            // Black queenside castle
            rook_from = board::D8;
            rook_to = board::A8;
            update_black_piece(rook_from, rook_to);
        } else {
            assert(false && "Invalid castle destination in undo");
            return;
        }
        square[rook_to] = square[rook_from];
        square[rook_from] = Piece::Empty;

        // Restore original from/to from history
        from = moves_history[history_ply].move.from;
        to = moves_history[history_ply].move.to;
    }

    // Update piece tracking
    if (side_to_move == Color::White) {
        update_white_piece(to, from);
        if (square[to] == Piece::WhiteKing) {
            white_king_position = from;
        }
    } else {
        update_black_piece(to, from);
        if (square[to] == Piece::BlackKing) {
            black_king_position = from;
        }
    }

    // Move piece back to origin
    square[from] = square[to];
    square[to] = captured;

    // Handle promotion undo
    if (flag == MoveFlag::Promotion) {
        if (side_to_move == Color::White) {
            square[from] = Piece::WhitePawn;
        } else {
            square[from] = Piece::BlackPawn;
        }
    }

    // Restore captured piece
    if (captured != Piece::Empty) {
        if (side_to_move == Color::White) {
            add_black_piece(to);
        } else {
            add_white_piece(to);
        }
    } else if (flag == MoveFlag::CaptureEnPassant) {
        if (side_to_move == Color::White) {
            add_black_piece(to + board::kSouth);
            square[to + board::kSouth] = Piece::BlackPawn;
        } else {
            add_white_piece(to + board::kNorth);
            square[to + board::kNorth] = Piece::WhitePawn;
        }
    }

    moves_history[history_ply + 1] = kEmptyHistoryMove;
}

void Chessboard::play_null_move() {
    // Get the previous move to base null move on
    Move null_move = moves_history[history_ply - 1].move;
    null_move.flag = MoveFlag::NullMove;

    HistoryMove history_data(null_move, g_hash, fifty);

    moves_history[history_ply] = history_data;
    en_passant[history_ply + 1] = board::kNoEnPassant;
    fifty = fifty + 1;
    white_castle_kingside[history_ply + 1] = white_castle_kingside[history_ply];
    white_castle_queenside[history_ply + 1] = white_castle_queenside[history_ply];
    black_castle_kingside[history_ply + 1] = black_castle_kingside[history_ply];
    black_castle_queenside[history_ply + 1] = black_castle_queenside[history_ply];

    side_to_move = flip(side_to_move);

    ply = ply + 1;
    history_ply = history_ply + 1;

    update_hash();
}

void Chessboard::undo_null_move() {
    ply = ply - 1;
    history_ply = history_ply - 1;
    fifty = fifty - 1;

    side_to_move = flip(side_to_move);

    moves_history[history_ply + 1] = kEmptyHistoryMove;
}

void Chessboard::play_check_move(const Move& move) {
    board::Square from = move.from;
    board::Square to = move.to;
    Piece captured = move.captured;
    Piece promote = move.promotion;
    MoveFlag flag = move.flag;

    HistoryMove tmp(move, g_hash, fifty);

    // Prepare data for the next ply
    en_passant[history_ply + 1] = board::kNoEnPassant;

    // Handle castle
    if (flag == MoveFlag::Castle) {
        board::Square rook_from, rook_to;
        if (to == board::G1) {
            rook_from = board::H1;
            rook_to = board::F1;
            update_white_piece(board::H1, board::F1);
            white_castle_kingside[history_ply + 1] = false;
        } else if (to == board::C1) {
            rook_from = board::A1;
            rook_to = board::D1;
            update_white_piece(board::A1, board::D1);
            white_castle_queenside[history_ply + 1] = false;
        } else if (to == board::G8) {
            rook_from = board::H8;
            rook_to = board::F8;
            update_black_piece(board::H8, board::F8);
            black_castle_kingside[history_ply + 1] = false;
        } else if (to == board::C8) {
            rook_from = board::A8;
            rook_to = board::D8;
            update_black_piece(board::A8, board::D8);
            black_castle_queenside[history_ply + 1] = false;
        } else {
            assert(false && "Invalid castle destination");
            return;
        }
        square[rook_to] = square[rook_from];
        square[rook_from] = Piece::Empty;
        // Restore original from/to positions
        from = move.from;
        to = move.to;
    }

    // Detect the en-passant square, if any
    if (flag == MoveFlag::PawnMoveTwoSquares) {
        if (side_to_move == Color::White) {
            en_passant[history_ply + 1] = to + board::kSouth;
        } else {
            en_passant[history_ply + 1] = to + board::kNorth;
        }
    }

    // Remove the captured piece, if any
    if (captured != Piece::Empty) {
        if (side_to_move == Color::White) {
            delete_black_piece(to);
        } else {
            delete_white_piece(to);
        }
    } else if (flag == MoveFlag::CaptureEnPassant) {
        if (side_to_move == Color::White) {
            delete_black_piece(to + board::kSouth);
            square[to + board::kSouth] = Piece::Empty;
        } else {
            delete_white_piece(to + board::kNorth);
            square[to + board::kNorth] = Piece::Empty;
        }
    }

    // Move the piece in the square
    square[to] = square[from];
    square[from] = Piece::Empty;

    // Update the other data involved into the move
    if (side_to_move == Color::White) {
        update_white_piece(from, to);

        if (square[to] == Piece::WhiteKing) {
            white_king_position = to;
        }

        if (flag == MoveFlag::Promotion) {
            if (is_black_piece(square[to])) {
                delete_black_piece(to);
            }
            square[to] = promote;
        }
    } else {
        update_black_piece(from, to);

        if (square[to] == Piece::BlackKing) {
            black_king_position = to;
        }

        if (flag == MoveFlag::Promotion) {
            if (is_white_piece(square[to])) {
                delete_white_piece(to);
            }
            square[to] = promote;
        }
    }

    // Finally, set up the data for the next move
    side_to_move = flip(side_to_move);
    moves_history[history_ply] = tmp;
    ply = ply + 1;
    history_ply = history_ply + 1;
}

void Chessboard::undo_check_move() {
    ply = ply - 1;
    history_ply = history_ply - 1;
    HistoryMove hmove = moves_history[history_ply];

    side_to_move = flip(side_to_move);

    board::Square from = hmove.move.from;
    board::Square to = hmove.move.to;
    MoveFlag flag = hmove.move.flag;
    Piece captured = hmove.move.captured;

    // Handle castle undo
    if (flag == MoveFlag::Castle) {
        board::Square rook_from, rook_to;
        if (to == board::G1) {
            rook_from = board::F1;
            rook_to = board::H1;
            update_white_piece(rook_from, rook_to);
        } else if (to == board::C1) {
            rook_from = board::D1;
            rook_to = board::A1;
            update_white_piece(rook_from, rook_to);
        } else if (to == board::G8) {
            rook_from = board::F8;
            rook_to = board::H8;
            update_black_piece(rook_from, rook_to);
        } else if (to == board::C8) {
            rook_from = board::D8;
            rook_to = board::A8;
            update_black_piece(rook_from, rook_to);
        } else {
            assert(false && "Invalid castle destination in undo");
            return;
        }
        square[rook_to] = square[rook_from];
        square[rook_from] = Piece::Empty;

        from = moves_history[history_ply].move.from;
        to = moves_history[history_ply].move.to;
    }

    if (side_to_move == Color::White) {
        update_white_piece(to, from);
        if (square[to] == Piece::WhiteKing) {
            white_king_position = from;
        }
    } else {
        update_black_piece(to, from);
        if (square[to] == Piece::BlackKing) {
            black_king_position = from;
        }
    }

    square[from] = square[to];
    square[to] = captured;

    if (flag == MoveFlag::Promotion) {
        if (side_to_move == Color::White) {
            square[from] = Piece::WhitePawn;
        } else {
            square[from] = Piece::BlackPawn;
        }
    }

    if (captured != Piece::Empty) {
        if (side_to_move == Color::White) {
            add_black_piece(to);
        } else {
            add_white_piece(to);
        }
    } else if (flag == MoveFlag::CaptureEnPassant) {
        if (side_to_move == Color::White) {
            add_black_piece(to + board::kSouth);
            square[to + board::kSouth] = Piece::BlackPawn;
        } else {
            add_white_piece(to + board::kNorth);
            square[to + board::kNorth] = Piece::WhitePawn;
        }
    }

    moves_history[history_ply + 1] = kEmptyHistoryMove;
}

void Chessboard::play_see_move(const Move& move) {
    board::Square from = move.from;
    board::Square to = move.to;
    Piece captured = move.captured;
    Piece promote = move.promotion;
    MoveFlag flag = move.flag;

    HistoryMove tmp(move, g_hash, fifty);

    // Prepare data for the next ply
    en_passant[history_ply + 1] = board::kNoEnPassant;

    // Detect the en-passant square, if any
    if (flag == MoveFlag::PawnMoveTwoSquares) {
        if (side_to_move == Color::White) {
            en_passant[history_ply + 1] = to + board::kSouth;
        } else {
            en_passant[history_ply + 1] = to + board::kNorth;
        }
    }

    // Remove the captured piece, if any
    if (is_chess_piece(captured)) {
        if (side_to_move == Color::White) {
            delete_black_piece(to);
        } else {
            delete_white_piece(to);
        }
    }

    // Move the piece in the square
    square[to] = square[from];
    square[from] = Piece::Empty;

    // Update the other data involved into the move
    if (side_to_move == Color::White) {
        update_white_piece(from, to);

        if (square[to] == Piece::WhiteKing) {
            white_king_position = to;
        }

        if (flag == MoveFlag::Promotion) {
            if (is_black_piece(square[to])) {
                delete_black_piece(to);
            }
            square[to] = promote;
        }
    } else {
        update_black_piece(from, to);

        if (square[to] == Piece::BlackKing) {
            black_king_position = to;
        }

        if (flag == MoveFlag::Promotion) {
            if (is_white_piece(square[to])) {
                delete_white_piece(to);
            }
            square[to] = promote;
        }
    }

    // Finally, set up the data for the next move
    side_to_move = flip(side_to_move);
    moves_history[history_ply] = tmp;
    ply = ply + 1;
    history_ply = history_ply + 1;
}

void Chessboard::undo_see_move() {
    ply = ply - 1;
    history_ply = history_ply - 1;
    HistoryMove hmove = moves_history[history_ply];

    side_to_move = flip(side_to_move);

    board::Square from = hmove.move.from;
    board::Square to = hmove.move.to;
    MoveFlag flag = hmove.move.flag;
    Piece captured = hmove.move.captured;

    if (side_to_move == Color::White) {
        update_white_piece(to, from);
        if (square[to] == Piece::WhiteKing) {
            white_king_position = from;
        }
    } else {
        update_black_piece(to, from);
        if (square[to] == Piece::BlackKing) {
            black_king_position = from;
        }
    }

    square[from] = square[to];
    square[to] = captured;

    if (flag == MoveFlag::Promotion) {
        if (side_to_move == Color::White) {
            square[from] = Piece::WhitePawn;
        } else {
            square[from] = Piece::BlackPawn;
        }
    }

    if (side_to_move == Color::White) {
        add_black_piece(to);
    } else {
        add_white_piece(to);
    }

    moves_history[history_ply + 1] = kEmptyHistoryMove;
}

} // namespace chess::engine

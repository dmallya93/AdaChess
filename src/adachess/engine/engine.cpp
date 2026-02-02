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

#include <iostream>
#include <iomanip>

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

} // namespace chess::engine

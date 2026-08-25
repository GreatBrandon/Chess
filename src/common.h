#pragma once
#include <array>

namespace White {
    constexpr char KING = 'K',
        QUEEN = 'Q',
        BISHOP = 'B',
        KNIGHT = 'N',
        ROOK = 'R',
        PAWN = 'P';
}

namespace Black {
    constexpr char KING = 'k',
        QUEEN = 'q',
        BISHOP = 'b',
        KNIGHT = 'n',
        ROOK = 'r',
        PAWN = 'p';
}

struct Move {
    int sRow;
    int sCol;
    int eRow;
    int eCol;
};

inline bool isWhitePiece(char piece) {
    return piece == White::KING ||
        piece == White::QUEEN ||
        piece == White::BISHOP ||
        piece == White::KNIGHT ||
        piece == White::ROOK ||
        piece == White::PAWN;
}

inline bool isBlackPiece(char piece) {
    return piece == Black::KING ||
        piece == Black::QUEEN ||
        piece == Black::BISHOP ||
        piece == Black::KNIGHT ||
        piece == Black::ROOK ||
        piece == Black::PAWN;
}

const std::array<std::array<char, 8>, 8> START_BOARD = {{
    { { 'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r' } },
    { {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'} },
    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    { {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'} },
    { {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'} }
}};

struct BoardState {
    std::array<std::array<char, 8>, 8> board = START_BOARD;
    bool isWhite = true;
    bool whiteCanShortCastle = true;
    bool whiteCanLongCastle = true;
    bool blackCanShortCastle = true;
    bool blackCanLongCastle = true;
    int enPassantRow = -1;
    int enPassantCol = -1;
};
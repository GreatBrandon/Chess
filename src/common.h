#pragma once
#include <array>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>

namespace White {
    constexpr char KING = 'K',
        QUEEN = 'Q',
        BISHOP = 'B',
        KNIGHT = 'N',
        ROOK = 'R',
        PAWN = 'P';

    constexpr std::array pieces{
        QUEEN, BISHOP, KNIGHT, ROOK, PAWN
    };
}

namespace Black {
    constexpr char KING = 'k',
        QUEEN = 'q',
        BISHOP = 'b',
        KNIGHT = 'n',
        ROOK = 'r',
        PAWN = 'p';
    constexpr std::array pieces{
        QUEEN, BISHOP, KNIGHT, ROOK, PAWN
    };
}

struct Move {
    int sRow;
    int sCol;
    int eRow;
    int eCol;
    int evaluation;

    bool operator==(const Move& other) const {
        return sRow == other.sRow 
            && sCol == other.sCol 
            && eRow == other.eRow 
            && eCol == other.eCol;
    }
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

const std::array<std::array<char, 8>, 8> START_BOARD = { {
    { {'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r' } },
    { {'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'} },
    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    { {'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'} },
    { {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'} }
} };

//const std::array<std::array<char, 8>, 8> START_BOARD = { {
//    { {'r', 'n', 'b', ' ', ' ', 'b', 'n', 'r' } },
//    { {'p', 'p', 'p', 'p', ' ', 'k', ' ', 'p'} },
//    { {' ', ' ', ' ', ' ', ' ', ' ', 'p', ' '} },
//    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
//    { {' ', ' ', ' ', 'q', 'P', ' ', ' ', ' '} },
//    { {' ', ' ', ' ', 'B', ' ', ' ', ' ', 'N'} },
//    { {'P', 'P', 'P', 'P', ' ', ' ', 'P', 'P'} },
//    { {'R', 'N', 'B', 'Q', 'K', ' ', ' ', 'R'} }
//} };

struct BoardState {
    std::array<std::array<char, 8>, 8> board = START_BOARD;
    bool isWhite = true;
    bool whiteCanShortCastle = true;
    bool whiteCanLongCastle = true;
    bool blackCanShortCastle = true;
    bool blackCanLongCastle = true;
    bool checkForMate = false; // only used INTERNALLY to determine mating
    std::pair<int, int> enPassantPos = { -1, -1 };
    std::pair<int, int> whiteKingPos = { 7, 4 };
    std::pair<int, int> blackKingPos = { 0, 4 };
    int stalemateMoveCounter = 0;
    int evaluation = 0;
    std::vector<std::pair<std::array<char, 8>, Move>> legalMoves;
    std::map<std::array<char, 8>, std::vector<Move>> ambigiousMoves;

    BoardState(){
        legalMoves.reserve(64);
    }

    BoardState(const BoardState& other) : 
        board(other.board),
        isWhite(other.isWhite),
        whiteCanShortCastle(other.whiteCanShortCastle),
        whiteCanLongCastle(other.whiteCanLongCastle),
        blackCanShortCastle(other.blackCanShortCastle),
        blackCanLongCastle(other.blackCanLongCastle),
        checkForMate(other.checkForMate),
        enPassantPos(other.enPassantPos),
        whiteKingPos(other.whiteKingPos),
        blackKingPos(other.blackKingPos),
        stalemateMoveCounter(other.stalemateMoveCounter),
        evaluation(other.evaluation)
    {
        legalMoves.reserve(64);
    }

    BoardState& operator=(const BoardState& other) {
        board = other.board;
        isWhite = other.isWhite;
        whiteCanShortCastle = other.whiteCanShortCastle;
        whiteCanLongCastle = other.whiteCanLongCastle;
        blackCanShortCastle = other.blackCanShortCastle;
        blackCanLongCastle = other.blackCanLongCastle;
        checkForMate = other.checkForMate;
        enPassantPos = other.enPassantPos;
        whiteKingPos = other.whiteKingPos;
        blackKingPos = other.blackKingPos;
        stalemateMoveCounter = other.stalemateMoveCounter;
        evaluation = other.evaluation;

        legalMoves.clear();
        ambigiousMoves.clear();

        return *this;
    }
};
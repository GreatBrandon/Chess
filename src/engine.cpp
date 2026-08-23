#include "engine.h"
#include <iostream>

Engine::Engine() {};

map<string, Move> Engine::generateLegalMoves(array<array<char, 8>, 8> board, bool white) {
    this->board = board;
    isWhite = white;
    legalMoves.clear();

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = board[row][col];
            if (white and isWhitePiece(piece)) {
                if (piece == White::KING) {

                } else if (piece == White::QUEEN) {
                    generateLegalMovesRow(row, col);
                    generateLegalMovesCol(row, col);
                    generateLegalMovesDiagonal(row, col);
                } else if (piece == White::BISHOP) {
                    generateLegalMovesDiagonal(row, col);
                } else if (piece == White::KNIGHT) {
                    generateLegalMovesKnight(row, col);
                } else if (piece == White::ROOK) {
                    generateLegalMovesRow(row, col);
                    generateLegalMovesCol(row, col);
                } else {

                }
            } else if (!white and isBlackPiece(piece)) {
                if (piece == Black::KING) {

                } else if (piece == Black::QUEEN) {
                    generateLegalMovesRow(row, col);
                    generateLegalMovesCol(row, col);
                    generateLegalMovesDiagonal(row, col);
                } else if (piece == Black::BISHOP) {
                    generateLegalMovesDiagonal(row, col);
                } else if (piece == Black::KNIGHT) {
                    generateLegalMovesKnight(row, col);
                } else if (piece == Black::ROOK) {
                    generateLegalMovesRow(row, col);
                    generateLegalMovesCol(row, col);
                } else {

                }
            }
        }
    }
    for (auto const& [notation, move] : legalMoves) {
        std::cout << notation << ':' << move.sRow << ',' << move.sCol << ',' << move.eRow << ',' << move.eCol << std::endl;
    }

    return legalMoves;
}

// TODO add check and mate for all below
void Engine::generateLegalMovesRow(int sRow, int sCol) {
    for (int i = sCol + 1; i < 8; i++) {
        if (addMove(sRow, sCol, sRow, i)) break;
    }
    for (int i = sCol - 1; i >= 0; i--) {
        if (addMove(sRow, sCol, sRow, i)) break;
    }
}

void Engine::generateLegalMovesCol(int sRow, int sCol) {
    for (int i = sRow + 1; i < 8; i++) {
        if (addMove(sRow, sCol, i, sCol)) break;
    }
    for (int i = sRow - 1; i >= 0; i--) {
        if (addMove(sRow, sCol, i, sCol)) break;
    }
}

void Engine::generateLegalMovesDiagonal(int sRow, int sCol) {
    for (int col = sCol + 1, row = sRow - 1; col < 8 && row >= 0; col++, row--) {
        if (addMove(sRow, sCol, row, col)) break;
    }
    for (int col = sCol + 1, row = sRow + 1; col < 8 && row < 8; col++, row++) {
        if (addMove(sRow, sCol, row, col)) break;
    }
    for (int col = sCol - 1, row = sRow - 1; col >= 0 && row >= 0; col--, row--) {
        if (addMove(sRow, sCol, row, col)) break;
    }
    for (int col = sCol - 1, row = sRow + 1; col >= 0 && row < 8; col--, row++) {
        if (addMove(sRow, sCol, row, col)) break;
    }
}

void Engine::generateLegalMovesKnight(int sRow, int sCol) {
    if (sCol - 2 >= 0) {
        if (sRow - 1 >= 0) addMove(sRow, sCol, sRow - 1, sCol - 2);
        if (sRow + 1 < 8) addMove(sRow, sCol, sRow + 1, sCol - 2);
    }
    if (sCol + 2 < 8) {
        if (sRow - 1 >= 0) addMove(sRow, sCol, sRow - 1, sCol + 2);
        if (sRow + 1 < 8) addMove(sRow, sCol, sRow + 1, sCol + 2);
    }
    if (sCol - 1 >= 0) {
        if (sRow - 2 >= 0) addMove(sRow, sCol, sRow - 2, sCol - 1);
        if (sRow + 2 < 8) addMove(sRow, sCol, sRow + 2, sCol - 1);
    }
    if (sCol + 1 < 8) {
        if (sRow - 2 >= 0) addMove(sRow, sCol, sRow - 2, sCol + 1);
        if (sRow + 2 < 8) addMove(sRow, sCol, sRow + 2, sCol + 1);
    }
}

bool Engine::addMove(int sRow, int sCol, int eRow, int eCol) {
    char end = board[eRow][eCol];
    if ((isWhite && isWhitePiece(end)) || (!isWhite && isBlackPiece(end))) return true;
    string move = string();

    move += toupper(board[sRow][sCol]);

    if (board[eRow][eCol] != ' ') {
        move += 'x';
    }

    move = move + (char)('a' + eCol) + (char)('8' - eRow);
    bool conflict = legalMoves.find(move) != legalMoves.end();
    if (conflict) {
        cout << "Disambiguate move: " << move << endl;
    } else {
        legalMoves[move] = Move(sRow, sCol, eRow, eCol);
    }
    // TODO generate notation
    // TODO check
    if (end != ' ') return true;
    return false;
}
#include "board.h"
#include <iostream>

chessBoard::chessBoard() {
    newGame();

    // for testing
    board = {
    {
        {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}},
        {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}},
        {{' ', ' ', ' ', ' ', 'p', ' ', ' ', ' '}},
        {{' ', 'b', ' ', 'Q', 'R', ' ', ' ', 'p'}},
        {{' ', ' ', 'p', ' ', ' ', ' ', ' ', ' '}},
        {{' ', ' ', ' ', ' ', 'N', ' ', ' ', ' '}},
        {{' ', ' ', ' ', ' ', ' ', ' ', 'P', ' '}},
        {{' ', 'K', ' ', ' ', ' ', ' ', ' ', ' '}}
    }
    };
    legalMoves = engine.generateLegalMoves(board, boardState);
}

void chessBoard::newGame() {
    board = START_BOARD;
    moves.clear();
    boardState = BoardState();
    previousMoves.clear();
    legalMoves = engine.generateLegalMoves(board, boardState);
}

void chessBoard::playMove(int start, int end) {
    const int sRow = start / 8;
    const int sCol = start % 8;
    const int eRow = end / 8;
    const int eCol = end % 8;

    for (auto const& [notation, move] : legalMoves) {
        if (move.sRow == sRow && move.sCol == sCol && move.eRow == eRow && move.eCol == eCol) {
            playMove(sRow, sCol, eRow, eCol);
            // TODO pawn promotion
            moves.push_back(notation);
            changePlayer();
            return;
        }
    }
}

void chessBoard::playMove(int sRow, int sCol, int eRow, int eCol) {
    board[eRow][eCol] = board[sRow][sCol];
    board[sRow][sCol] = ' ';
}

void chessBoard::changePlayer() {
    boardState.isWhite = !boardState.isWhite;
    legalMoves = engine.generateLegalMoves(board, boardState);
}

vector<pair<int, int>> chessBoard::getValidMovesFromPosition(int sRow, int sCol) {
    vector<pair<int, int>> validMoves;
    for (auto const& [notation, move] : legalMoves) {
        if (move.sRow == sRow && move.sCol == sCol) {
            validMoves.push_back({ move.eRow, move.eCol });
        }
    }
    return validMoves;
}

bool chessBoard::isWhite() {
    return boardState.isWhite;
}

bool chessBoard::isBlack() {
    return !boardState.isWhite;
}
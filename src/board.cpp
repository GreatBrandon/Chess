#include "board.h"
#include <iostream>
#include <map>

chessBoard::chessBoard() {
    newGame();

    // for testing
    //board = {
    //{
    //    {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}},
    //    {{'k', ' ', ' ', 'p', ' ', ' ', ' ', ' '}},
    //    {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}},
    //    {{' ', 'b', ' ', 'Q', 'R', ' ', ' ', ' '}},
    //    {{' ', ' ', 'p', ' ', ' ', 'p', ' ', 'p'}},
    //    {{' ', ' ', ' ', ' ', 'N', ' ', ' ', ' '}},
    //    {{' ', ' ', ' ', ' ', ' ', ' ', 'P', ' '}},
    //    {{' ', 'K', ' ', ' ', ' ', ' ', ' ', ' '}}
    //}
    //};
    //board = { {
    //    { { 'r', 'r', ' ', ' ', 'k', ' ', ' ', 'r' } },
    //    { {'P', ' ', 'P', ' ', ' ', ' ', ' ', ' '} },
    //    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    //    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    //    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    //    { {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '} },
    //    { {' ', ' ', 'P', ' ', 'P', ' ', ' ', ' '} },
    //    { {'R', ' ', 'P', 'K', 'P', ' ', ' ', 'R'} }
    //} };
    //boardState.board = board;
    //previousMoves.clear();
    //previousMoves.push_back(boardState);
    //boardState.legalMoves = engine.generateLegalMoves(boardState);
}

void chessBoard::newGame() {
    board = START_BOARD;
    moves.clear();
    boardState = BoardState();
    isDraw = false;
    isCheckmate = false;
    lastIrreversibleMove = 0;
    boardState.legalMoves = engine.generateLegalMoves(boardState);
    previousMoves.clear();
    previousMoves.push_back(boardState);
}

void chessBoard::playMove(int start, int end, char promotionPiece) {
    const int sRow = start / 8;
    const int sCol = start % 8;
    const int eRow = end / 8;
    const int eCol = end % 8;

    for (auto const& [notation, move] : boardState.legalMoves) {
        if (move.sRow == sRow && move.sCol == sCol && move.eRow == eRow && move.eCol == eCol) {
            if (notation.find('=') != string::npos && notation[notation.find('=') + 1] != promotionPiece) continue;

            // 50 move rule
            if (board[eRow][eCol] != ' ' || board[sRow][sCol] == White::PAWN || board[sRow][sCol] == Black::PAWN) {
                boardState.stalemateMoveCounter = 0;
                lastIrreversibleMove = previousMoves.size();
            } else boardState.stalemateMoveCounter++;

            // Remove castling rights
            if (board[sRow][sCol] == White::KING) {
                boardState.whiteCanLongCastle = false;
                boardState.whiteCanShortCastle = false;
            }
            if (board[sRow][sCol] == Black::KING) {
                boardState.blackCanLongCastle = false;
                boardState.blackCanShortCastle = false;
            }
            if ((sRow == 7 && sCol == 0 && board[sRow][sCol] == White::ROOK)
                || (eRow == 7 && eCol == 0 && board[eRow][eCol] == White::ROOK)) {
                boardState.whiteCanLongCastle = false;
            }
            if ((sRow == 7 && sCol == 7 && board[sRow][sCol] == White::ROOK)
                || (eRow == 7 && eCol == 7 && board[eRow][eCol] == White::ROOK)) {
                boardState.whiteCanShortCastle = false;
            }
            if ((sRow == 0 && sCol == 0 && board[sRow][sCol] == Black::ROOK)
                || (eRow == 0 && eCol == 0 && board[eRow][eCol] == Black::ROOK)) {
                boardState.blackCanLongCastle = false;
            }
            if ((sRow == 0 && sCol == 7 && board[sRow][sCol] == Black::ROOK)
                || (eRow == 0 && eCol == 7 && board[eRow][eCol] == Black::ROOK)) {
                boardState.blackCanShortCastle = false;
            }

            // En passant
            if (board[sRow][sCol] == White::PAWN && sRow - eRow == 2) {
                boardState.enPassantCol = eCol;
                boardState.enPassantRow = sRow - 1;
            } else if (board[sRow][sCol] == Black::PAWN && eRow - sRow == 2) {
                boardState.enPassantCol = eCol;
                boardState.enPassantRow = eRow - 1;
            } else {
                if (eRow == boardState.enPassantRow && eCol == boardState.enPassantCol) board[sRow][eCol] = ' ';
                boardState.enPassantCol = -1;
                boardState.enPassantRow = -1;
            }

            playMove(sRow, sCol, eRow, eCol);

            // Pawn promotion
            if (board[eRow][eCol] == White::PAWN && eRow == 0) {
                board[eRow][eCol] = notation[notation.find('=') + 1];
            } else if (board[eRow][eCol] == Black::PAWN && eRow == 7) {
                board[eRow][eCol] = notation[notation.find('=') + 1] + 0x20;
            }

            // Castle
            if (notation.starts_with("O-O-O")) {
                playMove(sRow, 0, sRow, 3);
            } else if (notation.starts_with("O-O")) {
                playMove(sRow, 7, sRow, 5);
            }

            moves.push_back(notation);
            previousMoves.push_back(boardState);

            // Check stalemate
            if (boardState.stalemateMoveCounter == 100) {
                cout << "DRAW BY 50 MOVE RULE" << endl;
                isDraw = true;
            }
            map<array<array<char, 8>, 8>, int> previousPositions;
            for (int i = lastIrreversibleMove; i < previousMoves.size(); i++) {
                if (++previousPositions[previousMoves[i].board] == 3) {
                    cout << "DRAW BY REPETITION" << endl;
                    isDraw = true;
                    // TODO add castling and en passant checks to this to fully satisfy FIDE rules
                }
            }

            changePlayer();
            return;
        }
    }
}

void chessBoard::playMove(int sRow, int sCol, int eRow, int eCol) {
    board[eRow][eCol] = board[sRow][sCol];
    board[sRow][sCol] = ' ';
    boardState.board = board;
}

void chessBoard::changePlayer() {
    boardState.isWhite = !boardState.isWhite;
    boardState.legalMoves = engine.generateLegalMoves(boardState);
    //legalMoves = engine.generateLegalMoves(boardState);
    if (boardState.legalMoves.size() == 0) {
        if (boardState.isInCheck) {
            cout << "CHECKMATE" << endl;
            isCheckmate = true;
        } else {
            cout << "DRAW BY STALEMATE" << endl;
            isDraw = true;
        }
    } else if (isDraw) {
        boardState.legalMoves.clear();
    }
}

vector<pair<int, int>> chessBoard::getValidMovesFromPosition(int sRow, int sCol) {
    vector<pair<int, int>> validMoves;
    for (auto const& [notation, move] : boardState.legalMoves) {
        if (move.sRow == sRow && move.sCol == sCol) {
            validMoves.push_back({ move.eRow, move.eCol });
        }
    }
    return validMoves;
}

bool chessBoard::isWhite() const {
    return boardState.isWhite;
}

bool chessBoard::isBlack() const {
    return !boardState.isWhite;
}
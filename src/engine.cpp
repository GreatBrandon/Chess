#include "engine.h"
#include <iostream>

Engine::Engine() {};

map<string, Move> Engine::generateLegalMoves(BoardState state) {
    this->board = state.board;
    this->isWhite = state.isWhite;
    this->boardState = state;
    legalMoves.clear();

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = board[row][col];
            this->sRow = row;
            this->sCol = col;
            if (isWhite and isWhitePiece(piece)) {
                if (piece == White::KING) {
                    generateLegalMovesKing();
                } else if (piece == White::QUEEN) {
                    generateLegalMovesRow();
                    generateLegalMovesCol();
                    generateLegalMovesDiagonal();
                } else if (piece == White::BISHOP) {
                    generateLegalMovesDiagonal();
                } else if (piece == White::KNIGHT) {
                    generateLegalMovesKnight();
                } else if (piece == White::ROOK) {
                    generateLegalMovesRow();
                    generateLegalMovesCol();
                } else {
                    generateLegalMovesPawn();
                }
            } else if (!isWhite and isBlackPiece(piece)) {
                if (piece == Black::KING) {
                    generateLegalMovesKing();
                } else if (piece == Black::QUEEN) {
                    generateLegalMovesRow();
                    generateLegalMovesCol();
                    generateLegalMovesDiagonal();
                } else if (piece == Black::BISHOP) {
                    generateLegalMovesDiagonal();
                } else if (piece == Black::KNIGHT) {
                    generateLegalMovesKnight();
                } else if (piece == Black::ROOK) {
                    generateLegalMovesRow();
                    generateLegalMovesCol();
                } else {
                    generateLegalMovesPawn();
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
void Engine::generateLegalMovesRow() {
    for (int i = sCol + 1; i < 8; i++) {
        if (addMove(sRow, i)) break;
    }
    for (int i = sCol - 1; i >= 0; i--) {
        if (addMove(sRow, i)) break;
    }
}

void Engine::generateLegalMovesCol() {
    for (int i = sRow + 1; i < 8; i++) {
        if (addMove(i, sCol)) break;
    }
    for (int i = sRow - 1; i >= 0; i--) {
        if (addMove(i, sCol)) break;
    }
}

void Engine::generateLegalMovesDiagonal() {
    for (int col = sCol + 1, row = sRow - 1; col < 8 && row >= 0; col++, row--) {
        if (addMove(row, col)) break;
    }
    for (int col = sCol + 1, row = sRow + 1; col < 8 && row < 8; col++, row++) {
        if (addMove(row, col)) break;
    }
    for (int col = sCol - 1, row = sRow - 1; col >= 0 && row >= 0; col--, row--) {
        if (addMove(row, col)) break;
    }
    for (int col = sCol - 1, row = sRow + 1; col >= 0 && row < 8; col--, row++) {
        if (addMove(row, col)) break;
    }
}

void Engine::generateLegalMovesKnight() {
    generateLegalMovesUsingOffsets({ {
        {-2, -1}, {-2, 1},
        {-1, -2}, {-1, 2},
        {1, -2}, {1, 2},
        {2, -1}, {2, 1}
    } });
}

void Engine::generateLegalMovesKing() {
    generateLegalMovesUsingOffsets({ {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1},
    } });
    if (isWhite && sRow == 7 && sCol == 4) {
        if (boardState.whiteCanLongCastle && board[7][1] == ' ' && board[7][2] == ' ' && board[7][3] == ' ') {
            legalMoves["O-O-O"] = Move(sRow, sCol, sRow, 2);
        }
        if (boardState.whiteCanShortCastle && board[7][5] == ' ' && board[7][6] == ' ') {
            legalMoves["O-O"] = Move(sRow, sCol, sRow, 6);
        }
    } else if (!isWhite && sRow == 0 && sCol == 4) {
        if (boardState.blackCanLongCastle && board[0][1] == ' ' && board[0][2] == ' ' && board[0][3] == ' ') {
            legalMoves["O-O-O"] = Move(sRow, sCol, sRow, 2);
        }
        if (boardState.blackCanShortCastle && board[0][5] == ' ' && board[0][6] == ' ') {
            legalMoves["O-O"] = Move(sRow, sCol, sRow, 6);
        }
    }
    // TODO check
}

void Engine::generateLegalMovesPawn() {
    if (isWhite) {
        if (board[sRow - 1][sCol] == ' ') {
            addMove(sRow - 1, sCol);
            if (sRow == 6 && board[sRow - 2][sCol] == ' ') {
                addMove(sRow - 2, sCol);
            }
        }
        if (sCol - 1 >= 0 && isBlackPiece(board[sRow - 1][sCol - 1])) addMove(sRow - 1, sCol - 1);
        if (sCol + 1 < 8 && isBlackPiece(board[sRow - 1][sCol + 1])) addMove(sRow - 1, sCol + 1);
        if (abs(boardState.enPassantCol - sCol) == 1 && boardState.enPassantRow == sRow - 1) addMove(boardState.enPassantRow, boardState.enPassantCol);
    } else {
        if (board[sRow + 1][sCol] == ' ') {
            addMove(sRow + 1, sCol);
            if (sRow == 1 && board[sRow + 2][sCol] == ' ') {
                addMove(sRow + 2, sCol);
            }
        }
        if (sCol - 1 >= 0 && isWhitePiece(board[sRow + 1][sCol - 1])) addMove(sRow + 1, sCol - 1);
        if (sCol + 1 < 8 && isWhitePiece(board[sRow + 1][sCol + 1])) addMove(sRow + 1, sCol + 1);
        if (abs(boardState.enPassantCol - sCol) == 1 && boardState.enPassantRow == sRow + 1) addMove(boardState.enPassantRow, boardState.enPassantCol);

    }
}

void Engine::generateLegalMovesUsingOffsets(array<pair<int, int>, 8> offsets) {
    for (auto [oRow, oCol] : offsets) {
        int eRow = sRow + oRow;
        int eCol = sCol + oCol;

        if (eRow >= 0 && eRow < 8 && eCol >= 0 && eCol < 8) addMove(eRow, eCol);
    }
}

bool Engine::addMove(int eRow, int eCol) {
    char end = board[eRow][eCol];
    if ((isWhite && isWhitePiece(end)) || (!isWhite && isBlackPiece(end))) return true;
    bool isPawn = board[sRow][sCol] == White::PAWN || board[sRow][sCol] == Black::PAWN;
    string move = string();

    if (!isPawn) {
        move += toupper(board[sRow][sCol]);
    }

    if (isPawn && board[eRow][eCol] == ' ' && abs(eCol - sCol) == 1) {
        move = move + (char)('a' + sCol) + 'x';
    } else if (board[eRow][eCol] != ' ') {
        if (isPawn) {
            move = move + (char)('a' + sCol) + 'x';
        } else {
            move += 'x';
        }
    }

    move = move + (char)('a' + eCol) + (char)('8' - eRow);

    if (isPawn && ((isWhite && eRow == 0) || (!isWhite && eRow == 7))) {
        array<string, 4> promotionMoves = { {
            move + "=Q", move + "=B", move + "=N", move + "=R"
            } };
        for (auto& m : promotionMoves) {
            bool conflict = legalMoves.find(m) != legalMoves.end();
            if (conflict) {
                cout << "Disambiguate move: " << m << endl;
            } else {
                legalMoves[m] = Move(sRow, sCol, eRow, eCol);
            }
        }
        return false;
    }

    bool conflict = legalMoves.find(move) != legalMoves.end();
    if (conflict) {
        cout << "Disambiguate move: " << move << endl;
    } else {
        legalMoves[move] = Move(sRow, sCol, eRow, eCol);
    }
    // TODO check
    if (end != ' ') return true;
    return false;
}
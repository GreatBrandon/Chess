#include "engine.h"
#include <iostream>

const array<pair<int, int>, 8> knightOffsets = { {
    {-2, -1}, {-2, 1},
    {-1, -2}, {-1, 2},
    {1, -2}, {1, 2},
    {2, -1}, {2, 1}
} };

Engine::Engine() {};

unordered_map<string, Move> Engine::generateLegalMoves(BoardState state, bool checkingMode) {
    this->board = state.board;
    this->isWhite = state.isWhite;
    this->boardState = state;
    this->checkingMode = checkingMode;
    legalMoves.clear();

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (board[row][col] == White::KING) {
                if (isWhite) {
                    kingPos = { row,col };
                } else {
                    opponentKingPos = { row,col };
                }
            } else if (board[row][col] == Black::KING) {
                if (!isWhite) {
                    kingPos = { row,col };
                } else {
                    opponentKingPos = { row,col };
                }
            }
        }
    }

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
    if (isWhite) {
        cout << "White to move" << endl;
    } else {
        cout << "Black to move" << endl;
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
    generateLegalMovesUsingOffsets(knightOffsets);
}

void Engine::generateLegalMovesKing() {
    generateLegalMovesUsingOffsets({ {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1},
    } });

    bool shortCastled = false;
    bool longCastled = false;

    if (isWhite && sRow == 7 && sCol == 4 && !boardState.isInCheck) {
        if (boardState.whiteCanLongCastle && board[7][1] == ' ' && board[7][2] == ' ' && board[7][3] == ' ') {
            longCastled = true;
        }
        if (boardState.whiteCanShortCastle && board[7][5] == ' ' && board[7][6] == ' ') {
            shortCastled = true;
        }
    } else if (!isWhite && sRow == 0 && sCol == 4 && !boardState.isInCheck) {
        if (boardState.blackCanLongCastle && board[0][1] == ' ' && board[0][2] == ' ' && board[0][3] == ' ') {
            longCastled = true;
        }
        if (boardState.blackCanShortCastle && board[0][5] == ' ' && board[0][6] == ' ') {
            shortCastled = true;
        }
    }

    string move = string();
    array<array<char, 8>, 8> tempBoard = board;
    bool isCheck = false;

    if (shortCastled) {
        tempBoard = board;
        tempBoard[sRow][6] = tempBoard[sRow][sCol];
        tempBoard[sRow][sCol] = ' ';
        tempBoard[sRow][5] = tempBoard[sRow][0];
        tempBoard[sRow][7] = ' ';
        move = "O-O";

        if (isOppositeKingChecked(tempBoard, sRow, 5)) {
            move += "+";
        }

        legalMoves[move] = Move(sRow, sCol, sRow, 6);
    }
    if (longCastled) {
        tempBoard = board;
        tempBoard[sRow][2] = tempBoard[sRow][sCol];
        tempBoard[sRow][sCol] = ' ';
        tempBoard[sRow][3] = tempBoard[sRow][0];
        tempBoard[sRow][0] = ' ';
        move = "O-O-O";

        if (isOppositeKingChecked(tempBoard, sRow, 3)) {
            auto tempState = boardState;
            tempState.isInCheck = true;
            tempState.isWhite = !boardState.isWhite;
            tempState.board = tempBoard;

            move += '+';
            //if (!checkingMode && Engine::generateLegalMoves(tempState, true).size() == 0) {
            //    move += '#';
            //} else {
            //    move += '+';
            //}
        }

        legalMoves[move] = Move(sRow, sCol, sRow, 2);
    }
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

    array<array<char, 8>, 8> tempBoard = board;


    if (isPawn && ((isWhite && eRow == 0) || (!isWhite && eRow == 7))) {
        array<string, 4> promotionMoves = { {
            move + "=Q", move + "=B", move + "=N", move + "=R"
            } };
        for (auto& m : promotionMoves) {
            if (isWhite) tempBoard[eRow][eCol] = m.back();
            else tempBoard[eRow][eCol] = m.back() + 0x20;
            tempBoard[sRow][sCol] = ' ';

            bool conflict = legalMoves.find(m) != legalMoves.end();
            if (conflict) {
                cout << "Disambiguate move: " << m << endl; // TODO
            } else {
                if (isOppositeKingChecked(tempBoard, eRow, eCol)) m += '+';
                legalMoves[m] = Move(sRow, sCol, eRow, eCol);
            }
        }
        return false;
    }

    bool conflict = legalMoves.find(move) != legalMoves.end();
    if (conflict) {
        cout << "Disambiguate move: " << move << endl; //TODO
    } else {
        if (isOppositeKingChecked(tempBoard, eRow, eCol)) {
            move += "+";
            // TODO MATE
        }
        legalMoves[move] = Move(sRow, sCol, eRow, eCol);
    }
    if (end != ' ') return true;
    return false;
}

bool Engine::isOppositeKingChecked(array<array<char, 8>, 8> board, int row, int col) {
    char piece = board[row][col];
    if ((isWhite && (piece == White::ROOK || piece == White::QUEEN)) || (!isWhite && (piece == Black::ROOK || piece == Black::QUEEN))) {
        if (row == opponentKingPos.first) {
            int s = min(col, opponentKingPos.second) + 1;
            int e = max(col, opponentKingPos.second);
            while (s < e) {
                if (board[row][s] != ' ') return false;
                s++;
            }
            return true;
        } else if (col == opponentKingPos.second) {
            int s = min(row, opponentKingPos.first) + 1;
            int e = max(row, opponentKingPos.first);
            while (s < e) {
                if (board[s][col] != ' ') return false;
                s++;
            }
            return true;
        }
    }
    if ((isWhite && (piece == White::BISHOP || piece == White::QUEEN)) || (!isWhite && (piece == Black::BISHOP || piece == Black::QUEEN))) {
        if (abs(row - opponentKingPos.first) == abs(col - opponentKingPos.second)) {
            int s = min(row, opponentKingPos.first) + 1;
            int e = max(row, opponentKingPos.first);
            int c = min(col, opponentKingPos.second) + 1;
            while (s < e) {
                if (board[s][c] != ' ') return false;
                s++;
                c++;
            }
            return true;
        }
    }
    if ((isWhite && piece == White::KNIGHT) || (!isWhite && piece == Black::KNIGHT)) {
        for (auto const& [r, c] : knightOffsets) {
            if (row + r == opponentKingPos.first && col + c == opponentKingPos.second) return true;
        }
    }
    if ((isWhite && piece == White::PAWN && ((col - 1 >= 0 && board[row - 1][col - 1] == Black::KING) || (col + 1 < 8 && board[row - 1][col + 1] == Black::KING))) || (!isWhite && piece == Black::PAWN && ((col - 1 >= 0 && board[row + 1][col - 1] == White::KING) || (col + 1 < 8 && board[row + 1][col + 1] == White::KING)))) return true;
    return false;
}
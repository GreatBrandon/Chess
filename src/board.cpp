#include "board.h"
#include <iostream>

chessBoard::chessBoard() {
    newGame();
}

void chessBoard::newGame() {
    board = {
    {
        {{'r', 'n', 'b', 'q', 'k', 'b', 'n', 'r'}},
        {{'p', 'p', 'p', 'p', 'p', 'p', 'p', 'p'}},
        {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}},
        {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}},
        {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}},
        {{' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '}},
        {{'P', 'P', 'P', 'P', 'P', 'P', 'P', 'P'}},
        {{'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'}}
    }
    };
    moves.clear();
    whiteCanCastle = true;
    blackCanCastle = true;
    white = true;
    black = false;
}


void chessBoard::displayBoard() {
    cout << "    A B C D E F G H" << endl << "-----------------------" << endl;
    for (int i = 0; i < 8; i++) {
        cout << '|' << 8 - i << "| ";
        for (int j = 0; j < 8; j++) {
            cout << board[i][j] << ' ';
        }
        cout << "|" << 8 - i << '|' << endl;
    }
    cout << "-----------------------" << endl;
}

void chessBoard::playMove(int start, int end) {
    const int sRow = start / 8;
    const int sCol = start % 8;
    const int eRow = end / 8;
    const int eCol = end % 8;

    if (start < 0 || start > 63) {
        cout << "Invalid start index" << start << endl;
    } else if (end < 0 || end > 63) {
        cout << "Invalid end index" << end << endl;
    } else if (start == end) {
        cout << "Start and end are the same square" << endl;
    } else if (board[sRow][sCol] == ' ') {
        cout << "Cannot move empty square" << endl;
    } else if (white && (board[sRow][sCol] < 'A' || (board[sRow][sCol] > 'Z'))) {
        cout << "White player cannot move black pieces" << endl;
    } else if (black && (board[sRow][sCol] < 'a' || (board[sRow][sCol] > 'z'))) {
        cout << "Black player cannot move white pieces" << endl;
    } else if (white && board[eRow][eCol] > 'A' && (board[eRow][eCol] < 'Z')) {
        cout << "White player cannot take their own piece" << endl;
    } else if (black && board[eRow][eCol] > 'a' && (board[eRow][eCol] < 'z')) {
        cout << "Black player cannot take their own piece" << endl;
        // TODO get rid of error messages
    } else {
        bool isPawn = (board[sRow][sCol] == 'p' || board[sRow][sCol] == 'P');
        bool promoted = false;
        if (isPawn) {
            if (abs(sRow - eRow) > 2 || abs(sCol - eCol) > 1) return;
            if ((white && eRow == sRow - 1) || (black && eRow == sRow + 1)) { // pawn moves by 1
                if (abs(sCol - eCol) == 1) {
                    if (board[eRow][eCol] == ' ') {
                        if (enPassantCol == eCol && ((white && sRow == 3) || (black && sRow == 4))) { // en passant valid
                            board[sRow][eCol] = ' ';
                            board[eRow][eCol] = 'x'; // so notation works fine
                        } else {
                            return;
                        }
                    }
                } else {
                    if (board[eRow][eCol] != ' ') return;
                }
            } else { // pawn moves by 2
                if (board[eRow][eCol] != ' ' || abs(sCol - eCol) != 0 || (white && board[sRow - 1][sCol] != ' ') || (white && sRow != 6) || (black && board[sRow + 1][eCol] != ' ') || (black && sRow != 1)) return;
                enPassantCol = eCol;
                canEnPassant = true;
            }

            if ((white && eRow == 0) || (black && eRow == 7)) {
                promoted = true;
            }
        } else if (board[sRow][sCol] == 'r' || board[sRow][sCol] == 'R') {
            if (sRow == eRow) {
                int s = min(sCol, eCol);
                int e = max(sCol, eCol);
                if (!checkRow(sRow, s, e)) return;
            } else if (sCol == eCol) {
                int s = min(sRow, eRow);
                int e = max(sRow, eRow);
                if (!checkCol(sCol, s, e)) return;
            } else {
                return;
            }
        } else if (board[sRow][sCol] == 'b' || board[sRow][sCol] == 'B') {
            if (abs(sRow - eRow) == abs(sCol - eCol)) {
                if (!checkDiagonal(sRow, sCol, eRow, eCol)) return;
            } else {
                return;
            }
        } else if (board[sRow][sCol] == 'q' || board[sRow][sCol] == 'Q') {
            if (sRow == eRow) {
                int s = min(sCol, eCol);
                int e = max(sCol, eCol);
                if (!checkRow(sRow, s, e)) return;
            } else if (sCol == eCol) {
                int s = min(sRow, eRow);
                int e = max(sRow, eRow);
                if (!checkCol(sCol, s, e)) return;
            } else if (abs(sRow - eRow) == abs(sCol - eCol)) {
                if (!checkDiagonal(sRow, sCol, eRow, eCol)) return;
            } else {
                return;
            }
        } else if (board[sRow][sCol] == 'n' || board[sRow][sCol] == 'N') {
            if (!((abs(sRow - eRow) == 2 && abs(sCol - eCol) == 1) || (abs(sRow - eRow) == 1 && abs(sCol - eCol) == 2))) return;
        } else {
            if (board[sRow][sCol] == 'K' && whiteCanCastle) {
                if (eRow == 7 && eCol == 2 && checkRow(7, 0, 4)) {
                    playMove(sRow, sCol, eRow, eCol);
                    playMove(7, 0, 7, 3);
                    moves.push_back("O-O-O");
                    return;
                    // TODO check
                } else if (eRow == 7 && eCol == 6 && checkRow(7, 4, 7)) {
                    playMove(sRow, sCol, eRow, eCol);
                    playMove(7, 7, 7, 5);
                    moves.push_back("O-O");
                    changePlayer();
                    return;
                    // TODO check
                } 
                // TODO castling, check, mate etc.
            } else {
                if (abs(sRow - eRow) > 1 || abs(sCol - eCol) > 1) {
                    return;
                }
            }

        }

        string move = string();
        if (!isPawn) {
            move += toupper(board[sRow][sCol]);
        }

        if (board[eRow][eCol] != ' ') {
            if (isPawn) {
                move = move + (char)('a' + sCol) + 'x';
            } else {
                move += 'x';
            }
        }

        move = move + (char)('a' + eCol) + (char)('8' - eRow);

        playMove(sRow, sCol, eRow, eCol);

        if (promoted) {
            move += "=Q";
            if (white) {
                board[eRow][eCol] = 'Q';
            } else {
                board[eRow][eCol] = 'q';
            }
        }
        moves.push_back(move);

        changePlayer();
    }
}

void chessBoard::playMove(int sRow, int sCol, int eRow, int eCol) {
    board[eRow][eCol] = board[sRow][sCol];
    board[sRow][sCol] = ' ';
    if (!canEnPassant) enPassantCol = -1;
    if (canEnPassant) canEnPassant = false;
}

void chessBoard::changePlayer() {
    white = !white;
    black = !black;
}

bool chessBoard::checkRow(int row, int s, int e) {
    for (int i = s + 1; i < e; i++) {
        if (board[row][i] != ' ') return false;
    }
    return true;
}

bool chessBoard::checkCol(int col, int s, int e) {
    for (int i = s + 1; i < e; i++) {
        if (board[i][col] != ' ') return false;
    }
    return true;
}

bool chessBoard::checkDiagonal(int sRow, int sCol, int eRow, int eCol) {
    if (sCol > eCol) {
        int temp = sRow;
        sRow = eRow;
        eRow = temp;
        temp = sCol;
        sCol = eCol;
        eCol = temp;
    }
    if (sRow < eRow) {
        for (int i = 1; i < eRow - sRow; i++) {
            if (board[sRow + i][sCol + i] != ' ') return false;
        }
    } else {
        for (int i = 1; i < sRow - eRow; i++) {
            if (board[sRow - i][sCol + i] != ' ') return false;
        }
    }
    return true;
}
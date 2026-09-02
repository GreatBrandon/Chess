#include "engine.h"
#include <iostream>

const array<pair<int, int>, 8> knightOffsets = { {
    {-2, -1}, {-2, 1},
    {-1, -2}, {-1, 2},
    {1, -2}, {1, 2},
    {2, -1}, {2, 1}
} };

Engine::Engine() {
    this->ambigiousMoves.reserve(8);
};

unordered_map<string, Move> Engine::generateLegalMoves(BoardState state, bool checkingMode) {
    state.legalMoves.clear();
    ambigiousMoves.clear(); // TODO possibly being overwritten when evaluating mate

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            char piece = state.board[row][col];
            this->sRow = row;
            this->sCol = col;
            if (state.isWhite and isWhitePiece(piece)) {
                if (piece == White::KING) {
                    generateLegalMovesKing(state);
                } else if (piece == White::QUEEN) {
                    generateLegalMovesRow(state);
                    generateLegalMovesCol(state);
                    generateLegalMovesDiagonal(state);
                } else if (piece == White::BISHOP) {
                    generateLegalMovesDiagonal(state);
                } else if (piece == White::KNIGHT) {
                    generateLegalMovesKnight(state);
                } else if (piece == White::ROOK) {
                    generateLegalMovesRow(state);
                    generateLegalMovesCol(state);
                } else {
                    generateLegalMovesPawn(state);
                }
            } else if (!state.isWhite and isBlackPiece(piece)) {
                if (piece == Black::KING) {
                    generateLegalMovesKing(state);
                } else if (piece == Black::QUEEN) {
                    generateLegalMovesRow(state);
                    generateLegalMovesCol(state);
                    generateLegalMovesDiagonal(state);
                } else if (piece == Black::BISHOP) {
                    generateLegalMovesDiagonal(state);
                } else if (piece == Black::KNIGHT) {
                    generateLegalMovesKnight(state);
                } else if (piece == Black::ROOK) {
                    generateLegalMovesRow(state);
                    generateLegalMovesCol(state);
                } else {
                    generateLegalMovesPawn(state);
                }
            }
        }
    }

    disambiguateMoves(state);
    if (state.isWhite) {
        cout << "White to move" << endl;
    } else {
        cout << "Black to move" << endl;
    }
    for (auto const& [notation, move] : state.legalMoves) {
        std::cout << notation << ':' << move.sRow << ',' << move.sCol << ',' << move.eRow << ',' << move.eCol << std::endl;
    }

    return state.legalMoves;
}

void Engine::generateLegalMovesRow(BoardState& state) {
    for (int i = sCol + 1; i < 8; i++) {
        if (addMove(state, sRow, i)) break;
    }
    for (int i = sCol - 1; i >= 0; i--) {
        if (addMove(state, sRow, i)) break;
    }
}

void Engine::generateLegalMovesCol(BoardState& state) {
    for (int i = sRow + 1; i < 8; i++) {
        if (addMove(state, i, sCol)) break;
    }
    for (int i = sRow - 1; i >= 0; i--) {
        if (addMove(state, i, sCol)) break;
    }
}

void Engine::generateLegalMovesDiagonal(BoardState& state) {
    for (int col = sCol + 1, row = sRow - 1; col < 8 && row >= 0; col++, row--) {
        if (addMove(state, row, col)) break;
    }
    for (int col = sCol + 1, row = sRow + 1; col < 8 && row < 8; col++, row++) {
        if (addMove(state, row, col)) break;
    }
    for (int col = sCol - 1, row = sRow - 1; col >= 0 && row >= 0; col--, row--) {
        if (addMove(state, row, col)) break;
    }
    for (int col = sCol - 1, row = sRow + 1; col >= 0 && row < 8; col--, row++) {
        if (addMove(state, row, col)) break;
    }
}

void Engine::generateLegalMovesKnight(BoardState& state) {
    generateLegalMovesUsingOffsets(state, knightOffsets);
}

void Engine::generateLegalMovesKing(BoardState& state) {
    generateLegalMovesUsingOffsets(state, { {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1},
    } });

    bool shortCastled = false;
    bool longCastled = false;

    // TODO PREVENT CASTLING WHEN CHECKED

    if (state.isWhite && sRow == 7 && sCol == 4 && !state.isInCheck) {
        if (state.whiteCanLongCastle && state.board[7][1] == ' ' && state.board[7][2] == ' ' && state.board[7][3] == ' ') {
            longCastled = true;
        }
        if (state.whiteCanShortCastle && state.board[7][5] == ' ' && state.board[7][6] == ' ') {
            shortCastled = true;
        }
    } else if (!state.isWhite && sRow == 0 && sCol == 4 && !state.isInCheck) {
        if (state.blackCanLongCastle && state.board[0][1] == ' ' && state.board[0][2] == ' ' && state.board[0][3] == ' ') {
            longCastled = true;
        }
        if (state.blackCanShortCastle && state.board[0][5] == ' ' && state.board[0][6] == ' ') {
            shortCastled = true;
        }
    }

    string move = string();
    array<array<char, 8>, 8> tempBoard = state.board;
    bool isCheck = false;

    if (shortCastled) {
        tempBoard = state.board;
        tempBoard[sRow][6] = tempBoard[sRow][sCol];
        tempBoard[sRow][sCol] = ' ';
        tempBoard[sRow][5] = tempBoard[sRow][0];
        tempBoard[sRow][7] = ' ';
        move = "O-O";

        if (!isKingInCheck(tempBoard, state.isWhite, true)) {
            if (isKingInCheck(tempBoard, state.isWhite, false)) {
                move += '+';
                // TODO MATE
                // TODO PREVENT CASTLE THROUGH CHECK
            }

            state.legalMoves[move] = Move(sRow, sCol, sRow, 6);
        }

    }
    if (longCastled) {
        tempBoard = state.board;
        tempBoard[sRow][2] = tempBoard[sRow][sCol];
        tempBoard[sRow][sCol] = ' ';
        tempBoard[sRow][3] = tempBoard[sRow][0];
        tempBoard[sRow][0] = ' ';
        move = "O-O-O";

        if (!isKingInCheck(tempBoard, state.isWhite, true)) {
            if (isKingInCheck(tempBoard, state.isWhite, false)) {
                move += '+';
                // TODO MATE
                // TODO PREVENT CASTLE THROUGH CHECK
            }

            state.legalMoves[move] = Move(sRow, sCol, sRow, 2);
        }
    }
}

void Engine::generateLegalMovesPawn(BoardState& state) {
    if (state.isWhite) {
        if (state.board[sRow - 1][sCol] == ' ') {
            addMove(state, sRow - 1, sCol);
            if (sRow == 6 && state.board[sRow - 2][sCol] == ' ') {
                addMove(state, sRow - 2, sCol);
            }
        }
        if (sCol - 1 >= 0 && isBlackPiece(state.board[sRow - 1][sCol - 1])) addMove(state, sRow - 1, sCol - 1);
        if (sCol + 1 < 8 && isBlackPiece(state.board[sRow - 1][sCol + 1])) addMove(state, sRow - 1, sCol + 1);
        if (abs(state.enPassantCol - sCol) == 1 && state.enPassantRow == sRow - 1) addMove(state, state.enPassantRow, state.enPassantCol);
    } else {
        if (state.board[sRow + 1][sCol] == ' ') {
            addMove(state, sRow + 1, sCol);
            if (sRow == 1 && state.board[sRow + 2][sCol] == ' ') {
                addMove(state, sRow + 2, sCol);
            }
        }
        if (sCol - 1 >= 0 && isWhitePiece(state.board[sRow + 1][sCol - 1])) addMove(state, sRow + 1, sCol - 1);
        if (sCol + 1 < 8 && isWhitePiece(state.board[sRow + 1][sCol + 1])) addMove(state, sRow + 1, sCol + 1);
        if (abs(state.enPassantCol - sCol) == 1 && state.enPassantRow == sRow + 1) addMove(state, state.enPassantRow, state.enPassantCol);
    }
}

void Engine::generateLegalMovesUsingOffsets(BoardState& state, array<pair<int, int>, 8> offsets) {
    for (auto [oRow, oCol] : offsets) {
        int eRow = sRow + oRow;
        int eCol = sCol + oCol;

        if (eRow >= 0 && eRow < 8 && eCol >= 0 && eCol < 8) addMove(state, eRow, eCol);
    }
}

bool Engine::addMove(BoardState& state, int eRow, int eCol) {
    char end = state.board[eRow][eCol];
    if ((state.isWhite && isWhitePiece(end)) || (!state.isWhite && isBlackPiece(end))) return true;
    bool isPawn = state.board[sRow][sCol] == White::PAWN || state.board[sRow][sCol] == Black::PAWN;
    string move = string();

    if (!isPawn) {
        move += toupper(state.board[sRow][sCol]);
    }

    if (isPawn && state.board[eRow][eCol] == ' ' && abs(eCol - sCol) == 1) {
        move = move + (char)('a' + sCol) + 'x';
    } else if (state.board[eRow][eCol] != ' ') {
        if (isPawn) {
            move = move + (char)('a' + sCol) + 'x';
        } else {
            move += 'x';
        }
    }

    move = move + (char)('a' + eCol) + (char)('8' - eRow);

    array<array<char, 8>, 8> tempBoard = state.board;

    if (isPawn && ((state.isWhite && eRow == 0) || (!state.isWhite && eRow == 7))) {
        array<string, 4> promotionMoves = { {
            move + "=Q", move + "=B", move + "=N", move + "=R"
            } };
        for (auto& m : promotionMoves) {
            if (state.isWhite) tempBoard[eRow][eCol] = m.back();
            else tempBoard[eRow][eCol] = m.back() + 0x20;
            tempBoard[sRow][sCol] = ' ';

            if (!isKingInCheck(tempBoard, state.isWhite, true)) {
                if (isKingInCheck(tempBoard, state.isWhite, false)) {
                    m += '+';
                    // TODO PROMOTION MATE
                }
                state.legalMoves[m] = Move(sRow, sCol, eRow, eCol);
            }
        }
        return false;
    }

    tempBoard[eRow][eCol] = tempBoard[sRow][sCol];
    tempBoard[sRow][sCol] = ' ';

    if (!isKingInCheck(tempBoard, state.isWhite, true)) {
        if (isKingInCheck(tempBoard, state.isWhite, false)) {
            move += '+';
            // TODO MATE
        }

        if (state.legalMoves.contains(move)) {
            cout << "Disambiguate move: " << move << endl;
            if (!ambigiousMoves.contains(move)) {
                cout << "Disambiguate original move: " << move << endl;
                ambigiousMoves[move].push_back(state.legalMoves[move]);
            }
            ambigiousMoves[move].push_back(Move(sRow, sCol, eRow, eCol));
        }
        state.legalMoves[move] = Move(sRow, sCol, eRow, eCol);
    }

    if (end != ' ') return true;
    return false;
}

bool Engine::isKingInCheck(array<array<char, 8>, 8> &board, bool isWhite, bool checkSelf) const{
    bool checkWhite;
    if (isWhite) {
        if (checkSelf) checkWhite = true;
        else checkWhite = false;
    } else {
        if (checkSelf) checkWhite = false;
        else checkWhite = true;
    }

    int kRow = -1;
    int kCol = -1;
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if ((checkWhite && board[row][col] == White::KING) || (!checkWhite && board[row][col] == Black::KING)) {
                kRow = row;
                kCol = col;
                break;
            }
        }
        if (kRow != -1) break;
    }

    for (int row = kRow - 1; row >= 0; row--) {
        if (board[row][kCol] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][kCol], true)) return true;
        break;
    }
    for (int row = kRow + 1; row < 8; row++) {
        if (board[row][kCol] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][kCol], true)) return true;
        break;
    }
    for (int col = kCol - 1; col >= 0; col--) {
        if (board[kRow][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[kRow][col], true)) return true;
        break;
    }
    for (int col = kCol + 1; col < 8; col++) {
        if (board[kRow][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[kRow][col], true)) return true;
        break;
    }
    for (int col = kCol + 1, row = kRow - 1; col < 8 && row >= 0; col++, row--) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], true)) return true;
        break;
    }
    for (int col = kCol + 1, row = kRow + 1; col < 8 && row < 8; col++, row++) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], true)) return true;
        break;
    }
    for (int col = kCol - 1, row = kRow - 1; col >= 0 && row >= 0; col--, row--) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], true)) return true;
        break;
    }
    for (int col = kCol - 1, row = kRow + 1; col >= 0 && row < 8; col--, row++) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], true)) return true;
        break;
    }
    for (auto const& offset : knightOffsets) {
        int row = kRow + offset.first;
        int col = kCol + offset.second;
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
            if ((checkWhite && board[row][col] == Black::KNIGHT) || (!checkWhite && board[row][col] == White::KNIGHT)) return true;
        }
    }
    if (checkWhite && kRow - 1 >= 0) {
        if ((kCol - 1 >= 0 && board[kRow - 1][kCol - 1] == Black::PAWN) || (kCol + 1 < 8 && board[kRow - 1][kCol + 1] == Black::PAWN)) return true;
    } else if (!checkWhite && kRow + 1 < 8) {
        if ((kCol - 1 >= 0 && board[kRow + 1][kCol - 1] == White::PAWN) || (kCol + 1 < 8 && board[kRow + 1][kCol + 1] == White::PAWN)) return true;
    }
    return false;
}

bool Engine::actuallyCheckIsKingInCheck(bool checkWhite, char p, bool rookMode) const{
    char p1 = checkWhite ? rookMode ? Black::ROOK : Black::BISHOP : rookMode ? White::ROOK : White::BISHOP;
    if (p == p1 || (checkWhite && p == Black::QUEEN) || (!checkWhite && p == White::QUEEN)) return true;
    return false;
}

void Engine::disambiguateMoves(BoardState& state) {
    for (auto const&[notation, moves] : ambigiousMoves) {
        state.legalMoves.erase(notation);

        for (const Move& move : moves) {
            bool fileUnique = true;
            bool rankUnique = true;

            for (const Move& other : moves) {
                if (&move == &other) continue;
                if (move.sCol == other.sCol) fileUnique = false;
                if (move.sRow == other.sRow) rankUnique = false;
            }

            string newNotation = notation;

            if (fileUnique) {
                // Use file: Rae1
                newNotation.insert(1, 1, (char)('a' + move.sCol));
            } else if (rankUnique) {
                // Use rank: R3e1
                newNotation.insert(1, 1, (char)('8' - move.sRow));
            } else {
                // Need both: Ra3e1
                newNotation.insert(1, 1, (char)('a' + move.sCol));
                newNotation.insert(2, 1, (char)('8' - move.sRow));
            }

            state.legalMoves[newNotation] = move;
        }
    }
    ambigiousMoves.clear();
}
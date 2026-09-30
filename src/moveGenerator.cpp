#include "moveGenerator.h"


constexpr array<pair<int, int>, 8> knightOffsets = { {
    {-2, -1}, {-2, 1},
    {-1, -2}, {-1, 2},
    {1, -2}, {1, 2},
    {2, -1}, {2, 1}
} };

void MoveGenerator::generateLegalMoves(BoardState& state) {
    state.legalMoves.clear();

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const char& piece = state.board[row][col];
            if (piece == ' ') continue;
            sRow = row;
            sCol = col;
            if (state.isWhite && isWhitePiece(piece)) {
                if (piece == White::KING) {
                    generateLegalMovesKing(state);
                }
                else if (piece == White::QUEEN) {
                    generateLegalMovesRow(state);
                    generateLegalMovesCol(state);
                    generateLegalMovesDiagonal(state);
                }
                else if (piece == White::BISHOP) {
                    generateLegalMovesDiagonal(state);
                }
                else if (piece == White::KNIGHT) {
                    generateLegalMovesKnight(state);
                }
                else if (piece == White::ROOK) {
                    generateLegalMovesRow(state);
                    generateLegalMovesCol(state);
                }
                else {
                    generateLegalMovesPawn(state);
                }
            }
            else if (!state.isWhite && isBlackPiece(piece)) {
                if (piece == Black::KING) {
                    generateLegalMovesKing(state);
                }
                else if (piece == Black::QUEEN) {
                    generateLegalMovesRow(state);
                    generateLegalMovesCol(state);
                    generateLegalMovesDiagonal(state);
                }
                else if (piece == Black::BISHOP) {
                    generateLegalMovesDiagonal(state);
                }
                else if (piece == Black::KNIGHT) {
                    generateLegalMovesKnight(state);
                }
                else if (piece == Black::ROOK) {
                    generateLegalMovesRow(state);
                    generateLegalMovesCol(state);
                }
                else {
                    generateLegalMovesPawn(state);
                }
            }
            if (state.checkForMate && !state.legalMoves.empty()) return; // legal move found, return early
        }
    }
}

void MoveGenerator::generateLegalMovesRow(BoardState& state) {
    for (int i = sCol + 1; i < 8; i++) {
        if (addMove(state, sRow, i)) break;
    }
    for (int i = sCol - 1; i >= 0; i--) {
        if (addMove(state, sRow, i)) break;
    }
}

void MoveGenerator::generateLegalMovesCol(BoardState& state) {
    for (int i = sRow + 1; i < 8; i++) {
        if (addMove(state, i, sCol)) break;
    }
    for (int i = sRow - 1; i >= 0; i--) {
        if (addMove(state, i, sCol)) break;
    }
}

void MoveGenerator::generateLegalMovesDiagonal(BoardState& state) {
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

void MoveGenerator::generateLegalMovesKnight(BoardState& state) {
    generateLegalMovesUsingOffsets(state, knightOffsets);
}

void MoveGenerator::generateLegalMovesKing(BoardState& state) {
    generateLegalMovesUsingOffsets(state, { {
        {-1, -1}, {-1, 0}, {-1, 1},
        {0, -1}, {0, 1},
        {1, -1}, {1, 0}, {1, 1},
    } });

    bool shortCastled = false;
    bool longCastled = false;

    if (state.isWhite && sRow == 7 && sCol == 4 && !state.checkForMate) {
        if (state.whiteCanLongCastle && state.board[7][1] == ' ' && state.board[7][2] == ' ' && state.board[7][3] == ' ') {
            longCastled = true;
        }
        if (state.whiteCanShortCastle && state.board[7][5] == ' ' && state.board[7][6] == ' ') {
            shortCastled = true;
        }
    }
    else if (!state.isWhite && sRow == 0 && sCol == 4 && !state.checkForMate) {
        if (state.blackCanLongCastle && state.board[0][1] == ' ' && state.board[0][2] == ' ' && state.board[0][3] == ' ') {
            longCastled = true;
        }
        if (state.blackCanShortCastle && state.board[0][5] == ' ' && state.board[0][6] == ' ') {
            shortCastled = true;
        }
    }

    if (shortCastled || longCastled) {
        if (isKingInCheck(state, true)) return;
    }

    const auto boardCopy(state.board);
    const auto kingPosCopy(state.isWhite ? state.whiteKingPos : state.blackKingPos);
    auto& board = state.board;
    const int initialEval = state.evaluation;

    if (shortCastled) {
        moveAndEvaluate(state, Move(sRow, sCol, sRow, 5)); // move king 1 square
        if (!isKingInCheck(state, true)) {
            moveAndEvaluate(state, Move(sRow, 5, sRow, 6)); // move king another square
            if (!isKingInCheck(state, true)) {
                moveAndEvaluate(state, Move(sRow, 7, sRow, 5)); // move rook
                if (isKingInCheck(state, false)) {
                    if (checkMate(state)) {
                        state.legalMoves.emplace_back(sRow, sCol, sRow, 6, state.evaluation, false, true);
                    } else {
                        state.legalMoves.emplace_back(sRow, sCol, sRow, 6, state.evaluation, true);
                    }
                } else {
                    state.legalMoves.emplace_back(sRow, sCol, sRow, 6, state.evaluation);
                }
            }
        }
    }
    if (longCastled) {
        // Undo short castle move, if applicable
        board = boardCopy;
        state.evaluation = initialEval;
        moveAndEvaluate(state, Move(sRow, sCol, sRow, 3)); // move king 1 square
        if (!isKingInCheck(state, true)) {
            moveAndEvaluate(state, Move(sRow, 3, sRow, 2)); // move king another square
            if (!isKingInCheck(state, true)) {
                moveAndEvaluate(state, Move(sRow, 0, sRow, 3)); // move rook
                if (isKingInCheck(state, false)) {
                    if (checkMate(state)) {
                        state.legalMoves.emplace_back(sRow, sCol, sRow, 2, state.evaluation, false, true);
                    } else {
                        state.legalMoves.emplace_back(sRow, sCol, sRow, 2, state.evaluation, true);
                    }
                } else {
                    state.legalMoves.emplace_back(sRow, sCol, sRow, 2, state.evaluation);
                }
            }
        }
    }
    state.board = boardCopy;
    if (state.isWhite) state.whiteKingPos = kingPosCopy;
    else state.blackKingPos = kingPosCopy;
    state.evaluation = initialEval;
}

void MoveGenerator::generateLegalMovesPawn(BoardState& state) {
    const int epR = state.enPassantPos.first;
    const int epC = state.enPassantPos.second;
    if (state.isWhite) {
        if (state.board[sRow - 1][sCol] == ' ') {
            addMove(state, sRow - 1, sCol);
            if (sRow == 6 && state.board[sRow - 2][sCol] == ' ') {
                addMove(state, sRow - 2, sCol);
            }
        }
        if (sCol - 1 >= 0 && isBlackPiece(state.board[sRow - 1][sCol - 1])) addMove(state, sRow - 1, sCol - 1);
        if (sCol + 1 < 8 && isBlackPiece(state.board[sRow - 1][sCol + 1])) addMove(state, sRow - 1, sCol + 1);
        if (abs(epC - sCol) == 1 && sRow == 3 && epR == 2) addMove(state, epR, epC);
    }
    else {
        if (state.board[sRow + 1][sCol] == ' ') {
            addMove(state, sRow + 1, sCol);
            if (sRow == 1 && state.board[sRow + 2][sCol] == ' ') {
                addMove(state, sRow + 2, sCol);
            }
        }
        if (sCol - 1 >= 0 && isWhitePiece(state.board[sRow + 1][sCol - 1])) addMove(state, sRow + 1, sCol - 1);
        if (sCol + 1 < 8 && isWhitePiece(state.board[sRow + 1][sCol + 1])) addMove(state, sRow + 1, sCol + 1);
        if (abs(epC - sCol) == 1 && sRow == 4 && epR == 5) addMove(state, epR, epC);
    }
}

void MoveGenerator::generateLegalMovesUsingOffsets(BoardState& state, const array<pair<int, int>, 8>& offsets) {
    for (auto const& [oRow, oCol] : offsets) {
        const int eRow = sRow + oRow;
        const int eCol = sCol + oCol;

        if (eRow >= 0 && eRow < 8 && eCol >= 0 && eCol < 8) addMove(state, eRow, eCol);
    }
}

bool MoveGenerator::addMove(BoardState& state, const int& eRow, const int& eCol) {
    auto& board = state.board;
    const char end = board[eRow][eCol];
    if ((state.isWhite && isWhitePiece(end)) || (!state.isWhite && isBlackPiece(end))) return true;
    const bool isPawn = board[sRow][sCol] == White::PAWN || board[sRow][sCol] == Black::PAWN;
    const int initialEval(state.evaluation);

    if (isPawn && ((state.isWhite && eRow == 0) || (!state.isWhite && eRow == 7))) {
        // Pawn promotion
        const char promotionMoves[4] = { 'Q', 'B', 'N', 'R' };
        moveAndEvaluate(state, Move(sRow, sCol, eRow, eCol));
        for (const char& piece : promotionMoves) {
            promotePawnAndEvaluate(state, eRow, eCol, (state.isWhite) ? piece : piece + 0x20);
            if (!isKingInCheck(state, true)) {
                if (isKingInCheck(state, false)) {
                    if (checkMate(state)) {
                        state.legalMoves.emplace_back(sRow, sCol, eRow, eCol, state.evaluation, false, true, piece);
                    } else { 
                        state.legalMoves.emplace_back(sRow, sCol, eRow, eCol, state.evaluation, true, false, piece);
                    }
                } else {
                    state.legalMoves.emplace_back(sRow, sCol, eRow, eCol, state.evaluation, false, false, piece);
                }
            }
        }
        board[sRow][sCol] = state.isWhite ? White::PAWN : Black::PAWN;
        board[eRow][eCol] = end;
        state.evaluation = initialEval;
        return false;
    }
    moveAndEvaluate(state, Move(sRow, sCol, eRow, eCol, state.evaluation));

    if (isPawn && abs(eCol - sCol) == 1 && end == ' ') {
        moveAndEvaluate(state, Move(sRow, sCol, state.isWhite ? eRow + 1 : eRow - 1, eCol, state.evaluation));
    }

    if (!isKingInCheck(state, true)) {
        if (isKingInCheck(state, false)) {
            if (checkMate(state)) {
                state.legalMoves.emplace_back(sRow, sCol, eRow, eCol, state.evaluation, false, true);
            } else {
                state.legalMoves.emplace_back(sRow, sCol, eRow, eCol, state.evaluation, true);
            }
        } else {
            state.legalMoves.emplace_back(sRow, sCol, eRow, eCol, state.evaluation);
        }
    }

    // Undo move on state
    if (board[eRow][eCol] == White::KING || board[eRow][eCol] == Black::KING) {
        if (state.isWhite) state.whiteKingPos = { sRow,sCol };
        else state.blackKingPos = { sRow, sCol };
    }
    if (isPawn && abs(eCol - sCol) == 1 && end == ' ') {
        if (state.isWhite) board[eRow + 1][eCol] = Black::PAWN;
        else board[eRow - 1][eCol] = White::PAWN;
    }

    state.board[sRow][sCol] = state.board[eRow][eCol];
    state.board[eRow][eCol] = end;
    state.evaluation = initialEval;

    if (end != ' ') return true;
    return false;
}

bool MoveGenerator::checkMate(BoardState& state) {
    if (!state.checkForMate) {
        auto tempState(state);
        tempState.checkForMate = true;
        tempState.isWhite = !state.isWhite;
        const int tempSRow = sRow;
        const int tempSCol = sCol;
        generateLegalMoves(tempState);
        sRow = tempSRow;
        sCol = tempSCol;
        if (tempState.legalMoves.size() == 0) {
            return true;
        }
    }
    return false;
}

bool MoveGenerator::isKingInCheck(const BoardState& state, const bool checkSelf) const {
    const auto& board = state.board;
    const bool checkWhite = state.isWhite ? checkSelf ? true : false : checkSelf ? false : true;
    const int kRow = checkWhite ? state.whiteKingPos.first : state.blackKingPos.first;
    const int kCol = checkWhite ? state.whiteKingPos.second : state.blackKingPos.second;

    // ↑
    for (int row = kRow - 1; row >= 0; row--) {
        if (board[row][kCol] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][kCol], true)) return true;
        break;
    }
    // ↓
    for (int row = kRow + 1; row < 8; row++) {
        if (board[row][kCol] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][kCol], true)) return true;
        break;
    }
    // ←
    for (int col = kCol - 1; col >= 0; col--) {
        if (board[kRow][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[kRow][col], true)) return true;
        break;
    }
    // →
    for (int col = kCol + 1; col < 8; col++) {
        if (board[kRow][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[kRow][col], true)) return true;
        break;
    }
    // ↗
    for (int col = kCol + 1, row = kRow - 1; col < 8 && row >= 0; col++, row--) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], false)) return true;
        break;
    }
    // ↘
    for (int col = kCol + 1, row = kRow + 1; col < 8 && row < 8; col++, row++) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], false)) return true;
        break;
    }
    // ↖
    for (int col = kCol - 1, row = kRow - 1; col >= 0 && row >= 0; col--, row--) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], false)) return true;
        break;
    }
    // ↙
    for (int col = kCol - 1, row = kRow + 1; col >= 0 && row < 8; col--, row++) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], false)) return true;
        break;
    }
    for (auto const& [oRow, oCol] : knightOffsets) {
        const int row = kRow + oRow;
        const int col = kCol + oCol;
        if (row >= 0 && row < 8 && col >= 0 && col < 8) {
            if ((checkWhite && board[row][col] == Black::KNIGHT) || (!checkWhite && board[row][col] == White::KNIGHT)) return true;
        }
    }
    if (checkWhite && kRow - 1 >= 0) {
        if ((kCol - 1 >= 0 && board[kRow - 1][kCol - 1] == Black::PAWN) || (kCol + 1 < 8 && board[kRow - 1][kCol + 1] == Black::PAWN)) return true;
    }
    else if (!checkWhite && kRow + 1 < 8) {
        if ((kCol - 1 >= 0 && board[kRow + 1][kCol - 1] == White::PAWN) || (kCol + 1 < 8 && board[kRow + 1][kCol + 1] == White::PAWN)) return true;
    }
    return false;
}

bool MoveGenerator::actuallyCheckIsKingInCheck(const bool checkWhite, const char p, const bool rookMode) const {
    const char p1 = checkWhite ? rookMode ? Black::ROOK : Black::BISHOP : rookMode ? White::ROOK : White::BISHOP;
    if (p == p1 || (checkWhite && p == Black::QUEEN) || (!checkWhite && p == White::QUEEN)) return true;
    return false;
}
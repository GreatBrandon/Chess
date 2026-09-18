#include "engine.h"
#include "evaluate.h"
#include <iostream>
#include <algorithm>

constexpr array<pair<int, int>, 8> knightOffsets = { {
    {-2, -1}, {-2, 1},
    {-1, -2}, {-1, 2},
    {1, -2}, {1, 2},
    {2, -1}, {2, 1}
} };

constexpr int INF = 1000000000;
constexpr int CHECKMATE_EVAL = 100000;
constexpr int CHECKMATE_OFFSET = 100;
int CALLCOUNT = 0;

Engine::Engine() {};

void Engine::generateLegalMoves(BoardState& state) {
    state.legalMoves.clear();
    state.ambigiousMoves.clear();

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const char& piece = state.board[row][col];
            if (piece == ' ') continue;
            this->sRow = row;
            this->sCol = col;
            if (state.isWhite && isWhitePiece(piece)) {
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
            } else if (!state.isWhite && isBlackPiece(piece)) {
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
            if (state.checkForMate && !state.legalMoves.empty()) return; // legal move found, return early
        }
    }
    disambiguateMoves(state);
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

    if (state.isWhite && sRow == 7 && sCol == 4 && !state.checkForMate) {
        if (state.whiteCanLongCastle && state.board[7][1] == ' ' && state.board[7][2] == ' ' && state.board[7][3] == ' ') {
            longCastled = true;
        }
        if (state.whiteCanShortCastle && state.board[7][5] == ' ' && state.board[7][6] == ' ') {
            shortCastled = true;
        }
    } else if (!state.isWhite && sRow == 0 && sCol == 4 && !state.checkForMate) {
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
                array<char, 8> moveBuf = { 'O', '-', 'O' };
                int len = 3;
                if (isKingInCheck(state, false)) {
                    if (checkMate(state)) moveBuf[len++] = '#';
                    else moveBuf[len++] = '+';
                }
                state.legalMoves.emplace_back(moveBuf, Move(sRow, sCol, sRow, 6, state.evaluation));
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
                array<char, 8> moveBuf = { 'O', '-', 'O', '-', 'O' };
                int len = 5;
                if (isKingInCheck(state, false)) {
                    if (checkMate(state)) moveBuf[len++] = '#';
                    else moveBuf[len++] = '+';
                }
                state.legalMoves.emplace_back(moveBuf, Move(sRow, sCol, sRow, 2, evaluatePosition(state)));
            }
        }
    }
    state.board = boardCopy;
    if (state.isWhite) state.whiteKingPos = kingPosCopy;
    else state.blackKingPos = kingPosCopy;
    state.evaluation = initialEval;
}

void Engine::generateLegalMovesPawn(BoardState& state) {
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
    } else {
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

void Engine::generateLegalMovesUsingOffsets(BoardState& state, const array<pair<int, int>, 8>& offsets) {
    for (auto const&[oRow, oCol] : offsets) {
        const int eRow = sRow + oRow;
        const int eCol = sCol + oCol;

        if (eRow >= 0 && eRow < 8 && eCol >= 0 && eCol < 8) addMove(state, eRow, eCol);
    }
}

bool Engine::addMove(BoardState& state, const int& eRow, const int& eCol) {
    auto& board = state.board;
    const char end = board[eRow][eCol];
    if ((state.isWhite && isWhitePiece(end)) || (!state.isWhite && isBlackPiece(end))) return true;
    const bool isPawn = board[sRow][sCol] == White::PAWN || board[sRow][sCol] == Black::PAWN;
    const int initialEval(state.evaluation);
    array<char, 8> moveBuf = {};
    int len = 0;

    if (!isPawn) {
        if (state.isWhite) moveBuf[len++] = board[sRow][sCol];
        else moveBuf[len++] = board[sRow][sCol] - 0x20;
    }

    if (isPawn && end == ' ' && abs(eCol - sCol) == 1) {
        moveBuf[len++] = (char)('a' + sCol);
        moveBuf[len++] = 'x';
    } else if (end != ' ') {
        if (isPawn) {
            moveBuf[len++] = (char)('a' + sCol);
        } 
        moveBuf[len++] = 'x';
    }

    moveBuf[len++] = (char)('a' + eCol);
    moveBuf[len++] = (char)('8' - eRow);

    if (isPawn && ((state.isWhite && eRow == 0) || (!state.isWhite && eRow == 7))) {
        const char promotionMoves[4] = { 'Q', 'B', 'N', 'R' };
        moveBuf[len++] = '=';
        array<char, 8> moveCopy(moveBuf);
        int baseLen = len;
        moveAndEvaluate(state, Move(sRow, sCol, eRow, eCol));
        for (const char &m : promotionMoves) {
            moveBuf = moveCopy;
            len = baseLen;
            moveBuf[len++] = m;
            promotePawnAndEvaluate(state, eRow, eCol, (state.isWhite) ? m : m + 0x20);
            if (!isKingInCheck(state, true)) {
                if (isKingInCheck(state, false)) {
                    if (checkMate(state)) moveBuf[len++] = '#';
                    else moveBuf[len++] = '+';
                }
                state.legalMoves.emplace_back(moveBuf, Move(sRow, sCol, eRow, eCol, evaluatePosition(state)));
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
            if (checkMate(state)) moveBuf[len++] = '#';
            else moveBuf[len++] = '+';
        }

        uint64_t moveKey;
        uint64_t existingKey;
        memcpy(&moveKey, moveBuf.data(), 8);
        const Move* duplicateMove = nullptr;
        for (auto const& [n, m] : state.legalMoves) {
            memcpy(&existingKey, n.data(), 8);
            if (moveKey == existingKey) {
                duplicateMove = &m;
                break;
            }
        }
        if (duplicateMove != nullptr) {
            if (!state.ambigiousMoves.contains(moveBuf)) {
                state.ambigiousMoves[moveBuf].emplace_back(*duplicateMove);
            }
            state.ambigiousMoves[moveBuf].emplace_back(sRow, sCol, eRow, eCol, state.evaluation);
        } else {
            state.legalMoves.emplace_back(moveBuf, Move(sRow, sCol, eRow, eCol, state.evaluation));
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

bool Engine::checkMate(BoardState& state) {
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
            //move += '#';
            return true;
        }
        //else move += '+';
    } else {
        //move += '+'; // we don't really care about this I think, function returns early when any legal move is found even a counter check
    }
    return false;
}

bool Engine::isKingInCheck(BoardState& state, bool checkSelf) const {
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
    } else if (!checkWhite && kRow + 1 < 8) {
        if ((kCol - 1 >= 0 && board[kRow + 1][kCol - 1] == White::PAWN) || (kCol + 1 < 8 && board[kRow + 1][kCol + 1] == White::PAWN)) return true;
    }
    return false;
}

bool Engine::actuallyCheckIsKingInCheck(const bool checkWhite, const char p, const bool rookMode) const {
    const char p1 = checkWhite ? rookMode ? Black::ROOK : Black::BISHOP : rookMode ? White::ROOK : White::BISHOP;
    if (p == p1 || (checkWhite && p == Black::QUEEN) || (!checkWhite && p == White::QUEEN)) return true;
    return false;
}

void Engine::disambiguateMoves(BoardState& state) {
    for (auto const&[notation, moves] : state.ambigiousMoves) {
        for (int i = 0; i < state.legalMoves.size(); i++) {
            if (state.legalMoves[i].first == notation) {
                state.legalMoves.erase(state.legalMoves.begin() + i);
                break;
            }
        }

        for (const Move& move : moves) {
            bool fileUnique = true;
            bool rankUnique = true;

            for (const Move& other : moves) {
                if (&move == &other) continue;
                if (move.sCol == other.sCol) fileUnique = false;
                if (move.sRow == other.sRow) rankUnique = false;
            }

            array<char, 8> moveBuf = {};
            int len = 0;

            moveBuf[len++] = notation[0]; // piece letter (e.g. 'R')

            if (fileUnique) {
                moveBuf[len++] = (char)('a' + move.sCol);
            } else if (rankUnique) {
                moveBuf[len++] = (char)('8' - move.sRow);
            } else {
                moveBuf[len++] = (char)('a' + move.sCol);
                moveBuf[len++] = (char)('8' - move.sRow);
            }

            for (int j = 1; j < notation.size() && len < 8; j++) {
                moveBuf[len++] = notation[j];
            }

            state.legalMoves.emplace_back(moveBuf, move);
        }
    }
    state.ambigiousMoves.clear();
}

void Engine::playMove(BoardState& boardState, const array<char, 8>& notation, const Move& move) {
    auto& board = boardState.board;
    const int sRow = move.sRow;
    const int sCol = move.sCol;
    const int eRow = move.eRow;
    const int eCol = move.eCol;
    int evaluation = move.evaluation;

    int eqPos = -1;
    int lastPos = -1;
    for (int i = 0; i < 8; i++) {
        if (notation[i] == '\0') {
            lastPos = i;
            break;
        }
        if (notation[i] == '=') eqPos = i;
    }
    // 50 move rule
    if (board[eRow][eCol] != ' ' || board[sRow][sCol] == White::PAWN || board[sRow][sCol] == Black::PAWN) {
        boardState.stalemateMoveCounter = 0;
        //lastIrreversibleMove = previousMoves.size();
    } else boardState.stalemateMoveCounter++;

    // Remove castling rights
    if (board[sRow][sCol] == White::KING) {
        boardState.whiteCanLongCastle = false;
        boardState.whiteCanShortCastle = false;
        boardState.whiteKingPos = { eRow, eCol };
    }
    if (board[sRow][sCol] == Black::KING) {
        boardState.blackCanLongCastle = false;
        boardState.blackCanShortCastle = false;
        boardState.blackKingPos = { eRow, eCol };
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
        boardState.enPassantPos = { sRow - 1, eCol };
    } else if (board[sRow][sCol] == Black::PAWN && eRow - sRow == 2) {
        boardState.enPassantPos = { eRow - 1, eCol };
    } else {
        if ((board[sRow][sCol] == White::PAWN || board[sRow][sCol] == Black::PAWN) && eRow == boardState.enPassantPos.first && eCol == boardState.enPassantPos.second) board[sRow][eCol] = ' ';
        boardState.enPassantPos = { -1, -1 };
    }

    board[eRow][eCol] = board[sRow][sCol];
    board[sRow][sCol] = ' ';

    // Pawn promotion
    if (board[eRow][eCol] == White::PAWN && eRow == 0) {
        board[eRow][eCol] = notation[eqPos + 1];
    } else if (board[eRow][eCol] == Black::PAWN && eRow == 7) {
        board[eRow][eCol] = notation[eqPos + 1] + 0x20;
    }

    // Castle
    if (notation[0] == 'O') {
        if (notation[4] == 'O') {
            board[sRow][3] = board[sRow][0];
            board[sRow][0] = ' ';
        } else {
            board[sRow][5] = board[sRow][7];
            board[sRow][7] = ' ';
        }
    }

    // Checkmate
    if (notation[lastPos - 1] == '#') {
        if (boardState.isWhite) evaluation = CHECKMATE_EVAL;
        else evaluation = -CHECKMATE_EVAL;
    } else {
        // Check stalemate
        if (boardState.stalemateMoveCounter == 100) {
            evaluation = 0;
        }
    }

    
    //map<array<array<char, 8>, 8>, int> previousPositions;
    //for (int i = lastIrreversibleMove; i < previousMoves.size(); i++) {
    //    if (++previousPositions[previousMoves[i].board] == 3) {
    //        isDraw = true;
    //        // TODO add castling and en passant checks to this to fully satisfy FIDE rules
    //        // Use zobrist hash for this
    //    }
    //}

    boardState.evaluation = evaluation;
}

pair<array<char, 8>, Move> Engine::getBestMove(BoardState& root, int depth) {
    Move bestMove(-1,-1,-1,-1,-1);
    int alpha = -INF;
    array<char, 8> bestMoveNotation;

    // sort the initial list of moves
    sort(root.legalMoves.begin(), root.legalMoves.end(),
        [sign = root.isWhite ? 1 : -1](auto const& a, auto const& b) {
            return a.second.evaluation * sign > b.second.evaluation * sign;
        });
    CALLCOUNT = 0;

    for (const auto& [notation, move] : root.legalMoves) {
        auto newState(root);

        playMove(newState, notation, move);
        newState.isWhite = !root.isWhite;
        generateLegalMoves(newState);

        int score = -alphaBeta(newState, -INF, -alpha, depth - 1);
        
        //cout << notation << ':' << CALLCOUNT << endl;
        //CALLCOUNT = 0;
        if (score > alpha) {
            alpha = score;
            bestMove = move;
            bestMoveNotation = notation;
        }
    }
    cout << "Best move: " << bestMoveNotation.data() << ", nodes checked: " << CALLCOUNT << endl;
    return { bestMoveNotation, bestMove };
}

// Using negamax
int Engine::alphaBeta(BoardState& prevState, int alpha, int beta, int depthleft) {
    CALLCOUNT++;
    if (depthleft == 0) return quiesce(prevState, alpha, beta);

    if (depthleft > 1) {
        sort(prevState.legalMoves.begin(), prevState.legalMoves.end(),
            [sign = prevState.isWhite ? 1 : -1](auto const& a, auto const& b) {
                return a.second.evaluation * sign > b.second.evaluation * sign;
            });
    }
    int bestValue = -INF;
    for (const auto& [notation, move] : prevState.legalMoves) {
        auto newState(prevState);
        playMove(newState, notation, move);
        newState.isWhite = !prevState.isWhite;
        if (depthleft - 1 > 0) generateLegalMoves(newState);
        const int score = -alphaBeta(newState, -beta, -alpha, depthleft - 1);
        if (score > bestValue) {
            bestValue = score;
            if (score > alpha) alpha = score; // alpha acts like max in MiniMax
        }
        if (score >= beta) return bestValue;   //  fail soft beta-cutoff, existing the loop here is also fine
    }
    return bestValue;
}

int Engine::quiesce(BoardState& prevState, int alpha, int beta) {
    return prevState.isWhite ? prevState.evaluation : -prevState.evaluation;
}
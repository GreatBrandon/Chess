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
            const char piece = state.board[row][col];
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

    if (shortCastled || longCastled) {
        if (isKingInCheck(state.board, state.isWhite, true)) return;
    }

    auto move = string();
    auto tempState(state);
    auto &tempBoard = tempState.board;

    if (shortCastled) {
        tempBoard = state.board;
        tempBoard[sRow][5] = tempBoard[sRow][sCol];
        tempBoard[sRow][sCol] = ' ';
        if (!isKingInCheck(tempBoard, state.isWhite, true)) {
            tempBoard[sRow][6] = tempBoard[sRow][5];
            tempBoard[sRow][5] = tempBoard[sRow][0];
            tempBoard[sRow][7] = ' ';
            move = "O-O";

            if (!isKingInCheck(tempBoard, state.isWhite, true)) {
                if (isKingInCheck(tempBoard, state.isWhite, false)) {
                    checkMate(state, tempState, move);
                }
                state.legalMoves.push_back({ move, Move(sRow, sCol, sRow, 6, evaluatePosition(tempState)) });
            }
        }
    }
    if (longCastled) {
        tempBoard = state.board;
        tempBoard[sRow][3] = tempBoard[sRow][sCol];
        tempBoard[sRow][sCol] = ' ';
        if (!isKingInCheck(tempBoard, state.isWhite, true)) {
            tempBoard[sRow][2] = tempBoard[sRow][3];
            tempBoard[sRow][3] = tempBoard[sRow][0];
            tempBoard[sRow][0] = ' ';
            move = "O-O-O";

            if (!isKingInCheck(tempBoard, state.isWhite, true)) {
                if (isKingInCheck(tempBoard, state.isWhite, false)) {
                    checkMate(state, tempState, move);
                }
                state.legalMoves.push_back({ move, Move(sRow, sCol, sRow, 2, evaluatePosition(tempState)) });
            }
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
        if (abs(state.enPassantCol - sCol) == 1 && sRow == 3 && state.enPassantRow == 2) addMove(state, state.enPassantRow, state.enPassantCol);
    } else {
        if (state.board[sRow + 1][sCol] == ' ') {
            addMove(state, sRow + 1, sCol);
            if (sRow == 1 && state.board[sRow + 2][sCol] == ' ') {
                addMove(state, sRow + 2, sCol);
            }
        }
        if (sCol - 1 >= 0 && isWhitePiece(state.board[sRow + 1][sCol - 1])) addMove(state, sRow + 1, sCol - 1);
        if (sCol + 1 < 8 && isWhitePiece(state.board[sRow + 1][sCol + 1])) addMove(state, sRow + 1, sCol + 1);
        if (abs(state.enPassantCol - sCol) == 1 && sRow == 4 && state.enPassantRow == 5) addMove(state, state.enPassantRow, state.enPassantCol);
    }
}

void Engine::generateLegalMovesUsingOffsets(BoardState& state, array<pair<int, int>, 8> offsets) {
    for (auto const&[oRow, oCol] : offsets) {
        int eRow = sRow + oRow;
        int eCol = sCol + oCol;

        if (eRow >= 0 && eRow < 8 && eCol >= 0 && eCol < 8) addMove(state, eRow, eCol);
    }
}

bool Engine::addMove(BoardState& state, int eRow, int eCol) {
    const char end = state.board[eRow][eCol];
    if ((state.isWhite && isWhitePiece(end)) || (!state.isWhite && isBlackPiece(end))) return true;
    bool isPawn = state.board[sRow][sCol] == White::PAWN || state.board[sRow][sCol] == Black::PAWN;
    string move = string();

    if (!isPawn) {
        move += toupper(state.board[sRow][sCol]);
    }

    if (isPawn && end == ' ' && abs(eCol - sCol) == 1) {
        move = move + (char)('a' + sCol) + 'x';
    } else if (end != ' ') {
        if (isPawn) {
            move = move + (char)('a' + sCol) + 'x';
        } else {
            move += 'x';
        }
    }

    move = move + (char)('a' + eCol) + (char)('8' - eRow);

    auto tempState(state);
    auto &tempBoard = tempState.board;

    if (isPawn && ((state.isWhite && eRow == 0) || (!state.isWhite && eRow == 7))) {
        array<string, 4> promotionMoves = { {
            move + "=Q", move + "=B", move + "=N", move + "=R"
            } };
        for (auto &m : promotionMoves) {
            if (state.isWhite) tempBoard[eRow][eCol] = m.back();
            else tempBoard[eRow][eCol] = m.back() + 0x20;
            tempBoard[sRow][sCol] = ' ';

            if (!isKingInCheck(tempBoard, state.isWhite, true)) {
                if (isKingInCheck(tempBoard, state.isWhite, false)) {
                    checkMate(state, tempState, m);
                }
                state.legalMoves.push_back({ m, Move(sRow, sCol, eRow, eCol, evaluatePosition(tempState)) });
            }
        }
        return false;
    }

    tempBoard[eRow][eCol] = tempBoard[sRow][sCol];
    tempBoard[sRow][sCol] = ' ';

    if (isPawn && abs(eCol - sCol) == 1 && end == ' ') {
        if (state.isWhite) tempBoard[eRow + 1][eCol] = ' ';
        else tempBoard[eRow - 1][eCol] = ' ';
    }

    if (!isKingInCheck(tempBoard, state.isWhite, true)) {
        if (isKingInCheck(tempBoard, state.isWhite, false)) {
            checkMate(state, tempState, move);
        }

        bool duplicate = false;
        Move duplicateMove;
        for (auto const& [n, m] : state.legalMoves) {
            if (move == n) {
                duplicate = true;
                duplicateMove = m;
                break;
            }
        }
        if (duplicate) {
            if (!state.ambigiousMoves.contains(move)) {
                state.ambigiousMoves[move].push_back(duplicateMove);
            }
            state.ambigiousMoves[move].push_back(Move(sRow, sCol, eRow, eCol, evaluatePosition(tempState)));
        } else {
            state.legalMoves.push_back({ move, Move(sRow, sCol, eRow, eCol, evaluatePosition(tempState)) });
        }
    }

    if (end != ' ') return true;
    return false;
}

void Engine::checkMate(BoardState& state, BoardState& tempState, string& move) {
    if (!state.isInCheck) {
        tempState.isInCheck = true;
        tempState.isWhite = !state.isWhite;
        const int tempSRow = sRow;
        const int tempSCol = sCol;
        generateLegalMoves(tempState);
        sRow = tempSRow;
        sCol = tempSCol;
        if (tempState.legalMoves.size() == 0) move += '#';
        else move += '+';
    } else {
        move += '+'; // we don't really care about this I think
    }
}

bool Engine::isKingInCheck(array<array<char, 8>, 8> &board, bool isWhite, bool checkSelf) const{
    bool checkWhite = isWhite ? checkSelf ? true : false : checkSelf ? false : true;
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
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], false)) return true;
        break;
    }
    for (int col = kCol + 1, row = kRow + 1; col < 8 && row < 8; col++, row++) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], false)) return true;
        break;
    }
    for (int col = kCol - 1, row = kRow - 1; col >= 0 && row >= 0; col--, row--) {
        if (board[row][col] == ' ') continue;
        if (actuallyCheckIsKingInCheck(checkWhite, board[row][col], false)) return true;
        break;
    }
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

            state.legalMoves.push_back({ newNotation, move });
        }
    }
    state.ambigiousMoves.clear();
}

void Engine::playMove(BoardState& boardState, const string& notation, const Move& move) {
    auto& board = boardState.board;
    const int sRow = move.sRow;
    const int sCol = move.sCol;
    const int eRow = move.eRow;
    const int eCol = move.eCol;
    int evaluation = move.evaluation;
    // 50 move rule
    if (board[eRow][eCol] != ' ' || board[sRow][sCol] == White::PAWN || board[sRow][sCol] == Black::PAWN) {
        boardState.stalemateMoveCounter = 0;
        //lastIrreversibleMove = previousMoves.size();
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

    board[eRow][eCol] = board[sRow][sCol];
    board[sRow][sCol] = ' ';

    // Pawn promotion
    if (board[eRow][eCol] == White::PAWN && eRow == 0) {
        board[eRow][eCol] = notation[notation.find('=') + 1];
    } else if (board[eRow][eCol] == Black::PAWN && eRow == 7) {
        board[eRow][eCol] = notation[notation.find('=') + 1] + 0x20;
    }

    // Castle
    if (notation.starts_with("O-O-O")) {
        board[sRow][3] = board[sRow][0];
        board[sRow][0] = ' ';
    } else if (notation.starts_with("O-O")) {
        board[sRow][5] = board[sRow][7];
        board[sRow][7] = ' ';
    }


    // Checkmate
    if (notation.ends_with('#')) {
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

pair<string, Move> Engine::getBestMove(BoardState& root, int depth) {
    Move bestMove(-1,-1,-1,-1,-1);
    int alpha = -INF;
    string bestMoveNotation;

    // sort the initial list of moves
    sort(root.legalMoves.begin(), root.legalMoves.end(),
        [&](pair<string, Move> const& a, pair<string, Move> const& b) {
            return root.isWhite
                ? a.second.evaluation > b.second.evaluation   // White wants highest first
                : a.second.evaluation < b.second.evaluation;  // Black wants lowest first
        });
    CALLCOUNT = 0;

    for (auto& [notation, move] : root.legalMoves) {
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
    cout << "Best move: " << bestMoveNotation << ", nodes checked: " << CALLCOUNT << endl;
    return { bestMoveNotation, bestMove };
}

// Using negamax
int Engine::alphaBeta(BoardState& prevState, int alpha, int beta, int depthleft) {
    CALLCOUNT++;
    if (depthleft == 0) return quiesce(prevState, alpha, beta);

    if (depthleft > 1) {
        sort(prevState.legalMoves.begin(), prevState.legalMoves.end(),
            [&](pair<string, Move> const& a, pair<string, Move> const& b) {
                return prevState.isWhite
                    ? a.second.evaluation > b.second.evaluation   // White wants highest first
                    : a.second.evaluation < b.second.evaluation;  // Black wants lowest first
            });
    }
    int bestValue = -INF;
    for (auto& [notation, move] : prevState.legalMoves) {
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
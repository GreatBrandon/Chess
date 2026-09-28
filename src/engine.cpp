#include "engine.h"
#include "evaluate.h"
#include "moveGenerator.h"
#include <iostream>
#include <algorithm>

constexpr int INF = 1000000000;
constexpr int CHECKMATE_EVAL = 100000;
constexpr int CHECKMATE_OFFSET = 100;
int CALLCOUNT = 0;

Engine::Engine() {};

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
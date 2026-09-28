#include "engine.h"

constexpr int INF = 1000000000;
constexpr int CHECKMATE_EVAL = 100000;
constexpr int CHECKMATE_OFFSET = 100;
int CALLCOUNT = 0;

Engine::Engine() {};

void Engine::playMove(BoardState& boardState, const Move& move) {
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
        board[eRow][eCol] = move.promotionPiece;
    } else if (board[eRow][eCol] == Black::PAWN && eRow == 7) {
        board[eRow][eCol] = move.promotionPiece;
    }

    // Castle
    if ((board[eRow][eCol] == White::KING || board[eRow][eCol] == Black::KING)) {
        if (eCol - sCol == 2) {
            // Short castle
            board[sRow][5] = board[sRow][7];
            board[sRow][7] = ' ';
        } else if (sCol - eCol == 2) {
            // Long castle
            board[sRow][3] = board[sRow][0];
            board[sRow][0] = ' ';
        }
    }

    // Checkmate
    if (move.isMate) {
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

Move& Engine::getBestMove(BoardState& root, int depth) {
    Move* bestMove = nullptr;
    int alpha = -INF;

    // sort the initial list of moves
    sort(root.legalMoves.begin(), root.legalMoves.end(),
        [sign = root.isWhite ? 1 : -1](const auto& a, const auto& b) {
            return a.evaluation * sign > b.evaluation * sign;
        });
    CALLCOUNT = 0;

    for (auto& move : root.legalMoves) {
        auto newState(root);

        playMove(newState, move);
        newState.isWhite = !root.isWhite;
        mg.generateLegalMoves(newState);

        int score = -alphaBeta(newState, -INF, -alpha, depth - 1);
        
        //cout << notation << ':' << CALLCOUNT << endl;
        //CALLCOUNT = 0;
        if (bestMove == nullptr || score > alpha) {
            alpha = score;
            bestMove = &move;
        }
    }
    cout << "Nodes checked: " << CALLCOUNT << endl;
    return *bestMove;
}

// Using negamax
int Engine::alphaBeta(BoardState& prevState, int alpha, int beta, int depthleft) {
    CALLCOUNT++;
    if (depthleft == 0) return quiesce(prevState, alpha, beta);

    if (depthleft > 1) {
        sort(prevState.legalMoves.begin(), prevState.legalMoves.end(),
            [sign = prevState.isWhite ? 1 : -1](const auto& a, const auto& b) {
                return a.evaluation * sign > b.evaluation * sign;
            });
    }
    int bestValue = -INF;
    for (const auto& move : prevState.legalMoves) {
        auto newState(prevState);
        playMove(newState, move);
        newState.isWhite = !prevState.isWhite;
        if (depthleft - 1 > 0) mg.generateLegalMoves(newState);
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
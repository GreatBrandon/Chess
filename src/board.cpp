#include "board.h"
#include "evaluate.h"
#include <map>
#include <chrono>
#include <iostream>

chessGame::chessGame() {
    newGame(false);
}

void chessGame::newGame(bool botGame) {
    pieceCount.clear();
    moves.clear();
    boardState = BoardState();
    isDraw = false;
    isCheckmate = false;
    this->botGame = botGame;
    lastIrreversibleMove = 0;
    engine.generateLegalMoves(boardState);
    previousMoves.clear();
    previousMoves.push_back(boardState);
}

void chessGame::playMove(int start, int end, char promotionPiece) {
    const int sRow = start / 8;
    const int sCol = start % 8;
    const int eRow = end / 8;
    const int eCol = end % 8;
    playMove(sRow, sCol, eRow, eCol, promotionPiece);
}

void chessGame::playMove(int sRow, int sCol, int eRow, int eCol, char promotionPiece) {
    auto& board = boardState.board;

    for (auto const& [notation, move] : boardState.legalMoves) {
        if (move.sRow == sRow && move.sCol == sCol && move.eRow == eRow && move.eCol == eCol) {
            if (notation.find('=') != string::npos && notation[notation.find('=') + 1] != promotionPiece) continue;

            engine.playMove(boardState, notation, move);

            // optional optimisation for 3 time repetition
            if (boardState.stalemateMoveCounter = 0) lastIrreversibleMove = previousMoves.size();

            moves.push_back(notation);

            // Checkmate
            if (notation.ends_with('#')) {
                isCheckmate = true;
                return;
            }

            // Check stalemate
            if (boardState.stalemateMoveCounter == 100) {
                isDraw = true;
            }
            map<array<array<char, 8>, 8>, int> previousPositions;
            for (int i = lastIrreversibleMove; i < previousMoves.size(); i++) {
                if (++previousPositions[previousMoves[i].board] == 3) {
                    isDraw = true;
                    // TODO add castling and en passant checks to this to fully satisfy FIDE rules
                    // Use zobrist hash for this
                }
            }
            changePlayer();
            countPieces();
            return;
        }
    }
}

void chessGame::changePlayer() {
    boardState.isWhite = !boardState.isWhite;
    engine.generateLegalMoves(boardState);
    //cout << boardState.legalMoves.size() << "legal moves found for " << boardState.isWhite << endl;
    previousMoves.push_back(boardState);

    if (boardState.legalMoves.size() == 0) {
        isDraw = true;
    } else if (isDraw) {
        boardState.legalMoves.clear();
    }

    evaluatePosition(boardState);

    if (botGame && !boardState.isWhite) {
        playBotMove();
    }
}

vector<pair<int, int>> chessGame::getValidMovesFromPosition(int sRow, int sCol) {
    vector<pair<int, int>> validMoves;
    for (auto const& [notation, move] : boardState.legalMoves) {
        if (move.sRow == sRow && move.sCol == sCol) {
            validMoves.push_back({ move.eRow, move.eCol });
        }
    }
    return validMoves;
}

bool chessGame::isWhite() const {
    return boardState.isWhite;
}

bool chessGame::isBlack() const {
    return !boardState.isWhite;
}

void chessGame::playBotMove() {
    auto start = chrono::high_resolution_clock::now();
    auto bestMove = engine.getBestMove(boardState, engineDepth);
    auto end = chrono::high_resolution_clock::now();
    auto durationMs = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Took " << durationMs << " ms" << endl;
    auto& notation = bestMove.first;
    auto& move = bestMove.second;
    const char promotionPiece = notation.find('=') != string::npos ? notation[notation.find('=') + 1] : 'Q';
    playMove(move.sRow, move.sCol, move.eRow, move.eCol, promotionPiece);
}

void chessGame::countPieces() {
    pieceCount.clear();
    for (auto& row : boardState.board) {
        for (char& piece : row) {
            if (piece == ' ') continue;
            ++pieceCount[piece];
        }
    }
}

void chessGame::increaseDepth() {
    if (engineDepth < MAX_DEPTH) engineDepth++;
}

void chessGame::decreaseDepth() {
    if (engineDepth > MIN_DEPTH) engineDepth--;
}
#include "board.h"

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
    mg.generateLegalMoves(boardState);
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

    for (auto const& move : boardState.legalMoves) {
        if (move.sRow == sRow && move.sCol == sCol && move.eRow == eRow && move.eCol == eCol) {
            const char start = board[sRow][sCol];
            const char end = board[eRow][eCol];

            if ((start == White::PAWN && move.eRow == 0) || (start == Black::PAWN && move.eRow == 7)) {
                if (move.promotionPiece != promotionPiece) continue;
            }

            moves.push_back(getNotationFromMove(move));
            engine.playMove(boardState, move);

            if (boardState.stalemateMoveCounter = 0) lastIrreversibleMove = previousMoves.size();


            // Checkmate
            if (move.isMate) {
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
    mg.generateLegalMoves(boardState);
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
    for (auto const& move : boardState.legalMoves) {
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
    auto &move = engine.getBestMove(boardState, engineDepth);
    auto end = chrono::high_resolution_clock::now();
    auto durationMs = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << "Best move: " << getNotationFromMove(move) << ", took " << durationMs << " ms" << endl;
    playMove(move.sRow, move.sCol, move.eRow, move.eCol, move.promotionPiece);
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


// Invoke this function BEFORE the move has been played
string chessGame::getNotationFromMove(const Move& move) {
    const char start = boardState.board[move.sRow][move.sCol];
    const char end = boardState.board[move.eRow][move.eCol];
    const bool isPawn = start == White::PAWN || start == Black::PAWN;
    const bool isKing = start == White::KING || start == Black::KING;
    string notation;
    bool castled = false;

    if (isKing) {
        if (move.eCol - move.sCol == 2) {
            notation += "O-O";
            castled = true;
        } else if (move.sCol - move.eCol == 2) {
            notation += "O-O-O";
            castled = true;
        }
    }

    if (!castled) {
        if (!isPawn) {
            if (boardState.isWhite) notation += start;
            else notation += start - 0x20;

            if (!isKing) {
                bool ambiguous = false;
                bool fileUnique = true;
                bool rankUnique = true;

                for (const auto& move2 : boardState.legalMoves) {
                    if (&move == &move2) continue;
                    if (boardState.board[move2.sRow][move2.sCol] == start && move2.eRow == move.eRow && move2.eCol == move.eCol) {
                        ambiguous = true;
                        if (move.sCol == move2.sCol) fileUnique = false;
                        if (move.sRow == move2.sRow) rankUnique = false;
                    }
                }

                if (ambiguous) {
                    if (fileUnique) {
                        notation += (char)('a' + move.sCol);
                    } else if (rankUnique) {
                        notation += (char)('8' - move.sRow);
                    } else {
                        notation += (char)('a' + move.sCol);
                        notation += (char)('8' - move.sRow);
                    }
                }
            }
        }

        if (isPawn && abs(move.eCol - move.sCol) == 1) {
            notation += (char)('a' + move.sCol);
            notation += 'x';
        } else if (end != ' ') {
            if (isPawn) {
                notation += (char)('a' + move.sCol);
            }
            notation += 'x';
        }

        notation += (char)('a' + move.eCol);
        notation += (char)('8' - move.eRow);

        if (move.promotionPiece != '\0') {
            notation += '=';
            notation += move.promotionPiece;
        }
    }

    if (move.isCheck) notation += '+';
    else if (move.isMate) notation += '#';

    return notation;
}
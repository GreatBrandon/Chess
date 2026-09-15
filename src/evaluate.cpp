#include "evaluate.h"

int evaluatePosition(BoardState& boardState) {
    int evaluation = 0;

    // count material value and position using pre-calculated weights
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            const char& piece = boardState.board[row][col];
            evaluation += pieceValue(piece);
            evaluation += positionValue(piece, isWhitePiece(piece) ? row : 7 - row, col);
        }
    }
	return evaluation;
}

static constexpr int pieceValue(const char &piece) {
    switch (piece) {
        case White::PAWN:   return  PAWN;
        case White::KNIGHT: return  KNIGHT;
        case White::BISHOP: return  BISHOP;
        case White::ROOK:   return  ROOK;
        case White::QUEEN:  return  QUEEN;

        case Black::PAWN:   return -PAWN;
        case Black::KNIGHT: return -KNIGHT;
        case Black::BISHOP: return -BISHOP;
        case Black::ROOK:   return -ROOK;
        case Black::QUEEN:  return -QUEEN;

        default:            return 0;
    }
}

static constexpr int positionValue(const char& piece, const int& row, const int& col) {
    switch (piece) {
        case White::PAWN:   return  PAWN_WEIGHTS[row][col];
        case White::KNIGHT: return  KNIGHT_WEIGHTS[row][col];
        case White::BISHOP: return  BISHOP_WEIGHTS[row][col];
        case White::ROOK:   return  ROOK_WEIGHTS[row][col];
        case White::QUEEN:  return  QUEEN_WEIGHTS[row][col];
        case White::KING:   return  KING_WEIGHTS_MIDDLE[row][col];

        case Black::PAWN:   return -PAWN_WEIGHTS[row][col];
        case Black::KNIGHT: return -KNIGHT_WEIGHTS[row][col];
        case Black::BISHOP: return -BISHOP_WEIGHTS[row][col];
        case Black::ROOK:   return -ROOK_WEIGHTS[row][col];
        case Black::QUEEN:  return -QUEEN_WEIGHTS[row][col];
        case Black::KING:   return -KING_WEIGHTS_MIDDLE[row][col];

        default:            return 0;
    }
}
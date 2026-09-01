#pragma once

#include "common.h"
#include <array>
#include <unordered_map>
#include <string>

using namespace std;

class Engine {
public:
	unordered_map<string, Move> generateLegalMoves(BoardState, bool = false);
	Engine();
private:
	BoardState boardState;
	int sRow = -1;
	int sCol = -1;
	pair<int, int> kingPos;
	pair<int, int> opponentKingPos;
	bool checkingMode = false;
	void generateLegalMovesRow(BoardState&);
	void generateLegalMovesCol(BoardState&);
	void generateLegalMovesDiagonal(BoardState&);
	void generateLegalMovesKnight(BoardState&);
	void generateLegalMovesKing(BoardState&);
	void generateLegalMovesPawn(BoardState&);
	void generateLegalMovesUsingOffsets(BoardState&, array<pair<int, int>, 8>);
	bool addMove(BoardState&, int, int);
	bool isOppositeKingChecked(BoardState&, array<array<char, 8>, 8>, int, int);
};
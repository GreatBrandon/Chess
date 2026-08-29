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
	array<array<char, 8>, 8> board = START_BOARD;
	unordered_map<string, Move> legalMoves;
	bool isWhite = true;
	int sRow = -1;
	int sCol = -1;
	BoardState boardState;
	pair<int, int> kingPos;
	pair<int, int> opponentKingPos;
	bool checkingMode = false;
	void generateLegalMovesRow();
	void generateLegalMovesCol();
	void generateLegalMovesDiagonal();
	void generateLegalMovesKnight();
	void generateLegalMovesKing();
	void generateLegalMovesPawn();
	void generateLegalMovesUsingOffsets(array<pair<int, int>, 8>);
	bool addMove(int, int);
	bool isOppositeKingChecked(array<array<char, 8>, 8>, int, int);
};
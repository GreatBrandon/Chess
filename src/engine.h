#pragma once

#include "common.h"
#include <array>
#include <map>
#include <string>

using namespace std;

class Engine {
public:
	map<string, Move> generateLegalMoves(array<array<char, 8>, 8>, BoardState);
	Engine();
private:
	array<array<char, 8>, 8> board = START_BOARD;
	map<string, Move> legalMoves;
	bool isWhite = true;
	int sRow = -1;
	int sCol = -1;
	void generateLegalMovesRow();
	void generateLegalMovesCol();
	void generateLegalMovesDiagonal();
	void generateLegalMovesKnight();
	void generateLegalMovesKing();
	void generateLegalMovesPawn();
	void generateLegalMovesUsingOffsets(array<pair<int, int>, 8>);
	bool addMove(int, int);
};
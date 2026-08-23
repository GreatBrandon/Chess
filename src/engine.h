#pragma once

#include "common.h"
#include <array>
#include <map>
#include <string>

using namespace std;

class Engine {
public:
	map<string, Move> generateLegalMoves(array<array<char, 8>, 8>, bool);
	Engine();
private:
	array<array<char, 8>, 8> board = START_BOARD;
	map<string, Move> legalMoves;
	bool isWhite = true;
	void generateLegalMovesRow(int, int);
	void generateLegalMovesCol(int, int);
	void generateLegalMovesDiagonal(int, int);
	void generateLegalMovesKnight(int, int);
	bool addMove(int, int, int, int);
};
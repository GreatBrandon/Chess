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
	int sRow = -1;
	int sCol = -1;
	unordered_map<string, vector<Move>> ambigiousMoves;
	bool checkingMode = false;
	void generateLegalMovesRow(BoardState&);
	void generateLegalMovesCol(BoardState&);
	void generateLegalMovesDiagonal(BoardState&);
	void generateLegalMovesKnight(BoardState&);
	void generateLegalMovesKing(BoardState&);
	void generateLegalMovesPawn(BoardState&);
	void generateLegalMovesUsingOffsets(BoardState&, array<pair<int, int>, 8>);
	bool addMove(BoardState&, int, int);
	bool isKingInCheck(array<array<char, 8>, 8>&, bool, bool) const;
	bool actuallyCheckIsKingInCheck(bool, char, bool) const;
	void disambiguateMoves(BoardState&);
};
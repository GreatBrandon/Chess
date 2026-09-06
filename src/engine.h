#pragma once

#include "common.h"
#include <array>
#include <unordered_map>
#include <string>

using namespace std;

class Engine {
public:
	void generateLegalMoves(BoardState&);
	Engine();
private:
	int sRow = -1;
	int sCol = -1;
	void generateLegalMovesRow(BoardState&);
	void generateLegalMovesCol(BoardState&);
	void generateLegalMovesDiagonal(BoardState&);
	void generateLegalMovesKnight(BoardState&);
	void generateLegalMovesKing(BoardState&);
	void generateLegalMovesPawn(BoardState&);
	void generateLegalMovesUsingOffsets(BoardState&, array<pair<int, int>, 8>);
	void checkMate(BoardState&, BoardState&, string&);
	bool addMove(BoardState&, int, int);
	bool isKingInCheck(array<array<char, 8>, 8>&, bool, bool) const;
	bool actuallyCheckIsKingInCheck(bool, char, bool) const;
	void disambiguateMoves(BoardState&);
};
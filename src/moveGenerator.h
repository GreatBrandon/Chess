#pragma once

#include "common.h"
#include "evaluate.h"
#include <array>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

class MoveGenerator {
public:
	void generateLegalMoves(BoardState&);

private:
	int sRow = -1;
	int sCol = -1;
	void generateLegalMovesRow(BoardState&);
	void generateLegalMovesCol(BoardState&);
	void generateLegalMovesDiagonal(BoardState&);
	void generateLegalMovesKnight(BoardState&);
	void generateLegalMovesKing(BoardState&);
	void generateLegalMovesPawn(BoardState&);
	void generateLegalMovesUsingOffsets(BoardState&, const array<pair<int, int>, 8>&);
	bool checkMate(BoardState&);
	bool addMove(BoardState&, const int&, const int&);
	bool isKingInCheck(const BoardState&, const bool) const;
	bool actuallyCheckIsKingInCheck(bool, char, bool) const;
};
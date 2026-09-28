#pragma once

#include "common.h"
#include <array>
#include <string>

using namespace std;

void generateLegalMoves(BoardState&);

namespace {
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
	constexpr static bool isKingInCheck(BoardState&, bool);
	constexpr static bool actuallyCheckIsKingInCheck(bool, char, bool);
	void disambiguateMoves(BoardState&);
}
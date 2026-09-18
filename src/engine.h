#pragma once

#include "common.h"
#include <array>
#include <string>

using namespace std;

class Engine {
public:
	void generateLegalMoves(BoardState&);
	Engine();
	void playMove(BoardState&, const array<char, 8>&, const Move&);
	pair<array<char, 8>, Move> getBestMove(BoardState&, int);
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
	bool isKingInCheck(BoardState&, bool) const;
	bool actuallyCheckIsKingInCheck(bool, char, bool) const;
	void disambiguateMoves(BoardState&);
	int alphaBeta(BoardState&, int, int, int);
	int quiesce(BoardState&, int, int);
};
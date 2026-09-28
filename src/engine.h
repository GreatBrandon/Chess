#pragma once

#include "common.h"
#include <array>
#include <string>

using namespace std;

class Engine {
public:
	Engine();
	void playMove(BoardState&, const array<char, 8>&, const Move&);
	pair<array<char, 8>, Move> getBestMove(BoardState&, int);
private:
	int alphaBeta(BoardState&, int, int, int);
	int quiesce(BoardState&, int, int);
};
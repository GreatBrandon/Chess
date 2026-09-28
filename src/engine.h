#pragma once

#include "common.h"
#include "evaluate.h"
#include "moveGenerator.h"
#include <array>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

class Engine {
public:
	Engine();
	void playMove(BoardState&, const Move&);
	Move& getBestMove(BoardState&, int);
private:
	MoveGenerator mg;
	int alphaBeta(BoardState&, int, int, int);
	int quiesce(BoardState&, int, int);
};
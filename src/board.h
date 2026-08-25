#pragma once
#include "common.h"
#include "engine.h"
#include <array>
#include <vector>
#include <string>
#include <map>

using namespace std;

class chessBoard {
public:
	array<array<char, 8>, 8> board;
	Engine engine = Engine();
	vector<string> moves;
	map<string, Move> legalMoves;
	BoardState boardState;
	vector<BoardState> previousMoves;

	chessBoard();
	void newGame();
	void playMove(int, int);
	void changePlayer();
	vector<pair<int, int>> getValidMovesFromPosition(int, int);
	bool isWhite();
	bool isBlack();
private:
	void playMove(int, int, int, int);
};
#pragma once
#include "common.h"
#include "engine.h"
#include <array>
#include <vector>
#include <string>

using namespace std;

constexpr int MAX_DEPTH = 6;
constexpr int MIN_DEPTH = 2;

class chessGame {
public:
	Engine engine = Engine();
	vector<string> moves;
	BoardState boardState;
	vector<BoardState> previousMoves;
	bool isDraw = false;
	bool isCheckmate = false;
	bool botGame = false;
	int lastIrreversibleMove = 0;
	int engineDepth = 5;
	unordered_map<char, int> pieceCount;

	chessGame();
	void newGame(bool);
	void playMove(int, int, char);
	void playMove(int, int, int, int, char);
	void changePlayer();
	vector<pair<int, int>> getValidMovesFromPosition(int, int);
	bool isWhite() const;
	bool isBlack() const;
	void countPieces();
	void increaseDepth();
	void decreaseDepth();
private:
	void playBotMove();
};
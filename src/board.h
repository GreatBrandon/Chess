#pragma once
#include "common.h"
#include "engine.h"
#include <array>
#include <vector>
#include <string>

using namespace std;

class chessBoard {
public:
	array<array<char, 8>, 8> board;
	Engine engine = Engine();
	vector<string> moves;
	BoardState boardState;
	vector<BoardState> previousMoves;
	bool isDraw = false;
	bool isCheckmate = false;
	bool botGame = false;
	int lastIrreversibleMove = 0;
	unordered_map<char, int> pieceCount;

	chessBoard();
	void newGame(bool);
	void playMove(int, int, char);
	void changePlayer();
	vector<pair<int, int>> getValidMovesFromPosition(int, int);
	bool isWhite() const;
	bool isBlack() const;
	void countPieces();
private:
	void playMove(int, int, int, int);
	void playBotMove();
};
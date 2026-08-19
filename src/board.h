#pragma once
#include <array>
#include <vector>
#include <string>

using namespace std;

class chessBoard {
public:
	array<array<char, 8>, 8> board;
	bool white = true;
	bool black = false;
	vector<string> moves;
	bool whiteCanLongCastle = true;
	bool whiteCanShortCastle = true;
	bool blackCanLongCastle = true;
	bool blackCanShortCastle = true;
	bool canEnPassant = false;
	int enPassantCol = -1;

	chessBoard();
	void newGame();
	void displayBoard();
	void playMove(int, int);
	void changePlayer();
private:
	bool checkRow(int, int, int);
	bool checkCol(int, int, int);
	bool checkDiagonal(int, int, int, int);
	void playMove(int, int, int, int);
};
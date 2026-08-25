#include "raylib.h"
#include "common.h"
#include "board.h"
#include <string>
#include <map>
#include <iostream>

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#define ORIGIN 60
#define SIZE 75 // matches texture size without scaling

int main () {
	auto board = chessBoard();

	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	InitWindow(1280, 720, "Chess");

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");
	map<char, Texture> pieces;
	pieces['K'] = LoadTexture("wk.png");
	pieces['Q'] = LoadTexture("wq.png");
	pieces['B'] = LoadTexture("wb.png");
	pieces['R'] = LoadTexture("wr.png");
	pieces['N'] = LoadTexture("wn.png");
	pieces['P'] = LoadTexture("wp.png");
	pieces['k'] = LoadTexture("bk.png");
	pieces['q'] = LoadTexture("bq.png");
	pieces['b'] = LoadTexture("bb.png");
	pieces['r'] = LoadTexture("br.png");
	pieces['n'] = LoadTexture("bn.png");
	pieces['p'] = LoadTexture("bp.png");

	array<array<Rectangle, 8>, 8> rects;
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			rects[row][col] = Rectangle(ORIGIN + SIZE * col, ORIGIN + SIZE * row, SIZE, SIZE);
		}
	}
	int selectedRect = -1;
	int droppedRect = -1;
	vector<pair<int, int>> validMoves;
	
	Rectangle newGameButton = Rectangle(700, 30, 130, 30);
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
			Vector2 mousePos = GetMousePosition();
			for (int row = 0; row < 8; row++) {
				for (int col = 0; col < 8; col++) {
					if (CheckCollisionPointRec(mousePos, rects[row][col]) && 
						((board.isWhite() && isWhitePiece(board.board[row][col]))
						|| (board.isBlack() && isBlackPiece(board.board[row][col])))) {
							selectedRect = row * 8 + col;
							validMoves = board.getValidMovesFromPosition(row, col);
					}
				}
			}
			if (CheckCollisionPointRec(mousePos, newGameButton)) board.newGame();
		} else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			if (selectedRect != -1) {
				Vector2 mousePos = GetMousePosition();
				for (int row = 0; row < 8; row++) {
					for (int col = 0; col < 8; col++) {
						if (CheckCollisionPointRec(mousePos, rects[row][col]) && ((board.isWhite() && !isWhitePiece(board.board[row][col])) || (board.isBlack() && !isBlackPiece(board.board[row][col])))) {
							droppedRect = row * 8 + col;
						}
					}
				}
				if (droppedRect != -1 && droppedRect != selectedRect) {
					board.playMove(selectedRect, droppedRect);
				}
				droppedRect = -1;
				selectedRect = -1;
			}
		}


		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);
		const int FONT_SIZE = 24;

		for (int col = 0; col < 8; col++) {
			char text[2] = { 'A' + col, '\0' };
			DrawText(text, ORIGIN + SIZE * col + SIZE / 2 - FONT_SIZE / 2, ORIGIN - 25, FONT_SIZE, WHITE);
		}

		for (int row = 0; row < 8; row++) {
			char text[2] = {'8'- row, '\0'};
			DrawText(text, ORIGIN - 25, ORIGIN + SIZE * row + SIZE / 2 - FONT_SIZE / 2, FONT_SIZE, WHITE);
			for (int col = 0; col < 8; col++) {
				DrawRectangleRec(rects[row][col], ((row + col) % 2 == 0) ? BROWN : DARKBROWN);
			}
		}

		auto b = board.board;
		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				if (row * 8 + col == selectedRect) {
					continue;
				} else {
					DrawTextureEx(pieces[b[row][col]], Vector2(ORIGIN + SIZE * col, ORIGIN + SIZE * row), 0, 0.5, WHITE);
				}
			}
		}

		if (selectedRect != -1) {
			for (auto const& [row, col] : validMoves) {
				DrawCircle(rects[row][col].x + SIZE / 2, rects[row][col].y + SIZE / 2, 15, { 200, 200, 200, 80 });
			}

			Vector2 pos = GetMousePosition();
			pos.x -= SIZE / 2;
			pos.y -= SIZE / 2;
			DrawTextureEx(pieces[b[selectedRect / 8][selectedRect % 8]], pos, 0, 0.5, WHITE);
		}

		const char* text = (board.isWhite()) ? "White" : "Black";
		DrawText(text, 700, 100, FONT_SIZE, WHITE);

		const int coln = 700;
		const int colw = 750;
		const int colb = 850;
		const int row1 = 130;

		for (int i = 0; i < board.moves.size(); i++) {
			const int row = row1 + i / 2 * (FONT_SIZE + 2);
			if (i % 2 == 0) {
				string num = to_string(i / 2 + 1);
				DrawText(num.c_str(), coln, row, FONT_SIZE, WHITE);
				DrawText(board.moves[i].c_str(), colw, row, FONT_SIZE, WHITE);
			} else {
				DrawText(board.moves[i].c_str(), colb, row, FONT_SIZE, WHITE);
			}
		}

		DrawRectangleRounded(newGameButton, 0.75, 0, GREEN);
		DrawText("New Game", 710, 35, FONT_SIZE, WHITE);

		DrawTexture(wabbit, 1000, 100, WHITE);
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	UnloadTexture(wabbit);
	
	for (map<char, Texture>::iterator it = pieces.begin(); it != pieces.end(); it++) {
		UnloadTexture(it->second);
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

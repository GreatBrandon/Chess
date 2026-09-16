#include "raylib.h"
#include "common.h"
#include "board.h"
#include <string>
#include <map>
#include <iostream>

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

enum class Screens {
	MENU,
	SINGLE_PLAYER,
	TWO_PLAYER,
	ONLINE_MATCH,
	DRAW,
	CHECKMATE
};

// Constants
constexpr int ORIGIN = 60;
constexpr int SIZE = 75;
constexpr int SMALL_FONT = 18;
constexpr int MEDIUM_FONT = 24;
constexpr int MEDIUM_LARGE_FONT = 32;
constexpr int LARGE_FONT = 40;
constexpr int NOTATION_ROWS = 22;
constexpr int NOTATION_ROW_HEIGHT = MEDIUM_FONT + 2;

static void DrawTextCenteredHorizontally(const char* text, const int y, const int font) {
	DrawText(text, (GetScreenWidth() - MeasureText(text, font)) / 2, y, font, WHITE);
}

int main() {
	auto currentScreen = Screens::MENU;
	auto game = chessGame();

	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(1280, 720, "Chess");
	SearchAndSetResourceDir("resources");

	// Initialise textures
	map<char, Texture> pieces;
	pieces[White::KING] = LoadTexture("wk.png");
	pieces[White::QUEEN] = LoadTexture("wq.png");
	pieces[White::BISHOP] = LoadTexture("wb.png");
	pieces[White::ROOK] = LoadTexture("wr.png");
	pieces[White::KNIGHT] = LoadTexture("wn.png");
	pieces[White::PAWN] = LoadTexture("wp.png");
	pieces[Black::KING] = LoadTexture("bk.png");
	pieces[Black::QUEEN] = LoadTexture("bq.png");
	pieces[Black::BISHOP] = LoadTexture("bb.png");
	pieces[Black::ROOK] = LoadTexture("br.png");
	pieces[Black::KNIGHT] = LoadTexture("bn.png");
	pieces[Black::PAWN] = LoadTexture("bp.png");

	// Initialise rectangles
	array<array<Rectangle, 8>, 8> rects{};
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			rects[row][col] = Rectangle(ORIGIN + SIZE * col, ORIGIN + SIZE * row, SIZE, SIZE);
		}
	}
	
	Rectangle newGameButton = Rectangle(1080, 300, 130, 30);
	Rectangle singlePlayerButton = Rectangle(440, 200, 400, 70);
	Rectangle doublePlayerButton = Rectangle(440, 325, 400, 70);
	Rectangle onlinePlayButton = Rectangle(440, 450, 400, 70);
	Rectangle notationRect = Rectangle(700, ORIGIN + NOTATION_ROW_HEIGHT, 300, NOTATION_ROW_HEIGHT * NOTATION_ROWS + 10);
	Rectangle roundedNotationRect = Rectangle(700, ORIGIN, 300, NOTATION_ROW_HEIGHT * (NOTATION_ROWS + 1) + 10);
	const char* backToGameButtonText = "Back";
	const int textWidth = MeasureText(backToGameButtonText, MEDIUM_FONT);
	Rectangle backToGameButton = Rectangle((GetScreenWidth() - textWidth) / 2, 400, textWidth + 20, MEDIUM_FONT + 20);


	// Initialise other
	int selectedRect = -1;
	int droppedRect = -1;
	vector<pair<int, int>> validMoves;
	char promotionPiece = 'Q';
	bool backPressed = false;
	bool showEval = false;

	// Game loop
	while (!WindowShouldClose()) {
		auto const& board = game.boardState.board;
		// Check inputs
		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			Vector2 mousePos = GetMousePosition();
			if (currentScreen == Screens::MENU) {
				if (CheckCollisionPointRec(mousePos, singlePlayerButton)) {
					game.newGame(true);
					currentScreen = Screens::SINGLE_PLAYER;
				}
				if (CheckCollisionPointRec(mousePos, doublePlayerButton)) {
					game.newGame(false);
					currentScreen = Screens::TWO_PLAYER;
				}
				if (CheckCollisionPointRec(mousePos, onlinePlayButton)) {
					//board.newGame(false);
					//currentScreen = Screens::ONLINE_MATCH;
				}
			} else if (currentScreen == Screens::DRAW || currentScreen == Screens::CHECKMATE) {
				if (CheckCollisionPointRec(mousePos, backToGameButton)) {
					currentScreen = Screens::SINGLE_PLAYER; // potentially maybe can break something in the future
					backPressed = true;
				}
			} else {
				if (CheckCollisionPointRec(mousePos, newGameButton)) currentScreen = Screens::MENU;
				if (!game.isDraw && !game.isCheckmate) {
					for (int row = 0; row < 8; row++) {
						for (int col = 0; col < 8; col++) {
							if (CheckCollisionPointRec(mousePos, rects[row][col]) &&
								((game.isWhite() && isWhitePiece(board[row][col]))
									|| (game.isBlack() && isBlackPiece(board[row][col])))) {
								selectedRect = row * 8 + col;
								validMoves = game.getValidMovesFromPosition(row, col);
							}
						}
					}
				}
			}
		} else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
			if (selectedRect != -1) {
				Vector2 mousePos = GetMousePosition();
				for (int row = 0; row < 8; row++) {
					for (int col = 0; col < 8; col++) {
						if (CheckCollisionPointRec(mousePos, rects[row][col]) && ((game.isWhite() && !isWhitePiece(board[row][col])) || (game.isBlack() && !isBlackPiece(board[row][col])))) {
							droppedRect = row * 8 + col;
						}
					}
				}
				if (droppedRect != -1 && droppedRect != selectedRect) {
					game.playMove(selectedRect, droppedRect, promotionPiece);
				}
				droppedRect = -1;
				selectedRect = -1;
			}
		}
		if (currentScreen == Screens::MENU) {
			if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) game.increaseDepth();
			else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) game.decreaseDepth();
		}
		if (IsKeyDown(KEY_B)) promotionPiece = 'B';
		else if (IsKeyDown(KEY_N)) promotionPiece = 'N';
		else if (IsKeyDown(KEY_R)) promotionPiece = 'R';
		else if (IsKeyDown(KEY_Q)) promotionPiece = 'Q';
		else if (IsKeyDown(KEY_E)) showEval = true;
		else if (IsKeyUp(KEY_E)) showEval = false;
		// drawing
		BeginDrawing();

		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(BLACK);

		// Menu Screen
		if (currentScreen == Screens::MENU) {
			DrawText("CHESS", 510, 100, 72, WHITE);
			DrawRectangleRounded(singlePlayerButton, 0.9f, 10, BROWN);
			DrawRectangleRounded(doublePlayerButton, 0.9f, 10, BROWN);
			DrawRectangleRounded(onlinePlayButton, 0.9f, 10, BROWN);
			DrawTextCenteredHorizontally("Single player", singlePlayerButton.y + 15, LARGE_FONT);
			DrawTextCenteredHorizontally("Double player", doublePlayerButton.y + 15, LARGE_FONT);
			DrawTextCenteredHorizontally("Online play", onlinePlayButton.y + 15, LARGE_FONT);
			string depthText = "Engine depth: ";
			depthText += to_string(game.engineDepth);
			DrawTextCenteredHorizontally(depthText.c_str(), singlePlayerButton.y + 75, MEDIUM_FONT);
			DrawTextCenteredHorizontally("W|S to change", singlePlayerButton.y + 100, SMALL_FONT);
			EndDrawing();
			continue;
		} else if (currentScreen == Screens::DRAW || currentScreen == Screens::CHECKMATE) {
			const char* text = currentScreen == Screens::DRAW ? "Game ended in a draw!" : game.boardState.isWhite ? "White won!" : "Black won!";
			const int textWidth = MeasureText(text, LARGE_FONT);
			DrawText(text, (GetScreenWidth() - textWidth) / 2, (GetScreenHeight() - LARGE_FONT) / 2, LARGE_FONT, WHITE);
			DrawRectangleRounded(backToGameButton, 0.2, 10, BROWN);
			DrawRectangleRoundedLinesEx(backToGameButton, 0.2, 10, 2, DARKBROWN);
			DrawText(backToGameButtonText, backToGameButton.x + 10, backToGameButton.y + MEDIUM_FONT / 2, MEDIUM_FONT, WHITE);
			EndDrawing();
			continue;
		}

		// Game screen
		// Board
		for (int col = 0; col < 8; col++) {
			char text[2] = { 'A' + col, '\0' };
			DrawText(text, ORIGIN + SIZE * col + SIZE / 2 - SMALL_FONT / 2 + 4, ORIGIN - SMALL_FONT + 1, SMALL_FONT, WHITE);
		}

		for (int row = 0; row < 8; row++) {
			char text[2] = {'8'- row, '\0'};
			DrawText(text, ORIGIN - SMALL_FONT + 1, ORIGIN + SIZE * row + SIZE / 2 - SMALL_FONT / 2 + 2, SMALL_FONT, WHITE);
			for (int col = 0; col < 8; col++) {
				DrawRectangleRec(rects[row][col], ((row + col) % 2 == 0) ? BROWN : DARKBROWN);
			}
		}

		for (int row = 0; row < 8; row++) {
			for (int col = 0; col < 8; col++) {
				if (row * 8 + col == selectedRect) {
					continue;
				} else {
					DrawTextureEx(pieces[board[row][col]], Vector2(ORIGIN + SIZE * col, ORIGIN + SIZE * row), 0, 0.5, WHITE);
				}
			}
		}

		// Draw dragged piece
		if (selectedRect != -1) {
			for (auto const& [row, col] : validMoves) {
				DrawCircle(rects[row][col].x + SIZE / 2, rects[row][col].y + SIZE / 2, 15, { 200, 200, 200, 80 });
			}

			Vector2 pos = GetMousePosition();
			pos.x -= SIZE / 2;
			pos.y -= SIZE / 2;
			DrawTextureEx(pieces[board[selectedRect / 8][selectedRect % 8]], pos, 0, 0.5, WHITE);
		}

		// Draw Player turn
		string text = (game.isWhite() ? "White" : "Black");
		text += " to play";
		DrawText(text.c_str(), notationRect.x + 10, ORIGIN - MEDIUM_LARGE_FONT - 2, MEDIUM_LARGE_FONT, WHITE);

		// Draw notation
		DrawRectangleRounded(roundedNotationRect, 0.1, 10, BROWN);
		DrawRectangleRoundedLinesEx(roundedNotationRect, 0.1, 10, 4, DARKBROWN);
		DrawText("Move history:", roundedNotationRect.x + 10, roundedNotationRect.y + 4, MEDIUM_FONT, WHITE);
		int startRow = notationRect.y + 10;

		BeginScissorMode(notationRect.x, notationRect.y, notationRect.width, notationRect.height);

		const int totalRows = (game.moves.size() + 1) / 2;

		if (totalRows > NOTATION_ROWS) {
			startRow -= (totalRows - NOTATION_ROWS) * NOTATION_ROW_HEIGHT;
		}

		for (int i = 0; i < game.moves.size(); i++) {
			const int row = startRow + i / 2 * NOTATION_ROW_HEIGHT;
			if (row < notationRect.y) continue;
			if (i % 2 == 0) {
				string numText = to_string(i / 2 + 1);
				numText += '.';
				DrawText(numText.c_str(), notationRect.x + 10, row, MEDIUM_FONT, WHITE);
				DrawText(game.moves[i].c_str(), notationRect.x + 60, row, MEDIUM_FONT, WHITE);
			} else {
				DrawText(game.moves[i].c_str(), notationRect.x + 170, row, MEDIUM_FONT, WHITE);
			}
		}

		EndScissorMode();

		// Draw taken pieces
		auto& pieceCount = game.pieceCount;
		int pieceX = ORIGIN + 60;
		DrawText("Black", ORIGIN, 10, MEDIUM_FONT, WHITE);
		for (const char piece : White::pieces) {
			int whiteCount = pieceCount[piece];
			const int blackCount = pieceCount[piece + 0x20];
			while (whiteCount++ < blackCount) {
				DrawTextureEx(pieces[piece], Vector2(pieceX += 14, 6), 0, 0.2, WHITE);
			}
		}

		pieceX = ORIGIN + 60;
		DrawText("White", ORIGIN, 670, MEDIUM_FONT, WHITE);
		for (const char piece : Black::pieces) {
			int blackCount = pieceCount[piece];
			const int whiteCount = pieceCount[piece - 0x20];
			while (blackCount++ < whiteCount) {
				DrawTextureEx(pieces[piece], Vector2(pieceX += 14, 670), 0, 0.2, WHITE);
			}
		}


		// Draw other buttons and text
		DrawRectangleRounded(newGameButton, 0.75, 10, BROWN);
		DrawRectangleRoundedLinesEx(newGameButton, 0.75, 10, 3, DARKBROWN);
		DrawText("New Game", newGameButton.x + 10, newGameButton.y + MEDIUM_FONT / 2 - 8, MEDIUM_FONT, WHITE);

		string promotionText = "Next pawn promotes to: ";
		switch (promotionPiece) {
			case 'Q': promotionText += "Queen"; break;
			case 'R': promotionText += "Rook"; break;
			case 'N': promotionText += "Knight"; break;
			case 'B': promotionText += "Bishop"; break;
		}
		DrawText(promotionText.c_str(), notationRect.x + 10, 675, SMALL_FONT, WHITE);
		DrawText("Press Q/R/N/B to change", notationRect.x + 10, 695, SMALL_FONT, WHITE);


		if (game.isDraw) {
			if (!backPressed) currentScreen = Screens::DRAW;
			DrawText("Draw", 1090, (GetScreenHeight() - LARGE_FONT) / 2, LARGE_FONT, WHITE);
		} else if (game.isCheckmate) {
			if (!backPressed) currentScreen = Screens::CHECKMATE;
			DrawText("Checkmate", 1030, (GetScreenHeight() - LARGE_FONT) / 2, LARGE_FONT, WHITE);
		}

		if (showEval) {
			DrawText(to_string((double) game.boardState.evaluation / 100).c_str(), 1150, newGameButton.y - 30, SMALL_FONT, WHITE);
		} else {
			DrawText("Hold E to show evaluation", 1020, newGameButton.y - 30, SMALL_FONT, WHITE);
		}
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	for (map<char, Texture>::iterator it = pieces.begin(); it != pieces.end(); it++) {
		UnloadTexture(it->second);
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}

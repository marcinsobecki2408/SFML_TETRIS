#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <random>
#include <algorithm>
#include "Structures.h"
#include "Generator.h"
#include "Tetromino.h"
#include "GameFieldManager.h"

using namespace sf;

int main(int argc, char* argv[])
{
	// Initialization
	const int heightWithBorder = 25;
	const int widthWithBorder = 12;
	int currentPoints = 0;
	int linesCleared = 0;
	Point* offset = new Point{ 4, 0 };
	Point nextPieceWindow = { 14 * 19 + 4, 5 * 19 + 4 };

	/*
	Game field manager
	Representation of numbers in field:
	- 0-6 tetrominoes where each number represents index of piece from Tetromino class,
	- 8 - empty space,
	- 9 - border of the game field.
	*/
	GameFieldManager gameFieldManager(widthWithBorder, heightWithBorder, offset);
	int* gameField = gameFieldManager.getGameField();

	// Creation of current and next tetromino
	Tetromino* tetromino = new Tetromino(offset, gameField, widthWithBorder);
	Tetromino* nextTetromino = new Tetromino(tetromino->getCurrentTetrominoIndex(), offset, gameField, widthWithBorder);	// Ensures, that generated tetrominoes are different
	nextTetromino->makeNewPiece();
	// Flags
	bool rotate = false,
		moveLeft = false,
		moveRight = false,
		moveDown = false,
		gameOver = false,
		pauseGame = false,
		resetGame = false,
		turboModeOn = false;

	if (argv[1] != nullptr)
	{
		if (strcmp(argv[1], "t") == 0)
			turboModeOn = true;
		else
			turboModeOn = false;
	}

	// Window render
	RenderWindow window(VideoMode(550, 21 * 19), "Tetris Marcinka");

	// Loading textures of the blocks
	Texture blockTexture;
	blockTexture.loadFromFile("Tetris.png");
	Sprite blockSprite(blockTexture);

	// Texture of the border
	Sprite border(blockTexture);
	border.setTextureRect(IntRect(133, 0, 19, 19));

	// Background of the game
	Texture background;
	background.loadFromFile("background.png");
	Sprite bgnd(background);
	bgnd.setPosition(19, 0);

	// Time mesurement
	Clock clock;
	float timer = 0;
	float timestep = 0.3;

	// Font and labels initialization
	Font font;
	if (!font.loadFromFile("Fonts/arial.ttf"))
	{
		std::cout << "Cannot read font" << std::endl;
		system("pause");
		return 1;
	}

	Text scoreLabel;
	scoreLabel.setFont(font);
	scoreLabel.setPosition(14 * 19, 2 * 19);
	scoreLabel.setCharacterSize(40);
	scoreLabel.setFillColor(Color::Black);

	Text controlsLabels;
	controlsLabels.setFont(font);
	controlsLabels.setPosition(14 * 19, 12 * 19);
	controlsLabels.setCharacterSize(24);
	controlsLabels.setFillColor(Color::Black);

	// Main loop of the program
	Event event;
	while (window.isOpen())
	{
		// Time mesurement
		float time = clock.getElapsedTime().asSeconds();
		clock.restart();
		timer += time;

		// Draw background and and current state of gameField
		window.clear(Color(160, 160, 160));
		window.draw(bgnd);
		for (int i = 0; i < widthWithBorder; i++)
		{
			for (int j = 0; j < heightWithBorder; j++)
			{
				if (gameField[widthWithBorder * j + i] == 9)
				{
					border.setPosition(i * 19, (j - 4) * 19);
					window.draw(border);
				}
				if (gameField[widthWithBorder * j + i] >= 0 && gameField[widthWithBorder * j + i] <= 6)
				{
					blockSprite.setPosition(i * 19, (j - 4) * 19);
					blockSprite.setTextureRect(IntRect(gameField[widthWithBorder * j + i] * 19, 0, 19, 19));
					window.draw(blockSprite);
				}
			}
		}

		// Check for the inputs from keyboard and set the right flags if needed
		while (window.pollEvent(event))
		{
			if (event.type == Event::Closed)
			{
				window.close();
			}
			if (event.type == Event::KeyPressed)
			{
				if (event.key.code == Keyboard::R)
				{
					resetGame = true;
				}
				else if (event.key.code == Keyboard::Up)
				{
					rotate = true;
				}
				else if (event.key.code == Keyboard::Left)
				{
					moveLeft = true;
				}
				else if (event.key.code == Keyboard::Right)
				{
					moveRight = true;
				}
				else if (event.key.code == Keyboard::P)
				{
					pauseGame = !pauseGame;
					rotate = false;
					moveDown = false;
					moveLeft = false;
					moveRight = false;
				}
				else if (event.key.code == Keyboard::T)
				{
					turboModeOn = !turboModeOn;
				}
			}
		}

		// Game logic based on flags set
		if (!gameOver)
		{
			if (!resetGame)
			{
				if (!pauseGame)
				{
					if (rotate)
					{
						tetromino->rotate();
						rotate = false;
					}

					if (moveLeft)
					{
						tetromino->moveLeft();
						moveLeft = false;
					}

					if (moveRight)
					{
						tetromino->moveRight();
						moveRight = false;
					}

					if (Keyboard::isKeyPressed(Keyboard::Down) && turboModeOn)
						timestep = 0.002;
					else if (Keyboard::isKeyPressed(Keyboard::Down) && !turboModeOn)
						timestep = 0.05;
					else
						timestep = 0.3;

					if (timer > timestep)
					{
						bool collisionOccured = !tetromino->moveDown();	// Method returns true if piece is successfully moved down
						if (collisionOccured)
						{
							std::swap(tetromino, nextTetromino);
							nextTetromino->makeNewNextPiece(tetromino->getCurrentTetrominoIndex());	// This is made to ensure that there are no two same pieces in the row

							// Checking for completed lines
							linesCleared = gameFieldManager.checkForFullLines();
						}
						timer = 0;
					}

					if (gameFieldManager.checkIfGameOver())
					{
						gameOver = true;
						continue;
					}
				}
			}
			else
			{
				resetGame = false;
				pauseGame = false;
				tetromino->makeNewPiece();
				nextTetromino->makeNewNextPiece(tetromino->getCurrentTetrominoIndex());	// This is made to ensure that there are no two same pieces in the row
				gameFieldManager.clearGameField();
				currentPoints = 0;
			}
			
			currentPoints += (linesCleared * linesCleared) * 100;	// Additional points for completing more lines in one move
			linesCleared = 0; // Reset number of cleared lines to 0 for the next game tick
		}
		else	// In case of game over, special information is displayed
		{
			if (resetGame)
			{
				gameOver = false;
				resetGame = false;
				pauseGame = false;
				tetromino->makeNewPiece();
				nextTetromino->makeNewNextPiece(tetromino->getCurrentTetrominoIndex());	// This is made to ensure that there are no two same pieces in the row
				gameFieldManager.clearGameField();
				currentPoints = 0;
			}

			RectangleShape rect(Vector2f(12 * 19 - 8, 8 * 19 - 4));
			rect.setPosition(4, 6 * 19);
			rect.setFillColor(Color(90, 154, 231, 235));
			rect.setOutlineColor(Color::Black);
			rect.setOutlineThickness(4);
			window.draw(rect);

			Text textGameOver;
			textGameOver.setCharacterSize(25);
			textGameOver.setFillColor(Color::Black);
			textGameOver.setFont(font);
			textGameOver.setPosition(50, 7 * 19);
			textGameOver.setString("Game over!\nPress R to \nstart again");
			window.draw(textGameOver);
		}
		// Display current score and instructions
		scoreLabel.setString("Score: " + std::to_string(currentPoints));
		window.draw(scoreLabel);
		String turboModeLabel = turboModeOn ? String("ON") : String("OFF");
		controlsLabels.setString("R - Restart\nP - Pause\nTurbo Mode: " + turboModeLabel);
		window.draw(controlsLabels);

		// Drawing window where next piece is displayed
		RectangleShape nextPieceRect(Vector2f(4 * 19, 4 * 19));
		nextPieceRect.setPosition(nextPieceWindow.x, nextPieceWindow.y);
		nextPieceRect.setFillColor(Color(50, 50, 50, 255));
		nextPieceRect.setOutlineColor(Color::Black);
		nextPieceRect.setOutlineThickness(4);
		window.draw(nextPieceRect);

		Text nextPieceLabel;
		nextPieceLabel.setFont(font);
		nextPieceLabel.setPosition(15 * 19, 9 * 19 + 8);
		nextPieceLabel.setCharacterSize(24);
		nextPieceLabel.setFillColor(Color::Black);
		nextPieceLabel.setString("Next");
		window.draw(nextPieceLabel);

		// Setting correct block texture for next piece and displaying it
		blockSprite.setTextureRect(IntRect(nextTetromino->getCurrentTetrominoIndex() * 19, 0, 19, 19));
		for (int i = 0; i < nextTetromino->getTetrominoTileSide(); i++)
		{
			blockSprite.setPosition(nextPieceWindow.x + nextTetromino->getCoordinatesInTile()[i].x * 19, nextPieceWindow.y + nextTetromino->getCoordinatesInTile()[i].y * 19);
			window.draw(blockSprite);
		}

		// Draw tetromino piece onto game field
		blockSprite.setTextureRect(IntRect(tetromino->getCurrentTetrominoIndex() * 19, 0, 19, 19));
		for (int i = 0; i < tetromino->getTetrominoTileSide(); i++)
		{
			blockSprite.setPosition((offset->x + tetromino->getCoordinatesInTile()[i].x) * 19, (offset->y + tetromino->getCoordinatesInTile()[i].y - 4) * 19);
			window.draw(blockSprite);
		}

		window.display();
	}

	// Cleaning up
	delete offset;
	delete tetromino;
	delete nextTetromino;
	return 0;
}

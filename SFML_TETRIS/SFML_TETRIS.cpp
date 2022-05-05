#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include "Point.h"
#include "Tetromino.h"
#include "GameFieldManager.h"
#include "connector.h"
#include "Checkpoint.h"

using namespace sf;

using std::cout;
using std::endl;

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

	// Make checkpoint
	Checkpoint* checkpoint = new Checkpoint(offset, gameField, heightWithBorder * widthWithBorder, tetromino->getCurrentTetrominoIndex(), nextTetromino->getCurrentTetrominoIndex());

	// Flags
	bool rotate = false,
		moveLeft = false,
		moveRight = false,
		hardDrop = false,
		gameOver = false,
		pauseGame = false,
		resetGame = false,
		turboModeOn = false,
		usePipes = false,
		makeCheckpoint = false,
		restoreCheckpoint = false;

	// Parameter handling
	std::vector<string> args(argv, argv + argc);
	if (std::find(begin(args), end(args), "t") != end(args))
	{
		turboModeOn = true;
	}
	if (std::find(begin(args), end(args), "p") != end(args))
	{
		usePipes = true;
	}

	// Window render
	RenderWindow window(VideoMode(550, 21 * 19), "Tetris Game");

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
		cout << "Cannot read font" << endl;
#ifdef _WIN32 || _WIN64
		system("pause");
#endif
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

    connector* inCon = nullptr;
    connector* outCon = nullptr;
	string msg;
    if (usePipes) {
        inCon = new connector("/tmp/biai_input");
        outCon = new connector("/tmp/biai_output");
        inCon->clear();
        outCon->clear();
    }

    // [gameField..., currentPiece, nextPiece]
    const int GSE_SIZE = 10*24+2;
    char gameStateExport[GSE_SIZE];

	// Main loop of the program
	Event event;
	while (window.isOpen())
	{
		if (usePipes && msg.empty())	// steering string is empty - it has been consumed OR the game is just beginning
		{
            gameFieldManager.createGameFieldExport(gameStateExport);
            gameStateExport[GSE_SIZE - 2] = (char) tetromino->getCurrentTetrominoIndex();
            gameStateExport[GSE_SIZE - 1] = (char) nextTetromino->getCurrentTetrominoIndex();
            outCon->write(gameStateExport, GSE_SIZE);

			while (msg.empty())	// wait for responce from AI - might be faulty
			{
				msg = inCon->read();
			}
		}
			
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
				else if (gameField[widthWithBorder * j + i] >= 0 && gameField[widthWithBorder * j + i] <= 6)
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
				switch (event.key.code)
				{
				case Keyboard::R:
					resetGame = true;
					break;
				case Keyboard::Up:
					rotate = true;
					break;
				case Keyboard::Left:
					moveLeft = true;
					break;
				case Keyboard::Right:
					moveRight = true;
					break;
				case Keyboard::Q:
					hardDrop = true;
					break;
				case Keyboard::P:
					pauseGame = !pauseGame;
					rotate = false;
					moveLeft = false;
					moveRight = false;
					hardDrop = false;
					break;
				case Keyboard::T:
					turboModeOn = !turboModeOn;
					break;
				case Keyboard::C:
					makeCheckpoint = true;
					break;
				case Keyboard::V:
					restoreCheckpoint = true;
					break;
				}
			}
		}

		if (!msg.empty())
		{
			char currentAction = msg.front();
			switch (currentAction)
			{
				case 'w':
					rotate = true;
					break;
				case 'a':
					moveLeft = true;
					break;
				case 'd':
					moveRight = true;
					break;
				case 'q':
					hardDrop = true;
					break;
			}
            msg.erase(0, 1);
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
					else if (moveLeft)
					{
						tetromino->moveLeft();
						moveLeft = false;
					}
					else if (moveRight)
					{
						tetromino->moveRight();
						moveRight = false;
					}
					else if (makeCheckpoint)
					{
						checkpoint->save(offset, gameField, tetromino->getCurrentTetrominoIndex(), nextTetromino->getCurrentTetrominoIndex(), currentPoints);
						makeCheckpoint = false;
					}
					else if (restoreCheckpoint)
					{
						checkpoint->restore(offset, gameField, tetromino, nextTetromino, currentPoints);
						restoreCheckpoint = false;
					}

					if (Keyboard::isKeyPressed(Keyboard::Down) && turboModeOn)
						timestep = 0.002;
					else if (Keyboard::isKeyPressed(Keyboard::Down) && !turboModeOn)
						timestep = 0.05;
					else
						timestep = 0.3;

					if (timer > timestep || usePipes)
					{
						bool collisionOccured;
						do
						{
							collisionOccured = !tetromino->moveDown();	// Method returns true if piece is successfully moved down
							if (collisionOccured)
								hardDrop = false;
						} while (hardDrop);

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
			
			if (linesCleared != 0)
			{
				currentPoints += (linesCleared * linesCleared) * 100;	// Additional points for completing more lines in one move
				linesCleared = 0; // Reset number of cleared lines to 0 for the next game tick
			}
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
	delete checkpoint;

    if(inCon)
        delete inCon;
    if(outCon)
        delete outCon;

	return 0;
}

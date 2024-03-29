﻿#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include "Point.h"
#include "Tetromino.h"
#include "GameFieldManager.h"
#include "connector.h"
#include "Checkpoint.h"
#include <sstream>
#include <iomanip>

using namespace sf;

using std::cout;
using std::endl;

int main(int argc, char* argv[])
{
	// Initialization
	const int heightWithBorder = 25;
	const int widthWithBorder = 12;
	int currentPoints = 0,
		linesCleared = 0,
		pipeIndex = -2;
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
		usePipes = false,
		makeCheckpoint = false,
		restoreCheckpoint = false,
        dontDraw = false,
		displayModeOn = false,
		displayModeDrawingFall = false;

	// Parameter handling
	std::vector<std::string> args(argv, argv + argc);
	if (std::find(begin(args), end(args), "p") != end(args))
	{
		usePipes = true;
		auto pArgIterator = std::find(begin(args), end(args), "p");
		if (pArgIterator + 1 != end(args))
		{
			string value = *(pArgIterator + 1);
			pipeIndex = stoi(value);
		}
	}
	if (std::find(begin(args), end(args), "d") != end(args))
	{
		displayModeOn = true;
	}

	// Window render
    const int W_WIDTH = 550;
    const int W_HEIGHT = 21 * 19;
	RenderWindow window(VideoMode(W_WIDTH, W_HEIGHT), "Tetris Game");

    if(pipeIndex >= 0) {
        window.setTitle("Tetris Game - " + std::to_string(pipeIndex));
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        int horizontal = desktop.width / W_WIDTH;
        int vertical = desktop.height / W_HEIGHT;
        int x = pipeIndex % horizontal;
        int y = pipeIndex % vertical;
        window.setPosition(sf::Vector2i(x * W_WIDTH, y * W_HEIGHT));
    }

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
#ifdef _WIN32
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

    innector* inCon = nullptr;
    outnector* outCon = nullptr;
	string msg, scoreString;
	std::stringstream ss;

    if (usePipes) {
		string inConName = "/tmp/biai_input_";
		inConName.append(std::to_string(pipeIndex));
        inCon = new innector(inConName);	// if does not work, add ".c_str()"
		string outConName = "/tmp/biai_output_";
		outConName.append(std::to_string(pipeIndex));
        outCon = new outnector(outConName);
    }

    // [gameField..., currentPiece, nextPiece, score, gameOver]
    const int GSE_SIZE = 10 * 24 + 2 + 10 + 1;
    char gameStateExport[GSE_SIZE];

	// Main loop of the program
	Event event;
	while (window.isOpen())
	{
		if (usePipes && msg.empty() && !displayModeDrawingFall)	// steering string is empty - it has been consumed OR the game is just beginning
		{
            gameFieldManager.createGameFieldExport(gameStateExport);
            gameStateExport[GSE_SIZE - 13] = (char) tetromino->getCurrentTetrominoIndex();
            gameStateExport[GSE_SIZE - 12] = (char) nextTetromino->getCurrentTetrominoIndex();

            ss.str("");
			ss << std::setw(10) << std::setfill('0') << currentPoints;
			scoreString = ss.str();
			for (int i = 0; i < 10; i++)
			{
				gameStateExport[GSE_SIZE - 11 + i] = scoreString[i];
			}
			gameStateExport[GSE_SIZE - 1] = gameOver ? 'N' : 'Y';
            outCon->write(gameStateExport, GSE_SIZE);

//			while (msg.empty())	// wait for responce from AI - might be faulty
//			{ // edit: never had to repeat after some changes
				msg = inCon->read();
//			}
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
					if (displayModeOn)
						displayModeDrawingFall = true;
					break;
				case Keyboard::P:
					pauseGame = !pauseGame;
					rotate = false;
					moveLeft = false;
					moveRight = false;
					hardDrop = false;
					break;
				case Keyboard::C:
					makeCheckpoint = true;
					break;
				case Keyboard::V:
					restoreCheckpoint = true;
					break;
				case Keyboard::O:
					displayModeOn = !displayModeOn;
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
					if (displayModeOn && !dontDraw)
						displayModeDrawingFall = true;
					break;
                case 'c':
                    makeCheckpoint = true;
                    break;
                case 'v':
                    restoreCheckpoint = true;
                    break;
                case 'r':
                    resetGame = true;
                    break;
                case '[':
                    dontDraw = true;
                    break;
                case ']':
                    dontDraw = false;
					break;
                default:
                    cout << "key '" << currentAction << "' not recognized" << endl;
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

					if (Keyboard::isKeyPressed(Keyboard::Down))
						timestep = 0.05;
					else if (displayModeDrawingFall)
					{
						timestep = 0.01;
					}
					else
						timestep = 0.3;

					{
						bool collisionOccured = false;
						if (usePipes)	// for pipes & AI
						{
							if (!dontDraw && displayModeOn && hardDrop)	// if AI draws result & display mode is on & hard drop is pressed
							{
								if(timer > timestep)	// decrease timestep to 0.01s and wait for the collision to occur
								{
									collisionOccured = !tetromino->moveDown();
									if (collisionOccured)
									{
										displayModeDrawingFall = false;
										hardDrop = false;
									}
									timer = 0;
								}
							}
							else	// else do instant hard drop without animation of falling down (e.g. in learing process)
							{
								do
								{
									collisionOccured = !tetromino->moveDown();	// Method returns true if piece is successfully moved down
									if (collisionOccured)
										hardDrop = false;
								} while (hardDrop);
							}
						}
						else if (timer > timestep)	// for people
						{
							if(displayModeOn && hardDrop)	// if person turned on display mode & hard drop was pressed
							{
								collisionOccured = !tetromino->moveDown();	// decrease timestep to 0.01s and wait for the collision to occur
								if (collisionOccured)
								{
									displayModeDrawingFall = false;
									hardDrop = false;
								}
							}
							else	// else do instant hard drop
							{
								do
								{
									collisionOccured = !tetromino->moveDown();	// Method returns true if piece is successfully moved down
									if (collisionOccured)
										hardDrop = false;
								} while (hardDrop);
							}
							timer = 0;
						}

						if (collisionOccured)
						{
							std::swap(tetromino, nextTetromino);
							nextTetromino->makeNewNextPiece(tetromino->getCurrentTetrominoIndex());	// This is made to ensure that there are no two same pieces in the row

							// Checking for completed lines
							linesCleared = gameFieldManager.checkForFullLines();
						}
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

        if(dontDraw) continue;

		// Display current score and instructions
		scoreLabel.setString("Score: " + std::to_string(currentPoints));
		window.draw(scoreLabel);
		controlsLabels.setString("R - Restart\nP - Pause");
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

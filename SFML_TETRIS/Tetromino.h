#pragma once
#include "Structures.h"
#include "Generator.h"

class Tetromino
{
private:
	int tetrominoTileSide = 4;
	const int tetrominoes[7][4 * 4] =
	{
		{
			0,0,1,0,
			0,0,1,0,		// I
			0,0,1,0,
			0,0,1,0
		},
		{
			0,0,0,0,
			0,1,1,0,		// L
			0,0,1,0,
			0,0,1,0	
		},
		{
			0,0,0,0,
			0,1,1,0,		// inverted L aka J
			0,1,0,0,
			0,1,0,0
		},
		{
			0,0,0,0,
			0,1,0,0,		// S
			0,1,1,0,
			0,0,1,0
		},
		{
			0,0,0,0,
			0,0,1,0,		// Z
			0,1,1,0,
			0,1,0,0
		},
		{
			0,0,0,0,
			0,0,1,0,		// T
			0,1,1,0,
			0,0,1,0
		},
		{
			0,0,0,0,
			0,0,0,0,		// Square aka O
			0,1,1,0,
			0,1,1,0
		}
	};
	Point coordinatesInTile[4];
	Generator* generator;
	int currentTetrominoIndex;
	int widthWithBorder;
	int* gameField;
	Point* offset;

public:
	Tetromino();
	Tetromino(int index);
	Tetromino(Point* offset, int* gameField, int widthWithBorder);
	Tetromino(int index, Point* offset, int* gameField, int widthWithBorder);
	~Tetromino();
	void setCurrentPiece(int index);
	void makeNewPiece();
	void makeNewNextPiece(int currentTetrominoIndex);
	void rotate();
	void moveLeft();
	void moveRight();
	bool moveDown();

	Point* getCoordinatesInTile();
	int getCurrentTetrominoIndex();
	int getTetrominoTileSide();
	void setWidthWithBorder(int widthWithBorder);
	void setGameField(int* gameField);
	void setOffset(Point* offset);
};


#include "Tetromino.h"

Tetromino::Tetromino()
{
	generator = new Generator(0, 6);
	setCurrentPiece(generator->generate());
}

Tetromino::Tetromino(int index)
{
	generator = new Generator(0, 6);
	setCurrentPiece(index);
}

Tetromino::Tetromino(Point* offset, int* gameField, int widthWithBorder)
{
	generator = new Generator(0, 6);
	this->offset = offset;
	this->gameField = gameField;
	this->widthWithBorder = widthWithBorder;
	setCurrentPiece(generator->generate());
}

Tetromino::Tetromino(int index, Point* offset, int* gameField, int widthWithBorder)
{
	generator = new Generator(0, 6);
	this->offset = offset;
	this->gameField = gameField;
	this->widthWithBorder = widthWithBorder;
	setCurrentPiece(index);
}

Tetromino::~Tetromino()
{
	delete generator;
}

void Tetromino::setCurrentPiece(int index)
{
	currentTetrominoIndex = index;
	int coordinateIndex = 0;
	for (int i = 0; i < tetrominoTileSide; i++)
	{
		for (int j = 0; j < tetrominoTileSide; j++)
		{
			if (tetrominoes[index][i * tetrominoTileSide + j] == 1)	// Save information about points that create tetromino within a tile (only those with '1')
			{
				coordinatesInTile[coordinateIndex].x = j;
				coordinatesInTile[coordinateIndex].y = i;
				coordinateIndex++;
			}
		}
	}
}

void Tetromino::makeNewPiece()
{
	offset->x = 4;	// Global offset of the tetromino tile
	offset->y = 0;
	int newTetrominoIndex = generator->generate();
	if (newTetrominoIndex == currentTetrominoIndex)
	{
		newTetrominoIndex = (newTetrominoIndex + 6) % 7;
	}
	setCurrentPiece(newTetrominoIndex);
	currentTetrominoIndex = newTetrominoIndex;
}

void Tetromino::rotate()
{
	Point pivot = coordinatesInTile[2];		// Pivot point which te tetromino will be rotated around
	Point temp;
	Point rotatedCoordinates[4];
	for (int i = 0; i < tetrominoTileSide; i++)		// Calculation of rotated points
	{
		temp.x = -(coordinatesInTile[i].y - pivot.y);
		temp.y = coordinatesInTile[i].x - pivot.x;
		rotatedCoordinates[i].x = temp.x + pivot.x;
		rotatedCoordinates[i].y = temp.y + pivot.y;
	}

	// Collision detection for rotated piece
	int controlSum = 0;
	for (int i = 0; i < tetrominoTileSide; i++)
	{
		if (gameField[(rotatedCoordinates[i].y + offset->y) * widthWithBorder + (rotatedCoordinates[i].x + offset->x)] != 8)	// 8 represents empty space in game field
			controlSum++;
	}
	if (controlSum == 0)	// If no collision was detected, write rotated coordinates as current coordinates
	{
		for (int i = 0; i < tetrominoTileSide; i++)
		{
			coordinatesInTile[i].x = rotatedCoordinates[i].x;
			coordinatesInTile[i].y = rotatedCoordinates[i].y;
		}
	}
}

void Tetromino::moveLeft()
{
	int controlSum = 0;
	Point globalCoordinates;
	for (int i = 0; i < tetrominoTileSide; i++)
	{
		globalCoordinates.x = coordinatesInTile[i].x + offset->x - 1;
		globalCoordinates.y = coordinatesInTile[i].y + offset->y;
		// Check globally whether any of the blocks would have caused a collision if they were to be moved to the left
		if (gameField[globalCoordinates.y * widthWithBorder + globalCoordinates.x] != 8)
			controlSum++;
	}
	if (controlSum == 0)	// If no collsion has been detected then move piece to the left
	{
		offset->x--;
	}
}

void Tetromino::moveRight()
{
	int controlSum = 0;
	Point globalCoordinates;
	for (int i = 0; i < tetrominoTileSide; i++)
	{
		globalCoordinates.x = coordinatesInTile[i].x + offset->x + 1;
		globalCoordinates.y = coordinatesInTile[i].y + offset->y;
		// Check globally whether any of the blocks would have caused a collision if they were to be moved to the right
		if (gameField[globalCoordinates.y * widthWithBorder + globalCoordinates.x] != 8)
			controlSum++;
	}
	if (controlSum == 0)	// If no collsion has been detected then move piece to the right
	{
		offset->x++;
	}
}

bool Tetromino::moveDown()
{
	int controlSum = 0;
	bool collisionOccured;
	Point globalCoordinates;
	for (int i = 0; i < tetrominoTileSide; i++)
	{
		globalCoordinates.x = coordinatesInTile[i].x + offset->x;
		globalCoordinates.y = coordinatesInTile[i].y + offset->y + 1;
		if (gameField[globalCoordinates.y * widthWithBorder + globalCoordinates.x] != 8)	// If any collision has been detected, increment control sum
			controlSum++;
	}
	if (controlSum == 0)	// If no collision has occured, move the piece down by one row
	{
		offset->y++;
		return true;
	}
	else	// If collision occured it means the piece needs to be placed in the game field
	{
		// Save current piece on current position to game field
		for (int i = 0; i < tetrominoTileSide; i++)
			gameField[(offset->y + coordinatesInTile[i].y) * widthWithBorder + (offset->x + coordinatesInTile[i].x)] = currentTetrominoIndex;
		return false;
	}
}

Point* Tetromino::getCoordinatesInTile()
{
	return coordinatesInTile;
}

int Tetromino::getCurrentTetrominoIndex()
{
	return currentTetrominoIndex;
}

void Tetromino::setCurrentTetrominoIndex(int index)
{
	this->currentTetrominoIndex = index;
}

int Tetromino::getTetrominoTileSide()
{
	return tetrominoTileSide;
}

void Tetromino::setWidthWithBorder(int widthWithBorder)
{
	this->widthWithBorder = widthWithBorder;
}

void Tetromino::setGameField(int* gameField)
{
	this->gameField = gameField;
}

void Tetromino::setOffset(Point* offset)
{
	this->offset = offset;
}

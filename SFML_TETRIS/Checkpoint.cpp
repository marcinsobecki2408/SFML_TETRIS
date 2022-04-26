#include "Checkpoint.h"
#include <iostream>

Checkpoint::Checkpoint(Point* offset, int* gameField, int gameFieldSize, int tetrominoIndex, int nextTetrominoIndex) :
	gameFieldSize(gameFieldSize),
	tetrominoIndex(tetrominoIndex),
	nextTetrominoIndex(nextTetrominoIndex),
	currentPoints(0)
{
	this->offset.x = offset->x;
	this->offset.y = offset->y;
	this->gameField = new int[gameFieldSize];
	for (int i = 0; i < gameFieldSize; i++) 
	{
		this->gameField[i] = gameField[i];
	}
}

Checkpoint::~Checkpoint()
{
	delete[] gameField;
}

void Checkpoint::save(Point* offset, int* gameField, const int& tetrominoIndex, const int& nextTetrominoIndex, const int& currentPoints)
{
	this->tetrominoIndex = tetrominoIndex;
	this->nextTetrominoIndex = nextTetrominoIndex;
	this->offset.x = offset->x;
	this->offset.y = offset->y;
	this->currentPoints = currentPoints;
	for (int i = 0; i < gameFieldSize; i++)
	{
		this->gameField[i] = gameField[i];
	}
}

void Checkpoint::restore(Point* offset, int* gameField, Tetromino* tetromino, Tetromino* nextTetromino, int& currentPoints)
{
	offset->x = this->offset.x;
	offset->y = this->offset.y;
	tetromino->setCurrentPiece(this->tetrominoIndex);
	nextTetromino->setCurrentPiece(this->nextTetrominoIndex);
	currentPoints = this->currentPoints;
	for (int i = 0; i < gameFieldSize; i++)
	{
		gameField[i] = this->gameField[i];
	}
}

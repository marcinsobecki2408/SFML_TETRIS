#ifndef CHECKPOINT_H
#define CHECKPOINT_H
#include "Point.h"
#include "Tetromino.h"


class Checkpoint
{
private:
	Point offset;
	int* gameField;
	int gameFieldSize;
	int tetrominoIndex;
	int nextTetrominoIndex;
	int currentPoints;

public:
	Checkpoint(Point* offset, int* gameField, int gameFieldSize, int tetrominoIndex, int nextTetrominoIndex);
	~Checkpoint();
	void save(Point* offset, int* gameField, const int& tetrominoIndex, const int& nextTetrominoIndex, const int& currentPoints);
	void restore(Point* offset, int* gameField, Tetromino* tetromino, Tetromino* nextTetromino, int& currentPoints);
};

#endif

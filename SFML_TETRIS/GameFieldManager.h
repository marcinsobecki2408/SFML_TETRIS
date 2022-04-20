#pragma once
#include "Structures.h"

class GameFieldManager
{
private:
	int heightWithBorder;
	int widthWithBorder;
	int* gameField;
	Point* offset = nullptr;

public:
	GameFieldManager(int widthWithBorder, int heightWithBorder, Point* offset);
	~GameFieldManager();
	void setOffset(Point* offset);
	int* getGameField();
	bool checkIfGameOver();
	void eraseLine(int rowIndex);
	int checkForFullLines();
	void clearGameField();
};


#pragma once
#include "Structures.h"

#include <iostream>
#include <string>
#include <array>

class GameFieldManager
{
private:
	const int heightWithBorder;
	const int widthWithBorder;
	int* gameField;
	Point* offset = nullptr;

public:
	GameFieldManager(int widthWithBorder, int heightWithBorder, Point* offset);
	~GameFieldManager();
	void setOffset(Point* offset);
	int* getGameField();
    void createGameFieldExport(char* buf);
	bool checkIfGameOver();
	void eraseLine(int rowIndex);
	int checkForFullLines();
	void clearGameField();
};


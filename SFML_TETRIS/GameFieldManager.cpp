#include "GameFieldManager.h"
#include <iostream>

GameFieldManager::GameFieldManager(int widthWithBorder, int heightWithBorder, Point* offset)
{
	this->widthWithBorder = widthWithBorder;
	this->heightWithBorder = heightWithBorder;
	this->offset = offset;
	gameField = new int[heightWithBorder * widthWithBorder];
	for (int i = 0; i < heightWithBorder; i++)
		for (int j = 0; j < widthWithBorder; j++)
			gameField[widthWithBorder * i + j] = (j == 0 || j == widthWithBorder - 1 || i == heightWithBorder - 1) ? 9 : 8;	
}

GameFieldManager::~GameFieldManager()
{
	delete[] gameField;
}

void GameFieldManager::setOffset(Point* offset)
{
	this->offset = offset;
}

int* GameFieldManager::getGameField()
{
	return gameField;
}

bool GameFieldManager::checkIfGameOver()
{
	for (int i = 1; i < widthWithBorder - 1; i++)
		if (gameField[3 * widthWithBorder + i] != 8)	// row of index 3 is the first row off screen - so if there is any block in that row the game is over
			return true;
	return false;
}

void GameFieldManager::eraseLine(int rowIndex)
{
	for (int j = rowIndex; j > 0; j--)
	{
		for (int i = 1; i < widthWithBorder - 1; i++)
		{
			gameField[j * widthWithBorder + i] = gameField[(j - 1) * widthWithBorder + i];
		}
	}
}

int GameFieldManager::checkForFullLines()
{
	int linesCleared = 0;
	for (int j = 0; j < heightWithBorder - 1; j++)
	{
		int controlSum = 0;
		for (int i = 1; i < widthWithBorder - 1; i++)
		{
			if (gameField[j * widthWithBorder + i] == 8)
				controlSum++;
		}
		if (controlSum == 0)
		{
			linesCleared++;
			eraseLine(j);
		}
	}
	return linesCleared;
}

void GameFieldManager::clearGameField()
{
	for (int i = 0; i < widthWithBorder; i++)
		for (int j = 0; j < heightWithBorder; j++)
			gameField[widthWithBorder * j + i] = (i == 0 || i == widthWithBorder - 1 || j == heightWithBorder - 1) ? 9 : 8;
	offset->x = 4;
	offset->y = 0;
}

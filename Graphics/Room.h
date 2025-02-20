#pragma once
#include "definitions.h"
class Room
{
private:
	int centerX, centerY;
	int width, height;
public:
	Room();
	Room(int x, int y, int w, int h, int board[MSZ][MSZ]);
//	void show(int board[MSZ][MSZ]);
	void setCenter(int x, int y) { centerX = x; centerY = y; }
	void setWidth(int w) { width = w; }
	void setHeight(int h) { height = h; }
	int getCenterX() { return centerX; }
	int getCenterY() { return centerY; }
	int getWidth() { return width; }
	int getHeight() { return height; }
	bool Overlap(int cx, int cy, int w, int h);

};


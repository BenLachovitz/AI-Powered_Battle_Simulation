#include "Room.h"
#include "glut.h"
#include <iostream>

using namespace std;

Room::Room()
{
	centerX = 0;
	centerY = 0;
	width = 0;
	height = 0;
}

Room::Room(int x, int y, int w, int h, int board[MSZ][MSZ], int roomID)
{
	int i, j;
	centerX = x;
	centerY = y;
	width = w;
	height = h;

	for (i = centerY - height / 2;i <= centerY + height / 2;i++)
		for (j = centerX - width / 2;j <= centerX + width / 2;j++)
			board[i][j] = roomID;
	cout << roomID << endl;

	int numObstacles = rand() % 9 + 2;

	// Place obstacles at random locations within the room boundaries
	for (int n = 0; n < numObstacles; n++) {
		int obstacleY = rand() % (height - 2) + (centerY - height / 2 + 1);
		int obstacleX = rand() % (width - 2) + (centerX - width / 2 + 1);
		board[obstacleY][obstacleX] = WALL;
	}

}
bool Room::Overlap(int cx, int cy, int w, int h)
{
	int xOffset = abs(cx - centerX);
	int yOffset = abs(cy - centerY);
	return xOffset<width/2+w/2+5 && yOffset < height / 2 + h / 2 + 5;
}
#pragma once
#include "definitions.h"
#include <vector>
using namespace std;

class Bullet
{
private: 
	double r, g, b;
	double x, y;
	double dir_angle;
	double dirX, dirY;
	double speed;
	bool isMoving;
	int rivalColor;

	bool isVisible;
public:
	Bullet(double xx, double yy, double angle, double red, double green, double blue, int rival);
	pair<double, double> move(int maze[MSZ][MSZ]);
	void show();
	void setIsMoving(bool value) { isMoving = value; }
	void setX(double xx) { x = xx; }
	void setY(double yy) { y = yy; }
	void SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ]);
	bool getIsMoving() { return isMoving; }
	bool getIsVisible() { return isVisible; }
	void moveForSecurityMap(int maze[MSZ][MSZ]);
	void setIsVisible(bool vis) { isVisible = vis; }
};


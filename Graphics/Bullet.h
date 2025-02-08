#pragma once
#include "definitions.h"
class Bullet
{
private: 
	double r, g, b;
	double x, y;
	double dir_angle;
	double dirX, dirY;
	double speed;
	bool isMoving;
public:
	Bullet(double xx, double yy, double angle, double red, double green, double blue);
	void move(int maze[MSZ][MSZ]);
	void show();
	void setIsMoving(bool value) { isMoving = value; }
	void setX(double xx) { x = xx; }
	void setY(double yy) { y = yy; }
	void SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ]);
};


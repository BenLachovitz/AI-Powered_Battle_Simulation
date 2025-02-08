#pragma once
#include "Bullet.h"


const int NUM_BULLET = 20;

class Granade
{
private:
	double x, y;
	double dir_angle;
	double dirX, dirY;
	double speed;
	bool isMoving;
	Bullet* bullets[NUM_BULLET];
	bool exploding;
	int movingDelay;

public:
	Granade(double r, double c, double angle);
	void setX(double xx) { x = xx; }
	void setY(double yy) { y = yy; }
	void move(int maze[MSZ][MSZ]);
	void explode();
	void expend(int maze[MSZ][MSZ]);
	void show();
	void SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ]);
	bool getIsMoving() { return isMoving; }
	bool getIsExploding() { return exploding; }
	void setIsMoving(bool val) { isMoving = val; }
	double getX() { return x; }
	double getY() { return y; }
};


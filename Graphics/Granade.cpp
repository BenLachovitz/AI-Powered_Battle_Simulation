#include "Granade.h"
#include <math.h>
#include "glut.h"


Granade::Granade(double r, double c, double angle)
{
	x = c;
	y = r;
	dir_angle = angle;
	dirX = cos(angle);
	dirY = sin(angle);
	speed = 0.1;
	isMoving = false;
	exploding = false;
	movingDelay = 0;
	int i;
	double alpha = 2 * PI / NUM_BULLET;
	for (i = 0; i < NUM_BULLET; i++) {
		bullets[i] = new Bullet(c, r, i * alpha,0,0,0);
	}
}

void Granade::move(int maze[MSZ][MSZ])
{
	if (isMoving)
	{
		x += speed * dirX;
		y += speed * dirY;
		for (int i = 0; i < NUM_BULLET; i++) {
			bullets[i]->setX(x);
			bullets[i]->setY(y);
		}
		if (movingDelay >= 50 || maze[(int)y][(int)x] != SPACE)
			isMoving = false;
	}
	movingDelay++;
}

void Granade::explode()
{
	for (int i = 0; i < NUM_BULLET; i++) {
		bullets[i]->setIsMoving(true);
	}
	exploding = true;
}

void Granade::expend(int maze[MSZ][MSZ])
{
	for (int i = 0; i < NUM_BULLET; i++) {
		bullets[i]->move(maze);
	}
}

void Granade::show()
{
	if(exploding)
		for (int i = 0; i < NUM_BULLET; i++) {
			bullets[i]->show();
		}
	if (isMoving)
	{
		glColor3d(0.51, 0.502, 0.055);
		glBegin(GL_POLYGON);
		glVertex2d(x - 0.5, y);
		glVertex2d(x, y + 0.5);
		glVertex2d(x + 0.5, y);
		glVertex2d(x, y - 0.5);
		glEnd();
	}
}

void Granade::SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ])
{
	int i;

	for (i = 0; i < NUM_BULLET; i++) {
		bullets[i]->SimulateExplosion(maze, sm);
	}
}

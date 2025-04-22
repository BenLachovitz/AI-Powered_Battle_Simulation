#include "Bullet.h"
#include <math.h>
#include "glut.h"

//angle is in radians
Bullet::Bullet(double xx, double yy, double angle,double red,double green,double blue, int rival)
{
	x = xx;
	y = yy;
	dir_angle = angle;
	dirX = cos(angle);
	dirY = sin(angle);
	speed = 0.3;
	isMoving = false;
	r = red;
	g = green;
	b = blue;
	rivalColor = rival;
    isVisible = true;
}

pair<double, double> Bullet::move(int maze[MSZ][MSZ]) {
    if (isMoving) {
        // Store original position
        double origX = x;
        double origY = y;

        // Calculate new position
        double newX = x + speed * dirX;
        double newY = y + speed * dirY;

        // Check for collision along the path (ray casting)
        double checkSteps = max(abs(newX - origX), abs(newY - origY)) * 10; // 10x the distance for fine checking
        double stepX = (newX - origX) / checkSteps;
        double stepY = (newY - origY) / checkSteps;

        // Check each point along the path
        for (int step = 1; step <= checkSteps; step++) {
            double checkX = origX + step * stepX;
            double checkY = origY + step * stepY;

            int cellX = (int)checkX;
            int cellY = (int)checkY;

            // Check boundaries
            if (cellX < 0 || cellX >= MSZ || cellY < 0 || cellY >= MSZ)
                continue;

            // Check collision with walls or rivals
            if ((maze[cellY][cellX] != SPACE) &&
                (maze[cellY][cellX] != (((rivalColor + 1) % 2) + 2))) {
                isMoving = false;
                isVisible = false;

                // Update position to collision point
                x = checkX;
                y = checkY;

                if (maze[cellY][cellX] == rivalColor) {
                    return pair<double, double>(checkX, checkY);
                }

                return pair<double, double>(-1, -1);
            }
        }

        // If no collision, update position
        x = newX;
        y = newY;
    }

    return pair<double, double>(-1, -1);
}

void Bullet::show()
{
    if (isVisible) {
        glColor3d(r, g, b);
        glBegin(GL_POLYGON);
        glVertex2d(x - 0.5, y);
        glVertex2d(x, y + 0.5);
        glVertex2d(x + 0.5, y);
        glVertex2d(x, y - 0.5);
        glEnd();
    }
}

void Bullet::moveForSecurityMap(int maze[MSZ][MSZ])
{
    if (isMoving)
    {
        x += speed * dirX;
        y += speed * dirY;
        if ((maze[(int)y][(int)x] == WALL)|| (maze[(int)y][(int)x] == AMMO_WAREHOUSE)|| (maze[(int)y][(int)x] == MEDICAL_WAREHOUSE))
            isMoving = false;

    }
}

void Bullet::SimulateExplosion(int maze[MSZ][MSZ], double sm[MSZ][MSZ])
{
	isMoving = true;
	while (isMoving) {
		sm[(int)y][(int)x] += SECURITY_FACTOR;
        moveForSecurityMap(maze);
	}
}

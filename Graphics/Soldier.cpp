#include "Soldier.h"

Soldier::Soldier()
{
	x = 0;
	y = 0;
	pb = new Bullet(x,y, (rand() % 360) * PI / 180,1,1,0);
	pg = new Granade(y,x, (rand() % 360) * PI / 180);
	
}

Soldier::Soldier(double sx, double sy)
{
	x = sx;
	y = sy;
	pb = new Bullet(x, y, (rand() % 360) * PI / 180,1,1,0);
	pg = new Granade(y,x, (rand() % 360) * PI / 180);

}

void Soldier::shoot()
{
	pb->setX(x);
	pb->setY(y);
	bulletFired = true;
	pb->setIsMoving(true);
}

void Soldier::ThrowGrenade()
{
	grenadeThrown = true;
	pg->setIsMoving(true);
	pg->explode();
}

#pragma once
#include "definitions.h"
#include "Bullet.h"
#include "Granade.h"
#include <stdlib.h>

class Soldier
{
private:
	double x, y;
	int health = 100;
	int ammo = 100;
	int numOfGrenades = 100;
	bool bulletFired = false;
	bool grenadeThrown = false;
	Bullet* pb = nullptr;
	Granade* pg = nullptr;

public:
	Soldier();
	Soldier(double sx, double sy);
	void setX(double sx) { x = sx; }
	void sety(double sy) { y = sy; }
	void setHealth(int h) { health = h; }
	void setAmmo(int a) { ammo = a; }
	void setGrenade(int g) { numOfGrenades = g; }
	void setGT(Granade* gt) { pg = gt; }
	void setBF(Bullet* bf) { pb = bf; }
	void setBulletFired(bool fired) { bulletFired = fired; }
	void setGrenadeThrown(bool thrown) { grenadeThrown = thrown; }
	void shoot();
	void ThrowGrenade();
	double getX() { return x; }
	double getY() { return y; }
	int getHealth(){ return health; }
	int getAmmo(){ return ammo; }
	int getGrenade(){ return numOfGrenades; }
	Granade* getGT() { return pg; }
	Bullet* getBF() { return pb; }
	bool getBulletFired() { return bulletFired; }
	bool getGrenadeThrown() { return grenadeThrown; }
};


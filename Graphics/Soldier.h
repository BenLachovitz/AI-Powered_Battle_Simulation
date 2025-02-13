#pragma once
#include "definitions.h"
#include "Bullet.h"
#include "Granade.h"
#include <stdlib.h>
#include "SoldierState.h"
#include "vector"
#include <queue>

using namespace std;

class SoldierState;

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
	SoldierState* pCurrentState;
	Soldier* target;
	bool isMoving;
	vector<pair<int, int>> soldierPath;
	int pathIndex = 0;
	int stepCounts;
	bool attacking;
	int roomID;
	int color;
	int lastStep;


public:
	Soldier();
	Soldier(double sx, double sy, int startRoom, int c);
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
	void setIsMoving(bool move) { isMoving = move; }
	bool getIsMoving() { return isMoving; }
	void MoveSoldier(int maze[MSZ][MSZ]);
	void FindPathToRival(int targetX, int targetY, int maze[MSZ][MSZ]);
	void setCurrentState(SoldierState* s) { pCurrentState = s; }
	SoldierState* getCurrentState() { return pCurrentState; }
	void setTarget(Soldier* s) { target = s; }
	Soldier* getTarget() { return target; }
	void setAttacking(bool attack) { attacking = attack; }
	bool getAttacking() { return attacking; }
	int getRoomID() { return roomID; }
};


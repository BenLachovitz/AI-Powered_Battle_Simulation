#pragma once
#include "definitions.h"
#include "Bullet.h"
#include "Granade.h"
#include <stdlib.h>
#include "SoldierState.h"
#include "Room.h"
#include "vector"
#include <queue>
#include <chrono>  
#include <ctime>   

using namespace std;

class Supporter;
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
	Soldier* teammate;
	bool isMoving;
	vector<pair<int, int>> soldierPath;
	Supporter* teamSupporter;
	int pathIndex = 0;
	int stepCounts;
	bool attacking;
	int roomID;
	int color;
	int lastStep;
	int backRoomX;
	int backRoomY;
	static const int PATH_DETECTION_RANGE = 5; // Detection radius for path crossing
	bool isRetreating;
	bool surviving;
	int roomToRun;
	double lastMoveTime;
	double lastShootTime;
	double moveInterval; // seconds
	double shootInterval; // seconds
	double securityMoveInterval;
	double lastSecurityMove;
	bool isDied;

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
	void setSupporter(Supporter* s) { teamSupporter = s; }
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
	Soldier* getTeammate() { return teammate; }
	void setTeammate(Soldier* s) { teammate = s; }
	void setAttacking(bool attack) { attacking = attack; }
	bool getAttacking() { return attacking; }
	int getRoomID() { return roomID; }
	void setRoomID(int r) { roomID = r; }
	int getColor() { return color; }
	bool isInPath() const { return roomID == -1; }
	bool isPathCrossing(Soldier* other) const;
	void handlePathCrossing(Soldier* other, int maze[MSZ][MSZ]);
	void startRetreat();
	void fight(int maze[MSZ][MSZ], double securityMap[MSZ][MSZ], Room* room);
	void setStepCounts(int c) { stepCounts = c; }
	double angleCalculation();
	void setSurviving(bool s) {surviving = s;}
	bool getSurviving() {return surviving;}
	void survive(int roomX, int roomY, int maze[MSZ][MSZ]);
	void clearPath() { soldierPath.clear(); pathIndex = 0; }
	bool needToSurvive();
	void setRoomToRun(int r) { roomToRun = r; }
	int getRoomToRun() { return roomToRun; }
	bool getIsRetreating() { return isRetreating; }
	// New method to check if next move is blocked
	void recalculatePathAroundObstacle(int maze[MSZ][MSZ]);
	bool isSoldierDead(int maze[MSZ][MSZ]);
	bool checkIfDead();
	bool getIsDied() { return isDied; }
	void handlePotentialDeadlock(int maze[MSZ][MSZ]);
	void generateSoldierSecurityMap(int maze[MSZ][MSZ], double securityMap[MSZ][MSZ], Room* room);
	void findSafestPlaceInRoom(int maze[MSZ][MSZ], double securityMap[MSZ][MSZ], Room* room);
	double angleSimulateCalculation(int checkX, int checkY);
};


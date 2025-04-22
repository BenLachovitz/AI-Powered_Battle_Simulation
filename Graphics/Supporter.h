#pragma once
#include "SupporterState.h"
#include "Soldier.h"
#include "definitions.h"
#include <chrono>  
#include <ctime>  
#include "glut.h"

class SupporterState;

class Supporter
{
private:
	double x, y;
	int ammoHolding = 200;
	int granadeHolding = 200;
	int medicalHolding = 200;
	int color;
	bool isFollowing;
	bool isRetreating;
	int stepCount;
	bool isHelping;
	Soldier* helpTarget;
	vector<pair<int, int>> supporterPath;
	int pathIndex;
	SupporterState* pCurrentState;
	bool needToGetSupplies;
	int warehouseToVisit; // AMMO_WAREHOUSE or MEDICAL_WAREHOUSE
	bool checkingSecondSoldier;
	double lastMoveTime;
	double lastHelpTime;
	double lastFillTime;
	double moveInterval; // seconds
	double helpInterval; // seconds
	double fillInterval;
	Soldier* secondHelpTarget;
	Soldier* teammates[];

public:
	static const int MIN_AMMO_REQUIRED = 40;
	static const int MIN_GRANADE_REQUIRED = 40;
	static const int MIN_MEDICAL_REQUIRED = 40;

	Supporter(double xx, double yy, int c);
	void setIsFollowing(bool follow) { isFollowing = follow; }
	bool getIsFollowing() { return isFollowing; }
	void setX(double sx) { x = sx; }
	void sety(double sy) { y = sy; }
	void setAmmo(int ah) { ammoHolding = ah; }
	void setGranade(int ah) { granadeHolding = ah; }
	void setMedical(int mh) { medicalHolding= mh; }
	void setStepCount(int c) { stepCount = 0; }
	void followTeammates(int cx, int cy, int maze[MSZ][MSZ]);
	void FindPathToTarget(int targetX, int targetY, int maze[MSZ][MSZ]);
	void setTeammates(Soldier* team[]) { teammates[0] = team[0]; teammates[1] = team[1]; }
	double getX() { return x; }
	double getY() { return y; }
	void setIsHelping(bool help) { isHelping = help; }
	bool getIsHelping() { return isHelping; }
	void setHelpingTarget(Soldier* helpI) { helpTarget = helpI; }
	Soldier* getHelpingTarget() { return helpTarget; }
	SupporterState* getCurrentState() { return pCurrentState; }
	void setCurrentState(SupporterState* newState) { pCurrentState = newState; }
	void moveToHelpTarget(int maze[MSZ][MSZ]);
	void clearSupporterPath() { supporterPath.clear(); pathIndex = 0; }
	// Method to recalculate path when blocked
	void recalculatePathAroundObstacle(int maze[MSZ][MSZ]);
	// Get the next intended position
	bool hasEnoughSupplies(Soldier* soldier);
	void fillSoldierSupplies(Soldier* soldier);
	pair<int, int> findClosestWarehouse(int warehouseType, int maze[MSZ][MSZ]);
	void moveToWarehouse(int maze[MSZ][MSZ]);
	bool isNearPosition(int targetX, int targetY, int distance = 1);
	// Method to check if any teammate needs help
	bool anyTeammateNeedsHelp();
	// Method to switch to the second soldier
	void switchToSecondSoldier();
};


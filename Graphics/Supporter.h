#pragma once
#include "SupporterState.h"
#include "definitions.h"
#include <vector>
#include "Soldier.h"

class SupporterState;

class Supporter
{
private:
	double x, y;
	int ammoHolding = 100;
	int medicalHolding = 100;
	int color;
	SupporterState* pCurrentState;
	bool isMoving = false;
	Soldier* helpTarget;
	bool targetNeedsHealth = false;
	vector<pair<int, int>> supporterPath;
	int pathIndex = 0;
	int stepCounts = 0;
	int lastStep;

public:
	Supporter(double xx, double yy, int c);
	void setX(double sx) { x = sx; }
	void sety(double sy) { y = sy; }
	void setAmmo(int ah) { ammoHolding = ah; }
	void setMedical(int mh) { medicalHolding= mh; }
	double getX() { return x; }
	double getY() { return y; }
	void setCurrentState(SupporterState* s) { pCurrentState = s; }
	SupporterState* getCurrentState() { return pCurrentState; }
	void setMoving(bool m) { isMoving = m; }
	bool getMoving() { return isMoving; }
	void setHelpTarget(Soldier* s, bool healthProb) { helpTarget = s; targetNeedsHealth = healthProb; }
	void MoveSupporter(int maze[MSZ][MSZ]);
	void FindPathToTarget(int targetX, int targetY, int maze[MSZ][MSZ]);
};


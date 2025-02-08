#pragma once
#include "definitions.h"
#include "Soldier.h"
#include "Supporter.h"

class Team
{
private:
	Soldier* soldiers[NUM_OF_SOLDIERS];
	Supporter* supporter;
public:
	Team();
	Team(Soldier* s1, Soldier* s2);
	void addSoldier(Soldier* s,int num);
	Soldier** getSoldiers() { return soldiers; }
	int getNumOfSoldiers() { return (sizeof(soldiers) / sizeof(Soldier)); }
	Soldier* getSoldier(int index) { return soldiers[index]; }
	void assignSupporter(Supporter* s) { supporter = s; }
	Supporter* getSupporter() { return supporter; }
};


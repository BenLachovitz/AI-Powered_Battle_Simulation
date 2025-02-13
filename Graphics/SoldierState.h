#pragma once
#include "Soldier.h"

class Soldier;

class SoldierState
{
public:
	virtual void OnEnter(Soldier* s) = 0;
	virtual void Transition(Soldier* s) = 0;
	virtual void OnExit(Soldier* s) = 0;

};


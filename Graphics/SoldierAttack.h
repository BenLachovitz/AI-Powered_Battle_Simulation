#pragma once
#include "SoldierState.h"

class SoldierAttack :
    public SoldierState

{
	void OnEnter(Soldier* s);
	void Transition(Soldier* s);
	void OnExit(Soldier* s);
};


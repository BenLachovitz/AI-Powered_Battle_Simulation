#pragma once
#include "SoldierState.h"
class SoldierSurvive :
    public SoldierState
{
public:
	void OnEnter(Soldier* s);
	void Transition(Soldier* s);
	void OnExit(Soldier* s);
};


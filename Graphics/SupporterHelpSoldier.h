#pragma once
#include "SupporterState.h"

class SupporterHelpSoldier :
    public SupporterState
{
	void OnEnter(Supporter* s);
	void Transition(Supporter* s);
	void OnExit(Supporter* s);
};


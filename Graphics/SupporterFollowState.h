#pragma once
#include "SupporterState.h"

class SupporterFollowState :
    public SupporterState
{

	void OnEnter(Supporter* s);
	void Transition(Supporter* s);
	void OnExit(Supporter* s);

};


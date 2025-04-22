#pragma once
#include "Supporter.h"

class Supporter;

class SupporterState
{

public:
	virtual void OnEnter(Supporter* s) = 0;
	virtual void Transition(Supporter* s) = 0;
	virtual void OnExit(Supporter* s) = 0;

};


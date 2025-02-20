#include "SoldierAttack.h"
#include "SoldierSearchRival.h"
#include "SoldierSurvive.h"

void SoldierAttack::OnEnter(Soldier* s)
{
	s->setAttacking(true);
	s->setStepCounts(0);
}

void SoldierAttack::Transition(Soldier* s)
{
	
	OnExit(s);
	if (s->needToSurvive()||s->getTeammate()->needToSurvive())
	{
		s->setCurrentState(new SoldierSurvive());
		s->getCurrentState()->OnEnter(s);
	}
	else {
		s->setCurrentState(new SoldierSearchRival());
		s->getCurrentState()->OnEnter(s);
	}
	
}

void SoldierAttack::OnExit(Soldier* s)
{
	s->setAttacking(false);
}

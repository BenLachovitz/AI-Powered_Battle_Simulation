#include "SoldierAttack.h"
#include "SoldierSearchRival.h"
#include "SoldierSurvive.h"

void SoldierAttack::OnEnter(Soldier* s)
{
	s->setAttacking(true);
	s->clearPath();
}

void SoldierAttack::Transition(Soldier* s)
{
	
	if (s->needToSurvive() || (!s->getTeammate()->getIsDied() && s->getTeammate()->needToSurvive() && s->getAttacking()))
	{
		s->setCurrentState(new SoldierSurvive());
		s->getCurrentState()->OnEnter(s);
	}
	else {
		s->setCurrentState(new SoldierSearchRival());
		s->getCurrentState()->OnEnter(s);
	}
	OnExit(s);
}

void SoldierAttack::OnExit(Soldier* s)
{
	s->setAttacking(false);
}

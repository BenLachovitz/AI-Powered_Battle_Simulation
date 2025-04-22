#include "SoldierSearchRival.h"
#include "SoldierAttack.h"

void SoldierSearchRival::OnEnter(Soldier* s)
{
	s->setIsMoving(true);
	s->clearPath();
	s->setStepCounts(0);
}

void SoldierSearchRival::Transition(Soldier* s)
{
	OnExit(s);
	s->setCurrentState(new SoldierAttack());
	s->getCurrentState()->OnEnter(s);
}

void SoldierSearchRival::OnExit(Soldier* s)
{
}

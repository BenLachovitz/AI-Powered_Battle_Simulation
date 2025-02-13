#include "SoldierSearchRival.h"
#include "SoldierAttack.h"

void SoldierSearchRival::OnEnter(Soldier* s)
{
	s->setIsMoving(true);
}

void SoldierSearchRival::Transition(Soldier* s)
{
	OnExit(s);
	s->setCurrentState(new SoldierAttack());
	s->getCurrentState()->OnEnter(s);
}

void SoldierSearchRival::OnExit(Soldier* s)
{
	s->setIsMoving(false);
}

#include "SoldierAttack.h"

void SoldierAttack::OnEnter(Soldier* s)
{
	s->setAttacking(true);
}

void SoldierAttack::Transition(Soldier* s)
{
	OnExit(s);
	//s->setCurrentState(new SoldierSearchRival());
	s->getCurrentState()->OnEnter(s);
}

void SoldierAttack::OnExit(Soldier* s)
{

}

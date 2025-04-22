#include "SoldierSurvive.h"
#include "SoldierSearchRival.h"

void SoldierSurvive::OnEnter(Soldier* s)
{
	s->setSurviving(true);
	s->setStepCounts(0);
	s->clearPath();
}

void SoldierSurvive::Transition(Soldier* s)
{
	OnExit(s);
	s->setCurrentState(new SoldierSearchRival());
	s->getCurrentState()->OnEnter(s);
}

void SoldierSurvive::OnExit(Soldier* s)
{
	s->setSurviving(false);
	s->setRoomToRun(-1);

}

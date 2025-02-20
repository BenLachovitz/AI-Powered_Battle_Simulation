#include "SupporterHelpSoldier.h"

void SupporterHelpSoldier::OnEnter(Supporter* s)
{
	s->setMoving(true);
}

void SupporterHelpSoldier::Transition(Supporter* s)
{
	OnExit(s);
}

void SupporterHelpSoldier::OnExit(Supporter* s)
{
	s->setMoving(false);
	s->setHelpTarget(nullptr, false);
}

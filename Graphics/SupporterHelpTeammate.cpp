#include "SupporterHelpTeammate.h"
#include "SupporterFollowState.h"

void SupporterHelpTeammate::OnEnter(Supporter* s)
{
	s->setIsHelping(true);
	s->clearSupporterPath();
	s->setStepCount(0);
}

void SupporterHelpTeammate::Transition(Supporter* s)
{
	OnExit(s);
	s->setCurrentState(new SupporterFollowState());
	s->getCurrentState()->OnEnter(s);
}

void SupporterHelpTeammate::OnExit(Supporter* s)
{
	s->setIsHelping(false);
}

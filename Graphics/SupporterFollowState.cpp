#include "SupporterFollowState.h"
#include "SupporterHelpTeammate.h"

void SupporterFollowState::OnEnter(Supporter* s)
{
	s->setIsFollowing(true);
	s->clearSupporterPath();
	s->setStepCount(0);
}

void SupporterFollowState::Transition(Supporter* s)
{
	OnExit(s);
	s->setCurrentState(new SupporterHelpTeammate());
	s->getCurrentState()->OnEnter(s);
}

void SupporterFollowState::OnExit(Supporter* s)
{
	s->setIsFollowing(false);
}

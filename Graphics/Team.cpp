#include "Team.h"

Team::Team()
{
	soldiers[0] = nullptr;
	soldiers[1] = nullptr;
}

Team::Team(Soldier* s1, Soldier* s2)
{
	soldiers[0] = s1;
	soldiers[1] = s2;
}

void Team::addSoldier(Soldier* s, int num)
{
	if (num >= 0 && num < sizeof(soldiers))
	{
		soldiers[num] = s;
	}
}



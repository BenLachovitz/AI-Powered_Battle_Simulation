#include "Team.h"
#include <iostream>

using namespace std;

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

void Team::survive(Room* rooms[NUM_ROOMS], int maze[MSZ][MSZ])
{
    // Check if either soldier is alive and needs to survive
    bool soldier0Alive = soldiers[0] != nullptr && !soldiers[0]->isSoldierDead(maze);
    bool soldier1Alive = soldiers[1] != nullptr && !soldiers[1]->isSoldierDead(maze);

    if (soldier0Alive && soldiers[0]->getSurviving() ||
        soldier1Alive && soldiers[1]->getSurviving())
    {
        int a = -2;
        // Choose room to run only if at least one soldier is alive
        if ((soldier0Alive && soldiers[0]->getRoomToRun() == -1) ||
            (soldier1Alive && soldiers[1]->getRoomToRun() == -1))
        {
            do {
                a = rand() % 12;
            } while (a == (soldier0Alive ? soldiers[0]->getRoomToRun() : -1));

            if (soldier0Alive)
                soldiers[0]->setRoomToRun(a);
            if (soldier1Alive)
                soldiers[1]->setRoomToRun(a);
        }

        // Use the first alive soldier's room to run
        a = soldier0Alive ? soldiers[0]->getRoomToRun() :
            (soldier1Alive ? soldiers[1]->getRoomToRun() : -1);

        if (a >= 0)
        {
            if (soldier0Alive) {
                soldiers[0]->survive(rooms[a]->getCenterX(), rooms[a]->getCenterY(), maze);
            }

            if (soldier1Alive) {
                soldiers[1]->survive(rooms[a]->getCenterX(), rooms[a]->getCenterY(), maze);
            }
        }
    }
}

void Team::setTeamates()
{
	soldiers[0]->setTeammate(soldiers[1]);
	soldiers[1]->setTeammate(soldiers[0]);
}



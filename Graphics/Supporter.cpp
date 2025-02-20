#include "Supporter.h"
#include "Cell.h"
#include "CompareCells.h"
#include <iostream>

Supporter::Supporter(double xx, double yy, int c)
{
	x = xx;
	y = yy;
	color = c;
}

void Supporter::MoveSupporter(int maze[MSZ][MSZ])
{
    if (!isMoving || helpTarget == nullptr) return;

    stepCounts++;
    if (stepCounts % 5 != 1) return;

    // Update path if needed
    if (supporterPath.empty() || pathIndex >= supporterPath.size()) {
        supporterPath.clear();
        FindPathToTarget(helpTarget->getX(), helpTarget->getY(), maze);
        pathIndex = 0;
    }

    // Move along path
    if (!supporterPath.empty()) {
        int temp = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = SPACE;

        // Normal forward movement
        if (pathIndex < supporterPath.size()) {
            x = supporterPath[pathIndex].first;
            y = supporterPath[pathIndex].second;
            pathIndex++;
        }

        lastStep = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = color;
    }
    if (x == helpTarget->getX() && y == helpTarget->getY())
    {
        int temp = 0;
        if (helpTarget->getAmmo() < 100)
        {
            temp = 100 - helpTarget->getAmmo();
            helpTarget->setAmmo(100);
            ammoHolding = 100 - temp;

        }
        //
        if (helpTarget->getGrenade() < 100)
        {
            temp = 100 - helpTarget->getGrenade();
            helpTarget->setGrenade(100);
            ammoHolding = 100 - temp;

        }
        //
        if (helpTarget->getHealth() < 100)
        {
            temp = 100 - helpTarget->getHealth();
            helpTarget->setHealth(100);
            medicalHolding = 100 - temp;

        }
        //

        helpTarget->getCurrentState()->Transition(helpTarget);
        helpTarget->getTeammate()->getCurrentState()->Transition(helpTarget->getTeammate());
        cout << "team: " << (maze[(int)y][(int)x] - 2)<<endl;
        isMoving = false;
    }
}

void Supporter::FindPathToTarget(int targetX, int targetY, int maze[MSZ][MSZ])
{

    // Continue with original A* implementation but using modified target
    priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
    bool visited[MSZ][MSZ] = { false };

    Cell* start = new Cell(y, x, targetY, targetX, 0, nullptr);
    pq.push(start);
    Cell* goal = nullptr;
    bool gotToTarget = false;

    while (!pq.empty() && !gotToTarget) {
        Cell* current = pq.top();
        pq.pop();
        int currentX = current->getCol();
        int currentY = current->getRow();

        if (!visited[currentY][currentX]) {
            visited[currentY][currentX] = true;

            if (currentX == targetX && currentY == targetY) {
                goal = current;
                gotToTarget = true;
            }

            for (int i = 0; i < 4 && !gotToTarget; i++) {
                int newX = currentX + dx[i];
                int newY = currentY + dy[i];

                if (newX >= 0 && newX < MSZ && newY >= 0 && newY < MSZ &&
                    !visited[newY][newX] && maze[newY][newX] != WALL) {
                    Cell* neighbor = new Cell(newY, newX, targetY, targetX, current->getG() + 1, current);
                    pq.push(neighbor);
                }
            }
        }
        else {
            delete current;
        }
    }

    if (goal != nullptr) {
        Cell* current = goal;
        while (current != nullptr) {
            supporterPath.push_back({ current->getCol(), current->getRow() });
            current = current->getParent();
        }
        reverse(supporterPath.begin(), supporterPath.end());
    }

    //clean memory
    while (!pq.empty()) {
        delete pq.top();
        pq.pop();
    }
    delete goal;

}

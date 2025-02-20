#include "Soldier.h"
#include "SoldierSearchRival.h"
#include "Cell.h"
#include "CompareCells.h"
#include <iostream>
#include "Supporter.h"
#include "SupporterHelpSoldier.h"



Soldier::Soldier(double sx, double sy,int startRoom,int c) : teamSupporter(nullptr)
{
	x = sx;
	y = sy;
	//pb = new Bullet(x, y, (rand() % 360) * PI / 180,1,1,0);
	//pg = new Granade(y,x, (rand() % 360) * PI / 180);
    pb = nullptr;
    pg = nullptr;
	isMoving = false;
	pCurrentState = new SoldierSearchRival();
	pCurrentState->OnEnter(this);
    stepCounts = 0;
    attacking = false;
    target = nullptr;
    roomID = -1;
    lastStep = startRoom;
    color = c;
    backRoomX = -1;
    backRoomY = -1;
    teammate = nullptr;
}

void Soldier::shoot()
{
    if (pb != nullptr)
    {
        pb->setX(x);
        pb->setY(y);
        bulletFired = true;
        pb->setIsMoving(true);
    }
}

void Soldier::ThrowGrenade()
{
    if (pg != nullptr)
    {
        grenadeThrown = true;
        pg->setIsMoving(true);
        pg->explode();
    }
}

void Soldier::startRetreat() {
    isRetreating = true;
    isMoving = true;

    // Start moving backwards in current path
    if (pathIndex > 0) {
        pathIndex -= 2; // Go back 2 steps to avoid immediate re-collision
    }
}

double Soldier::angleCalculation() {
    double dx = target->getX() - x;
    double dy = target->getY() - y;
    return atan2(dy, dx); // Get angle in radians pointing to target
}


bool Soldier::survive(int roomX, int roomY,int maze[MSZ][MSZ])
{
    if (!surviving) { return false; }

    stepCounts++;
    if (stepCounts % 10 != 1) return false;

    // Update path if needed
    if (soldierPath.empty() || pathIndex >= soldierPath.size()) {
        if (roomID == target->getRoomID())
        {
            soldierPath.clear();
            FindPathToRival(roomX, roomY, maze);
            pathIndex = 0;
        }
    }

    if (!soldierPath.empty())
    {
        int temp = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = SPACE;

        if (pathIndex < soldierPath.size()) {
            x = soldierPath[pathIndex].first;
            y = soldierPath[pathIndex].second;
            pathIndex++;
        }

        lastStep = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = color;
    }
    return true;
}

bool Soldier::needToSurvive()
{
    return ((health < 40) || (ammo == 0 && numOfGrenades == 0));
}

void Soldier::fight(int maze[MSZ][MSZ])
{
    if (!attacking) return;

    stepCounts++;
    if (stepCounts % 40 != 1) return;

    double angle = angleCalculation(); // Get angle in radians pointing to target

    if (rand() % 10 == 0)
    {
        if (numOfGrenades>0)
        {
            if (numOfGrenades == 100)
            {
                pg = new Granade(y, x, angle, target->getColor());
                ThrowGrenade();
                numOfGrenades--;
            }
            if(pg!=nullptr)
                if (!pg->getIsMoving())
                {
                    pg = new Granade(y, x, angle, target->getColor());
                    ThrowGrenade();
                    numOfGrenades--;
                }

        }
    }
    else if (ammo > 0)
    {
        if (ammo == 100)
        {
            pb = new Bullet(x, y, angle, 1, 1, 0, target->getColor());
            shoot();
            ammo--;
        }
        if(pb!=nullptr)
            if (!pb->getIsMoving())
            {
                pb = new Bullet(x, y, angle, 1, 1, 0, target->getColor());
                shoot();
                ammo--;
            }

    }

    if (needToSurvive()) {
        pCurrentState->Transition(this);
        teamSupporter->setHelpTarget(this, true);
        teamSupporter->setCurrentState(new SupporterHelpSoldier());
        teamSupporter->getCurrentState()->OnEnter(teamSupporter);
    }
    
    if (teammate->needToSurvive() || roomID != target->getRoomID())
    {
        pCurrentState->Transition(this);
    }
}

// In Soldier.cpp, implement the new methods
bool Soldier::isPathCrossing(Soldier* other) const {
    if (!other || roomID != -1) return false;

    // Check if soldiers are close to each other in a path
    double dx = x - other->getX();
    double dy = y - other->getY();
    double distance = sqrt(dx * dx + dy * dy);

    return distance <= PATH_DETECTION_RANGE;
}

void Soldier::handlePathCrossing(Soldier* other, int maze[MSZ][MSZ]) {
    if (surviving || other->getSurviving()) return;

    if (!isPathCrossing(other)) return;

    // Lower priority team (lower color number) should retreat
    if (color < other->getColor()) {
        startRetreat();
    }
}

void Soldier::MoveSoldier(int maze[MSZ][MSZ])
{
    if (!isMoving) return;

    stepCounts++;
    if (stepCounts % 10 != 1) return;

    // Check for path crossing before moving
    if (target && isInPath()) {
        handlePathCrossing(target, maze);
    }
		// Update path if needed
    if (!isRetreating && (soldierPath.empty() || pathIndex >= soldierPath.size())) {
        soldierPath.clear();
        FindPathToRival(target->getX(), target->getY(), maze);
        pathIndex = 0;
    }

	// Move along path
    if (!soldierPath.empty()) {
        int temp = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = SPACE;

        if (isRetreating) {
            // Move backwards along path
            if (pathIndex > 0) {
                pathIndex--;
                x = soldierPath[pathIndex].first;
                y = soldierPath[pathIndex].second;
            }
            else {
                // If we've backed up to the start of the path
                isRetreating = false;
            }
        }
        else {
            // Normal forward movement
            if (pathIndex < soldierPath.size()) {
                x = soldierPath[pathIndex].first;
                y = soldierPath[pathIndex].second;
                pathIndex++;
            }
        }

        lastStep = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = color;
    }
    //stepCounts = 0;
    if ((roomID == target->getRoomID()) && roomID != -1 && (maze[(int)y][(int)x] != maze[(int)target->getY()][(int)target->getX()])) {
        pCurrentState->Transition(this);
    }
}

void Soldier::FindPathToRival(int targetX, int targetY, int maze[MSZ][MSZ])
{

    //// Add a small random offset each ghost to evoid merging
    //// Add offset of +-2 cells to target position
    int offsetX = (rand() % 5) - 2;
    int offsetY = (rand() % 5) - 2;

    //// Apply offset to target position while keeping within maze bounds
    int modifiedTargetX = max(0, min(targetX + offsetX, MSZ - 1));
    int modifiedTargetY = max(0, min(targetY + offsetY, MSZ - 1));

    // Continue with original A* implementation but using modified target
    priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
    bool visited[MSZ][MSZ] = { false };

    Cell* start = new Cell(y, x, modifiedTargetY, modifiedTargetX, 0, nullptr);
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

            if (currentX == modifiedTargetX && currentY == modifiedTargetY) {
                goal = current;
                gotToTarget = true;
            }

            for (int i = 0; i < 4 && !gotToTarget; i++) {
                int newX = currentX + dx[i];
                int newY = currentY + dy[i];

                if (newX >= 0 && newX < MSZ && newY >= 0 && newY < MSZ &&
                    !visited[newY][newX] && maze[newY][newX] != WALL) {
                    Cell* neighbor = new Cell(newY, newX, modifiedTargetY, modifiedTargetX, current->getG() + 1, current);
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
            soldierPath.push_back({ current->getCol(), current->getRow() });
            current = current->getParent();
        }
        reverse(soldierPath.begin(), soldierPath.end());
    }

    //clean memory
    while (!pq.empty()) {
        delete pq.top();
        pq.pop();
    }
    delete goal;

}

void Soldier::nearbyRival()
{
    //BFS
    
}

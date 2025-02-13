#include "Soldier.h"
#include "SoldierSearchRival.h"
#include "Cell.h"
#include "CompareCells.h"

Soldier::Soldier(double sx, double sy)
{
	x = sx;
	y = sy;
	pb = new Bullet(x, y, (rand() % 360) * PI / 180,1,1,0);
	pg = new Granade(y,x, (rand() % 360) * PI / 180);
	isMoving = false;
	pCurrentState = new SoldierSearchRival();
	pCurrentState->OnEnter(this);
    stepCounts = 0;
}

void Soldier::shoot()
{
	pb->setX(x);
	pb->setY(y);
	bulletFired = true;
	pb->setIsMoving(true);
}

void Soldier::ThrowGrenade()
{
	grenadeThrown = true;
	pg->setIsMoving(true);
	pg->explode();
}

void Soldier::MoveSoldier(int maze[MSZ][MSZ])
{
    if (!isMoving)
        return;
	//add check for same room
    stepCounts++;
    if (stepCounts % 10 != 1)
        return;
		// Update path if needed
	if (soldierPath.empty() || pathIndex >= soldierPath.size()) {
		soldierPath.clear();
		FindPathToRival(target->getX(), target->getY(), maze);
		pathIndex = 0;
	}

	// Move along path
	if (pathIndex < soldierPath.size()) {
        int temp = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = SPACE;
		x = soldierPath[pathIndex].first;
		y = soldierPath[pathIndex].second;
		pathIndex++;
        maze[(int)y][(int)x] = temp;
	}
}

void Soldier::FindPathToRival(int targetX, int targetY, int maze[MSZ][MSZ])
{

    //// Add a small random offset each ghost to evoid merging
    //// Add offset of +-2 cells to target position
    //int offsetX = (rand() % 5) - 2;
    //int offsetY = (rand() % 5) - 2;

    //// Apply offset to target position while keeping within maze bounds
    //int modifiedTargetX = max(0, min(targetX + offsetX, MSZ - 1));
    //int modifiedTargetY = max(0, min(targetY + offsetY, MSZ - 1));

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

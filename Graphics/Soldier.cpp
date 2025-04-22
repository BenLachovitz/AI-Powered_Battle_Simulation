#include "Soldier.h"
#include "SoldierSearchRival.h"
#include "Cell.h"
#include "CompareCells.h"
#include <iostream>
#include "Supporter.h"
#include "glut.h"

Soldier::Soldier(double sx, double sy,int startRoom,int c)
{
	x = sx;
	y = sy;
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
    roomToRun = -1;
    lastMoveTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0; // Convert to seconds
    lastShootTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0; // Convert to seconds
    moveInterval = 0.1;
    shootInterval = 0.8;
    lastSecurityMove = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    securityMoveInterval = (rand() % 200 + 1) / 100.0 + 0.5;
    isDied = false;
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
        pathIndex -= 1; // Go back 2 steps to avoid immediate re-collision
    }
}

double Soldier::angleCalculation() {
    double dx = target->getX() - x;
    double dy = target->getY() - y;
    return atan2(dy, dx); // Get angle in radians pointing to target
}


void Soldier::survive(int roomX, int roomY,int maze[MSZ][MSZ])
{
    if (!surviving) return;
    double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if (currentTime - lastMoveTime < moveInterval) return;

    lastMoveTime = currentTime;
    // Update path if needed
    if (soldierPath.empty() || pathIndex >= soldierPath.size()) {
        if (roomID == target->getRoomID())
        {
            soldierPath.clear();
            if (roomID == roomToRun)
            {
                roomToRun = -1;
                teammate->setRoomToRun(-1);
            }
            FindPathToRival(roomX, roomY, maze);
            pathIndex = 0;
        }
    }

    if (pathIndex < soldierPath.size())
    {
        int temp = maze[(int)y][(int)x];
        if (lastStep != ((color + 1) % 2) + 2 || lastStep != ((color + 1) % 2) + 4)
            maze[(int)y][(int)x] = SPACE;
            // Check if next step is blocked
            if (maze[soldierPath[pathIndex].second][soldierPath[pathIndex].first] == SPACE) {
                x = soldierPath[pathIndex].first;
                y = soldierPath[pathIndex].second;
                pathIndex++;
                maze[(int)y][(int)x] = color;
            }
            else {
                x = soldierPath[pathIndex].first;
                y = soldierPath[pathIndex].second;
                pathIndex++;
            }
        lastStep = maze[(int)y][(int)x];
    }
}

bool Soldier::needToSurvive()
{
    return ((health < 40) || (ammo < 20 && numOfGrenades < 20));
}

double Soldier::angleSimulateCalculation(int checkX, int checkY) {
    double dx = x - checkX;
    double dy = y - checkY;
    return atan2(dy, dx); // Get angle in radians pointing to target
}

void Soldier::generateSoldierSecurityMap(int maze[MSZ][MSZ], double securityMap[MSZ][MSZ], Room* room)
{
    int numSimulations = 1000;
    int tempC1 = target->getX();
    int tempR1 = target->getY();
    int tempC2 = target->getTeammate()->getX();
    int tempR2 = target->getTeammate()->getY();
    int i;

    double angle1 = angleSimulateCalculation(tempC1, tempR1); // Get angle in radians pointing to target
    double angle2 = angleSimulateCalculation(tempC2, tempR2);
    for (i = 0; i <= 85; i++)
    {
        Granade* g = new Granade(tempR1, tempC1, angle1 + (i * PI / 180), -1);
        g->SimulateExplosion(maze, securityMap);
        g = new Granade(tempR2, tempC2, angle2 + (i * PI / 180), -1);
        g->SimulateExplosion(maze, securityMap);
    }
}

void Soldier::findSafestPlaceInRoom(int maze[MSZ][MSZ], double securityMap[MSZ][MSZ], Room* room)
{
    int maxDepth = 30;
    double temp = 1000;
    int resI = 0, resJ = 0;
    int startRoomX = room->getCenterX() - (room->getWidth() / 2);
    int endRoomX = room->getCenterX() + (room->getWidth() / 2);
    int startRoomY = room->getCenterY() - (room->getHeight() / 2); //divide by 2
    int endRoomY = room->getCenterY() + (room->getHeight() / 2);

    int startCheckX = x - maxDepth, startCheckY = y - maxDepth, endCheckX = x + maxDepth, endCheckY = y + maxDepth;
    if (startCheckX < startRoomX)
        startCheckX = startRoomX + 1;
    if (startCheckY < startRoomY)
        startCheckY = startRoomY + 1;
    if (endCheckX > endRoomX)
        endCheckX = endRoomX - 1;
    if (endCheckY > endRoomY)
        endCheckY = endRoomY - 1;

    int middleRivalsX = (target->getX() + target->getTeammate()->getX()) / 2;
    int middleRivalsY = (target->getY() + target->getTeammate()->getY()) / 2;
    int standCount = 0;
    int sizeOfCheck = (abs(startCheckX - endCheckX)) * (abs(startCheckY - endCheckY));
    for (int j = startCheckY + 1; j <= endCheckY - 1; j++)
        for (int i = startCheckX + 1; i <= endCheckX - 1; i++)
        {
            if (maze[j][i] == SPACE)
            {
                double a = securityMap[j][i];
                int dx = i - target->getX();
                int dy = j - target->getY();
                double distTarget = sqrt(dx * dx + dy * dy);
                dx = i - target->getTeammate()->getX();
                dy = j - target->getTeammate()->getY();
                double distTargetTeammate = sqrt(dx * dx + dy * dy);
                if (distTarget > 15 && distTargetTeammate > 15)
                {
                    if ((securityMap[j][i] < temp) || rand() % 100 == 0)
                    {
                        temp = securityMap[j][i];
                        resI = i;
                        resJ = j;
                    }
                }
                else
                {
                    standCount++;
                }
                if (standCount >= sizeOfCheck/2)
                {
                    int randX = startRoomX + rand() % (endRoomX - startRoomX + 1);
                    int randY = startRoomY + rand() % (endRoomY - startRoomY + 1);
                    FindPathToRival(randX, randY, maze);
                    return;
                }
            }
        }
    FindPathToRival(resI, resJ, maze);
}

void Soldier::fight(int maze[MSZ][MSZ], double securityMap[MSZ][MSZ], Room* room)
{
    if (!attacking) return;

    double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if (currentTime - lastShootTime < shootInterval) return;

    lastShootTime = currentTime;

    //security map changes

    if (soldierPath.empty() || pathIndex >= soldierPath.size())
    {

        double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
        if (currentTime - lastSecurityMove >= securityMoveInterval)
        {
            lastSecurityMove = currentTime;
            soldierPath.clear();
            generateSoldierSecurityMap(maze, securityMap, room);
            pathIndex = 0;
            findSafestPlaceInRoom(maze, securityMap, room);
            isMoving = true;
        }
    }

    double angle = angleCalculation(); // Get angle in radians pointing to target

    if (rand() % 10 == 0)
    {
        if (numOfGrenades > 0)
        {
            if (numOfGrenades == 100)
            {
                pg = new Granade(y, x, angle, target->getColor());
                ThrowGrenade();
                numOfGrenades--;
            }
            if (pg != nullptr)
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
        if (pb != nullptr)
            if (!pb->getIsMoving())
            {
                pb = new Bullet(x, y, angle, 1, 1, 0, target->getColor());
                shoot();
                ammo--;
            }
    }

    if (needToSurvive()) {
        pCurrentState->Transition(this);
        teamSupporter->setHelpingTarget(this);
        if (!teamSupporter->getIsHelping()) {
            teamSupporter->getCurrentState()->Transition(teamSupporter);
        }
    }
    else if ((!teammate->getIsDied() && attacking && teammate->needToSurvive()) || roomID != target->getRoomID())
    {
        pCurrentState->Transition(this);
    }
}

bool Soldier::isPathCrossing(Soldier* other) const {
    if (!other || roomID != -1) return false;

    // Check if soldiers are close to each other in a path
    double dx = x - other->getX();
    double dy = y - other->getY();
    double distance = sqrt(dx * dx + dy * dy);

    return distance <= PATH_DETECTION_RANGE;
}

void Soldier::handlePathCrossing(Soldier* other, int maze[MSZ][MSZ]) {
    if (surviving || other->getSurviving() || other->getTeammate()->getSurviving()) return;

    if (!isPathCrossing(other) && !isPathCrossing(other->getTeammate())) return;

    // Lower priority team (lower color number) should retreat
    if (color < other->getColor() || color < other->getTeammate()->getColor()) {
        startRetreat();
    }
    else if(!target->isRetreating && !target->getTeammate()->isRetreating){
        target->startRetreat();
        target->getTeammate()->startRetreat();
    }
}

void Soldier::recalculatePathAroundObstacle(int maze[MSZ][MSZ]) {
    // Allocate a contiguous block of memory for the temporary maze
    int* tempMazeMemory = new int[MSZ * MSZ];
    int(*tempMaze)[MSZ] = reinterpret_cast<int(*)[MSZ]>(tempMazeMemory);

    // Copy the original maze to the temporary maze
    memcpy(tempMaze, maze, sizeof(int) * MSZ * MSZ);

    // Mark the blocked cell as a temporary wall
    if (!soldierPath.empty() && pathIndex < soldierPath.size()) {
        int blockedX = soldierPath[pathIndex].first;
        int blockedY = soldierPath[pathIndex].second;
        tempMaze[blockedY][blockedX] = WALL; // Temporarily treat as wall
        
        // new
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                int nx = blockedX + j;
                int ny = blockedY + i;
                if (nx >= 0 && nx < MSZ && ny >= 0 && ny < MSZ &&
                    tempMaze[ny][nx] != SPACE) {
                    tempMaze[ny][nx] = WALL;
                }
            }
        }
    }

    // Save the target coordinates before clearing the path
    int targetX, targetY;
    if (surviving) {
        // If in survive mode, use room coordinates as target
        targetX = backRoomX;
        targetY = backRoomY;
    }
    else if (target) {
        // Normal mode, use rival as target
        targetX = target->getX();
        targetY = target->getY();
    }
    else {
        delete[] tempMazeMemory;
        return; // No valid target
    }

    // Clear current path and calculate new one using the temporary maze
    soldierPath.clear();
    FindPathToRival(targetX, targetY, tempMaze);
    pathIndex = 0;

    // Free allocated memory
    delete[] tempMazeMemory;
}

bool Soldier::checkIfDead() {
    return (health <= 0) ? true : false;
}

bool Soldier::isSoldierDead(int maze[MSZ][MSZ])
{
    if (!checkIfDead())
        return false;

    isDied = true;
    isMoving = false;
    surviving = false;
    isRetreating = false;
    attacking = false;

    maze[(int)y][(int)x] = SPACE;
    x = -1;
    y = -1;
    return true;
}

void Soldier::MoveSoldier(int maze[MSZ][MSZ])
{
    if (isDied) return;
    if (!isMoving) return;

    double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if (currentTime - lastMoveTime < moveInterval) return;

    lastMoveTime = currentTime;

    // Update path if needed
    if ((!attacking || (attacking && roomID == -1)) && (soldierPath.empty() || pathIndex >= soldierPath.size())) {
        soldierPath.clear();
        if (color == TEAM_1_SOLDIER && roomID == -1 && (isPathCrossing(target) || isPathCrossing(target->getTeammate())))
                FindPathToRival(teamSupporter->getX(), teamSupporter->getY(), maze);
        else
            FindPathToRival(target->getX(), target->getY(), maze);
        pathIndex = 0;
    }

    // Check for path crossing before moving
    if (color == TEAM_1_SOLDIER && !teammate->getAttacking()) {
        if (target && isInPath() && (pathIndex > soldierPath.size() / 4 && soldierPath.size() > 15)) {
            handlePathCrossing(target, maze);
        }
        else
            isRetreating = false;
    }
    else
        isRetreating = false;

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
                // Check if next step is blocked
                if (maze[soldierPath[pathIndex].second][soldierPath[pathIndex].first] == SPACE) {
                    x = soldierPath[pathIndex].first;
                    y = soldierPath[pathIndex].second;
                    pathIndex++;
                }
                else {
                    // Path is blocked, recalculate
                    recalculatePathAroundObstacle(maze);

                    // Try to move after recalculation if possible
                    if (!soldierPath.empty() && pathIndex < soldierPath.size() &&
                        maze[soldierPath[pathIndex].second][soldierPath[pathIndex].first] == SPACE) {
                        x = soldierPath[pathIndex].first;
                        y = soldierPath[pathIndex].second;
                        pathIndex++;
                    }
                }
            }
        }

        lastStep = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = color;

        handlePotentialDeadlock(maze);
    }

    if ((roomID == target->getRoomID()) && roomID != -1 && (maze[(int)y][(int)x] != maze[(int)target->getY()][(int)target->getX()])) {
        if(!attacking)
            pCurrentState->Transition(this);
    }
}

void Soldier::handlePotentialDeadlock(int maze[MSZ][MSZ]) {
    static int stuckCounter = 0;
    static double lastX = x;
    static double lastY = y;

    // Check if position hasn't changed for several attempts
    if (lastX == x && lastY == y) {
        stuckCounter++;
        if (stuckCounter > 5) {  // If stuck for 5 consecutive moves
            // Take more drastic measures
            soldierPath.clear();

            // Try to move in a random direction
            int dx[] = { 1, -1, 0, 0 };
            int dy[] = { 0, 0, 1, -1 };

            for (int i = 0; i < 4; i++) {
                int randomIndex = rand() % 4;
                int newX = x + dx[randomIndex];
                int newY = y + dy[randomIndex];

                if (newX >= 0 && newX < MSZ && newY >= 0 && newY < MSZ &&
                    maze[newY][newX] == SPACE) {
                    maze[(int)y][(int)x] = SPACE;
                    x = newX;
                    y = newY;
                    maze[(int)y][(int)x] = color;
                    stuckCounter = 0;
                    break;
                }
            }

            // Recalculate path from new position
            FindPathToRival(target->getX(), target->getY(), maze);
            pathIndex = 0;
        }
    }
    else {
        stuckCounter = 0;
    }

    lastX = x;
    lastY = y;
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
                    !visited[newY][newX] && maze[newY][newX] != WALL && maze[newY][newX] != MEDICAL_WAREHOUSE
                    && maze[newY][newX] != AMMO_WAREHOUSE) {
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
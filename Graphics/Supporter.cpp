#include "Supporter.h"
#include "Cell.h"
#include "CompareCells.h"
#include "SupporterFollowState.h"
#include <iostream>

using namespace std;

Supporter::Supporter(double xx, double yy, int c)
{
	x = xx;
	y = yy;
	color = c;
	stepCount = 0;
    isRetreating = false;
    pathIndex = 0;
	pCurrentState = new SupporterFollowState();
	pCurrentState->OnEnter(this);
    checkingSecondSoldier = false;
    secondHelpTarget = nullptr;
    lastMoveTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0; // Convert to seconds
    lastHelpTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0; // Convert to seconds
    moveInterval = 0.08;
    helpInterval = 0.04;
    fillInterval = 0.34;
}

bool Supporter::anyTeammateNeedsHelp() {
    // Check if any teammate needs help (low health or ammo)
    for (int i = 0; i < 2; i++) {
        if (teammates[i] && teammates[i] != helpTarget) {
            if (!teammates[i]->getIsDied() && teammates[i]->needToSurvive()) {
                return true;
            }
        }
    }
    return false;
}

void Supporter::switchToSecondSoldier() {
    // Find the other teammate (not current helpTarget)
    for (int i = 0; i < 2; i++) {
        if (teammates[i] && teammates[i] != helpTarget) {
            secondHelpTarget = helpTarget;  // Save current help target as secondary
            helpTarget = teammates[i];      // Set new primary help target
            break;
        }
    }

    // Reset path finding to target the new soldier
    supporterPath.clear();
    pathIndex = 0;
    checkingSecondSoldier = true;

}

void Supporter::recalculatePathAroundObstacle(int maze[MSZ][MSZ]) {
    // Allocate a contiguous block of memory for the temporary maze
    int* tempMazeMemory = new int[MSZ * MSZ];
    int(*tempMaze)[MSZ] = reinterpret_cast<int(*)[MSZ]>(tempMazeMemory);

    // Copy the original maze to the temporary maze
    memcpy(tempMaze, maze, sizeof(int) * MSZ * MSZ);

    // Mark the blocked cell as a temporary wall
    if (!supporterPath.empty() && pathIndex < supporterPath.size()) {
        int blockedX = supporterPath[pathIndex].first;
        int blockedY = supporterPath[pathIndex].second;
        tempMaze[blockedY][blockedX] = WALL; // Temporarily treat as wall
    }

    // Save the target coordinates before clearing the path
    int targetX, targetY;
    if (isHelping && helpTarget) {
        targetX = helpTarget->getX();
        targetY = helpTarget->getY();
    }
    else if (isFollowing && teammates[0] && teammates[0]->getTarget()) {
        targetX = teammates[0]->getTarget()->getX();
        targetY = teammates[0]->getTarget()->getY();
    }
    else {
        delete[] tempMazeMemory;
        return; // No valid target
    }

    // Clear current path and calculate new one using the temporary maze
    supporterPath.clear();
    FindPathToTarget(targetX, targetY, tempMaze);
    pathIndex = 0;

    // Free allocated memory
    delete[] tempMazeMemory;
}

// Modify the followTeammates and moveToHelpTarget methods in Supporter.cpp similarly:
void Supporter::followTeammates(int cx, int cy, int maze[MSZ][MSZ])
{
    if (!isFollowing) return;

    double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

    if (currentTime - lastMoveTime < moveInterval) return;

    lastMoveTime = currentTime;

    if (teammates[0]->getIsRetreating() && teammates[1]->getIsRetreating())
        isRetreating = true;
    else
        isRetreating = false;

    if (!isRetreating && (supporterPath.empty() || pathIndex >= supporterPath.size())) {
        supporterPath.clear();
        FindPathToTarget(cx, cy, maze);
        pathIndex = 0;
    }

    // Move along path
    if (!supporterPath.empty()) {
        int temp = maze[(int)y][(int)x];
        maze[(int)y][(int)x] = SPACE;

        if (isRetreating) {
            // Move backwards along path
            if (pathIndex > 0) {
                x = supporterPath[pathIndex].first;
                y = supporterPath[pathIndex].second;
                pathIndex--;
            }
            else {
                // If we've backed up to the start of the path
                isRetreating = false;
            }
        }
        else {
            // Normal forward movement
            if (pathIndex < supporterPath.size()) {
                // Check if next step is blocked
                if (maze[supporterPath[pathIndex].second][supporterPath[pathIndex].first] == SPACE) {
                    x = supporterPath[pathIndex].first;
                    y = supporterPath[pathIndex].second;
                    pathIndex++;
                }
                else {
                    // Path is blocked, recalculate
                    recalculatePathAroundObstacle(maze);

                    // Try to move after recalculation if possible
                    if (!supporterPath.empty() && pathIndex < supporterPath.size() &&
                        maze[supporterPath[pathIndex].second][supporterPath[pathIndex].first] == SPACE) {
                        x = supporterPath[pathIndex].first;
                        y = supporterPath[pathIndex].second;
                        pathIndex++;
                    }
                }
            }
        }

        maze[(int)y][(int)x] = color;
    }
}

bool Supporter::hasEnoughSupplies(Soldier* soldier) {
    // Check if supporter has enough supplies based on soldier needs
    bool needAmmo = soldier->getAmmo() < 40; // Assuming threshold values
    bool needHealth = soldier->getHealth() < 40;
    bool needGranade = soldier->getGrenade() < 40;

    if ((needAmmo && ammoHolding < MIN_AMMO_REQUIRED) || (needGranade && granadeHolding < MIN_GRANADE_REQUIRED)) 
        return false;
    if (needHealth && medicalHolding < MIN_MEDICAL_REQUIRED) return false;

    return true;
}

void Supporter::fillSoldierSupplies(Soldier* soldier) {
    // Refill soldier's supplies
    bool needAmmo = soldier->getAmmo() < 40;
    bool needGranade = soldier->getGrenade() < 40;
    bool needHealth = soldier->getHealth() < 40;

        if (needAmmo || needGranade) {
            int ammoToGive = min(ammoHolding, 100 - soldier->getAmmo());
            int granadeToGive = min(granadeHolding, 100 - soldier->getGrenade());
            soldier->setAmmo(soldier->getAmmo() + ammoToGive);
            soldier->setGrenade(soldier->getGrenade() + granadeToGive);
            ammoHolding -= ammoToGive;
            granadeHolding -= granadeToGive;
        }

        if (needHealth) {
            int healthToGive = min(medicalHolding, 100 - soldier->getHealth());
            soldier->setHealth(soldier->getHealth() + healthToGive);
            medicalHolding -= healthToGive;
        }
}

bool Supporter::isNearPosition(int targetX, int targetY, int distance) {
    return (abs(x - targetX) <= distance && abs(y - targetY) <= distance);
}

pair<int, int> Supporter::findClosestWarehouse(int warehouseType, int maze[MSZ][MSZ]) {
    // BFS to find closest warehouse of specified type
    queue<pair<int, int>> q;
    bool visited[MSZ][MSZ] = { false };

    // Start from current position
    q.push({ x, y });
    visited[(int)y][(int)x] = true;

    // Direction arrays for 4-way movement
    int dx[] = { 1, -1, 0, 0 };
    int dy[] = { 0, 0, 1, -1 };

    while (!q.empty()) {
        auto currentPos = q.front();
        q.pop();

        int currentX = currentPos.first;
        int currentY = currentPos.second;

        // Check if current position is the warehouse we're looking for
        if (maze[currentY][currentX] == warehouseType) {
            return { currentX, currentY };
        }

        // Try all four directions
        for (int i = 0; i < 4; i++) {
            int newX = currentX + dx[i];
            int newY = currentY + dy[i];

            // Check if valid position and not visited
            if (newX >= 0 && newX < MSZ && newY >= 0 && newY < MSZ &&
                !visited[newY][newX] && maze[newY][newX] != WALL) {

                q.push({ newX, newY });
                visited[newY][newX] = true;
            }
        }
    }

    // No warehouse found, return invalid position
    return { -1, -1 };
}

void Supporter::moveToWarehouse(int maze[MSZ][MSZ]) {
    if (warehouseToVisit == -1) return;

    // Find closest warehouse if we don't have a path yet
    if (supporterPath.empty() || pathIndex >= supporterPath.size()) {
        auto warehousePos = findClosestWarehouse(warehouseToVisit, maze);

        if (warehousePos.first != -1) {
            supporterPath.clear();
            FindPathToTarget(warehousePos.first, warehousePos.second, maze);
            pathIndex = 0;
        }
        else {
            // No warehouse found, revert to helping soldier directly
            needToGetSupplies = false;
            return;
        }
    }

    // Move along path
    if (!supporterPath.empty()) {
        maze[(int)y][(int)x] = SPACE;

        // Normal forward movement
        if (pathIndex < supporterPath.size()) {
            // Check if next step is blocked
            if (maze[supporterPath[pathIndex].second][supporterPath[pathIndex].first] == SPACE) {
                x = supporterPath[pathIndex].first;
                y = supporterPath[pathIndex].second;
                pathIndex++;
            }
            else {
                // Path is blocked, recalculate
                recalculatePathAroundObstacle(maze);

                // Try to move after recalculation if possible
                if (!supporterPath.empty() && pathIndex < supporterPath.size() &&
                    maze[supporterPath[pathIndex].second][supporterPath[pathIndex].first] == SPACE) {
                    x = supporterPath[pathIndex].first;
                    y = supporterPath[pathIndex].second;
                    pathIndex++;
                }
            }
        }

        maze[(int)y][(int)x] = color;
        int currentCell = -1;
        // Check if we've reached the warehouse
        try {
            if (pathIndex < supporterPath.size())
                currentCell = maze[supporterPath.at(pathIndex).second][supporterPath.at(pathIndex).first];
            else
                currentCell = maze[(int)y][(int)x];
        }
        catch (const std::out_of_range& e) {
            cerr << "Out of range error: " << e.what() << std::endl;
            // Handle the error or debug here
            return; // Or other fallback logic
        }

        if (currentCell == warehouseToVisit) {
            // Refill supplies
            if (warehouseToVisit == AMMO_WAREHOUSE) {
                ammoHolding = MAX_AMMO;
                granadeHolding = MAX_GRANADE;
            }
            else if (warehouseToVisit == MEDICAL_WAREHOUSE) {
                medicalHolding = 100;
            }

            // Mark that we no longer need supplies
            needToGetSupplies = false;
            warehouseToVisit = -1;

            // Clear path to prepare for moving back to soldier
            supporterPath.clear();
            pathIndex = 0;
        }
    }
}

// Version 5.1
void Supporter::moveToHelpTarget(int maze[MSZ][MSZ]) {
    if (!isHelping || helpTarget == nullptr) return;

    double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
    if (currentTime - lastHelpTime < helpInterval) return;
    lastHelpTime = currentTime;

    // Check if current target soldier is still alive
    if (helpTarget->getIsDied()) {

        // Switch to the other soldier if they're alive and need help
        if (anyTeammateNeedsHelp()) {
            switchToSecondSoldier();

            // If after switching, we still don't have a valid help target
            if (helpTarget == nullptr || helpTarget->getIsDied()) {
                // No valid soldiers to help, return to follow state

                if (!teammates[0]->getIsDied())
                    teammates[0]->getCurrentState()->Transition(teammates[0]);

                if (!teammates[1]->getIsDied())
                    teammates[1]->getCurrentState()->Transition(teammates[1]);

                pCurrentState->Transition(this);
                return;
            }
        }
        else {
            // No soldiers need help or all are dead, return to follow state
            if (!teammates[0]->getIsDied() && teammates[0]->getSurviving())
                teammates[0]->getCurrentState()->Transition(teammates[0]);
            
            if (!teammates[1]->getIsDied() && teammates[1]->getSurviving())
                teammates[1]->getCurrentState()->Transition(teammates[1]);
            
            pCurrentState->Transition(this);
            return;
        }

        // Clear path for new target
        supporterPath.clear();
        pathIndex = 0;
        needToGetSupplies = false; // Reassess supply needs for new target
    }

    // First, check if we need to gather supplies
    if (!needToGetSupplies && !hasEnoughSupplies(helpTarget)) {
        needToGetSupplies = true;
        // Determine which warehouse to visit based on soldier needs
        bool needAmmo = helpTarget->getAmmo() < 40;
        bool needHealth = helpTarget->getHealth() < 40;
        bool needGranade = helpTarget->getGrenade() < 40;
        if ((needAmmo && ammoHolding < MIN_AMMO_REQUIRED) || needGranade && granadeHolding < MIN_GRANADE_REQUIRED) {
            warehouseToVisit = AMMO_WAREHOUSE;
        }
        else if (needHealth && medicalHolding < MIN_MEDICAL_REQUIRED) {
            warehouseToVisit = MEDICAL_WAREHOUSE;
        }
        // Clear path to prepare for warehouse path
        supporterPath.clear();
        pathIndex = 0;
    }

    // If we need supplies, go to warehouse first
    if (needToGetSupplies) {
        moveToWarehouse(maze);
        return;
    }

    // Normal movement to help target
    if (supporterPath.empty() || pathIndex >= supporterPath.size()) {
        supporterPath.clear();
        FindPathToTarget(helpTarget->getX(), helpTarget->getY(), maze);
        pathIndex = 0;
    }

    // Move along path
    if (!supporterPath.empty()) {
        maze[(int)y][(int)x] = SPACE;

        // Normal forward movement
        if (pathIndex < supporterPath.size()) {
            // Check if the target has died during our movement
            if (helpTarget->getIsDied()) {
                // Interrupt the current path
                supporterPath.clear();
                pathIndex = 0;

                // Recursively call moveToHelpTarget to handle this situation
                moveToHelpTarget(maze);
                return;
            }

            // Check if next step is blocked
            if (maze[supporterPath[pathIndex].second][supporterPath[pathIndex].first] == SPACE) {
                x = supporterPath[pathIndex].first;
                y = supporterPath[pathIndex].second;
                pathIndex++;
            }
            else {
                // Path is blocked, recalculate
                recalculatePathAroundObstacle(maze);

                // Try to move after recalculation if possible
                if (!supporterPath.empty() && pathIndex < supporterPath.size() &&
                    maze[supporterPath[pathIndex].second][supporterPath[pathIndex].first] == SPACE) {
                    x = supporterPath[pathIndex].first;
                    y = supporterPath[pathIndex].second;
                    pathIndex++;
                }
            }
        }

        maze[(int)y][(int)x] = color;

        // Check if we are near the soldier to provide supplies
        // First verify the soldier is still alive
        if (!helpTarget->getIsDied() && isNearPosition(helpTarget->getX(), helpTarget->getY(), 1)) {

            if (helpTarget->getRoomID() == -1)
                lastFillTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;

            double currentTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0;
            if (currentTime - lastFillTime < fillInterval) return;

            fillSoldierSupplies(helpTarget);

            // After helping the current soldier, check if the other soldier needs help
            if (!checkingSecondSoldier && anyTeammateNeedsHelp()) {
                switchToSecondSoldier();
            }
            else {
                // Either we've already checked both soldiers, or the second one doesn't need help
                // If we were checking the second soldier, reset the flag
                if (checkingSecondSoldier) {
                    checkingSecondSoldier = false;
                }

                // All soldiers have been helped - transition to the follow state
                pCurrentState->Transition(this);

                // Also inform soldiers they can resume fighting
                for (int i = 0; i < 2; i++) {
                    if (teammates[i] && !teammates[i]->getIsDied()) {
                        teammates[i]->getCurrentState()->Transition(teammates[i]);
                    }
                }
            }
        }
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

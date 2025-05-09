#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include "Cell.h"
#include <iostream>
#include "Room.h"
#include "CompareCells.h"
#include "Bullet.h"
#include "Granade.h"
#include "Team.h"
#include "Soldier.h"
#include "Supporter.h"
#include <set>

using namespace std;

const double WALL_COST = 5;
const double SPACE_COST = 1;


Room* rooms[NUM_ROOMS];

bool bulletFired = false;
bool grenadeThrown = false;
Bullet* pb=nullptr;
Granade* pg = nullptr;

Team* teams[2];

pair<double, double> rivalCollition;

int maze[MSZ][MSZ] = {0}; // WALLs

double securityMap[MSZ][MSZ] = {0.0}; // WALLs

int team1DeathCount = 0;
int team2DeathCount = 0;


void RestorePath(Cell* pc)
{
	int r, c;
	int backup_r = -1, backup_c = -1;
	while (pc != nullptr)
	{
		r = pc->getRow();
		c = pc->getCol();
		if (maze[r][c] == WALL || pc->getParent() == nullptr) {
			maze[r][c] = SPACE;
			if (backup_r != -1 && backup_c != -1) {
				if (c != backup_c)
				{
					maze[r + 1][c] = SPACE;
					maze[r + 1][backup_c] = SPACE;
				}
				if (r != backup_r)
				{
					maze[r][c + 1] = SPACE;
					maze[backup_r][c + 1] = SPACE;
				}
			}
		}
		backup_r = r, backup_c = c;
		pc = pc->getParent();
	}
}

// row, col are the indices of neighbor cell
void AddNeighbor(int r, int c, Cell* pCurrent,
	priority_queue<Cell*, vector<Cell*>, CompareCells>& pq,
	vector <Cell>& grays,  vector <Cell> &black) // blacks shouldn't be changed
{
	double newg,cost;
	vector<Cell>::iterator itGray;
	vector<Cell>::iterator itBlack;


	if (maze[r][c] == WALL) cost = WALL_COST;
	else cost = SPACE_COST;
	newg = pCurrent->getG() + cost;
	Cell* pNeighbor = new Cell(r, c, pCurrent->getTargetRow(), pCurrent->getTargetCol(),
		newg, pCurrent);
	// check what to do with the neighbor Cell
	// 1. if the neighbor is black: do nothing
	// 2. if the neighbor is white: add it to PQ and to grays
	// 3. if it is gray:
	// 3.1 if F of neighbor is not below the neighbor copy that is stored in PQ then do nothing
	// 3.2 otherwise then we must update the PQ and grays
	itGray = find(grays.begin(), grays.end(), *pNeighbor);
	itBlack = find(black.begin(), black.end(), *pNeighbor);

	if (itBlack == black.end()) // then it is not black
	{
		if (itGray == grays.end()) // then it is not gray => it is white
		{
			// paint it gray
			pq.push(pNeighbor);
			grays.push_back(*pNeighbor);
		}
		else // it is gray
		{
			if (pNeighbor->getF() < itGray->getF()) // then we found a better path and we have to exchange it
			{
				grays.erase(itGray);
				grays.push_back(*pNeighbor);

				// and do the same with PQ
				vector<Cell*> tmp;
				while (!pq.empty() &&  !((*pq.top()) == (*pNeighbor)))
				{
					tmp.push_back(pq.top()); // save the top element in tmp
					pq.pop(); // remove top element from pq
				}
				if (pq.empty()) // ERROR!
				{
					exit(2);
				}
				else // we have found the neighbor cell in PQ
				{
					pq.pop(); // remove old neighbor from pq
					pq.push(pNeighbor);
					// now restore pq
					while (!tmp.empty())
					{
						pq.push(tmp.back());
						tmp.pop_back();
					}
				}
			}
		}
	}
}

// run A* from room at index1 to room at index2
void BuildPath(int index1, int index2)
{
	int r, c, tr, tc;

	r = rooms[index1]->getCenterY();
	c = rooms[index1]->getCenterX();
	tr = rooms[index2]->getCenterY();
	tc = rooms[index2]->getCenterX();
	Cell* pCurrent;
	Cell* start = new Cell(r,c ,tr ,tc , 0, nullptr);
	priority_queue<Cell*, vector<Cell*>, CompareCells> pq;
	vector <Cell> grays;
	vector <Cell> black;
	vector<Cell>::iterator itGray;

	pq.push(start);
	grays.push_back(*start);
	// pq shouldn't be empty because we are going to reach the target beforehand
	while (!pq.empty()) 	
	{
		pCurrent = pq.top();
		if (pCurrent->getH() < 0.001) // this is a targt cell
		{
			RestorePath(pCurrent);
			return;
		}
		else // target hasn't been reached
		{
			// 1. remove pCurrent from pq 
			pq.pop();
			// 2. find and remove pCurrent from grays
			itGray = find(grays.begin(), grays.end(), *pCurrent);
			if (itGray == grays.end()) // pCurrent wasn't found
			{
				cout << "Error: pCurrent wasn't found in grays\n";
				exit(1);
			}
			grays.erase(itGray);
			// 3. paint pCurrent black
			black.push_back(*pCurrent);
			// 4. take care of neighbors
			r = pCurrent->getRow();
			c = pCurrent->getCol();
			// up
			if (r + 1 < MSZ)
				AddNeighbor(r + 1, c, pCurrent, pq, grays, black);
			// down
			if (r - 1 >=0)
				AddNeighbor(r - 1, c, pCurrent, pq, grays, black);
			// left
			if (c - 1 >= 0)
				AddNeighbor(r , c-1, pCurrent, pq, grays, black);
			// right
			if (c + 1 < MSZ)
				AddNeighbor(r, c + 1, pCurrent, pq, grays, black);
		}

	}
	cout << "Error pq is empty, target hasn't been found\n";
}

void BuildPathBetweenTheRooms()
{
	int i, j;

	for (i = 0;i < NUM_ROOMS;i++)
		for (j = i + 1;j < NUM_ROOMS;j++)
		{
			BuildPath(i, j); // A*
		}
}

void PlaceSoldiers() {
	int roomX = 0, roomY = 0;
	int width = 0, height = 0;
	int soldierX = 0, soldierY = 0;
	Team* temp = nullptr;
	Soldier* s1 = nullptr;
	Supporter* sup = nullptr;
	//create 2 soldiers in each team
	for (int i = 0; i < (sizeof(teams) / sizeof(Team*)); i++)
	{
		teams[i] = new Team();
		roomX = rooms[i * (NUM_ROOMS - 1)]->getCenterX();
		roomY = rooms[i * (NUM_ROOMS - 1)]->getCenterY();
		width = rooms[i * (NUM_ROOMS - 1)]->getWidth();
		height = rooms[i * (NUM_ROOMS - 1)]->getHeight();

		while (maze[soldierY][soldierX] != SPACE)
		{
			soldierY = rand() % (height - 2) + (roomY - height / 2 + 1);
			soldierX = rand() % (width - 2) + (roomX - width / 2 + 1);
		}

		sup = new Supporter(soldierX, soldierY, i + 4);
		maze[soldierY][soldierX] = i + 4;
		teams[i]->assignSupporter(sup);

		for (int j = 0; j < NUM_OF_SOLDIERS; j++)
		{
			while (maze[soldierY][soldierX] != SPACE)
			{
				soldierY = rand() % (height - 2) + (roomY - height / 2 + 1);
				soldierX = rand() % (width - 2) + (roomX - width / 2 + 1);
			}
			s1 = new Soldier(soldierX, soldierY, maze[soldierY][soldierX], i + 2);
			maze[soldierY][soldierX] = i + 2;
			teams[i]->addSoldier(s1, j);
			s1->setSupporter(sup);
		}

		sup->setTeammates(teams[i]->getSoldiers());

	}

	teams[0]->getSoldiers()[0]->setTarget(teams[1]->getSoldiers()[0]);
	teams[0]->getSoldiers()[1]->setTarget(teams[1]->getSoldiers()[0]);
	teams[1]->getSoldiers()[0]->setTarget(teams[0]->getSoldiers()[0]);
	teams[1]->getSoldiers()[1]->setTarget(teams[0]->getSoldiers()[0]);

	teams[0]->setTeamates();
	teams[1]->setTeamates();
}

void PlaceWareHouses()
{
	int roomX = 0, roomY = 0;
	int width = 0, height = 0;
	int x = 0, y = 0;
	int roomNumber = 0;
	for (int i = 0; i < 4; i++)
	{
		roomNumber = 1 + rand() % NUM_ROOMS - 1;
		roomX = rooms[roomNumber]->getCenterX();
		roomY = rooms[roomNumber]->getCenterY();
		width = rooms[roomNumber]->getWidth();
		height = rooms[roomNumber]->getHeight();
		while (maze[y][x] != SPACE)
		{
			y = rand() % (height - 2) + (roomY - height / 2 + 1);
			x = rand() % (width - 2) + (roomX - width / 2 + 1);
		}
		if (i % 2 == 0)
			maze[y][x] = MEDICAL_WAREHOUSE;
		else
			maze[y][x] = AMMO_WAREHOUSE;
	}

}

void SetupDungeon()
{
	int i,j;
	int cx, cy, w, h;
	bool hasOverlap;

	for (i = 0;i < NUM_ROOMS;i++)
	{
		do
		{
			hasOverlap = false;
			w = 6 + rand() % (MSZ / 5);
			h = 6 + rand() % (MSZ / 5);
			cx = 2 + w / 2 + rand() % (MSZ - w - 4);
			cy = 2 + h / 2 + rand() % (MSZ - h - 4);
			for(j=0;j<i && !hasOverlap;j++)
				hasOverlap = rooms[j]->Overlap(cx, cy, w, h);
		} while (hasOverlap); // check the validity of the room
			
		rooms[i] = new Room(cx, cy, w, h, maze);
	}

	PlaceSoldiers();

	PlaceWareHouses();

	BuildPathBetweenTheRooms();
}

void init()
{
	glClearColor(0.5, 0.5, 0.5, 0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	srand(time(0));

	SetupDungeon();
}

void ShowDungeon()
{
	int i, j;
	double s;
	for(i=0;i<MSZ;i++)
		for (j = 0;j < MSZ;j++)
		{
			s = securityMap[i][j];
			//1. set color of cell
			switch (maze[i][j])
			{
			case SPACE:
				glColor3d(1, 1, 1); // white
				break;
			case WALL:
				glColor3d(0.3, 0.3, 0.4); // dark gray
				break;
			case TEAM_1_SOLDIER:
				glColor3d(0.7, 0.2, 0.1); //red
				break;
			case TEAM_2_SOLDIER:
				glColor3d(0.1, 0.2, 0.7); //blue
				break;
			case TEAM_1_SUPPORTER:
				glColor3d(0.98, 0.07, 0.67); //pink
				break;
			case TEAM_2_SUPPORTER:
				glColor3d(0.3, 0.9, 0.85); //cyan
				break;
			case MEDICAL_WAREHOUSE:
				glColor3d(0, 1, 0); //green
				break;
			case AMMO_WAREHOUSE:
				glColor3d(0.52, 0.32, 0.02); //brown
				break;
			}
			// show cell
			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j+1, i + 1);
			glVertex2d(j + 1, i );
			glEnd();
		}
}

void generateSecurityMap() {
	int numSimulations = 1000;

	int i;
	for (i = 0; i < numSimulations; i++) {
		Granade* g = new Granade(rand() % MSZ, rand() % MSZ, (rand() % 360) * PI / 180,-1);
		g->SimulateExplosion(maze,securityMap);
	}
}

void FindRoomID(Soldier* s)
{
	int startX, startY, endX, endY;
	int soldierX, soldierY;
	soldierX = (int)s->getX();
	soldierY = (int)s->getY();
	Room* r = nullptr;
	for (int i = 0; i < NUM_ROOMS; i++)
	{
		r = rooms[i];
		startX = r->getCenterX() - (r->getWidth() / 2);
		endX = r->getCenterX() + (r->getWidth() / 2);
		startY = r->getCenterY() - (r->getHeight() / 2);
		endY = r->getCenterY() + (r->getHeight() / 2);
		if ((soldierX >= startX) && (soldierX <= endX) && (soldierY >= startY) && (soldierY <= endY))
		{
			s->setRoomID(i);
			return;
		}
	}
	s->setRoomID(-1);
}

void reassignTargets(int i, int j) {
	Soldier** soldiers = teams[(i + 1) % 2]->getSoldiers();
	soldiers[0]->setTarget(teams[i]->getSoldier((j + 1) % 2));
	soldiers[1]->setTarget(teams[i]->getSoldier((j + 1) % 2));
}

Soldier* findSoldier(int teamNumber, pair<int, int> collition)
{
	for (int i = 0; i < 2; i++)
	{
		if (teams[teamNumber]->getSoldier(i)->getX() == collition.first)
			if (teams[teamNumber]->getSoldier(i)->getY() == collition.second)
				return teams[teamNumber]->getSoldier(i);
	}
}

void DrawWinnerText() {
	string gameOverText = "GAME OVER!";

	//background 
	float textWidth = gameOverText.length()*2.75;
	float textHeight = 13.5;
	glColor3f(0, 0, 0);
	glBegin(GL_QUADS);
	glVertex2f(MSZ / 2 - 10, MSZ / 2 + 5);
	glVertex2f(MSZ / 2 - 10+textWidth, MSZ / 2 + 5);
	glVertex2f(MSZ / 2 - 10 + textWidth, MSZ / 2 + 5 - textHeight);
	glVertex2f(MSZ / 2 - 10, MSZ / 2 + 5 - textHeight);
	glEnd();
	//

	string winnerTeam = "";
	if (team1DeathCount >= 2) {
		winnerTeam = "BLUE TEAM WINS";
		glColor3d(0, 0, 1);
	}
	else {
		winnerTeam = "RED TEAM WINS";
		glColor3d(1, 0, 0);
	}

	glRasterPos2f(MSZ / 2 - 5, MSZ / 2);
	for (char c : gameOverText) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}

	glRasterPos2f(MSZ / 2 - 7, MSZ / 2 - 5);
	for (char c : winnerTeam) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
	}
}

void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer

	ShowDungeon();
	if (team1DeathCount < 2 && team2DeathCount < 2) {
		for (int i = 0; i < 2; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				Soldier* s = teams[i]->getSoldier(j);
				if (s->getGT() != nullptr)
					s->getGT()->show();
				if (s->getBF() != nullptr)
					s->getBF()->show();
			}
		}
	}
	else
		DrawWinnerText();

	glutSwapBuffers(); // show all
}

void clearSecurityMap()
{
	for (int i = 0; i < MSZ; i++)
		for (int j = 0; j < MSZ; j++)
			securityMap[j][i] = 0.0;
}

void idle() 
{
	int roomIDToGo = -1;
	for (int i = 0; i < 2 && team1DeathCount < 2 && team2DeathCount < 2; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			Soldier* s = teams[i]->getSoldier(j);

			FindRoomID(s);
			if (!s->getIsDied() && s->getGrenadeThrown())
			{
				if (s->getGT()->getIsMoving())
				{
					s->getGT()->move(maze);
				}
				else if (s->getGT()->getIsExploding()) {
					vector<pair<int, int>> res = s->getGT()->expend(maze);
					if (!res.empty()) {
						set<Soldier*> hitRivals;
						// Check all pellet collisions
						for (int k = 0; k < res.size(); k++) {
							Soldier* rival = findSoldier((i + 1) % 2, res[k]);
							if (rival != nullptr) {
								// Add rival to the hit set
								hitRivals.insert(rival);
							}
						}

						// Apply damage once to each unique hit rival
						for (Soldier* rival : hitRivals) {
							rival->setHealth(rival->getHealth() - GRANADE_DAMAGE);
						}
					}
				}
			}
			if (!s->getIsDied() && s->getBulletFired())
			{	
				rivalCollition = s->getBF()->move(maze);
				if (rivalCollition.first != -1)
				{
					Soldier* rival = findSoldier((i + 1) % 2, rivalCollition);
					rival->setHealth(rival->getHealth() - BULLET_DAMAGE);
				}
			}
		
			s->MoveSoldier(maze);
			clearSecurityMap();
			if(s->getRoomID()!=-1)
				s->fight(maze, securityMap, rooms[s->getRoomID()]);

			//kill soldier
			if (!s->getIsDied() && s->isSoldierDead(maze)) {
				(i == 0) ? team1DeathCount++ : team2DeathCount++;
				if (team1DeathCount < 2 && team2DeathCount < 2)
					reassignTargets(i, j);
				break;
			}
		}
		teams[i]->survive(rooms, maze);
		roomIDToGo = rand() % NUM_ROOMS;
		try {
			teams[i]->getSupporter()->followTeammates(rooms[roomIDToGo]->getCenterX(), rooms[roomIDToGo]->getCenterY(), maze);
			teams[i]->getSupporter()->moveToHelpTarget(maze);
		}
		catch (const out_of_range& e) {
			cerr << "Out of range error: " << e.what() << std::endl;
			// Handle the error or debug here
			return; // Or other fallback logic
		}	
	}

	glutPostRedisplay(); // indirect call to display
}

void main(int argc, char* argv[]) 
{
	glutInit(&argc, argv);
	// definitions for visual memory (Frame buffer) and double buffer
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitWindowPosition(600, 20);
	glutCreateWindow("Dungen AI War");

	// display is a refresh function
	glutDisplayFunc(display);
	// idle is a update function
	glutIdleFunc(idle);

	init();

	glutMainLoop();
}
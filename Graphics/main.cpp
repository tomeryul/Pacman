
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "glut.h"
#include <queue>
#include <vector>
#include "Cell.h"
#include <iostream>
#include "CompareAStar.h"
#include "CompareBFS.h"
#include "Pacman.h"

using namespace std;

const int MSZ = 50;

const int SPACE = 0;
const int WALL = 1;
const int START = 2;
const int TARGET = 3;
const int GRAY = 4;
const int BLACK = 5;
const int PATH = 6;
const int GHOST = 7;

const int numGhost = 3;

int deadGhosts = 0;
int maze[MSZ][MSZ] = { 0 }; // space
bool Start = false, runAStar = false, runBestFirstSearch = false, runBFS = false, runDFS = false;
int Target_row, Target_col;
int iTarget_row, iTarget_col;// in order to randomize the coins on the map
int frame = 0;
Pacman* pacman = new Pacman(MSZ / 2, MSZ / 2);

queue <Cell*> grays; // queue of gray cells
vector <Cell*> dfs_grays; // stack of gray cells

queue <Cell*> PacmanMoveSet;
priority_queue <Cell*, vector <Cell*>, CompareAStar> GhostAStarMoveSet;

Cell* GhostArr[numGhost];
int GhostHP[numGhost] = { 100,100,100 };



void InitMaze();
void PacmanMoveFunc();
void PacmanRunFunc();
void PacmanGhostFight();
void GhosMoveFunc(int GhostNum);


void init()
{
	glClearColor(0.5, 0.5, 0.5, 0);// color of window background
	glOrtho(0, MSZ, 0, MSZ, -1, 1); // set the coordinates system

	srand(time(0));

	InitMaze();
}

void InitMaze()
{
	int i, j;

	for (i = 0; i < MSZ; i++)
	{
		maze[0][i] = WALL;
		maze[MSZ - 1][i] = WALL;
		maze[i][0] = WALL;
		maze[i][MSZ - 1] = WALL;
	}

	for (i = 1; i < MSZ - 1; i++)
		for (j = 1; j < MSZ - 1; j++)
		{
			if (i % 2 == 1) // mostly spaces
			{
				if (rand() % 25 < 2) // WALL
					maze[i][j] = WALL;
				else maze[i][j] = SPACE;
			}
			else // even lines
			{
				if (rand() % 10 < 4) // space
					maze[i][j] = SPACE;
				else
					maze[i][j] = WALL;
			}
		}

	maze[pacman->getX()][pacman->getY()] = START;

	Cell* pc;

	for (int i = 0; i < 10; i++)
	{
		do
		{
			iTarget_row = rand() % MSZ;
			iTarget_col = rand() % MSZ;
		} while (iTarget_row == MSZ - 1 || iTarget_row == 0 || iTarget_col == MSZ - 1 || iTarget_col == 0);
		maze[iTarget_row][iTarget_col] = TARGET;
	}

	for (int i = 0; i < numGhost; i++)
	{
		do
		{
			iTarget_row = rand() % MSZ;
			iTarget_col = rand() % MSZ;
		} while (iTarget_row == MSZ - 1 || iTarget_row == 0 || iTarget_col == MSZ - 1 || iTarget_col == 0);
		pc = new Cell(iTarget_row, iTarget_col, nullptr);
		GhostArr[i] = pc;
		maze[iTarget_row][iTarget_col] = GHOST;
	}

	pc = new Cell(pacman->getX(), pacman->getY(), nullptr);  // pointer to a cell

	grays.push(pc); // insert first cell to grays
	dfs_grays.push_back(pc); // insert first cell to grays
	PacmanMoveSet.push(pc); // initialize pacman set of moves
	
}

void DrawMaze()
{
	int i, j;

	for (i = 0; i < MSZ; i++)
		for (j = 0; j < MSZ; j++)
		{
			switch (maze[i][j]) // set convinient color of a cell
			{
			case SPACE:
				glColor3d(0, 0, 0); // white
				break;
			case WALL:
				glColor3d(0, 0, 0.75); // black
				break;
			case START:
				glColor3d(1, 1, 0); // blue
				break;
			case TARGET:
				glColor3d(1, 0.5, 0.25); // red
				break;
			case GRAY:
				glColor3d(1, 0.7, 0); // orange
				break;
			case BLACK:
				glColor3d(0.8, 1, 0.8); // green
				break;
			case GHOST:
				glColor3d(0, 1, 0); // green
				break;
			case PATH:
				glColor3d(1, 0, 1); // magenta
				break;

			}// switch

			glBegin(GL_POLYGON);
			glVertex2d(j, i);
			glVertex2d(j, i + 1);
			glVertex2d(j + 1, i + 1);
			glVertex2d(j + 1, i);
			glEnd();
		}
}

void resetMSZ()
{
	for (int i = 0; i < MSZ; i++)
	{
		for (int j = 0; j < MSZ; j++)
		{
			if (maze[i][j] == BLACK || maze[i][j] == GRAY)
				maze[i][j] = SPACE;
		}
	}
}

void consumeTime()
{
	for (int i = 0; i < 65000; i++)
		for (int j = 0; j < 500; j++) {}
}

double Distance(int first_row, int first_col, int second_row, int second_col)
{
	return abs(first_row - second_row) + abs(first_col - second_col);
}

int CheckGhostSearch(int row, int col, Cell* pCurrent)
{
	Cell* pn = nullptr;
	int pacmanRow, pacmanCol;
	bool isnear = false;
	if (maze[row][col] == GHOST)
	{
		while (pCurrent != nullptr)
		{
			if (pCurrent->getParent() == nullptr)
			{
				pacmanRow = pCurrent->getRow();
				pacmanCol = pCurrent->getCol();
				break;
			}
			pCurrent = pCurrent->getParent();
		}
		while (!PacmanMoveSet.empty())
			PacmanMoveSet.pop();
		PacmanMoveSet.push(pCurrent);

		double distance = Distance(pacmanRow, pacmanCol, row, col);
		if (distance <= 5.0)
		{
			return 1;
		}
		else
			return 2;
	}
	else // must be "White Neighbour" - SPACE
	{
		pn = new Cell(row, col, pCurrent); // create new Cell
		PacmanMoveSet.push(pn);
		maze[row][col] = GRAY;
		return 3;
	}
}

bool GhostSearch()
{
	Cell* pc, * thePacman;
	int row, col;
	int go_on = 3, count = 0;

	while (!PacmanMoveSet.empty() && go_on == 3)
	{
		if (count == 0)
		{
			thePacman = PacmanMoveSet.front();
		}
		// pick the first Cell from grays
		pc = PacmanMoveSet.front();
		PacmanMoveSet.pop();
		// paint it black
		if (maze[pc->getRow()][pc->getCol()] != START) maze[pc->getRow()][pc->getCol()] = BLACK;

		row = pc->getRow();
		col = pc->getCol();
		// now check all neighbors of pc and if they are white add them to grays
		// but if one of them is target then stop BFS because target has been found

		// up
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == GHOST)
			go_on = CheckGhostSearch(row + 1, col, pc);
		// down
		if (go_on == 3 && (maze[row - 1][col] == SPACE || maze[row - 1][col] == GHOST))
			go_on = CheckGhostSearch(row - 1, col, pc);
		// left
		if (go_on == 3 && (maze[row][col - 1] == SPACE || maze[row][col - 1] == GHOST))
			go_on = CheckGhostSearch(row, col - 1, pc);
		// right
		if (go_on == 3 && (maze[row][col + 1] == SPACE || maze[row][col + 1] == GHOST))
			go_on = CheckGhostSearch(row, col + 1, pc);
	}

	resetMSZ();

	if (go_on == 1)
		return true;
	else if (go_on == 2)
		return false;
	else
	{
		PacmanMoveSet.push(thePacman);
		return false;
	}

}


void PacmanGhostFight()
{
	Cell* ThePacMan = PacmanMoveSet.front();
	Cell* currentGhost;
	int PacManRow, PacManCol, GhostRow, GhostCol;
	int numOfGhostAttcking = 0;


	PacManRow = ThePacMan->getRow();
	PacManCol = ThePacMan->getCol();

	for (int i = 0; i < numGhost; i++)
	{
		currentGhost = GhostArr[i];
		GhostRow = currentGhost->getRow();
		GhostCol = currentGhost->getCol();

		// up
		if (PacManRow + 1 == GhostRow && PacManCol == GhostCol)
		{
			numOfGhostAttcking++;
			GhostHP[i] -= 10;
			if (GhostHP[i] == 0)
			{
				maze[GhostRow][GhostCol] = SPACE;
				pacman->SetIsOneBlockNearPacman(false);
			}
		}
		// down
		else if (PacManRow - 1 == GhostRow && PacManCol == GhostCol)
		{
			numOfGhostAttcking++;
			GhostHP[i] -= 10;
			if (GhostHP[i] == 0)
			{
				maze[GhostRow][GhostCol] = SPACE;
				pacman->SetIsOneBlockNearPacman(false);
			}
		}
		// left
		else if (PacManRow == GhostRow && PacManCol - 1 == GhostCol)
		{
			numOfGhostAttcking++;
			GhostHP[i] -= 10;
			if (GhostHP[i] == 0)
			{
				maze[GhostRow][GhostCol] = SPACE;
				pacman->SetIsOneBlockNearPacman(false);
			}
		}
		// right
		else if (PacManRow == GhostRow && PacManCol + 1 == GhostCol)
		{
			numOfGhostAttcking++;
			GhostHP[i] -= 10;
			if (GhostHP[i] == 0)
			{
				maze[GhostRow][GhostCol] = SPACE;
				pacman->SetIsOneBlockNearPacman(false);
			}
		}
	}

	if (numOfGhostAttcking > 1)
	{
		maze[PacManRow][PacManCol] = SPACE;
		PacmanMoveSet.pop();
		Start = false;
	}


}

void GhostMoveOneBlock(int row, int col, Cell* pc, int GhostNum)
{
	while (pc != nullptr)
	{
		if (pc->getParent() == nullptr)
		{
			//maze[row][col] = SPACE;
			while (!GhostAStarMoveSet.empty())
				GhostAStarMoveSet.pop();

		}

		else if (pc->getParent()->getParent() == nullptr)
		{
			maze[pc->getRow()][pc->getCol()] = GHOST;
			maze[pc->getParent()->getRow()][pc->getParent()->getCol()] = SPACE;
			GhostArr[GhostNum]->setRow(pc->getRow());
			GhostArr[GhostNum]->setCol(pc->getCol());
			pc->setParent(nullptr);
			while (!GhostAStarMoveSet.empty())
				GhostAStarMoveSet.pop();
		}
		pc = pc->getParent();
	}
}

void PacManMoveOneBlock(int TargetRow, int TargetCol, Cell* pc)
{
	while (pc != nullptr)
	{
		if (pc->getParent() == nullptr)
		{
			maze[TargetRow][TargetCol] = START;
			maze[pacman->getX()][pacman->getY()] = SPACE;
			pacman->SetX(TargetRow);
			pacman->SetY(TargetCol);
			pc->setRow(TargetRow);
			pc->setCol(TargetCol);
			while (!PacmanMoveSet.empty())
				PacmanMoveSet.pop();
			PacmanMoveSet.push(pc);
		}

		else if (pc->getParent()->getParent() == nullptr)
		{
			pacman->SetX(pc->getRow());
			pacman->SetY(pc->getCol());
			maze[pacman->getX()][pacman->getY()] = START;
			maze[pc->getParent()->getRow()][pc->getParent()->getCol()] = SPACE;
			pc->setParent(nullptr);
			while (!PacmanMoveSet.empty())
				PacmanMoveSet.pop();
			PacmanMoveSet.push(pc);
		}
		pc = pc->getParent();
	}
}

void PacManRunOneBlock(int GhostRow, int GhostCol, Cell* pc)
{
	while (pc != nullptr)
	{
		if (pc->getParent() == nullptr)
		{
			pacman->SetIsOneBlockNearPacman(true);
			while (!PacmanMoveSet.empty())
				PacmanMoveSet.pop();
			PacmanMoveSet.push(pc);
		}

		else if (pc->getParent()->getParent() == nullptr)
		{
			int RowToGo, ColToGo;
			int CurrentRow = pc->getParent()->getRow();
			int CurrentCol = pc->getParent()->getCol();
			int RowDis = CurrentRow - GhostRow;
			int ColDis = CurrentCol - GhostCol;

			if (RowDis >= 0) RowToGo = CurrentRow + 1;
			else RowToGo = CurrentRow - 1;
			if (ColDis >= 0) ColToGo = CurrentCol + 1;
			else ColToGo = CurrentCol - 1;

			if (maze[CurrentRow][ColToGo] != WALL)
			{
				pacman->SetX(CurrentRow);
				pacman->SetY(ColToGo);
				pc->setRow(CurrentRow);
				pc->setCol(ColToGo);
				maze[CurrentRow][ColToGo] = START;
				maze[CurrentRow][CurrentCol] = SPACE;
			}
			else if (maze[RowToGo][CurrentCol] != WALL)
			{
				pacman->SetX(RowToGo);
				pacman->SetY(CurrentCol);
				pc->setRow(RowToGo);
				pc->setCol(CurrentCol);
				maze[RowToGo][CurrentCol] = START;
				maze[CurrentRow][CurrentCol] = SPACE;
			}
			else // can't move and need to stay in place
			{
				pacman->SetX(CurrentRow);
				pacman->SetY(CurrentCol);
				pc->setRow(CurrentRow);
				pc->setCol(CurrentCol);
				maze[CurrentRow][CurrentCol] = START;
			}

			pc->setParent(nullptr);
			while (!PacmanMoveSet.empty())
				PacmanMoveSet.pop();
			PacmanMoveSet.push(pc);
		}
		pc = pc->getParent();
	}
}



bool CheckGhostNeighbours(int row, int col, Cell* pCurrent, int GhostNum)
{
	Cell* pn = nullptr;
	if (maze[row][col] == START)
	{
		GhostMoveOneBlock(row, col, pCurrent, GhostNum);
		return false;
	}
	else // must be "White Neighbour" - SPACE
	{
		pn = new Cell(row, col, pCurrent); // create new Cell
		pn->setG(Distance(row, col, GhostArr[GhostNum]->getRow(), GhostArr[GhostNum]->getCol()));
		pn->setH(Distance(row, col, pacman->getX(), pacman->getY()));
		GhostAStarMoveSet.push(pn);
		maze[row][col] = GRAY;
		return true; // go_on = true
	}
}

bool CheckPacManNeighbours(int row, int col, Cell* pCurrent)
{
	Cell* pn = nullptr;
	if (maze[row][col] == TARGET)
	{
		PacManMoveOneBlock(row, col, pCurrent);
		return false;
	}
	else // must be "White Neighbour" - SPACE
	{
		pn = new Cell(row, col, pCurrent); // create new Cell
		PacmanMoveSet.push(pn);
		maze[row][col] = GRAY;
		return true; // go_on = true
	}
}

bool CheckPacManRunPath(int row, int col, Cell* pCurrent)
{
	Cell* pn = nullptr;
	if (maze[row][col] == GHOST)
	{
		PacManRunOneBlock(row, col, pCurrent);
		return false;
	}
	else // must be "White Neighbour" - SPACE
	{
		pn = new Cell(row, col, pCurrent); // create new Cell
		PacmanMoveSet.push(pn);
		maze[row][col] = GRAY;
		return true; // go_on = true
	}
}


void PacmanMoveFunc()
{
	Cell* pc;
	int row, col;
	bool go_on = true;
	while (!PacmanMoveSet.empty() && go_on)
	{
		// pick the first Cell from grays
		pc = PacmanMoveSet.front();
		PacmanMoveSet.pop();
		// paint it black
		if (maze[pc->getRow()][pc->getCol()] != START) maze[pc->getRow()][pc->getCol()] = BLACK;

		row = pc->getRow();
		col = pc->getCol();
		// now check all neighbors of pc and if they are white add them to grays
		// but if one of them is target then stop BFS because target has been found

		// up
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == TARGET)
			go_on = CheckPacManNeighbours(row + 1, col, pc);
		// down
		if (go_on && (maze[row - 1][col] == SPACE || maze[row - 1][col] == TARGET))
			go_on = CheckPacManNeighbours(row - 1, col, pc);
		// left
		if (go_on && (maze[row][col - 1] == SPACE || maze[row][col - 1] == TARGET))
			go_on = CheckPacManNeighbours(row, col - 1, pc);
		// right
		if (go_on && (maze[row][col + 1] == SPACE || maze[row][col + 1] == TARGET))
			go_on = CheckPacManNeighbours(row, col + 1, pc);
	}

	resetMSZ();


}

void PacmanRunFunc()
{
	Cell* pc;
	int row, col;
	bool go_on = true;
	while (!PacmanMoveSet.empty() && go_on)
	{
		// pick the first Cell from grays
		pc = PacmanMoveSet.front();
		PacmanMoveSet.pop();
		// paint it black
		if (maze[pc->getRow()][pc->getCol()] != START) maze[pc->getRow()][pc->getCol()] = BLACK;

		row = pc->getRow();
		col = pc->getCol();
		// now check all neighbors of pc and if they are white add them to grays
		// but if one of them is target then stop BFS because target has been found

		// up
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == GHOST)
			go_on = CheckPacManRunPath(row + 1, col, pc);
		// down
		if (go_on && (maze[row - 1][col] == SPACE || maze[row - 1][col] == GHOST))
			go_on = CheckPacManRunPath(row - 1, col, pc);
		// left
		if (go_on && (maze[row][col - 1] == SPACE || maze[row][col - 1] == GHOST))
			go_on = CheckPacManRunPath(row, col - 1, pc);
		// right
		if (go_on && (maze[row][col + 1] == SPACE || maze[row][col + 1] == GHOST))
			go_on = CheckPacManRunPath(row, col + 1, pc);
	}

	resetMSZ();

}

void GhosMoveFunc(int GhostNum)
{
	Cell* pc;
	int row, col;
	bool go_on = true;
	while (!GhostAStarMoveSet.empty() && go_on)
	{
		// pick the first Cell from grays
		pc = GhostAStarMoveSet.top();
		GhostAStarMoveSet.pop();
		// paint it black
		if (maze[pc->getRow()][pc->getCol()] != GHOST) maze[pc->getRow()][pc->getCol()] = BLACK;

		row = pc->getRow();
		col = pc->getCol();
		// now check all neighbors of pc and if they are white add them to grays
		// but if one of them is target then stop BFS because target has been found

		// up
		if (maze[row + 1][col] == SPACE || maze[row + 1][col] == START)
			go_on = CheckGhostNeighbours(row + 1, col, pc, GhostNum);
		// down
		if (go_on && (maze[row - 1][col] == SPACE || maze[row - 1][col] == START))
			go_on = CheckGhostNeighbours(row - 1, col, pc, GhostNum);
		// left
		if (go_on && (maze[row][col - 1] == SPACE || maze[row][col - 1] == START))
			go_on = CheckGhostNeighbours(row, col - 1, pc, GhostNum);
		// right
		if (go_on && (maze[row][col + 1] == SPACE || maze[row][col + 1] == START))
			go_on = CheckGhostNeighbours(row, col + 1, pc, GhostNum);
	}

	resetMSZ();

}



void StartGame()
{
	if (deadGhosts == numGhost)
		pacman->SetIsGhostNearPacman(false);
	else
		pacman->SetIsGhostNearPacman(GhostSearch());
	pacman->DoSomeWork();
	frame++;
	int i;
	Cell* pc;
	if (frame % 2 == 0)
	{
		if (pacman->getMove() == 1)
			PacmanMoveFunc();
		if (pacman->getMove() == 2)
			PacmanRunFunc();
		if (pacman->getMove() == 3)
			PacmanGhostFight();
	}

	if (frame % 2 == 1)
	{
		for (i = 0; i < numGhost; i++)
		{
			pc = GhostArr[i];
			if (pc->getCol() != 400)
			{
				if (maze[pc->getRow()][pc->getCol()] != GHOST)
				{
					deadGhosts++;
					pc->setRow(400);
					pc->setCol(400);
				}
				else
				{
					pc->setG(0.0);
					pc->setH(Distance(pc->getRow(), pc->getCol(), pacman->getX(), pacman->getY()));
					GhostAStarMoveSet.push(pc);
					GhosMoveFunc(i);
				}
			}
		}
	}
	consumeTime();

}


// drawings are here
void display()
{
	glClear(GL_COLOR_BUFFER_BIT); // clean frame buffer with background color

	DrawMaze();

	glutSwapBuffers(); // show all
}

void idle()
{

	if (Start)
		StartGame();
	if (runAStar)
		PacmanMoveFunc();

	glutPostRedisplay(); // call to display indirectly
}

void menu(int choice)
{
	if (choice == 1) // BFS
		Start = true;
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(400, 100);
	glutCreateWindow("Pacman Game with AI");

	glutDisplayFunc(display); // refresh function
	glutIdleFunc(idle);  // updates function
	// menu
	glutCreateMenu(menu);
	glutAddMenuEntry("Start", 1);
	glutAttachMenu(GLUT_RIGHT_BUTTON);

	init();

	glutMainLoop();

	return 1;
}
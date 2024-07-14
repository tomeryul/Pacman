#pragma once
#include "State.h"
class State;

class Pacman
{
private:
	int x, y;
	bool isRunning, isFighting, isCollecting;
	int move;
	bool isGhostNearPacman;
	bool isOneBlockNearPacman;
	State* pCurrentState;

public:
	Pacman(int x, int y);
	void Draw();
	void DoSomeWork();

	int getX() { return x; }
	int getY() { return y; }
	int getMove() { return move; }

	void SetX(int newX) { x = newX; }
	void SetY(int newY) { y = newY; }
	void SetMove(int newMove) { move = newMove; }

	void SetIsRunning(bool value) { isRunning = value; }
	void SetIsCollecting(bool value) { isCollecting = value; }
	void SetIsFighting(bool value) { isFighting = value; }

	bool getIsRunning() { return this->isRunning; }
	bool getIsCollecting() { return this->isCollecting; }
	bool getIsFighting() { return this->isFighting; }

	void SetCurrentState(State* ps) { pCurrentState = ps; }
	State* GetCurrentState() { return pCurrentState; }

	bool GetIsGhostNearPacman() { return this->isGhostNearPacman; }
	void SetIsGhostNearPacman(bool value) { this->isGhostNearPacman = value; }

	bool GetIsOneBlockNearPacman() { return this->isOneBlockNearPacman; }
	void SetIsOneBlockNearPacman(bool value) { this->isOneBlockNearPacman = value; }


};
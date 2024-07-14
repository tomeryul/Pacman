#include "Pacman.h"
#include "State.h"
#include "SearchingCoins.h"


Pacman::Pacman(int x, int y)
{
	this->x = x;
	this->y = y;
	isFighting = false;
	isCollecting = false;
	isRunning = false;
	isGhostNearPacman = false;
	isOneBlockNearPacman = false;
	move = -1;
	pCurrentState = new SearchingCoins();
	pCurrentState->OnEnter(this);

}

void Pacman::Draw()
{

}

void Pacman::DoSomeWork()
{
	if (isCollecting)
	{
		if (isGhostNearPacman)
			pCurrentState->Transition(this);
	}

	if (isRunning)
	{
		if (isOneBlockNearPacman)
			pCurrentState->Transition(this);
	}

	if (isFighting)
	{
		if (!isOneBlockNearPacman)
			pCurrentState->Transition(this);
	}



}

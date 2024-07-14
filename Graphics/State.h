#pragma once
#include "Pacman.h"

class Pacman;

class State
{
public:
	virtual void OnEnter(Pacman* pn) = 0;
	virtual void Transition(Pacman* pn) = 0;
	virtual void OnExit(Pacman* pn) = 0;
};
#pragma once
#include "state.h"
class SearchingCoins :
    public State

{
    void OnEnter(Pacman* pn);
    void Transition(Pacman* pn); // to another state
    void OnExit(Pacman* pn);
};


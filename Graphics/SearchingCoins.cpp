#include "SearchingCoins.h"
#include "PacmanRun.h"

void SearchingCoins::OnEnter(Pacman* pn)
{
	pn->SetIsCollecting(true);
	pn->SetMove(1);

}

void SearchingCoins::Transition(Pacman* pn)
{
	OnExit(pn);
	pn->SetCurrentState(new PacmanRun());
	pn->GetCurrentState()->OnEnter(pn);

}

void SearchingCoins::OnExit(Pacman* pn)
{
	pn->SetIsCollecting(false);
}
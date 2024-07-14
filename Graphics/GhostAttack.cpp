#include "GhostAttack.h"
#include "SearchingCoins.h"

void GhostAttack::OnEnter(Pacman* pn)
{
	pn->SetIsFighting(true);
	pn->SetMove(3);

}

void GhostAttack::Transition(Pacman* pn)
{
	OnExit(pn);
	pn->SetCurrentState(new SearchingCoins());
	pn->GetCurrentState()->OnEnter(pn);

}

void GhostAttack::OnExit(Pacman* pn)
{
	pn->SetIsFighting(false);
}

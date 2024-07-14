#include "PacmanRun.h"
#include "GhostAttack.h"

void PacmanRun::OnEnter(Pacman* pn)
{
	pn->SetIsRunning(true);
	pn->SetMove(2);

}

void PacmanRun::Transition(Pacman* pn)
{
	OnExit(pn);
	pn->SetCurrentState(new GhostAttack());
	pn->GetCurrentState()->OnEnter(pn);

}

void PacmanRun::OnExit(Pacman* pn)
{
	pn->SetIsRunning(false);
}

#pragma once
#include "Cell.h"
class CompareAStar
{
public:
	bool operator() (Cell* pn1, Cell* pn2) {
		return pn1->getG() + pn1->getH() > pn2->getG() + pn2->getH();

	}
};


#pragma once
#include "Cell.h"
class CompareBFS
{
public:
	bool operator() (Cell* pn1, Cell* pn2) {
		return  pn1->getH() > pn2->getH();

	}
};


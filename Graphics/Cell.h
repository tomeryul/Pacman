#pragma once
class Cell
{
private:
	int row;
	int col;
	double G;
	double H;
	Cell* parent;

public:
	Cell();
	Cell(int r, int c, Cell* p);
	int getRow() { return row; }
	int getCol() { return col; }
	double getG() { return G; }
	double getH() { return H; }
	void setG(double g) { G = g; }
	void setH(double h) { H = h; }
	void setRow(int r) { row = r; }
	void setCol(int c) { col = c; }
	void setParent(Cell* p) { parent = p; }
	Cell* getParent() { return parent; }
};


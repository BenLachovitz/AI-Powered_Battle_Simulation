#include "Cell.h"
#include <math.h>

Cell::Cell()
{
	parent = nullptr;
	col = 0;
	row = 0;
	target_col = 0;
	target_row = 0;
	h = 0;
	g = 0;
	f = 0;
}
// gets row, col, target row, target col, parent g and parent itself
Cell::Cell(int r, int c, int tr, int tc, double newg, Cell* p)
{
	row = r;
	col = c;
	parent = p;
	target_row = tr;
	target_col = tc;
	g = newg;
	ComputeH();
	f = h + g;
}

// h is computed basing on Manhattan distance
void Cell::ComputeH()
{
	h = abs(row - target_row) + abs(col - target_col);
}

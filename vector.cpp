#include "vector.h"

static const double TOLERANCE = 1e-6;

int CAC(const POINT &checkpt, const POINT &p1, const POINT &p2)
{
	int cac = 0;

	double dx1 = p1.x - checkpt.x;
	double dy1 = p1.y - checkpt.y;
	double dx2 = p2.x - checkpt.x;
	double dy2 = p2.y - checkpt.y;

	//eliminate infinite gradients by multiplying them bet. 0-1 and 0-2
	if(dx1*dy2 > dx2*dy1)
		cac = 1; //counter-clock
	if(dx1*dy2 < dx2*dy1)
		cac = -1; //clockwise
	if((dx1*dy2 - dx2*dy1)<TOLERANCE)
	{
		if((dx1*dx2 < 0) || (dy1*dy2 < 0))
			cac = -1;
		else if((dx1*dx1 + dy1*dy1) >= (dx2*dx2 + dy2*dy2))
			cac = 0;
		else
			cac = 1;
	}
	return cac;
}
#ifndef _hello_h
#define _hello_h


//#include "game.h"
#include "mywin.h"
#include "vector.h"


#define MAXPTS 255

typedef int fixED24_8; 

#define SCALEFACTOR 8
#define BIGFACTOR 256
#define BIGFLOATFACTOR 256.0
#define float_to_fixED24_8(f) (f*BIGFACTOR)
#define fixED24_8_to_float(f) (f/BIGFLOATFACTOR)

struct ptFIXed {
	fixED24_8 X, Y;
};

struct floatPt {
	float X, Y;
};

#ifdef OLDSTUFF
void circledraw(int centerx, int centery, int radius, COLORREF color);

void ellipsedraw(int centerx, int centery, int a, int b, COLORREF color);

void linedrawB(long x0, long y0, long x1, long y1, COLORREF color);

void trianglefill(COORD *points, COLORREF color);

void trianglefill_int(POINT *points, COLORREF color);

void trianglefill_fixED(floatPt *floatPt, COLORREF color);

void triangle_line(POINT *points, COLORREF color);

void polygon(POINT *Pts, int numPoints, COLORREF color);

int ceil_it(int n, int d);

int floor_it(int n, int d);

int mod_it(int n, int d);
#endif //OLDSTUFF

#endif
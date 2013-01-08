#ifndef _primitive_h
#define _primitive_h

#include "gdi.h"


/**********************************************************
;
;	Name:		DrawLine
;
;	Purpose:	draws a line from x0,y0 to x1,y1
;				*NOTE:* THIS FUNCTION ASSUMES THAT YOU'VE
;				LOCKED THE BACKBUFFER, use GraphicsLock();
;				On 8 bit, r is the palette index
;
;	Input:		(x0,y0) to (x0,y0) and the color
;
;	Output:		the backbuffer is updated
;
;	Return:		none
;
**********************************************************/
PUBLIC void DrawLine(long x0, long y0, long x1, long y1, int r, int g, int b);

/**********************************************************
;
;	Name:		DrawFilledCircle
;
;	Purpose:	draws a filled circle *NOTE* LOCK THE BACKBUFFER FIRST
;
;	Input:		center x&y and the radius as well as the r,g,b
;
;	Output:		the back buffer is filled
;
;	Return:		
;
**********************************************************/
PUBLIC void DrawFilledCircle(int centerx, int centery, int radius, int r, int g, int b);

/**********************************************************
;
;	Name:		DrawFilledEllipse
;
;	Purpose:	draws a filled ellipse
;				*NOTE* LOCK THE BACKBUFFER FIRST
;
;	Input:		the center x&y, a and b and rgb
;
;	Output:		the back buffer is filled
;
;	Return:		none
;
**********************************************************/
PUBLIC void DrawFilledEllipse(int centerx, int centery, int a, int b, int red, int green, int blue);

/**********************************************************
;
;	Name:		DrawFilledTriangle
;
;	Purpose:	draws a filled triangle
;				*NOTE* LOCK THE BACKBUFFER FIRST
;
;	Input:		an array of points, assumes that there are
;				three of them.  r,g,b
;
;	Output:		the back buffer is filled
;
;	Return:		none
;
**********************************************************/
PUBLIC void DrawFilledTriangle(POINT *points, int r, int g, int b);

/**********************************************************
;
;	Name:		DrawFilledPolygon
;
;	Purpose:	draws a filled polygon with the given points
;				*NOTE* LOCK THE BACKBUFFER FIRST
;
;	Input:		POINT *Pts, int numPoints, int r, int g, int b
;
;	Output:		the back buffer is filled
;
;	Return:		none
;
**********************************************************/
PUBLIC void DrawFilledPolygon(POINT *Pts, int numPoints, int r, int g, int b);

#endif
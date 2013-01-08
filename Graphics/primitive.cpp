#include "i_gdi.h"
#include "primitive.h"
#include "vector.h"

//PRIVATE PGRAPHICS Graphics;

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
PUBLIC void DrawLine(long x0, long y0, long x1, long y1, int r, int g, int b)
{
	//
	// Add line clipping next time!
	// 
	long x = x0;
	long y = y0;
	long dx = x1-x0;
	long dy = y1-y0;
	long d;
	//long dy2 = dy<<1;
	//long dx2 = dx<<1;
	long dx2, dy2;
	long de;
	long dne;
	long ystep;
	long xstep;

	BYTE *vd_ptr = GraphicsGetPixelPtr(x0, y0);

	ystep = dy > 0 ? GraphicsGetBufferPitch() : -GraphicsGetBufferPitch();
	xstep = dx > 0 ? GraphicsGetNumChannel() : -GraphicsGetNumChannel();


	//I don't want negative deltas!
	if(dx < 0)
	{
		dx = -dx;
		//x = x1;
	}

	if (dy < 0)
	{	
		//y = y1;
		dy = -dy;
	}
	
	dy2 = dy<<1;
	dx2 = dx<<1;
	
	
		//do the um...the b thing
		SetPixelColor(vd_ptr, r,g,b);// *vd_ptr = thecolor; //SetPixel(G_hdc, x, y, color);

		//if((dx > 0) && (dy > 0))
		//{
		if (dx > dy) //line that is not so steep
		{
			dne = (dy-dx)<<1;
			d = dy2 - dx;
			de = dy2;

			while(dx--)
			{
				vd_ptr+=xstep;

				if(d>=0)
				{
					vd_ptr+=ystep;
					d+=dne;
				}
				else
					d+=de;

				SetPixelColor(vd_ptr, r,g,b);
			}
		}

		else //if (dy > dx) //line that is very stiff...um steep
			{
				dne = (dx-dy)<<1; 
				d = dx2 - dy;
				de = dx2;

				while(dy--)
				{
					vd_ptr+=ystep;

					if(d>=0)
					{
						vd_ptr+=xstep;
						d+=dne;
					}
					else
					{
						
						d+=de;
					}

					SetPixelColor(vd_ptr, r,g,b);
				}
			}
}

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
PUBLIC void DrawFilledCircle(int centerx, int centery, int radius, int r, int g, int b)
{
	int x = radius;
	int y = 0;

	int d = 1 - radius; //some trick from the book

	int n = 3; //from 1st order of n
	int nw = -2*radius+5; //from 1st order of nw

	//now let's draw the first points
	//SetPixel(G_hdc, centerx + x, centery + y, color);
	//SetPixel(G_hdc, centerx - x, centery + y, color);
	DrawLine(centerx + x, centery + y, centerx - x, centery + y, r,g,b);
	
	//SetPixel(G_hdc, centerx - x, centery - y, color);
	//SetPixel(G_hdc, centerx + x, centery - y, color);
	DrawLine(centerx - x, centery - y, centerx + x, centery - y, r,g,b);
	
	//SetPixel(G_hdc, centerx + y, centery + x, color);
	//SetPixel(G_hdc, centerx + y, centery - x, color);
	DrawLine(centerx + y, centery + x, centerx + y, centery - x, r,g,b);
	
	//SetPixel(G_hdc, centerx - y, centery - x, color);
	//SetPixel(G_hdc, centerx - y, centery + x, color);
	DrawLine(centerx - y, centery - x, centerx - y, centery + x, r,g,b);
	//whew!

	while(x > y) //think of it as a clock
	{
		if(d < 0) //check for d to decide which way to go!
		{
			//move to north
			d+=n;

			//get the next deltas to find the next step (the 2nd order thing)
			n += 2;
			nw += 2; //only because we will get a wierd if we plug in 4
		}
		else
		{
			//move to the north west
			d+=nw;

			//get the next deltas to find the next step (the 2nd order thing)
			n += 2;
			nw += 4;

			//decrement x
			x--;
		}

		//moving up man!
		y++;

		//draw a whole bunch of pixels
		//SetPixel(G_hdc, centerx + x, centery + y, color);
		//SetPixel(G_hdc, centerx - x, centery + y, color);
		DrawLine(centerx + x, centery + y, centerx - x, centery + y, r,g,b);

		//SetPixel(G_hdc, centerx - x, centery - y, color);
		//SetPixel(G_hdc, centerx + x, centery - y, color);
		DrawLine(centerx - x, centery - y, centerx + x, centery - y, r,g,b);

		//SetPixel(G_hdc, centerx + y, centery + x, color);
		//SetPixel(G_hdc, centerx + y, centery - x, color);
		DrawLine(centerx + y, centery + x, centerx + y, centery - x, r,g,b);

		//SetPixel(G_hdc, centerx - y, centery - x, color);
		//SetPixel(G_hdc, centerx - y, centery + x, color);
		DrawLine(centerx - y, centery - x, centerx - y, centery + x, r,g,b);
		
		//whew!
	}
}

/**********************************************************
;
;	Name:		DrawFilledEllipse
;
;	Purpose:	draws a filled ellipse
;
;	Input:		the center x&y, a and b and rgb
;
;	Output:		the back buffer is filled
;
;	Return:		none
;
**********************************************************/
PUBLIC void DrawFilledEllipse(int centerx, int centery, int a, int b, int red, int green, int blue)
{
	int a2 = a*a;
	int b2 = b*b;

	int x = a;
	int y = 0;

	int d = (b2 - (4*x*b2) + (4*a2))>>2;

	int n = 3*a2; //from 1st order of n
	int nw = (3*b2) - (2*b2*x) + (3*a2); //from 1st order of nw

	int bx = b2*x;
	int ay = a2*y;

	//now let's draw the first points
	DrawLine(centerx + x, centery + y, centerx - x, centery + y, red,green,blue);
	DrawLine(centerx - x, centery - y, centerx + x, centery - y, red,green,blue);
	//whew!

	while(bx > ay) //think of it as a clock
	{
		if(d < 0) //check for d to decide which way to go!
		{
			//move to north
			d+=n;

			//get the next deltas to find the next step (the 2nd order thing)
			n += a2<<1;
			nw += a2<<1;
		}
		else
		{
			//move to the north west
			d+=nw;

			//get the next deltas to find the next step (the 2nd order thing)
			n += a2<<1;
			nw += (a2<<1) + (b2<<1);

			//decrement x
			x--;
			bx -= b2;
		}

		//moving up man!
		y++;
		ay += a2;

		//now let's draw the first points
		DrawLine(centerx + x, centery + y, centerx - x, centery + y, red,green,blue);
		DrawLine(centerx - x, centery - y, centerx + x, centery - y, red,green,blue);
		//whew!
	}

	//draw the top and bottom
	d = ((-8*x*b2) + (4*b2) +(8*a2*y) + a2)>>2;

	n = -2*b2*x + b2; //from 1st order of n
//	nw = (3*a2) - (2*a2*x) + (3*b2); //from 1st order of nw

	while(x > 0) //think of it as a clock
	{
		if(d < 0) //check for d to decide which way to go!
		{
			//move to north
			d+=nw;

			//get the next deltas to find the next step (the 2nd order thing)
			n += 2*b2;// a2<<1;
			nw += (a2<<1) + (b2<<1);
			
			y++;
		}
		else
		{
			//move to the north west
			d+=n;

			//get the next deltas to find the next step (the 2nd order thing)
			
			
			n += 2*b2;//a2<<1;
			nw += 2*b2;//a2<<1;
			//decrement x
			
			//ay -= b2;
		}

		//moving up man!
		x--;
		//bx += a2;

		//now let's draw the first points
		DrawLine(centerx + x, centery + y, centerx - x, centery + y, red,green,blue);
		DrawLine(centerx - x, centery - y, centerx + x, centery - y, red,green,blue);
		//whew!
	}

}

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
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC void DrawFilledTriangle(POINT *points, int r, int g, int b)
{
	//let's just assume the user passed in an array of 3 points


	//our_points[3];
	//COORD the_slope[3];
	int top, middle, bottom;
	int leftslope, rightslope;
	//int numerator, denominator, increment = 0; //used later so that we don't have to divide anything
	float the_inv_slope[3];
	bool IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn;
	float x_left, x_right;

	//get the orders correctly, check from top to bottom
	if(points[0].y < points[1].y)
	{
		top = 0;
		bottom = 1;

		if(points[2].y < points[0].y)
		{
			top = 2;
			middle = 0;
			bottom = 1;
		} 
		else if(points[2].y < points[1].y)
			middle = 2;
		else
		{ 
			middle = 1;
			bottom = 2;
		}
			
	}
	else
	{
		top = 1;
		bottom = 0;

		if(points[2].y < points[1].y)
		{
			top = 2;
			middle = 1;
			bottom = 0;
		}
		else if(points[2].y < points[0].y)
			middle = 2;
		else
		{ 
			middle = 0;
			bottom = 2;
		}
			
	}

	//caliculate the slopes
	the_inv_slope[0] = (float)(points[top].x - points[middle].x)/(points[top].y - points[middle].y);
	the_inv_slope[1] = (float)(points[middle].x - points[bottom].x)/(points[middle].y - points[bottom].y);
	the_inv_slope[2] = (float)(points[top].x - points[bottom].x)/(points[top].y - points[bottom].y);

	//if(points[middle].x < points[top].x)
	if(the_inv_slope[0] < the_inv_slope[2])
	{
		IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn = 1;

		leftslope = 0;
		rightslope = 2;
	}
	else
	{
		IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn = 0;

		leftslope = 2;
		rightslope = 0;
	}

	//AAAAAAARRRRRRRRRRGGGGGGGGGGGGGGGGGG!!!!!!!!!!
	if(points[top].y == points[middle].y)
	{
		if(points[middle].x < points[top].x)
		{
			IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn = 1;

			leftslope = 0;
			rightslope = 2;
		}
		else
		{
			IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn = 0;

			leftslope = 2;
			rightslope = 0;
		}
	}

	//insert code here
	int y;
	int channel = GraphicsGetNumChannel();
	int pitch = GraphicsGetBufferPitch()/channel;

	x_left = (float)(points[top].x);
	x_right = (float)(points[top].x);

	BYTE *vd_ptr = GraphicsGetPixelPtr(points[top].x, points[top].y);

	//x_left = 0;//the_inv_slope[leftslope];
	//x_right = 0;//the_inv_slope[rightslope];

	for(y = points[top].y; y < points[middle].y; y++)
	{
		x_left += the_inv_slope[leftslope];
		x_right += the_inv_slope[rightslope];
		vd_ptr = GraphicsGetPixelPtr((int)x_left, y);

		for(float x = x_left; x < x_right; x++)
		{
			//SetPixel(G_hdc, (int)x, y, color);
			SetPixelColor(vd_ptr, r,g,b);
			vd_ptr+=channel;
		}
	}

	if(x_left >= x_right)
	{
		if(points[top].x > points[middle].x)
			x_left = (float)(points[middle].x);
		else
			x_right = (float)(points[middle].x);
	}

	if(IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn)
	{
		//if(points[top].y != points[middle].y)
			leftslope = 1;
	}
	else //I guess not!
	{
		//if(points[top].y != points[middle].y)
			rightslope = 1;
	}

	for(y = points[middle].y; y < points[bottom].y; y++)
	{
		x_left += the_inv_slope[leftslope];
		x_right += the_inv_slope[rightslope];
		vd_ptr = GraphicsGetPixelPtr((int)x_left, y);

		for(float x = x_left; x < x_right; x++)
		{
			//SetPixel(G_hdc, (int)x, y, color);
			SetPixelColor(vd_ptr, r,g,b);
			vd_ptr+=channel;
		}
	}
}

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
PUBLIC void DrawFilledPolygon(POINT *Pts, int numPoints, int r, int g, int b)
{
	if(numPoints == 0)
		return;
	else if(numPoints == 1)
	{ BYTE *vd_ptr = GraphicsGetPixelPtr(Pts[0].x, Pts[0].y);  SetPixelColor(vd_ptr, r,g,b); return; }
	else if(numPoints == 2)
	{ DrawLine(Pts[0].x, Pts[0].y, Pts[1].x, Pts[1].y, r,g,b); return; }
	else if(numPoints == 3)
	{ DrawFilledTriangle(Pts, r,g,b); return; }

	int numTriangle = numPoints - 2;
	Vector2D v1, v2;
	bool isConvex = true; //assume it is...hopefully...

	//check to see if the triangle is concave or convex
	int max = numTriangle;
	for(int i = 0; i < max; i++)
	{
		v1.setXY(Pts[i].x - Pts[i+1].x, Pts[i].y - Pts[i+1].y);
		v2.setXY(Pts[i+2].x - Pts[i+1].x, Pts[i+2].y - Pts[i+1].y);
		if(v1.CrossProd(v2) < 0) //ah shit, it's a concave poly
		{ isConvex = false; goto DONECHECKING; }
	}

	//check for the last two
	v1.setXY(Pts[numPoints-2].x - Pts[numPoints-1].x, Pts[numPoints-2].y - Pts[numPoints-1].y);
	v2.setXY(Pts[0].x - Pts[numPoints-1].x, Pts[0].y - Pts[numPoints-1].y);
	if(v1.CrossProd(v2) < 0) //damn!
		isConvex = false;

	v1.setXY(Pts[0].x - Pts[numPoints-1].x, Pts[0].y - Pts[numPoints-1].y);
	v2.setXY(Pts[1].x - Pts[0].x, Pts[1].y - Pts[0].y);
	if(v1.CrossProd(v2) < 0) //damn!
		isConvex = false;

DONECHECKING:
	if(isConvex)
	{
		POINT TriPts[3];
		//triangulate them all!
		for(int j = 0; j < numTriangle; j++)
		{
			TriPts[0] = Pts[0];
			TriPts[1] = Pts[j+1];
			TriPts[2] = Pts[j+2];
			DrawFilledTriangle(TriPts, r,g,b);
		}
	}
	else
	{
		//get the lowest point
		POINT IamTheLowest = Pts[0]; //assume that index 0 is the lowest
		int lowestIndex = 0;
		for(int k = 1; k < numPoints; k++)
		{
			if(Pts[k].y > IamTheLowest.y)
			{ IamTheLowest = Pts[k]; lowestIndex = k; }
			else if(Pts[k].y == IamTheLowest.y && Pts[k].x > IamTheLowest.x)
			{ IamTheLowest = Pts[k]; lowestIndex = k; }
		}

		//get the next point and the previous point
		POINT next, prev;
		int nextIndex, prevIndex;
		if(lowestIndex == 0) //there is no such thing as a -1 array
			prevIndex = numPoints - 1;
		else //other wise prev is the well...the previous index
			prevIndex = lowestIndex - 1;

		if(lowestIndex == numPoints - 1) //make sure we don't go over the array
			nextIndex = 0;
		else
			nextIndex = lowestIndex + 1;

		prev = Pts[prevIndex];
		next = Pts[nextIndex];

		POINT *thing = NULL;
		int thingIndex = 0;
		//Vector2D v1, v2;

		//now for the pain in the @$$ part.
		//look for the lowest pt. inside the three points
		for(int l = 0; l < numPoints; l++)
		{
			//exclude the three pts
			if(l != lowestIndex && l != prevIndex && l != nextIndex)
			{
				//now check for the lowest within the three pts.
				//first get their vector
				if(Pts[l].x > prev.x && Pts[l].x < next.x)
				{
					v1.setXY(Pts[l].x-prev.x, Pts[l].y-prev.y);
					v2.setXY(next.x-prev.x, next.y-prev.y);
					//that means the Pt. is within
					if(v1.CrossProd(v2) < 0)
					{
						//check for the lowest
						if(thing != NULL)
						{
							if(Pts[l].y > thing->y)
							{ thing = &Pts[l]; thingIndex = l; }
							else if(Pts[l].y == thing->y && Pts[l].x > thing->x)
							{ thing = &Pts[l]; thingIndex = l; }
						}
						else
						{ thing = &Pts[l]; thingIndex = l; }
					}
				}
			}
		}

		//split up the poly in different ways
		if(thing) //oh crap...
		{
			int index, newNumPts1 = 0, newNumPts2 = 0;
			POINT *newPts1, *newPts2;

			//damn, we don't know how many points the new polies have
			//so we must go through the loops twice, ugh!

			//first we go through the left split
			//if(thingIndex == numPoints-1)
			//	index = 0;
			//else
				index = thingIndex;

			int buga = index;

			while(1) //sigh...for the sake of getting the number of points
			{
				
				if(buga != lowestIndex)
				{
					newNumPts1++;
					buga++;

					if(buga == numPoints)
						buga = 0;
				}
				else
				{ newNumPts1++; break; }
				
				
			}

			newPts1 = (POINT*)malloc(sizeof(POINT)*newNumPts1);
			assert(newPts1);
			//fill it with crap
			for(int n = 0; n < newNumPts1; n++)
			{
				newPts1[n] = Pts[index];

				index++;

				if(index == numPoints)
					index = 0;
			}

			//now for the other half
			//if(lowestIndex == numPoints-1)
			//	index = 0;
			//else
				index = lowestIndex;

			buga = index;

			while(1) //sigh...for the sake of getting the number of points
			{
				
				if(buga != thingIndex)
				{
					newNumPts2++;
					buga++;

					if(buga == numPoints)
						buga = 0;
				}
				else
				{ newNumPts2++; break; }
				
				//else 
			}

			newPts2 = (POINT*)malloc(sizeof(POINT)*newNumPts2);
			assert(newPts2);
			//fill it with crap
			for(int o = 0; o < newNumPts2; o++)
			{
				newPts2[o] = Pts[index];

				index++;

				if(index == numPoints)
					index = 0;
			}

			//hurray!
			DrawFilledPolygon(newPts1, newNumPts1, r,g,b);
			DrawFilledPolygon(newPts2, newNumPts2, r,g,b);

			//at last
			free(newPts1);
			free(newPts2);
		}
		else
		{
			int index, newNumPts;
			POINT *newPts;
			POINT triPts[3];
			triPts[0] = prev;
			triPts[1] = IamTheLowest;
			triPts[2] = next;

			//allocate the newPts, yuck!
			//at least we know that the new poly has one less point
			newNumPts = numPoints-1;
			newPts = (POINT*)malloc(sizeof(POINT)*newNumPts);
			assert(newPts);

			//get the first index
			//if(nextIndex == numPoints-1)
			//	index = 0;
			//else
				index = nextIndex;

			for(int m = 0;m < newNumPts; m++, index++)
			{
				if(index == numPoints)
					index = 0;

				newPts[m] = Pts[index];

				//index++;

				
			}

			//display the triangle
			//polygon(triPts, 3, color);
			DrawFilledPolygon(triPts, 3, r,g,b);
			//go through the whole pain in the ass process baby, yeah!
			DrawFilledPolygon(newPts, newNumPts, r,g,b);
			
			//free the bastard
			free(newPts);
		}

	}
}
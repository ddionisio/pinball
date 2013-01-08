/*



*/

/*#include <windows.h>
#include <string.h>
#include <stdio.h>*/
#include "game.h"
#include "hello.h"

RETCODE load_config(char *filename, HWND *hwnd, WNDPROC WinCallBack, HINSTANCE hinstance)
{
	FILE *theFile = NULL;
	theFile = fopen(filename, "rt");// FileOpenFile (filename, FILEREAD | FILETEXT);
	if(!theFile)
	{ MESSAGE_BOX("Unable to open file: load_gamecfg failed", "Zoink!"); return RETCODE_FAILURE; }

	int isFullscreen;
	int gameDelay, frameDelay;
	VTYPE vidType;
	char pathBuff[MAXCHARBUFF];

	fscanf(theFile, "fullscreen=%d\n", &isFullscreen);
	fscanf(theFile, "gameDelay=%d\n", &gameDelay);
	fscanf(theFile, "frameDelay=%d\n", &frameDelay);
	fscanf(theFile, "game file=%s\n", pathBuff);

	if(isFullscreen == 1)
	{
		vidType = VDTYPE_FULLSCREEN;

		*hwnd = Create_Window_Popup("Pinball...sort of", WinCallBack, hinstance, NULL, NULL, 
								    0, 0, 640, 480);
	}
	else
	{
		vidType = VDTYPE_WINDOWED;

		*hwnd = Create_Window_Overlapped("Pinball...sort of", WinCallBack, hinstance, NULL, NULL, 
								    0, 0, 648, 480);
	}

	if(init_game(vidType, pathBuff, gameDelay, frameDelay) != RETCODE_SUCCESS)
		return RETCODE_FAILURE;

	fclose(theFile);

	return RETCODE_SUCCESS;
}

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam,
        LPARAM lparam);


int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hprev, PSTR cmdline,
        int ishow)
{
    HWND hwnd;
    MSG msg;

    
    //hwnd = Create_Window_Overlapped("Pinball...sort of", WinProc, hinstance, NULL, NULL, 
	//							    0, 0, 640, 480);

	//Initialize everything
	if(load_config("main.cfg", &hwnd, WinProc, hinstance) != RETCODE_SUCCESS)
	{ MESSAGE_BOX("Shit happens, exiting game now...", "ARG!"); goto HECK; }
	//init_game();
    
    ShowWindow(hwnd, ishow);
    UpdateWindow(hwnd);

    RETCODE ret_msg;
    while(1)
	{
		if (!(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)))
		{
			//do something
			ret_msg = update_stuff();
			if(ret_msg == RETCODE_FAILURE)
			{ MESSAGE_BOX("Something really bad happened in the game loop", "oh boy"); goto HECK; }
			else if(ret_msg == RETCODE_REQUESTTERMINATE)
				goto HECK; //quit anyways
		}
		else
		{
			if (msg.message == WM_QUIT)
			{
				break;
			}
			else
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
			}
		}
	}

HECK:
	destroy_game();
    return(msg.wParam);
}

#ifdef OLDSTUFF
HDC G_hdc;
#endif OLDSTUFF

LRESULT CALLBACK WinProc(HWND hwnd, UINT message, WPARAM wparam,
        LPARAM lparam)
{
    //HDC hdc;
    /*PAINTSTRUCT ps;
    RECT rect;
	//static COORD thisthat [3] = {{100, 50}, {50, 100}, {120, 200}};
	static POINT thisthat [MAXPTS];// = {{0, 0}, {50, 150}, {25, 200}, {125, 200}, {150, 150}};
	//static int numPts = 5;
	//static COORD thisthat [3] = {{50, 100}, {100, 50}, {120, 200}};
	//static COORD thisthat [3] = {{100, 50}, {50, 100}, {120, 100}};
	//static COORD thisthat [3] = {{50, 26}, {50, 100}, {120, 200}};
	//static COORD thisthat [3] = {{273, 138}, {716, 138}, {668, 227}};
	static current = 0;
	static bool drawit = false;
	char position[255];*/

    switch (message)
    {
        
        case WM_CREATE:  
			// Our window position has changed, so
            // get the client (drawing) rectangle.
            GetClientRect( G_hwnd, &G_clientarea );
            // Convert the coordinates from client relative
            // to screen
            ClientToScreen( G_hwnd, ( LPPOINT )&G_clientarea);
            ClientToScreen( G_hwnd, ( LPPOINT )&G_clientarea + 1 );
			return 0;

		case WM_MOVE:
            // Our window position has changed, so
            // get the client (drawing) rectangle.
            GetClientRect( G_hwnd, &G_clientarea);
            // Convert the coordinates from client relative
            // to screen
            ClientToScreen( G_hwnd, ( LPPOINT )&G_clientarea);
            ClientToScreen( G_hwnd, ( LPPOINT )&G_clientarea + 1 );
            return 0;

        
        case WM_DESTROY:
            
            PostQuitMessage(0);
            return 0;
    }

    
    return(DefWindowProc(hwnd, message, wparam, lparam));
}

#ifdef OLDSTUFF
void circledraw(int centerx, int centery, int radius, COLORREF color)
{
	int x = radius;
	int y = 0;

	int d = 1 - radius; //some trick from the book

	int n = 3; //from 1st order of n
	int nw = -2*radius+5; //from 1st order of nw

	//now let's draw the first points
	//SetPixel(G_hdc, centerx + x, centery + y, color);
	//SetPixel(G_hdc, centerx - x, centery + y, color);
	linedrawB(centerx + x, centery + y, centerx - x, centery + y, color);
	
	//SetPixel(G_hdc, centerx - x, centery - y, color);
	//SetPixel(G_hdc, centerx + x, centery - y, color);
	linedrawB(centerx - x, centery - y, centerx + x, centery - y, color);
	
	//SetPixel(G_hdc, centerx + y, centery + x, color);
	//SetPixel(G_hdc, centerx + y, centery - x, color);
	linedrawB(centerx + y, centery + x, centerx + y, centery - x, color);
	
	//SetPixel(G_hdc, centerx - y, centery - x, color);
	//SetPixel(G_hdc, centerx - y, centery + x, color);
	linedrawB(centerx - y, centery - x, centerx - y, centery + x, color);
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
		linedrawB(centerx + x, centery + y, centerx - x, centery + y, color);

		//SetPixel(G_hdc, centerx - x, centery - y, color);
		//SetPixel(G_hdc, centerx + x, centery - y, color);
		linedrawB(centerx - x, centery - y, centerx + x, centery - y, color);

		//SetPixel(G_hdc, centerx + y, centery + x, color);
		//SetPixel(G_hdc, centerx + y, centery - x, color);
		linedrawB(centerx + y, centery + x, centerx + y, centery - x, color);

		//SetPixel(G_hdc, centerx - y, centery - x, color);
		//SetPixel(G_hdc, centerx - y, centery + x, color);
		linedrawB(centerx - y, centery - x, centerx - y, centery + x, color);
		
		//whew!
	}
}

void ellipsedraw(int centerx, int centery, int a, int b, COLORREF color)
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
	SetPixel(G_hdc, centerx + x, centery + y, color);
	SetPixel(G_hdc, centerx - x, centery + y, color);
	SetPixel(G_hdc, centerx - x, centery - y, color);
	SetPixel(G_hdc, centerx + x, centery - y, color);
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
		SetPixel(G_hdc, centerx + x, centery + y, color);
		SetPixel(G_hdc, centerx - x, centery + y, color);
		SetPixel(G_hdc, centerx - x, centery - y, color);
		SetPixel(G_hdc, centerx + x, centery - y, color);	
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
		SetPixel(G_hdc, centerx + x, centery + y, color);
		SetPixel(G_hdc, centerx - x, centery + y, color);
		SetPixel(G_hdc, centerx - x, centery - y, color);
		SetPixel(G_hdc, centerx + x, centery - y, color);	
		//whew!
	}

}

void linedrawB(long x0, long y0, long x1, long y1, COLORREF color)
{
	long x = x0;
	long y = y0;
	long xother = x1;
	long yother = y1;
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

	ystep = dy > 0 ? 1 : -1;
	xstep = dx > 0 ? 1 : -1;

	//if(dx > 0 && dy > 0)
	//{ d = (dy-dx)<<1; dne = de - (dx<<1); }
	//else if(dx>0 && dy<0)
	//{ d = (dx-dy)<<1; de = dy<<1; dne = de + (dy<<1); }	


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

	dx = dx>>1;
	dy = dy>>1;

	
	dy2 = dy<<1;
	dx2 = dx<<1;
	
	
		//do the um...the b thing
		SetPixel(G_hdc, x, y, color);
		SetPixel(G_hdc, xother, yother, color);

		//if((dx > 0) && (dy > 0))
		//{
		if (dx > dy) //line that is not so steep
		{
			dne = (dy-dx)<<1;
			d = dy2 - dx;
			de = dy2;

			while(dx--)
			{
				x+=xstep;
				xother -= xstep;

				if(d>=0)
				{
					y+=ystep;
					yother -= ystep;
					d+=dne;
				}
				else
					d+=de;

				SetPixel(G_hdc, x,y,color);
				SetPixel(G_hdc, xother, yother, color);
			}
		}

		else //if (dy > dx) //line that is very stiff...um steep
			{
				dne = (dx-dy)<<1; 
				d = dx2 - dy;
				de = dx2;

				while(dy--)
				{
					y+=ystep;
					yother -= ystep;

					if(d>=0)
					{
						x+=xstep;
						xother-=xstep;
						d+=dne;
					}
					else
					{
						
						d+=de;
					}

					SetPixel(G_hdc, x,y,color);
					SetPixel(G_hdc, xother, yother, color);
				}
			}
		//}
		/*else if ((dx < 0) && (dy < 0))
		{
			while(dx--)
			{
				x+=xstep;

				if(d<0)
				{
					y+=ystep;
					d+=dne;
				}
				else
					d+=de;

				SetPixel(G_hdc, x,y,RGB(0,0,0));
			}
		}*/
}

void trianglefill(COORD *points, COLORREF color)
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
	if(points[0].Y < points[1].Y)
	{
		top = 0;
		bottom = 1;

		if(points[2].Y < points[0].Y)
		{
			top = 2;
			middle = 0;
			bottom = 1;
		} 
		else if(points[2].Y < points[1].Y)
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

		if(points[2].Y < points[1].Y)
		{
			top = 2;
			middle = 1;
			bottom = 0;
		}
		else if(points[2].Y < points[0].Y)
			middle = 2;
		else
		{ 
			middle = 0;
			bottom = 2;
		}
			
	}

	//caliculate the slopes
	the_inv_slope[0] = (float)(points[top].X - points[middle].X)/(points[top].Y - points[middle].Y);
	the_inv_slope[1] = (float)(points[middle].X - points[bottom].X)/(points[middle].Y - points[bottom].Y);
	the_inv_slope[2] = (float)(points[top].X - points[bottom].X)/(points[top].Y - points[bottom].Y);

	//if(points[middle].X < points[top].X)
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
	if(points[top].Y == points[middle].Y)
	{
		if(points[middle].X < points[top].X)
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

	x_left = points[top].X;
	x_right = points[top].X;

	//insert code here
	int y;

	//x_left = 0;//the_inv_slope[leftslope];
	//x_right = 0;//the_inv_slope[rightslope];

	for(y = points[top].Y; y < points[middle].Y; y++)
	{
		x_left += the_inv_slope[leftslope];
		x_right += the_inv_slope[rightslope];

		for(float x = x_left; x < x_right; x++)
		{
			SetPixel(G_hdc, (int)x, y, color);
		}
	}

	if(x_left >= x_right)
	{
		if(points[top].X > points[middle].X)
			x_left = points[middle].X;
		else
			x_right = points[middle].X;	
	}

	if(IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn)
	{
		//if(points[top].Y != points[middle].Y)
			leftslope = 1;
	}
	else //I guess not!
	{
		//if(points[top].Y != points[middle].Y)
			rightslope = 1;
	}

	for(y = points[middle].Y; y < points[bottom].Y; y++)
	{
		x_left += the_inv_slope[leftslope];
		x_right += the_inv_slope[rightslope];

		for(float x = x_left; x < x_right; x++)
		{
			SetPixel(G_hdc, (int)x, y, color);
		}
	}


}

void trianglefill_int(POINT *points, COLORREF color)
{
	//let's just assume the user passed in an array of 3 points


	//our_points[3];
	//COORD the_slope[3];
	int top, middle, bottom;
	int leftslope, rightslope;
	int the_inv_slope[3];
	int dx[3];
	int dy[3];
	bool IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn;

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

	//caliculate the deltas
	dx[0] = points[top].x - points[middle].x;
	dx[1] = points[middle].x - points[bottom].x;
	dx[2] = points[top].x - points[bottom].x;

	dy[0] = points[top].y - points[middle].y;
	dy[1] = points[middle].y - points[bottom].y;
	dy[2] = points[top].y - points[bottom].y;

	//caliculate the slopes
	the_inv_slope[0] = floor_it(dx[0], dy[0]);
	the_inv_slope[1] = floor_it(dx[1], dy[1]);
	the_inv_slope[2] = floor_it(dx[2], dy[2]);

	//if(points[middle].x < points[top].x)
	//if(dx[0] * dy[0] < dx[2] * dy[2])
	if(dx[0] * dy[2] < dx[2] * dy[0])
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

	//x_left = 0;//the_inv_slope[leftslope];
	//x_right = 0;//the_inv_slope[rightslope];
	int n_left,n_right;
	int x_left, x_right;


	n_left = -1; //dx(y-y0) - 1 -> y starts at y0 so...
	n_right = -1; //dx(y-y0) - 1 -> y starts at y0 so...

	//x_left = floor_it(n_left,dy[leftslope]) + 1 + points[top].x;
	//x_right = floor_it(n_right,dy[rightslope]) + 1 + points[top].x;
	x_left = points[top].x;
	x_right = points[top].x;

	for(y = points[top].y; y < points[middle].y; y++)
	{
		for(int x = x_left; x < x_right; x++)
		{
			SetPixel(G_hdc, x, y, color);
		}

		n_left += dx[leftslope];
		n_right += dx[rightslope];

		x_left += the_inv_slope[leftslope] + floor_it(mod_it(n_left, dy[leftslope]) + mod_it(dx[leftslope], dy[leftslope]), dy[leftslope]);
		x_right += the_inv_slope[rightslope] + floor_it(mod_it(n_right, dy[rightslope]) + mod_it(dx[rightslope], dy[rightslope]), dy[rightslope]);

		
	}

	if(x_left >= x_right)
	{
		if(points[top].x > points[middle].x)
			x_left = points[middle].x;
		else
			x_right = points[middle].x;	
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
		for(int x = x_left; x < x_right; x++)
		{
			SetPixel(G_hdc, x, y, color);
		}

		n_left += dx[leftslope];
		n_right += dx[rightslope];

		x_left += the_inv_slope[leftslope] + floor_it(mod_it(n_left, dy[leftslope]) + mod_it(dx[leftslope], dy[leftslope]), dy[leftslope]);
		x_right += the_inv_slope[rightslope] + floor_it(mod_it(n_right, dy[rightslope]) + mod_it(dx[rightslope], dy[rightslope]), dy[rightslope]);

		
	}
}

void trianglefill_fixED(floatPt *points, COLORREF color)
{
	//let's just assume the user passed in an array of 3 points


	//our_points[3];
	//COORD the_slope[3];
	int top, middle, bottom;
	int leftslope, rightslope;
	ptFIXed ptfixED[3]; //fixED version of the points
	int dm[3];
	int dn[3];
	bool IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn;

	//get the orders correctly, check from top to bottom
	if(points[0].Y < points[1].Y)
	{
		top = 0;
		bottom = 1;

		if(points[2].Y < points[0].Y)
		{
			top = 2;
			middle = 0;
			bottom = 1;
		} 
		else if(points[2].Y < points[1].Y)
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

		if(points[2].Y < points[1].Y)
		{
			top = 2;
			middle = 1;
			bottom = 0;
		}
		else if(points[2].Y < points[0].Y)
			middle = 2;
		else
		{ 
			middle = 0;
			bottom = 2;
		}
			
	}

	//if(points[middle].X < points[top].X)
	//if(dx[0] * dy[0] < dx[2] * dy[2])
	if((points[top].X - points[middle].X) * (points[top].Y - points[bottom].Y) 
		< (points[top].X - points[bottom].X) * (points[top].Y - points[middle].Y))
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
	if(points[top].Y == points[middle].Y)
	{
		if(points[middle].X < points[top].X)
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

	ptfixED[0].X = float_to_fixED24_8(points[0].X);
	ptfixED[0].Y = float_to_fixED24_8(points[0].Y);
	ptfixED[1].X = float_to_fixED24_8(points[1].X);
	ptfixED[1].Y = float_to_fixED24_8(points[1].Y);
	ptfixED[2].X = float_to_fixED24_8(points[2].X);
	ptfixED[2].Y = float_to_fixED24_8(points[2].Y);

	//caliculate the deltas
	dm[0] = ptfixED[top].X - ptfixED[middle].X;
	dm[1] = ptfixED[middle].X - ptfixED[bottom].X;
	dm[2] = ptfixED[top].X - ptfixED[bottom].X;

	dn[0] = ptfixED[top].Y - ptfixED[middle].Y;
	dn[1] = ptfixED[middle].Y - ptfixED[bottom].Y;
	dn[2] = ptfixED[top].Y - ptfixED[bottom].Y;


	//insert code here
	int y;
	int g_left,g_right;
	int x_left, x_right;


	g_left = ((dm[leftslope]>>SCALEFACTOR)*(BIGFACTOR*points[top].Y - ptfixED[top].Y)) + (dn[leftslope]*ptfixED[top].X - 1) + (dn[leftslope]>>SCALEFACTOR);
	g_right = ((dm[rightslope]>>SCALEFACTOR)*(BIGFACTOR*points[top].Y - ptfixED[top].Y)) + (dn[rightslope]*ptfixED[top].X - 1) + (dn[rightslope]>>SCALEFACTOR);

	x_left = points[top].X;
	x_right = points[top].X;

	for(y = points[top].Y; y < points[middle].Y; y++)
	{
		for(int x = x_left; x < x_right; x++)
		{
			SetPixel(G_hdc, x, y, RGB((log(y+x)*(y-x)),  (log(y+x)*(x-y)), (log(y+x)*(x+y))));
		}

		g_left += dm[leftslope]>>SCALEFACTOR;
		g_right += dm[rightslope]>>SCALEFACTOR;

		x_left += floor_it(dm[leftslope]>>SCALEFACTOR, dn[leftslope]>>SCALEFACTOR) + floor_it(mod_it(g_left, dn[leftslope]>>SCALEFACTOR) + mod_it(dm[leftslope]>>SCALEFACTOR, dn[leftslope]>>SCALEFACTOR), dn[leftslope]>>SCALEFACTOR);
		x_right += floor_it(dm[rightslope]>>SCALEFACTOR, dn[rightslope]>>SCALEFACTOR) + floor_it(mod_it(g_right, dn[rightslope]>>SCALEFACTOR) + mod_it(dm[rightslope]>>SCALEFACTOR, dn[rightslope]>>SCALEFACTOR), dn[rightslope]>>SCALEFACTOR);
	}

	if(x_left >= x_right)
	{
		if(points[top].X > points[middle].X)
			x_left = points[middle].X;
		else
			x_right = points[middle].X;	
	}


	if(IsMiddleVertexToLeftOfTheTriangeThatTheUserPassedIn)
	{
		//if(points[top].Y != points[middle].Y)
			leftslope = 1;
	}
	else //I guess not!
	{
		//if(points[top].Y != points[middle].Y)
			rightslope = 1;
	}


	for(y = points[middle].Y; y < points[bottom].Y; y++)
	{
		for(int x = x_left; x < x_right; x++)
		{
			SetPixel(G_hdc, x, y, RGB((log(y+x)*(y-x)),  (log(y+x)*(x-y)), (log(y+x)*(x+y))));
		}

		g_left += dm[leftslope]>>SCALEFACTOR;
		g_right += dm[rightslope]>>SCALEFACTOR;

		x_left += floor_it(dm[leftslope]>>SCALEFACTOR, dn[leftslope]>>SCALEFACTOR) + floor_it(mod_it(g_left, dn[leftslope]>>SCALEFACTOR) + mod_it(dm[leftslope]>>SCALEFACTOR, dn[leftslope]>>SCALEFACTOR), dn[leftslope]>>SCALEFACTOR);
		x_right += floor_it(dm[rightslope]>>SCALEFACTOR, dn[rightslope]>>SCALEFACTOR) + floor_it(mod_it(g_right, dn[rightslope]>>SCALEFACTOR) + mod_it(dm[rightslope]>>SCALEFACTOR, dn[rightslope]>>SCALEFACTOR), dn[rightslope]>>SCALEFACTOR);
	}
}

void triangle_line(POINT *points, COLORREF color)
{
	linedrawB(points[0].x, points[0].y, points[1].x, points[1].y, color);
	linedrawB(points[1].x, points[1].y, points[2].x, points[2].y, color);
	linedrawB(points[2].x, points[2].y, points[0].x, points[0].y, color);
}

void polygon(POINT *Pts, int numPoints, COLORREF color)
{
	if(numPoints == 0)
		return;
	else if(numPoints == 1)
	{ SetPixel(G_hdc, Pts[0].x, Pts[0].y, color); return; }
	else if(numPoints == 2)
	{ linedrawB(Pts[0].x, Pts[0].y, Pts[1].x, Pts[1].y, color); return; }
	else if(numPoints == 3)
	{ triangle_line(Pts, color); return; }

	int numTriangle = numPoints - 2;
	Vector2D v1, v2;
	bool isConvex = true; //assume it is...hopefully...

	//check to see if the triangle is concave or convex
	int max = numTriangle;
	for(int i = 0; i < max; i++)
	{
		v1.setXY(Pts[i+1].x - Pts[i].x, Pts[i+1].y - Pts[i].y);
		v2.setXY(Pts[i+1].x - Pts[i+2].x, Pts[i+1].y - Pts[i+2].y);
		if(v2.CrossProd(v1) < 0) //ah shit, it's a concave poly
		{ isConvex = false; goto DONECHECKING; }
	}

	//check for the last one
	v1.setXY(Pts[numPoints-1].x - Pts[0].x, Pts[numPoints-1].y - Pts[0].y);
	v2.setXY(Pts[0].x - Pts[1].x, Pts[0].y - Pts[1].y);
	if(v2.CrossProd(v1) < 0) //damn!
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
			triangle_line(TriPts, color);
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
				v1.setXY(prev.x-next.x, prev.y-next.y);
				v2.setXY(prev.x-Pts[l].x, prev.y-Pts[l].y);
				//that means the Pt. is within
				if(v2.CrossProd(v1) < 0)
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
			polygon(newPts1, newNumPts1, color);
			polygon(newPts2, newNumPts2, color);

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
			polygon(triPts, 3, color);
			//go through the whole pain in the ass process baby, yeah!
			polygon(newPts, newNumPts, color);
			
			//free the bastard
			free(newPts);
		}

	}
}

int ceil_it(int n, int d)
{
	if(d == 0)
		return 0;

	int thing = n/d;

	if(d*thing == n)
		return thing;
	else if(thing > 0)
		return thing + 1;
	else
		return thing;
}

int floor_it(int n, int d)
{
	if(d == 0)
		return 0;

	int thing = n/d;

	if(d*thing == n)
		return thing;
	else if(thing == 0)
	{
		if(n*d > 0)
			return 0;
		else 
			return -1;
	}
	else if(thing > 0)
		return thing;
	else
		return thing - 1;
}

int mod_it(int n, int d)
{
	//if(n < 0)
		return n - d*floor_it(n,d);
	//	return -((-n)%d);
	//else
	//	return n%d;
}
#endif OLDSTUFF

/***********************************************************************************************

		Purpose  : To get yet another good grade in CS160
		Author   : David Dionisio
		Revision : 3/27/00
		Version  : 1

 **********************************************************************************************/

#ifndef _mywin_h
#define _mywin_h

/* ////////////////////////////////////////////////////////////////
defines
//////////////////////////////////////////////////////////////// */
//we want to be LEAN AND MEAN!
#define WIN32_LEAN_AND_MEAN


/* ////////////////////////////////////////////////////////////////
includes
     //////////////////////////////////////////////////////////////// */
//#include "themain.h"
#include <windows.h>
#include <windowsx.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
//#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#include <math.h>
#include <commdlg.h>
#include <time.h>

#define NOTHINGATALL 0

//Gets the index of an array as if it was a double array
#define BOARDPOS(board, row, col, numcols) (board[((row) * (numcols)) + (col)])

//These are global stuff so that we can access it anywhere
extern HWND G_hwnd;
extern HINSTANCE G_hinstance;
extern RECT G_clientarea;

/* ////////////////////////////////////////////////////////////////
data structs from Bruce McQuistin's example
     //////////////////////////////////////////////////////////////// */
typedef struct __LOADICON_ARGS {
        HINSTANCE       Arg1;
        CONST CHAR *    Flags;
        } LOADICON_ARGS, *PLOADICON_ARGS;

typedef struct __LOADCURSOR_ARGS {
        HINSTANCE       Arg1;
        CONST CHAR *    Flags;
        } LOADCURSOR_ARGS, *PLOADCURSOR_ARGS;



/* ////////////////////////////////////////////////////////////////
prototypes
     //////////////////////////////////////////////////////////////// */
VOID
InitWndClass(
    INT                 Style,
    WNDPROC             WinCallBack,
    INT                 ClsExtra,
    INT                 WndExtra,
    HINSTANCE           HInstance,
    PLOADICON_ARGS      LoadIconArgs1,
    PLOADCURSOR_ARGS    LoadCursorArgs,
    INT                 BrushFlag,
	INT					BkgroundColor,
    CONST CHAR *        MenuName,
    LPSTR               AppName
    );

HWND get_hwnd(void);
WNDCLASS get_wndclass(void);

/**********************************************************
;
;	Name:		Create_Window_Overlapped
;
;	Purpose:	Just so that I don't have to cut and paste.
;
;	Input:		title of window, the Window Function Callback,
;				the hinstance handle, the menu handler if you want, 
;				data to be passed to WM_CREATE, x & y position 
;				and dimension.
;
;	Output:		a lot of stuff happens
;
;	Return:		The handle of the window, if you want...better to use G_hwnd instead.
;
**********************************************************/
HWND Create_Window_Overlapped(char *name, WNDPROC WinCallBack, HINSTANCE hinstance, HMENU hmenu, void *data, int x, int y, int sizex, int sizey);

/**********************************************************
;
;	Name:		Create_Window_Popup
;
;	Purpose:	Just so that I don't have to cut and paste.
;
;	Input:		title of window, the Window Function Callback,
;				the hinstance handle, the menu handler if you want, 
;				data to be passed to WM_CREATE, x & y position 
;				and dimension.
;
;	Output:		a lot of stuff happens
;
;	Return:		The handle of the window, if you want...better to use G_hwnd instead.
;
**********************************************************/
HWND Create_Window_Popup(char *name, WNDPROC WinCallBack, HINSTANCE hinstance, HMENU hmenu, void *data, int x, int y, int sizex, int sizey);

/**********************************************************
;
;	Name:		
;
;	Purpose:	
;
;	Input:		
;
;	Output:		
;
;	Return:		
;
**********************************************************/
UINT GetWindowMessage();

/**********************************************************
;
;	Name:		LoadBox
;
;	Purpose:	A simple load dialog box is opened and will return the pathname
;
;	Input:		HWND, max char, filter ie. *.exe, DefExt ie. exe, title of load box,
;				flags.
;
;	Output:		load box is shown
;
;	Return:		Full pathname of the file
;
**********************************************************/
char * LoadBox(HWND hwnd, int MaxFile, char *filter, char *DefExt, char *title, DWORD flags);

/**********************************************************
;
;	Name:		SaveBox
;
;	Purpose:	A simple save dialog box is opened and will return the pathname
;
;	Input:		HWND, max char, filter ie. *.exe, DefExt ie. exe, title of load box,
;				flags.
;
;	Output:		load box is shown
;
;	Return:		Full pathname of the file
;
**********************************************************/
char * SaveBox(HWND hwnd, int MaxFile, char *filter, char *DefExt, char *title, DWORD flags);

/**********************************************************
;
;	Name:	Random	
;
;	Purpose:	makes a random number between a given range
;
;	Input:		min and max number
;
;	Output:		none
;
;	Return:		the generated number
;
**********************************************************/
int Random(int a1, int a2);

#endif
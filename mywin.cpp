#include "mywin.h"

HWND G_hwnd;
HINSTANCE G_hinstance;
WNDCLASS wndclass;
RECT G_clientarea;

WNDCLASS get_wndclass(void) { return wndclass; }
HWND get_hwnd(void) { return G_hwnd; }

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
    )
{
	memset(&wndclass, 0, sizeof(WNDCLASS));

    wndclass.style         = Style;
    wndclass.lpfnWndProc   = WinCallBack;
    wndclass.cbClsExtra    = ClsExtra;
    wndclass.cbWndExtra    = WndExtra;
    wndclass.hInstance     = HInstance;
    wndclass.hIcon         = NULL;

    if (LoadIconArgs1)
            wndclass.hIcon = LoadIcon(LoadIconArgs1->Arg1,
                                       LoadIconArgs1->Flags);

    wndclass.hCursor       = LoadCursor(LoadCursorArgs->Arg1,
                                         LoadCursorArgs->Flags);

	if(BkgroundColor != 0)
		wndclass.hbrBackground = (HBRUSH)GetStockObject(BkgroundColor);
	else
		wndclass.hbrBackground = NULL;

    wndclass.lpszMenuName  = MenuName;
    wndclass.lpszClassName = AppName;

	RegisterClass(&wndclass);

	G_hinstance = HInstance;
}

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
HWND Create_Window_Overlapped(char *name, WNDPROC WinCallBack, HINSTANCE hinstance, HMENU hmenu, void *data, int x, int y, int sizex, int sizey)
{
	WNDCLASS wndclass;

    
    memset(&wndclass, 0, sizeof(WNDCLASS));

    
    wndclass.style = 0;//CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WinCallBack;
    wndclass.hInstance = hinstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszClassName = name;

    
    RegisterClass(&wndclass);

    
    G_hwnd = CreateWindow(name, 
            name, 
            WS_OVERLAPPEDWINDOW,
            x, // X position.
            y, // Y position.
            sizex, // width.
            sizey, // height.
            NULL, //it's not a child, so it's NULL
            hmenu, //if we are going to put menu and stuff
            hinstance,
            data);

	return G_hwnd;
}

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
HWND Create_Window_Popup(char *name, WNDPROC WinCallBack, HINSTANCE hinstance, HMENU hmenu, void *data, int x, int y, int sizex, int sizey)
{
	WNDCLASS wndclass;

    
    memset(&wndclass, 0, sizeof(WNDCLASS));

    
    wndclass.style = 0;//CS_HREDRAW | CS_VREDRAW;
    wndclass.lpfnWndProc = WinCallBack;
    wndclass.hInstance = hinstance;
    wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
    wndclass.hbrBackground = NULL;//(HBRUSH)GetStockObject(WHITE_BRUSH);
    wndclass.lpszClassName = name;

    
    RegisterClass(&wndclass);

    
    G_hwnd = CreateWindow(name, 
            name, 
            WS_POPUP,
            x, // X position.
            y, // Y position.
            sizex, // width.
            sizey, // height.
            NULL, //it's not a child, so it's NULL
            hmenu, //if we are going to put menu and stuff
            hinstance,
            data);

	return G_hwnd;
}

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
/*UINT GetWindowMessage()
{
	MSG msg;
	while(1)
	{
		if (!(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)))
			return NOTHINGATALL;
		else
		{
			if (msg.message == WM_QUIT)
			{
				return WM_QUIT;
			}
			else
			{
				TranslateMessage (&msg);
				DispatchMessage (&msg);
				return msg.message;
			}
		}
	}
}*/

UINT GetWindowMessage()
{
	MSG msg;
	msg.message = NOTHINGATALL;
	
	while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.message;
}

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
char * LoadBox(HWND hwnd, int MaxFile, char *filter, char *DefExt, char *title, DWORD flags)
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));	//instead of filling in all data structure of ofn, we do this

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = 0;
	ofn.lpstrFile = NULL;
	ofn.nMaxFile = MaxFile;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = (ULONG)NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrDefExt = DefExt;
	ofn.lpstrTitle = title;
	ofn.Flags = flags;

	GetOpenFileName(&ofn);

	return ofn.lpstrFile;
}

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
char * SaveBox(HWND hwnd, int MaxFile, char *filter, char *DefExt, char *title, DWORD flags)
{
	OPENFILENAME ofn;

	memset(&ofn, 0, sizeof(ofn));	//instead of filling in all data structure of ofn, we do this

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = 0;
	ofn.lpstrFile = NULL;
	ofn.nMaxFile = MaxFile;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = (ULONG)NULL;
	ofn.lpstrFilter = filter;
	ofn.lpstrDefExt = DefExt;
	ofn.lpstrTitle = title;
	ofn.Flags = flags;

	GetSaveFileName(&ofn);

	return ofn.lpstrFile;
}

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
int Random(int a1, int a2)
{
	int tempnum = a2 - a1 + 1;
	int tempnum2;

	tempnum2 = a2 - (rand()%tempnum);

	return tempnum2;
}
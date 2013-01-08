#ifndef _ddrawstuffp_h
#define _ddrawstuffp_h

#include "ddrawstuff.h"

/* ////////////////////////////////////////////////////////////////
structs
     //////////////////////////////////////////////////////////////// */
typedef struct _guids
{
	LPSTR driverdesc;		//description of driver
	LPSTR drivername;		//name of driver
	GUID *guid;				//General Unique Identifier of the drive
	struct _guids *next;	//duh....next?
} GUIDS, * PGUIDS;

typedef struct _modes
{
	DDSURFACEDESC mode;	//mode is the display mode that your window can handle
	struct _modes *next;	//next
} MODES, * PMODES;

typedef struct __IMAGE {
	LPDIRECTDRAWSURFACE Buffer; //(LPDIRECTDRAWSURFACE) ; Container for image data
	BYTE TransType;             //(BYTE) ; Type of transparency
	BYTE BltFastType;           //(BYTE) ; Type of blit routine to use
	short ID;                   //(short) ; Image identifier
	SIZE ImageSize;             //(SIZE) ; Size of image
	void * VidMem;              //(void *) ; Access to image memory
	long Pitch;                 //(long) ; Pitch value used to manipulate image memory
	FLAGS Status;               //(FLAGS) ; Flag variable used to control image display
}IMAGE, PIMAGE;

typedef struct __ddrawdude
{
	LPDIRECTDRAW ddrawobj;						//this is the first ddobj you create
	LPDIRECTDRAW2 ddrawobj2;					//this is the obj of the latest version of DX
	BOOL             bActive;                 // Is the app active? maybe...
    BOOL             bDoFlip;                 // Was the timer set? don't know...

	DDSURFACEDESC currentmode; //the current mode the screen have

	LPDIRECTDRAWPALETTE	lpDDPal;				//The Palette for the Primary Surface
	PPALETTEENTRY       pPaletteEntry;			//These are the palette entries of the whole display
	DDPIXELFORMAT pixelformat;					//Pixel format of the primary surface

	LPDIRECTDRAWCLIPPER lpDDClipper;			// DirectDraw clipper for use with window.

	LPDIRECTDRAWSURFACE  lpDDSurfacePrimary1;   // DirectDraw primary surface.  Used only if 3rd edition failed
	LPDIRECTDRAWSURFACE  surfacebackbuff1;		// The backbuffer surface 1.

	DDCAPS vidcap;			//A struct Video Capabilities that has a bunch of stuff
	DDCAPS hel;				//A struct Hardware Emulation that has a bunch of stuff
	PGUIDS guidlist;		//A linked-list of guids(video adapters)
	GUID *current_adapter;  //The currently selected video card
	PMODES modes;			//A linked-list of modes that the window display mode can be set to

	DDSURFACEDESC usersurfacedesc; //The current surface description the user made to make the surface

	bool backbufferisoffscreen; //bool. used only if we are doing windowed ddraw :P
	ULONG backbuffsizeX, backbuffsizeY; //size of backbuffer if backbufferisoffscreen = true...otherwie it's 0
	bool ddraw_init_is_called; //bool.  true if we have initialized all the crap
} ddrawdude, *Pddrawdude;


//don't you dare use this!  Use dd_update_screen()
bool (*dd_update)(DDRAWHANDLE ddrawstuff, RECT *windowarea);

//don't you dare use this!  Use dd_update_screen()
bool dd_update_fullscreen(DDRAWHANDLE ddrawstuff, RECT *windowarea);

//don't you dare use this!  Use dd_update_screen()
bool dd_update_windowed(DDRAWHANDLE ddrawstuff, RECT *windowarea);


//
// Dangerous Stuff.  USED ONLY BY FUNCTIONS
//

static void destroy_the_friggin_surfaces(DDRAWHANDLE ddrawstuff);  //Only if you are pissed off of the surfaces...careful!

static void set_backbuffer_as_offscreen(DDRAWHANDLE ddrawstuff, ULONG sizeX = 0, ULONG sizeY = 0); //only if you know what you are doing...(used for window mode)

static HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE lpDDSurface, HBITMAP hbm, int x, int y, int dx, int dy);	//copy to surface
	
BOOL dd_check_error(BOOL displaymessage, HRESULT result);

/* ////////////////////////////////////////////////////////////////
function prototypes that no user should care about
//////////////////////////////////////////////////////////////// */
BOOL PASCAL enumcallback(GUID * guid, LPSTR driverdesc, LPSTR drivername, LPVOID context);
BOOL PASCAL enumdisplaycallback(LPDDSURFACEDESC mode, LPVOID context);

BOOL CALLBACK vidopt(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);   //who cares
BOOL CALLBACK modeopt(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);  //who cares
BOOL CALLBACK vidstuff(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam); //who cares
BOOL CALLBACK pixels(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);   //who cares

#endif
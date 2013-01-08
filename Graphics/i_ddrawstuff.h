#ifndef _i_ddrawstuff_h
#define _i_ddrawstuff_h

#include "ddrawstuff.h"
#include "ddresource.h"


#define MAXPATHNAME 256
#define MAXPALETTE 256

//
// Stupid #defines
//
#define GUIDMAX 5
#define MODEMAX 100

/* ////////////////////////////////////////////////////////////////
structs
     //////////////////////////////////////////////////////////////// */
typedef struct _guids
{
	char driverdesc[MAXCHARBUFF];		//description of driver
	char drivername[MAXCHARBUFF];		//name of driver
	GUID *guid;				//General Unique Identifier of the drive
} GUIDS, * PGUIDS;

typedef struct __IMAGE {
	char filename[MAXPATHNAME];	//(char [256]) ; path of the image for reloading purpose
	IMGTYPE imageType;			//(IMGTYPE) ; type of image file ex: IMG_BMP
	LPDIRECTDRAWSURFACE Buffer; //(LPDIRECTDRAWSURFACE) ; Container for image data
	BYTE TransType;             //(BYTE) ; Type of transparency
	BYTE BltFastType;           //(BYTE) ; Type of blit routine to use
	short ID;                   //(short) ; Image identifier
	long index;					//(long) ; index within the list
	SIZE ImageSize;             //(SIZE) ; Size of image
	BYTE *VidMem;              //(void *) ; Access to image memory
	long Pitch;                 //(long) ; Pitch value used to manipulate image memory
	long Channel;				//(long) ; The number of channel per pixel
	FLAGS Status;               //(FLAGS) ; Flag variable used to control image display
}IMAGE, PIMAGE;

typedef struct __ddrawdude
{
	FLAGS status;							 //Flag variable used to control video manipulation
	LPDIRECTDRAW ddrawobj;					 //this is the first ddobj you create
	LPDIRECTDRAW2 ddrawobj2;				 //this is the obj of the latest version of DX
	LPDIRECTDRAWPALETTE	lpDDPal;			 //The Palette for the Primary Surface
	PALETTEENTRY       pPaletteEntry[MAXPALETTE];	 //These are the palette entries of the whole display
	DDPIXELFORMAT pixelformat;				 //Pixel format of the primary surface
	LPDIRECTDRAWCLIPPER lpDDClipper;		 //DirectDraw clipper for use with window.
	LPDIRECTDRAWSURFACE  lpDDSurfacePrimary; //DirectDraw primary surface.
	LPDIRECTDRAWSURFACE  backbuffer;		 //The backbuffer surface 1.
	LPDIRECTDRAWCLIPPER lpDDBackBuffClipper; //Clipper for backbuffer, so we don't have to clip manually
	BYTE *VidMem;							 //Access to backbuffer memory
	long Pitch;								 //Pitch value used to manipulate backbuffer memory
	long Channel;							 //the number of channels on each pixel
	DDSURFACEDESC currentmode;				 //the current mode the screen have
	SIZE backbuffsize;						 //size of backbuffer if backbufferisoffscreen = true...otherwie it's 0
	DDCAPS vidcap;							 //A struct Video Capabilities that has a bunch of stuff
	DDCAPS hel;								 //A struct Hardware Emulation that has a bunch of stuff
	DDSURFACEDESC usersurfacedesc;			 //The current surface description the user made to make the surface
	GUID *current_adapter;					 //The currently selected video card
	LISTHANDLE guidlist;					 //A linked-list of guids(video adapters)
	LISTHANDLE modes;						 //A linked-list of modes that the window display mode can be set to
	LISTHANDLE Images;						 //A linked-list of images
} ddrawdude, *Pddrawdude;


//don't you dare use this!  Use dd_update_screen()
PROTECTED RETCODE (*dd_update)(DDRAWHANDLE ddrawstuff, RECT *windowarea);

//don't you dare use this!  Use dd_update_screen()
PROTECTED RETCODE dd_update_fullscreen(DDRAWHANDLE ddrawstuff, RECT *windowarea);

//don't you dare use this!  Use dd_update_screen()
PROTECTED RETCODE dd_update_windowed(DDRAWHANDLE ddrawstuff, RECT *windowarea);

//methods for get color
PROTECTED void (*SetPixelColorPtr)(UCHAR *buff, int Red, int Green, int Blue);
//in 8-bit, the first and second param is the only thing used
PROTECTED void SetPixelColor8(UCHAR *buff, int Red, int Green, int Blue);
PROTECTED void SetPixelColor555(UCHAR *buff, int Red, int Green, int Blue);
PROTECTED void SetPixelColor565(UCHAR *buff, int Red, int Green, int Blue);
PROTECTED void SetPixelColor24(UCHAR *buff, int Red, int Green, int Blue);
PROTECTED void SetPixelColor32(UCHAR *buff, int Red, int Green, int Blue);


//
// Dangerous Stuff.  USED ONLY BY FUNCTIONS
//

PRIVATE RETCODE destroy_the_friggin_surfaces(DDRAWHANDLE ddrawstuff);  //Only if you are pissed off of the surfaces...careful!

PRIVATE RETCODE set_backbuffer_as_offscreen(DDRAWHANDLE ddrawstuff, long sizeX = 0, long sizeY = 0); //only if you know what you are doing...(used for window mode)

static HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE lpDDSurface, char *imgpath, int x, int y, int dx, int dy);	//copy to surface
	
BOOL dd_check_error(BOOL displaymessage, HRESULT result);

// Image ID search function
RETCODE ImageIDSearch(void *thing1, void *thing2);

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
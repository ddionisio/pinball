#ifndef _ddrawstuff_h
#define _ddrawstuff_h

// You must include this define to use QueryInterface
#define INITGUID

#define MAX_BACKBUFF 1

#include "..\Common.h" 
#include "..\File\filetool.h"
#include "..\List\list.h"
#include "..\Memory\memory.h"

#include "ddraw.h"
//#include "themain.h"
//#include "pddrawstuff.h"

#define DEFAULTWIDTH 640
#define DEFAULTHEIGHT 480
#define DEFAULTCBIT 32 //color bits per pixel
#define DEFAULTRR 0 //refresh rate

//insignificant image ID, usually used for fonts and non-image surfaces
#define ImageDummyID 666
//Used for making image outside the list
#define INDIVIDUALIMAGE -666

//bah!
#define INIT_DDSD(ddsd) memset(&ddsd, 0, sizeof(DDSURFACEDESC)); \
						 ddsd.dwSize = sizeof(DDSURFACEDESC);

//this is better than INIT_DDSD!
//#define INIT_STRUCT(data, size) memset(&data, 0, size); \
//						 data.dwSize = size;

//no!  this is MUCH MUCH better!
#define INIT_STRUCT(data) memset(&data, 0, sizeof(data)); \
						 data.dwSize = sizeof(data);

#define	BLUE_BITMAP_OFFSET	0
#define	GREEN_BITMAP_OFFSET	1
#define	RED_BITMAP_OFFSET	2
#define	ALPHA_BITMAP_OFFSET	3

//RGB stuff
#define RGB16BIT555(r,g,b) ((b%32) + ((g%32)<<5) + ((r%32)<<10))
#define RGB16BIT565(r,g,b) ((b%32) + ((g%64)<<6) + ((r%32)<<11))
#define RGB32BIT(r,g,b) (0+((g)<<8)+((r)<<16)+(b<<24))



//if we want to get the list of modes
//deactivated for now...only because we don't have dynamic lists
#define MODEACTIVATE 1


//
// Status FLAG
//
//typedef enum {
#define	DDFLAG_INIT_CALLED 1
#define	DDFLAG_ISACTIVE 2
#define	DDFLAG_IMAGELISTCREATED 4
#define	DDFLAG_BACKBUFFERISOFFSCRN 8
#define	DDFLAG_LOCKED 16
#define	DDFLAG_UNLOCKED 32
#define	DDFLAG_USINGCLIPPER 64
	//DDFLAG_IMAGE
//} DDFLAG;


////////////////////////////////////////////////////////////
//
//   Our opaque handles
//
////////////////////////////////////////////////////////////
typedef struct __ddrawdude *DDRAWHANDLE;
typedef struct __IMAGE *IMAGEHANDLE;

//
// Return codes
//
typedef enum {
	DDRAW_BAD,
	DDRAW_OK,
	DDRAW_SURFACELOST,
	DDRAW_OUTOFBOUND
} DDRAWCODE;


////////////////////////////////////////////////////////////
//
//   DDRAW Functions that the user should only care about
//
////////////////////////////////////////////////////////////

/**********************************************************
;
;	Name:		ddraw_create
;
;	Purpose:	not much, just creates the DDRAWHANDLE
;
;	Input:		none
;
;	Output:		a newly malloced ddrawhandle
;
;	Return:		the ddrawhandle
;
**********************************************************/
PUBLIC DDRAWHANDLE ddraw_create();

/**********************************************************
;
;	Name:		ddraw_destroy
;
;	Purpose:	destroys everything inside the ddraw
;
;	Input:		the ddraw handle
;
;	Output:		ddraw handle will be empty
;
;	Return:		some RETCODE
;
**********************************************************/
PUBLIC RETCODE ddraw_destroy(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		get_user_made_surfacedesc
;
;	Purpose:	get the last surfacedesc made by user
;
;	Input:		none
;
;	Output:		none
;
;	Return:		The surface description by the user
;
**********************************************************/
PRIVATE  DDSURFACEDESC get_user_made_surfacedesc(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		get_surfacedesc
;
;	Purpose:	gives you the surface description of the primary surface
;
;	Input:		none
;
;	Output:		none
;
;	Return:		surface description
;
**********************************************************/
PROTECTED  DDSURFACEDESC get_surfacedesc(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		get_screensize
;
;	Purpose:	gets the screen size (or backbuffer size)
;
;	Input:		the handle to ddraw
;
;	Output:		none
;
;	Return:		the size
;
**********************************************************/
PUBLIC  SIZE get_screensize(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		ddraw_create_obj
;
;	Purpose:	creates the object for abusement
;
;	Input:		the handle wih the objs. and the guid
;
;	Output:		ddraw object is created
;
;	Return:		some result you would care aboot, eh?
;
**********************************************************/
PRIVATE RETCODE ddraw_create_obj(DDRAWHANDLE ddrawstuff, GUID *guid);

/**********************************************************
;
;	Name:		ddraw_init
;
;	Purpose:	initialize the direct draw object
;
;	Input:		
;
;	Output:		
;
;	Return:		a code...oh does it matter!?
;
**********************************************************/
PROTECTED RETCODE ddraw_init(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, SOMEINSTANCE hinst, DWORD cooperative_flag);

/**********************************************************
;
;	Name:		ddraw_remake
;
;	Purpose:	recreate ddraw without initializing all the other crap
;
;	Input:		handle to window and the cooperative flag
;
;	Output:		ddraw object is remade
;
;	Return:		some ideal code of the paparazi
;
**********************************************************/
PROTECTED RETCODE ddraw_remake(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, DWORD cooperative_flag);

/**********************************************************
;
;	Name:		set_cooperation
;
;	Purpose:	set up cooperation level.
;
;	Input:		HWND hwnd, DWORD cooperative_flag
;
;	Output:		we set up some cooperation to with the window and directdraw...Simon says!
;
;	Return:		don't expect much
;
**********************************************************/
PROTECTED RETCODE set_cooperation(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, DWORD cooperative_flag);

/**********************************************************
;
;	Name:		destroy_ddrawobj
;
;	Purpose:	This will just destroy the ddraw object and nothing else
;
;	Input:		DDRAWHANDLE ddrawstuff
;
;	Output:		dead objects
;
;	Return:		none
;
**********************************************************/
PRIVATE void destroy_ddrawobj(DDRAWHANDLE ddrawstuff); //you better know what you are doing!!!

/**********************************************************
;
;	Name:		we_have_ddrawobj
;
;	Purpose:	check to see if we have the ddraw object initialized
;
;	Input:		none
;
;	Output:		none
;
;	Return:		DDRAW_OK if we do have it
;
**********************************************************/
PUBLIC RETCODE we_have_ddrawobj(DDRAWHANDLE ddrawstuff); //check to see if we have the ddraw object initialized

/**********************************************************
;
;	Name:		create_ddraw_fullscreen
;
;	Purpose:	set up everything for fullscreen use(includes surface creation)
;
;	Input:		handle to ddraw and window, hinstance and the mode -> default is 640x480x8x0
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC RETCODE create_ddraw_fullscreen(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, SOMEINSTANCE hinst, Pdisplaymode themode = NULL); //set up everything for fullscreen use(includes surface creation)

/**********************************************************
;
;	Name:		create_ddraw_windowed
;
;	Purpose:	set up everything for window use(includes surface creation)
;
;	Input:		the handle to window and the hinstance for 
;				something, size x, y of backbuffer...default 
;				is the current window size.
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC RETCODE create_ddraw_windowed(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, SOMEINSTANCE hinst, long sizeX = 0, long sizeY = 0); //set up everything for window use(includes surface creation)

/**********************************************************
;
;	Name:		create_ddraw_fullscrnclipper
;
;	Purpose:	set up everything for fullscreen use with 
;				clipper(includes surface creation)
;
;	Input:		handle to ddraw and window, hinstance and the mode -> default is 640x480x8x0
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC RETCODE create_ddraw_fullscrnclipper(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, SOMEINSTANCE hinst, Pdisplaymode themode = NULL);

/**********************************************************
;
;	Name:		set_our_display_mode
;
;	Purpose:	reset mode to what the user want.  caution: DONT USE IT IN WINDOWED MODE!!!
;
;	Input:		ULONG width, ULONG height, ULONG colorbits, ULONG refreshrate
;
;	Output:		mode is set
;
;	Return:		a code of creed
;
**********************************************************/
PUBLIC RETCODE set_our_display_mode(DDRAWHANDLE ddrawstuff, ULONG width, ULONG height, ULONG colorbits, ULONG refreshrate, bool willrecreatesurface = true); //set up the mode baby!

/**********************************************************
;
;	Name:		restore_display_mode
;
;	Purpose:	to restore our display mode to the desktop mode
;
;	Input:		nothing
;
;	Output:		notha
;
;	Return:		the code of life
;
**********************************************************/
PUBLIC RETCODE restore_display_mode(DDRAWHANDLE ddrawstuff, bool willrecreatesurface = true); //restore the mode baby!

/**********************************************************
;
;	Name:		make_surface_primary
;
;	Purpose:	well what do you think???
;
;	Input:		surface description...pretty much the description of the surface
;
;	Output:		lots of things are filled up
;
;	Return:		black code of death
;
**********************************************************/
PROTECTED RETCODE make_surface_primary(DDRAWHANDLE ddrawstuff, DDSURFACEDESC surfacedesc, int WeWantBackBufferAsOffscreen = 0, ULONG backbuffsizeX = 0, ULONG backbuffsizeY = 0); //make the surface with backbuffer and if we want offscrn backbuff, then do that with the given size X,Y

/**********************************************************
;
;	Name:		make_surface_clipper
;
;	Purpose:	make clipper(used for windowed direct draw)
;
;	Input:		The handle to the window
;
;	Output:		A clipper is made...whoopie! (sarcasm)
;
;	Return:		a code of a sort
;
**********************************************************/
PROTECTED RETCODE make_surface_clipper(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd); //make clipper(used for windowed direct draw)

/**********************************************************
;
;	Name:		destroy_surface_clipper
;
;	Purpose:	a wrapper for destroying the clipper
;
;	Input:		the handle to ddraw
;
;	Output:		dead clipper
;
;	Return:		kapish!
;
**********************************************************/
PROTECTED RETCODE destroy_surface_clipper(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		DDAttachClipping
;
;	Purpose:	so that you don't have to do the clipping manually
;
;	Input:		ddrawstuff, the surface, the number of rect 
;				region and the actual array of rects
;
;	Output:		the surface is given a clipper
;
;	Return:		the newly created clipper of the surface
;
**********************************************************/
PROTECTED LPDIRECTDRAWCLIPPER DDAttachClipping(DDRAWHANDLE ddrawstuff, LPDIRECTDRAWSURFACE lpDDSurface, int num_rects, LPRECT clip_list);

/**********************************************************
;
;	Name:		DDReleaseClipper
;
;	Purpose:	just a wrapper for destroying a given clipper
;
;	Input:		the clipper to be destroyed
;
;	Output:		the clipper is destroyed
;
;	Return:		none
;
**********************************************************/
PROTECTED void DDReleaseClipper(LPDIRECTDRAWCLIPPER lpddclipper);

/**********************************************************
;
;	Name:		DDIsImageValid
;
;	Purpose:	checks to see if image is initialized and
;				is filled
;
;	Input:		the image himself
;
;	Output:		none
;
;	Return:		true if valid
;
**********************************************************/
PUBLIC bool DDIsImageValid(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDBltImage
;
;	Purpose:	blits the given image to the backbuffer
;
;	Input:		the handle to ddraw, the image handle, the
;				area of the image and the screen location.
;
;	Output:		the backbuffer is filled with an image
;
;	Return:		a retcode of a sort
;
**********************************************************/
PUBLIC RETCODE DDBltImage(DDRAWHANDLE ddrawstuff, IMAGEHANDLE image, RECT rcRect, LOCATION loc);

/**********************************************************
;
;	Name:		DDInitImageList
;
;	Purpose:	initializes the image list with a specified 
;				number of image.  must be > 0
;
;	Input:		the ddraw handle and the number of image
;
;	Output:		the list is created
;
;	Return:		some code
;
**********************************************************/
PUBLIC RETCODE DDInitImageList(DDRAWHANDLE ddrawstuff, short numImage);

/**********************************************************
;
;	Name:		DDClearImageList
;
;	Purpose:	clears up the image list of the ddrawhandle
;
;	Input:		the handle to ddraw
;
;	Output:		the image list is clear
;
;	Return:		a return code of a sort
;
**********************************************************/
PUBLIC RETCODE DDClearImageList(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		DDGetImage
;
;	Purpose:	grabs the specified image from the list
;
;	Input:		the ddraw handle and the ID of the image
;
;	Output:		none
;
;	Return:		the image handle
;
**********************************************************/
PUBLIC IMAGEHANDLE DDGetImage(DDRAWHANDLE ddrawstuff, short ID);

/**********************************************************
;
;	Name:		DDInsertImageToList
;
;	Purpose:	inserts an image to the list from a given file
;
;	Input:		ddrawhandle, newID, filename and 
;				image type ex: IMG_BMP
;				setSrcColorKey and low and hi color if 
;				setSrcColorKey is true
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC IMAGEHANDLE DDCreateImage(DDRAWHANDLE ddrawstuff, short newID, char *filename, bool setSrcColorKey = false, ULONG low_color_val = 0, ULONG high_color_val = 0, IMGTYPE imgType = IMG_BMP);

/**********************************************************
;
;	Name:		DDCreateMyOwnImage
;
;	Purpose:	Creates an image that won't be inserted to the list
;
;	Input:		ddrawhandle, newID, filename and 
;				image type ex: IMG_BMP
;				setSrcColorKey and low and hi color if 
;				setSrcColorKey is true
;
;	Output:		none
;
;	Return:		The imagehandle, *NOTE* You must destroy this image manually
;
**********************************************************/
PUBLIC IMAGEHANDLE DDCreateMyOwnImage(DDRAWHANDLE ddrawstuff, char *filename, bool setSrcColorKey = false, ULONG low_color_val = 0, ULONG high_color_val = 0, IMGTYPE imgType = IMG_BMP);

/**********************************************************
;
;	Name:		DDReloadImage
;
;	Purpose:	reloads the image file to the given image
;
;	Input:		the ddrawhandle, the imagehandle
;
;	Output:		the image should be restored
;
;	Return:		a code
;
**********************************************************/
PUBLIC RETCODE DDReloadImage(DDRAWHANDLE ddrawstuff, IMAGEHANDLE image, char *filename = NULL, IMGTYPE imgType = IMG_BMP);

/**********************************************************
;
;	Name:		DDLoadBitmap
;
;	Purpose:	To ruin your life...load bitmap to a surface, 
;				the surface will be created as offscreen
;
;	Input:		The empty surface and the name of file, note: 
;				surface must be empty, else it will be terminated
;
;	Output:		surface offscreen is created...DON'T FORGET TO DESTROY IT LATER!!!
;
;	Return:		size of bitmap, if the size is 0,0 then there is something wrong
;
**********************************************************/
PRIVATE SIZE DDLoadBitmap(DDRAWHANDLE ddrawstuff, LPDIRECTDRAWSURFACE *lpDDSurfaceOffScr, char *bmfilename, long dx = 0, long dy = 0);

/**********************************************************
;
;	Name:		DDReloadBitmap
;
;	Purpose:	Load a bitmap to an existing surface
;
;	Input:		The surface and the bitmap string.
;				This is used only for reloading the same bmp
;
;	Output:		The surface is loaded with balls
;
;	Return:		size of bitmap, if the size is 0,0 then there is something wrong
;
**********************************************************/
PRIVATE SIZE DDReloadBitmap(LPDIRECTDRAWSURFACE lpDDSurface, char *bmfilename); //Load a bitmap to an existing surface

/**********************************************************
;
;	Name:		DDKillImageData
;
;	Purpose:	destroys the data of the image, no more no less
;
;	Input:		the image handle...or pointer to image
;
;	Output:		image is leen and clean.
;
;	Return:		some code
;
**********************************************************/
PRIVATE RETCODE DDKillImageData(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDRestoreImage
;
;	Purpose:	restores the image and if that fails, 
;				reload the image
;
;	Input:		the image handle
;
;	Output:		the image is restored
;
;	Return:		some retcode
;
**********************************************************/
PUBLIC RETCODE DDRestoreImage(DDRAWHANDLE ddrawstuff, IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDRestoreImageList
;
;	Purpose:	restores all image in ddrawhandle
;
;	Input:		the handle to ddraw
;
;	Output:		the list is restored
;
;	Return:		some code
;
**********************************************************/
PUBLIC RETCODE DDRestoreImageList(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		make_surface_palette
;
;	Purpose:	set up the primary surface palette entry
;
;	Input:		a custom set of colors if you want...but you don't have to, really!  I mean totally
;
;	Output:		lots and lots of things happen
;
;	Return:		OK or BAD, either way
;
**********************************************************/
PUBLIC RETCODE make_surface_palette(DDRAWHANDLE ddrawstuff, PPALETTEENTRY destptPalette = NULL); //set up the primary surface palette entry

/**********************************************************
;
;	Name:		SetColorKeySrc
;
;	Purpose:	to cut of the icky stuff
;
;	Input:		The surface, range of color value LO-HI
;
;	Output:		surface becomes clean
;
;	Return:		OK if all is cool
;
**********************************************************/
PUBLIC RETCODE SetColorKeySrc(LPDIRECTDRAWSURFACE surface, ULONG low_color_val, ULONG high_color_val); //set a given surface with transparency

																												
/**********************************************************
;
;	Name:		dd_update_screen
;
;	Purpose:	Updates the primary surface by flipping 
;				or blitting the back buffer
;
;	Input:		The handle to ddrawstuff, the rect window area
;				only used for window mode.
;
;	Output:		stuff displayed on screen
;
;	Return:		false if anything bad happened, you should
;				restore the images if it did.
;
**********************************************************/
void dd_update_screen(DDRAWHANDLE ddrawstuff, RECT *windowarea = NULL);

//
// Surface Manipulation
//

/**********************************************************
;
;	Name:		DDClearBuffer
;
;	Purpose:	clears up the back-buffer
;
;	Input:		the ddraw handle and the color
;
;	Output:		the backbuffer is clean
;
;	Return:		none
;
**********************************************************/
PUBLIC void DDClearBuffer(DDRAWHANDLE ddrawstuff, ULONG color = 0);

/**********************************************************
;
;	Name:		dd_lock
;
;	Purpose:	will lock the backbuffer, ready to be 
;				manipulated.  The vidmem in DDRAWHANDLE 
;				will be filled as well as the lPitch.
;
;	Input:		the handle to ddraw
;
;	Output:		the backbuffer is locked
;
;	Return:		none
;
**********************************************************/
PUBLIC  void dd_lock(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		dd_unlock
;
;	Purpose:	will unlock the backbuffer.  The vidmem is 
;				set to NULL as well as the lPitch.
;
;	Input:		the handle to ddraw
;
;	Output:		the backbuffer is unlocked
;
;	Return:		none
;
**********************************************************/
PUBLIC  void dd_unlock(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		dd_lock
;
;	Purpose:	will lock the image surface, ready to be 
;				manipulated.  The vidmem in PIMAGE will be 
;				filled as well as the lPitch.
;
;	Input:		the image handle
;
;	Output:		the image is locked
;
;	Return:		none
;
**********************************************************/
PUBLIC  void dd_lock(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		
;
;	Purpose:	will unlock the image surface.  The vidmem 
;				is set to NULL as well as the lPitch.
;
;	Input:		the image handle
;
;	Output:		the image is unlocked
;
;	Return:		none
;
**********************************************************/
PUBLIC  void dd_unlock(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDGetPitch
;
;	Purpose:	grabs the pitch of the backbuffer
;
;	Input:		the ddrawhandle
;
;	Output:		none
;
;	Return:		the pitch
;
**********************************************************/
PUBLIC long DDGetPitch(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		DDGetPitch
;
;	Purpose:	grabs the pitch of the image buffer
;
;	Input:		the image handle
;
;	Output:		none
;
;	Return:		the pitch
;
**********************************************************/
PUBLIC long DDGetPitch(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDGetSize
;
;	Purpose:	get's the size of the buffer
;
;	Input:		the handle
;
;	Output:		none
;
;	Return:		SIZE
;
**********************************************************/
PUBLIC SIZE DDGetSize(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		DDGetSize
;
;	Purpose:	get's the size of the buffer
;
;	Input:		the handle
;
;	Output:		none
;
;	Return:		SIZE
;
**********************************************************/
PUBLIC SIZE DDGetSize(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDGetChannel
;
;	Purpose:	grabs the channel of the buffer
;
;	Input:		the handle
;
;	Output:		none
;
;	Return:		the channel
;
**********************************************************/
PUBLIC long DDGetChannel(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		DDGetChannel
;
;	Purpose:	grabs the channel of the buffer
;
;	Input:		the handle
;
;	Output:		none
;
;	Return:		the channel
;
**********************************************************/
PUBLIC long DDGetChannel(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDGetLinePtr
;
;	Purpose:	grabs the pointer of the buffer and returns it.
;				The image must be locked first.
;
;	Input:		the ddraw handle and the y value
;
;	Output:		none
;
;	Return:		a pointer to the specified location of the buffer
;
**********************************************************/
PUBLIC BYTE * DDGetLinePtr(DDRAWHANDLE ddrawstuff, long y);

/**********************************************************
;
;	Name:		DDGetPixelPtr
;
;	Purpose:	grpabs the pointer of the buffer and return it.
;				The image must be locked first.
;
;	Input:		the ddraw handle and the x, y value
;
;	Output:		none
;
;	Return:		a pointer to the specified location of the buffer
;
**********************************************************/
PUBLIC BYTE * DDGetPixelPtr(DDRAWHANDLE ddrawstuff, long x, long y);

/**********************************************************
;
;	Name:		DDGetLinePtr
;
;	Purpose:	grabs the pointer of the buffer and returns it.
;				The image must be locked first.
;
;	Input:		the image handle and the y value
;
;	Output:		none
;
;	Return:		a pointer to the specified location of the buffer
;
**********************************************************/
PUBLIC BYTE * DDGetLinePtr(IMAGEHANDLE image, long y);

/**********************************************************
;
;	Name:		DDGetPixelPtr
;
;	Purpose:	grpabs the pointer of the buffer and return it.
;				The image must be locked first.
;
;	Input:		the image handle and the x, y value
;
;	Output:		none
;
;	Return:		a pointer to the specified location of the buffer
;
**********************************************************/
PUBLIC BYTE * DDGetPixelPtr(IMAGEHANDLE image, long x, long y);

//
// Color stuff, for getting the correct RGB depending on the mode, except for 8-bit
//

/**********************************************************
;
;	Name:		SetPixelColor
;
;	Purpose:	sets the given pointer to pixel to a color
;
;	Input:		buffer, R, G, B
;
;	Output:		the buffer is updated
;
;	Return:		none
;
**********************************************************/
PUBLIC void SetPixelColor(UCHAR *buff, int Red, int Green, int Blue);

//
// Stupid Image functions for the sake of the font engine and FX
//

/**********************************************************
;
;	Name:		DDIsImageNotInList
;
;	Purpose:	Checks to see if the image is not part
;				of the list
;
;	Input:		the image
;
;	Output:		none
;
;	Return:		true if it is not included to the list
;
**********************************************************/
PROTECTED bool DDIsImageNotInList(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDIsImageDummy
;
;	Purpose:	checks to see if the image ID is a dummy
;
;	Input:		the image
;
;	Output:		none
;
;	Return:		true if it is
;
**********************************************************/
PROTECTED bool DDIsImageDummy(IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDCreateEmptyImage
;
;	Purpose:	Creates an empty image buffer of a given size
;
;	Input:		the ddrawhandle, the size X/Y, if you want to add source 
;				color key...if so you must give the low/high
;				value for the color
;
;	Output:		none
;
;	Return:		A newly created imagehandle, NULL if error occur
;
**********************************************************/
PROTECTED IMAGEHANDLE DDCreateEmptyImage(DDRAWHANDLE ddrawstuff, long sizeX, long sizeY, short ImageID = ImageDummyID, bool setSrcColorKey = false, ULONG low_color_val = 0, ULONG high_color_val = 0, IMGTYPE imgType = IMG_BMP);

/**********************************************************
;
;	Name:		DDCreateMyOwnEmptyImage
;
;	Purpose:	Creates an empty image buffer of a given size
;				*note*: YOU MUST DESTROY THE IMAGE AFTER USE.
;
;	Input:		the ddrawhandle, the size X/Y, if you want to add source 
;				color key...if so you must give the low/high
;				value for the color
;
;	Output:		none
;
;	Return:		A newly created imagehandle, NULL if error occur
;
**********************************************************/
PROTECTED IMAGEHANDLE DDCreateMyOwnEmptyImage(DDRAWHANDLE ddrawstuff, long sizeX, long sizeY, bool setSrcColorKey = false, ULONG low_color_val = 0, ULONG high_color_val = 0, IMGTYPE imgType = IMG_BMP);

/**********************************************************
;
;	Name:		DDBltImageToImage
;
;	Purpose:	blits the given srcImage to the destImage
;
;	Input:		the destination image, the source image, the
;				source image area rect and location to the destination
;
;	Output:		The destination image is updated with crap
;
;	Return:		some code of success or failure
;
**********************************************************/
PROTECTED RETCODE DDBltImageToImage(DDRAWHANDLE ddrawstuff, IMAGEHANDLE destImage, IMAGEHANDLE srcImage,  RECT rcRect, LOCATION loc);

/**********************************************************
;
;	Name:		DDImageDestroy
;
;	Purpose:	destroys the given image, usually used for
;				dummy image
;
;	Input:		the ddraw & image handle
;
;	Output:		the list is updated
;
;	Return:		fail or success
;
**********************************************************/
PROTECTED RETCODE DDImageDestroy(DDRAWHANDLE ddrawstuff, IMAGEHANDLE image);

/**********************************************************
;
;	Name:		DDKillAllDummyImage
;
;	Purpose:	this will rid of all imagehandles that are dummy
;
;	Input:		the ddraw handle
;
;	Output:		the imagelist is clean of dummies
;
;	Return:		failure or success
;
**********************************************************/
PROTECTED RETCODE DDKillAllDummyImage(DDRAWHANDLE ddrawstuff);

/**********************************************************
;
;	Name:		DDImageStretchDIBits
;
;	Purpose:	for some obscure reason, we need this for the font engine
;
;	Input:		image handle, loc and size of image, bits and bitmapinfo
;
;	Output:		IMAGEHANDLE is updated
;
;	Return:		success or failure
;
**********************************************************/
PROTECTED RETCODE DDImageStretchDIBits(IMAGEHANDLE image, LOCATION loc, SIZE imgSize, const void *bits, const BITMAPINFO *bmi);

/**********************************************************
;
;	Name:		DDSetImageColorKey
;
;	Purpose:	sets the color key of the image
;
;	Input:		the image handle and the new color vals(you don't have to)
;
;	Output:		the iamge now has a color key
;
;	Return:		some code
;
**********************************************************/
PROTECTED void DDSetImageColorKey(IMAGEHANDLE image, ULONG low_color_val = 0, ULONG high_color_val = 0);

//
// Other stuff you shouldn't worry about
//

/**********************************************************
;
;	Name:		TransferLoc
;
;	Purpose:	clips the area of the location relative to
;				to the other area.
;
;	Input:		the location and it's area and the other area
;				to compare with.
;
;	Output:		The location might change, as well as the RECT
;
;	Return:		DDRAW_OK if all went well, DDRAW_OUTOFBOUND if the
;				given loc is not within the other area
;
**********************************************************/
PROTECTED RETCODE TransferLoc(PLOCATION loc, PRECT locArea, const RECT otherArea);

/**********************************************************
;
;	Name:		DDGetListModes
;
;	Purpose:	for the sake of changing resolution
;
;	Input:		the ddrawhandle
;
;	Output:		none
;
;	Return:		the LISTHANDLE
;
**********************************************************/
PROTECTED LISTHANDLE DDGetListModes(DDRAWHANDLE ddrawstuff);

void display_mode_opt(DDRAWHANDLE ddrawstuff, HWND hwnd, HINSTANCE hinst); //display stuff on the screen
void display_vidcap(DDRAWHANDLE ddrawstuff, HWND hwnd, HINSTANCE hinst);   //video capabilities that you don't need to know
void display_pixelformat(DDRAWHANDLE ddrawstuff, HWND hwnd, HINSTANCE hinst); //crappy pixel format



#endif
#ifndef _gdi_h
#define _gdi_h

/**********************************************************

  Game:		TED
  Team:     Plomero
  Author:   David Dionisio
  Modified: 10/19/00
 
**********************************************************/

//#include "..\Common.h" 
//#include "..\File\filetool.h"
//#include "..\List\list.h"
//#include "..\Memory\memory.h"
#include "..\Timer\timer.h"
#include "ddrawstuff.h"

//reserve area for sprite duplication so you don't have to 
//worry about making up sprite IDs all the time
#define SpriteDummyID 666
#define ImageFontID 711
#define LayerMaxReserve 1
#define AntiSocialSpriteThatDoesNotBelongToAnyLayer 777


//
// macro for text
//

//use this for color
#define TEXTCOLOR(r, g, b) ((DWORD) (((BYTE) (b) | \
						   ((WORD) (g) << 8)) | \
						   (((DWORD) (BYTE) (r)) << 16)))


//
// defines for text stuff, used inside functions
//
#define ASCIIOFFSET 32 
#define ASCIILIMIT 224

#define ASCIIRET   15
#define ASCIITAB   9
#define TAB 5 //number of spaces to skip

//
// RETCODEs
//
typedef enum {
	VD_BAD,
	VD_OK,
	VD_UNKNOWN,
	VD_BADTYPE,
	VD_NOTEXISTS,
	VD_IDALREADYMADE,
	VD_NOHANDLE,
	VD_NORECOVER,
	VD_BUFFERISLOCKED,
	VD_BADRECT,
	VD_RANGEOUTOFBOUND,
	VD_CANNOTALLOCATE,
	VD_STUPIDERROR
} VDCODE;


//
// Video types
//
typedef enum {
        VDTYPE_FULLSCREEN,		//use POP UP window
        VDTYPE_WINDOWED,		//use overlapped window
		VDTYPE_FULLSCRNCLIPPER	//use POP UP window
} VTYPE;

//
// Text Alignment
//
typedef enum {
	TXT_RIGHT,
	TXT_LEFT,
	TXT_CENTER
} ALIGN;

//
// GDI Flags
//
//typedef enum {
#define	GFLAG_DEACTIVATE 1
#define	GFLAG_ACTIVATE 2
//} GFLAG;


//
// GDI handles
//

typedef struct __SPRITE   *SPRITEHANDLE;
typedef struct __FONT     *FONTHANDLE;

//do not use, it's in here for a reason.
typedef struct __LAYER    *LAYERHANDLE;
typedef struct __FX       *FXHANDLE;

//used for FX, dumbParam is SPRITEHANDLE otherParam is usually used for initialization
typedef long (* EFFECT) (void *dumbParam, void *otherParam, long message);

//
// Public Data Structures
//
typedef struct __FRAME {
	short firstframe;
	short lastframe;
} FRAME, * PFRAME;



/************************************************************************************************

GDI functions

************************************************************************************************/


//
//  Graphics Management
//

/**********************************************************
;
;	Name:		GraphicsInit
;
;	Purpose:	Initializes a graphics manager.  The 
;				video handle is also initialized depending 
;				on the type with default values. 
;				*note:* YOU MUST INITIALIZE image list and
;				layer list separately!!!
;
;	Input:		HWND & HINSTANCE, pointer to the GRAPHICHANDLE, 
;				handle to memory, type of video initialization.  
;				Fullscrn/Windowed.  The display mode used for fullscreen
;				
;
;	Output:		The GRAPHICHANDLE will be allocated.
;
;	Return:		A code indicating the results of the 
;				initialization
;
**********************************************************/
PROTECTED RETCODE GraphicsInit (WINHANDLE hwnd, SOMEINSTANCE hinstance, VTYPE vidtype, Pdisplaymode mode = NULL);

/**********************************************************
;
;	Name:		GraphicsTerm
;
;	Purpose:	Deinitializes a graphics manager
;
;	Input:		Graphics pointer to deinitialize through
;
;	Output:		the GRAPHICHANDLE is destroyed
;
;	Return:		A code indicating the results of the 
;				termination
;
**********************************************************/
PROTECTED RETCODE GraphicsTerm();

/**********************************************************
;
;	Name:		GraphicsSetMode
;
;	Purpose:	changes the mode of the screen, must be 
;				in fullscreen
;
;	Input:		the handle, the W*H*Color*Resfresh
;
;	Output:		the screen is changed
;
;	Return:		A code indicating some result
;
**********************************************************/
PUBLIC RETCODE GraphicsSetMode(int width, int height, int colordepth, int refreshrate);

/**********************************************************
;
;	Name:		GraphicsChangeType
;
;	Purpose:	recreates the handle with the new type
;
;	Input:		the hwnd, hinstance and the new video type
;
;	Output:		the type is changed
;
;	Return:		A code indicating some result
;
**********************************************************/
PUBLIC RETCODE GraphicsChangeType(VTYPE vidtype, WINHANDLE hwnd, SOMEINSTANCE hinstance);

/**********************************************************
;
;	Name:		GraphicsLoadImage
;
;	Purpose:	loads an image from the filename and assign 
;				it with a newID.
;
;	Input:		the handle, the name of file and the ID
;
;	Output:		The list of image is appended.
;
;	Return:		A code indicating some result
;
**********************************************************/
PUBLIC RETCODE GraphicsLoadImage(char *filename, short newID, bool setSrcColorKey = false, ULONG low_color_val = 0, ULONG high_color_val = 0, IMGTYPE ImgType = IMG_BMP);

/**********************************************************
;
;	Name:		GraphicsMakeImageList
;
;	Purpose:	creates/recreates the image list in the 
;				graphics handle.  This will destroy all
;				previous data.
;
;	Input:		The new amount of image
;
;	Output:		the list is initialized and ready to be filled
;
;	Return:		some code
;
**********************************************************/
PUBLIC RETCODE GraphicsMakeImageList(short numImage);

/**********************************************************
;
;	Name:		GraphicsCreateSprite
;
;	Purpose:	creates a sprite out of thin air with a 
;				bunch of values. *note*: you must create an array of 
;				PFRAME states(don't forget to free it after calling this function!!!)
;				and fill it with what you want.
;
;	Input:		short imageID, short layer, 
;				short newID, LOCATION newLoc, 
;				short numImageColumn, LOCATION offset, SIZE newFrameSize, 
;				long newDelay, short newNumFrames, short newNumStates, 
;    		    PFRAME states
;				11 inputs, dang!
;
;	Output:		A sprite is created and inserted in the list
;
;	Return:		The sprite handle
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsCreateSprite(short imageID, short layer, 
									   short newID, LOCATION newLoc, 
									   short numImageColumn, LOCATION offset, SIZE newFrameSize, 
									   DWORD newDelay, short newNumFrames, short newNumStates, 
									   PFRAME states);

/**********************************************************
;
;	Name:		GraphicsCreateSimpleSprite
;
;	Purpose:	This will create a sprite with no other fuss
;				and hassles.  Although it's quite dull and boring,
;				the sprite will be nothing more than a mere
;				image static.
;
;	Input:		the imageID, layer, newID and location
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsCreateSimpleSprite(short imageID, short layer, 
									   short newID, LOCATION newLoc);

/**********************************************************
;
;	Name:		GraphicsCreateMyOwnSprite
;
;	Purpose:	creates a sprite out of thin air with a 
;				bunch of values. *note*: you must create an array of 
;				PFRAME states(don't forget to free it after calling this function!!!)
;				and fill it with what you want.
;
;	Input:		short imageID,
;				short newID, LOCATION newLoc, 
;				short numImageColumn, LOCATION offset, SIZE newFrameSize, 
;				long newDelay, short newNumFrames, short newNumStates, 
;    		    PFRAME states
;				heh, at least it's one lest param from the other version
;				So it's only 10, not that much.
;
;	Output:		A sprite is created and inserted in the list
;
;	Return:		The sprite handle
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsCreateMyOwnSprite(short imageID, short newID, LOCATION newLoc, 
									   short numImageColumn, LOCATION offset, SIZE newFrameSize, 
									   DWORD newDelay, short newNumFrames, short newNumStates, 
									   PFRAME states);

/**********************************************************
;
;	Name:		GraphicsCreateMyOwnSimpleSprite
;
;	Purpose:	This will create a sprite with no other fuss
;				and hassles.  Although it's quite dull and boring,
;				the sprite will be nothing more than a mere
;				image static.
;
;	Input:		the imageID, layer, newID and location
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsCreateMyOwnSimpleSprite(short imageID, 
									   short newID, LOCATION newLoc);

/**********************************************************
;
;	Name:		GraphicsCreateSimpleSpriteFromFile
;
;	Purpose:	This will create a sprite with no other fuss
;				and hassles.  Although it's quite dull and boring,
;				the sprite will be nothing more than a mere
;				image static.
;
;	Input:		the image path name,  color key, layer, newID and location
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsCreateSimpleSpriteFromFile(char *path, bool setColorkey, short layer, 
									   short newID, LOCATION newLoc, int surfacesizeX = 0, int surfacesizeY = 0);

/**********************************************************
;
;	Name:		GraphicsCreateMyOwnSimpleSpriteFromFile
;
;	Purpose:	This will create a sprite with no other fuss
;				and hassles.  Although it's quite dull and boring,
;				the sprite will be nothing more than a mere
;				image static.
;
;	Input:		char *path, bool setColorkey, newID and location
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsCreateMyOwnSimpleSpriteFromFile(char *path, bool setColorkey,
									   short newID, LOCATION newLoc, int surfacesizeX = 0, int surfacesizeY = 0);

/**********************************************************
;
;	Name:		SpriteDestroy
;
;	Purpose:	for the sake of destroying independent sprites,
;				can also destroy sprite from layer
;
;	Input:		the friggin' handle
;
;	Output:		dead sprite
;
;	Return:		none
;
**********************************************************/
PUBLIC void SpriteDestroy(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		SpriteCopyToLayer
;
;	Purpose:	copies a sprite to a given layer
;
;	Input:		the layer to copy to and the sprite
;
;	Output:		the layer is updated...OOOO wow! *sarcasm*
;
;	Return:		the new sprite that you just copied
;
**********************************************************/
PUBLIC SPRITEHANDLE SpriteCopyToLayer(short layer, SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		SpriteRemoveFromLayer
;
;	Purpose:	removes the given sprite from the layer
;				without getting killed
;
;	Input:		The sprite
;
;	Output:		the layer is updated
;
;	Return:		if successful or not
;
**********************************************************/
PUBLIC RETCODE SpriteRemoveFromLayer(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		SpriteRestoreImage
;
;	Purpose:	restore the sprite's image
;
;	Input:		the sprite handle
;
;	Output:		the sprite's image is reloaded
;
;	Return:		a code of something
;
**********************************************************/
PROTECTED RETCODE SpriteRestoreImage(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		GraphicsRestore
;
;	Purpose:	restores all sprites from layer
;
;	Input:		none
;
;	Output:		all image from the list is restored, it better!
;
;	Return:		a code of success...or not
;
**********************************************************/
PROTECTED RETCODE GraphicsRestore();

/**********************************************************
;
;	Name:		GraphicsCreateDisplayMap
;
;	Purpose:	Creates a display map to a specified layer
;				and will automatically be blitted on update.
;				*note*: This will copy the given tiles...
;				so if you plan to change the tiles, you
;				have to call GraphicsChangeDisplayMapTiles(short *newTiles);
;
;	Input:		the layer, the sprite handle, the num Row/Col, 
;				and the actual tiles
;
;	Output:		allocates a display map to a given layer
;
;	Return:		a code of success or failure
;
**********************************************************/
PUBLIC RETCODE GraphicsCreateDisplayMap(short layer, SPRITEHANDLE Sprite, short numCol, short numRow, short *tiles);

/**********************************************************
;
;	Name:		GraphicsDestroyDisplayMap
;
;	Purpose:	destroys the display map, MUWAHAHAHAHAHA!
;
;	Input:		the layer
;
;	Output:		dead displaymap
;
;	Return:		a worthless code
;
**********************************************************/
PUBLIC RETCODE GraphicsDestroyDisplayMap(short layer);

/**********************************************************
;
;	Name:		GraphicsChangeDisplayMapTiles
;
;	Purpose:	*note*: Make sure that you are giving it 
;				the tiles with the same number of row/col
;
;	Input:		which layer and the tiles
;
;	Output:		the tiles of the given layer is updated
;
;	Return:		none
;
**********************************************************/
PUBLIC void GraphicsChangeDisplayMapTiles(short layer, short *tiles);

/**********************************************************
;
;	Name:		GraphicsDeactivateDisplayMap
;
;	Purpose:	deactivates the display map of the given layer
;
;	Input:		the layer
;
;	Output:		the status of display map
;
;	Return:		none
;
**********************************************************/
PUBLIC void GraphicsDeactivateDisplayMap(short layer);

/**********************************************************
;
;	Name:		GraphicsActivateDisplayMap
;
;	Purpose:	activates the display map of the given layer
;
;	Input:		the layer
;
;	Output:		the status of display map
;
;	Return:		none
;
**********************************************************/
PUBLIC void GraphicsActivateDisplayMap(short layer);

/**********************************************************
;
;	Name:		GraphicsClearLayer
;
;	Purpose:	cleans up the given layer of friggin' sprite
;
;	Input:		the layer
;
;	Output:		a barren waste of a layer
;
;	Return:		some worthless code that even you wouldn't bother to check for
;
**********************************************************/
PUBLIC RETCODE GraphicsClearLayer(short layer);

/**********************************************************
;
;	Name:		GraphicsClearAllLayer
;
;	Purpose:	cleans up all the layer's friggin' sprites
;
;	Input:		none
;
;	Output:		a barren waste
;
;	Return:		some worthless code that even you wouldn't bother to check for
;
**********************************************************/
PUBLIC RETCODE GraphicsClearAllLayer();

/**********************************************************
;
;	Name:		GraphicsGetReserveLayerIndex
;
;	Purpose:	gives you the layer where all sprites are 
;				isolated and ignored.
;
;	Input:		none
;
;	Output:		none
;
;	Return:		the index, so then you can use it to add more isolated fools
;
**********************************************************/
PUBLIC long GraphicsGetReserveLayerIndex();

/**********************************************************
;
;	Name:		GraphicsMakeLayerList
;
;	Purpose:	creates/recreates the layer and sprite list
;
;	Input:		the number of layer nodes to make, the amount 
;				of sprite reserve for later use, the 
;				amount of sprite for each node and the
				boolean ThereCanBeOnlyOne...which means 
;				if all layers will have the same number of sprites
;
;	Output:		the sprite list is created/recreated
;
;	Return:		a code
;
**********************************************************/
PUBLIC RETCODE GraphicsMakeLayerList(short numLayer, short numSpriteReserve, short *numSprite, bool ThereCanBeOnlyOne = false);

/**********************************************************
;
;	Name:		GraphicsCopySprite
;
;	Purpose:	copies a specified sprite from the reserve 
;				layer to the given layer.
;
;	Input:		the ID of the sprite, the layer to duplicate to,
;				a new location.
;
;	Output:		the given layer is appended with a newly cloned sprite
;
;	Return:		the new spritehandle
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsCopySprite(short spriteID, short layer, LOCATION newLoc);

/**********************************************************
;
;	Name:		GraphicsGetSprite #1
;
;	Purpose:	grabs the specified sprite from the 
;				list...if it exists
;
;	Input:		the handle, pointer to sprite handle and the ID
;
;	Output:		ptrSprite will have the sprite, I garantee it!
;
;	Return:		A code indicating some result
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsGetSprite(short spriteID);

/**********************************************************
;
;	Name:		GraphicsGetSprite #2
;
;	Purpose:	grabs the specified sprite from the 
;				list...if it exists
;
;	Input:		the handle, pointer to sprite handle and the ID
;				and the layer to look into
;
;	Output:		ptrSprite will have the sprite, I garantee it!
;
;	Return:		A code indicating some result
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsGetSprite(short spriteID, short layer);

/**********************************************************
;
;	Name:		SpriteDeactivate
;
;	Purpose:	deactivates the given sprite, this means 
;				that the given sprite will be ignored by 
;				update.
;
;	Input:		the handle
;
;	Output:		the status of the sprite is changed
;
;	Return:		none
;
**********************************************************/
PUBLIC  void SpriteDeactivate(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		SpriteActivate
;
;	Purpose:	Activates the given sprite, this means 
;				that the given sprite will be updated
;
;	Input:		the handle
;
;	Output:		the status of the sprite is changed
;
;	Return:		none
;
**********************************************************/
PUBLIC  void SpriteActivate(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		SpriteSetLocation
;
;	Purpose:	set the sprite's new location x, y
;
;	Input:		the handle to sprite and the x & y
;
;	Output:		location of sprite updated
;
;	Return:		none
;
**********************************************************/
PUBLIC  void SpriteSetLocation(SPRITEHANDLE Sprite, long x, long y);

/**********************************************************
;
;	Name:		SpriteGetLocation
;
;	Purpose:	Get the sprite's location
;
;	Input:		the handle to sprite
;
;	Output:		none
;
;	Return:		pointer to sprite location
;
**********************************************************/
PUBLIC  PLOCATION SpriteGetLocation(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		GraphicsDeactivateLayer
;
;	Purpose:	Deactivates the layer
;
;	Input:		the layer to Deactivate
;
;	Output:		status of layer
;
;	Return:		none
;
**********************************************************/
PUBLIC  void GraphicsDeactivateLayer(short layer);

/**********************************************************
;
;	Name:		GraphicsActivateLayer
;
;	Purpose:	activates the layer
;
;	Input:		the layer to be activated
;
;	Output:		status of layer
;
;	Return:		none
;
**********************************************************/
PUBLIC  void GraphicsActivateLayer(short layer);

/**********************************************************
;
;	Name:		GraphicsSetLayerOffset
;
;	Purpose:	sets the offset of the layer
;
;	Input:		the layer number and the x/y
;
;	Output:		the layer offset is updated
;
;	Return:		none
;
**********************************************************/
PUBLIC  void GraphicsSetLayerOffset(short layer, long x, long y);

/**********************************************************
;
;	Name:		GraphicsSetLayerScroll
;
;	Purpose:	set up the layer's scroll ratio direction
;				when viewport moves.
;
;	Input:		the layer, the x and y
;
;	Output:		the layer's scrollration is changed
;
;	Return:		none
;
**********************************************************/
PUBLIC void GraphicsSetLayerScroll(short layer, long x, long y);

/**********************************************************
;
;	Name:		GraphicsCheckLocToViewport
;
;	Purpose:	Checks to see if the given location is within
;				the viewport or not...Padding included
;
;	Input:		the location and the Padding
;
;	Output:		none
;
;	Return:		true if location is within, otherwise...you know what
;
**********************************************************/
PUBLIC bool GraphicsCheckLocToViewport(const LOCATION loc, long Padding = 0, long paddingy = 0);

//
// Graphics Output
//

/**********************************************************
;
;	Name:		GraphicsUpdateScreen
;
;	Purpose:	updates the back buffer to the primary, 
;				thus updating the screen.
;
;	Input:		The rectangle of window rect for window mode
;				(use GetClientRect())
;
;	Output:		The primary is updated
;
;	Return:		A code indicating the results of updating 
;				the screen
;
**********************************************************/
PUBLIC  void GraphicsUpdateScreen(RECT *windowarea = NULL);

/**********************************************************
;
;	Name:		GraphicsBltSprite
;
;	Purpose:	blits the given sprite to the back buffer 
;				with it's current state, frame and location.
;
;	Input:		the handle and the sprite
;
;	Output:		the backbuffer is updated
;
;	Return:		A code indicating the results of blitting 
;				a sprite
;
**********************************************************/
PUBLIC  RETCODE GraphicsBltSprite(SPRITEHANDLE Sprite, PLOCATION otherLoc = NULL, bool viewPortConstraint = true);

/**********************************************************
;
;	Name:		GraphicsBltSpriteLayers #1
;
;	Purpose:	Blits all sprite from all layers.
;
;	Input:		none
;
;	Output:		the backbuffer is updated with a bunch 
;				of sprites
;
;	Return:		A code indicating some result
;
**********************************************************/
PUBLIC RETCODE GraphicsBltSpriteLayers();

/**********************************************************
;
;	Name:		GraphicsBltSpriteLayers #2
;
;	Purpose:	Blits all sprite that resides within the 
;				given layer range.
;
;	Input:		the handle and the layer Min/Max
;
;	Output:		the backbuffer is updated with a bunch 
;				of sprites
;
;	Return:		A code indicating some result
;
**********************************************************/
PUBLIC RETCODE GraphicsBltSpriteLayers(long layerMin, long layerMax);

/**********************************************************
;
;	Name:		GraphicsDisplaySpriteTiled
;
;	Purpose:	this will display a frame on the screen 
;				tiled from the top to bottom.  The screen 
;				location of the sprite is used for offsetting 
;				the position of the starting tile.
;
;	Input:		the sprite
;
;	Output:		backbuffer is filled with the sprite
;
;	Return:		A code indicating some result
;
**********************************************************/
PROTECTED RETCODE GraphicsDisplaySpriteTiled(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		SpriteDisplayMap
;
;	Purpose:	displays the sprite as a map
;
;	Input:		SPRITEHANDLE Sprite, short *tiles, short numCol, short numRow
;
;	Output:		none
;
;	Return:		some sort of code
;
**********************************************************/
PUBLIC RETCODE SpriteDisplayMap(SPRITEHANDLE Sprite, LOCATION layerOffset, short *tiles, short numCol, short numRow);

//
// Animation
//

/**********************************************************
;
;	Name:		SpriteSetState
;
;	Purpose:	Used to set/change the state of a sprite
;
;	Input:		the handle to sprite and the state
;
;	Output:		The Sprite's state is changed
;
;	Return:		A code indicating the results of setting 
;				the state
;
**********************************************************/
PUBLIC RETCODE SpriteSetState(SPRITEHANDLE Sprite, short state);

/**********************************************************
;
;	Name:		SpriteUpdateAnimation
;
;	Purpose:	Used to update a sprite's animation/frame
;
;	Input:		the handle to sprite
;
;	Output:		the current state of the sprite's frame 
;				is updated
;
;	Return:		none
;
**********************************************************/
PUBLIC  void SpriteUpdateAnimation(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		GraphicsUpdateAnimationLayers #1
;
;	Purpose:	same as SpriteUpdateAnimation, updates 
;				all sprites
;
;	Input:		none
;
;	Output:		the current state of the sprite's frame 
;				from all layer
;
;	Return:		A code indicating some result.
;
**********************************************************/
PUBLIC RETCODE GraphicsUpdateAnimationLayers();

/**********************************************************
;
;	Name:		GraphicsUpdateAnimationLayers #2
;
;	Purpose:	same as SpriteUpdateAnimation, updates 
;				the sprites within layer range.
;
;	Input:		the handle and layer Min/Max
;
;	Output:		the current state of the sprite's frame 
;				from the given range of the layer
;				is updated.
;
;	Return:		A code indicating some result.
;
**********************************************************/
PUBLIC RETCODE GraphicsUpdateAnimationLayers(long layerMin, long layerMax);

//
// Surface Manipulation
//

//AFTER GETTING THE SPRITE IMAGE, YOU CAN USE THE IMAGE MANIPULATION
//FUNCTIONS FOUND IN DDRAWSTUFF.H
//GOT IT!?  YOU BETTER!

/**********************************************************
;
;	Name:		SpriteGetImage
;
;	Purpose:	gives you the image handle
;
;	Input:		the sprite handle
;
;	Output:		none
;
;	Return:		the imagehandle
;
**********************************************************/
PROTECTED IMAGEHANDLE SpriteGetImage(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		GraphicsClearBuffer
;
;	Purpose:	clears up the backbuffer
;
;	Input:		none
;
;	Output:		the back buffer is cleared
;
;	Return:		none
;
**********************************************************/
PUBLIC void GraphicsClearBuffer(ULONG color);

/**********************************************************
;
;	Name:		GraphicsLock
;
;	Purpose:	will lock the backbuffer, ready to be 
;				manipulated.  The vidmem in DDRAWHANDLE 
;				will be filled as well as the lPitch.
;
;	Input:		none
;
;	Output:		the backbuffer is locked
;
;	Return:		none
;
**********************************************************/
PUBLIC  void GraphicsLock();

/**********************************************************
;
;	Name:		GraphicsUnlock
;
;	Purpose:	will unlock the backbuffer.  The vidmem is 
;				set to NULL as well as the lPitch.
;
;	Input:		none
;
;	Output:		the backbuffer is unlocked
;
;	Return:		none
;
**********************************************************/
PUBLIC  void GraphicsUnlock();

/**********************************************************
;
;	Name:		GraphicsGetBufferPitch
;
;	Purpose:	grabs the pitch of the backbuffer
;
;	Input:		none
;
;	Output:		none
;
;	Return:		the pitch
;
**********************************************************/
PUBLIC long GraphicsGetBufferPitch();

/**********************************************************
;
;	Name:		GraphicsGetBufferSize
;
;	Purpose:	grabs the size of the back buffer(might be different from screensize)
;
;	Input:		none
;
;	Output:		none
;
;	Return:		SIZE
;
**********************************************************/
PUBLIC SIZE GraphicsGetBufferSize();

/**********************************************************
;
;	Name:		GraphicsGetChannel
;
;	Purpose:	grabs the number of channel of the buffer
;
;	Input:		none
;
;	Output:		none
;
;	Return:		the number of channel
;
**********************************************************/
PUBLIC long GraphicsGetNumChannel();

/**********************************************************
;
;	Name:		GraphicsGetLinePtr
;
;	Purpose:	grabs the pointer of the buffer and returns it.
;				The backbuffer must be locked first.
;
;	Input:		the y value
;
;	Output:		none
;
;	Return:		a pointer to the specified location of the buffer
;
**********************************************************/
PUBLIC BYTE * GraphicsGetLinePtr(long y);

/**********************************************************
;
;	Name:		GraphicsGetPixelPtr
;
;	Purpose:	grpabs the pointer of the buffer and return it.
;				The image must be locked first.
;
;	Input:		the x, y value
;
;	Output:		none
;
;	Return:		a pointer to the specified location of the buffer
;
**********************************************************/
PUBLIC BYTE * GraphicsGetPixelPtr(long x, long y);

//
// FONT Management and Manipulation
//

//use this for color
/*#define TEXTCOLOR(r, g, b)*/

/**********************************************************
;
;	Name:		FontCreate
;
;	Purpose:	creates a font handle with the specified file and
;				color *note*: USE TEXTCOLOR(r,g,b) macro for color!!!
;
;	Input:		the filename and the color
;
;	Output:		none
;
;	Return:		the newly created FONTHANDLE
;
**********************************************************/
PUBLIC FONTHANDLE FontCreate(char *filename, DWORD color = TEXTCOLOR(255,255,255));

/**********************************************************
;
;	Name:		FontDestroy
;
;	Purpose:	destroys the given FONTHANDLE
;
;	Input:		the font handle
;
;	Output:		font handle is destroyed
;
;	Return:		a code indicating failure or success
;
**********************************************************/
PUBLIC RETCODE FontDestroy(FONTHANDLE FontType);

/**********************************************************
;
;	Name:		FontChangeColor
;
;	Purpose:	changes the color of the given font
;				*note*: USE TEXTCOLOR(r,g,b) macro for color!!!
;
;	Input:		the font handle and the new color
;
;	Output:		the font will have a new color...*wow!*
;
;	Return:		a code of success or failure
;
**********************************************************/
PUBLIC RETCODE FontChangeColor(FONTHANDLE FontType, DWORD color);

/**********************************************************
;
;	Name:		SpriteDisplayText
;
;	Purpose:	displays a text using a sprite, *note*: the 
;				sprite must contain 255 frames and they must 
;				be in ASCII order.  The location of the sprite
;				is used for the top-left of the text area
;
;	Input:		the sprite, the text, lower-left area, 
;				alignment and if you want it to be within 
;				the viewport
;
;	Output:		the buffer is updated
;
;	Return:		the left over index of the text (-1 if something bad happened)
;
**********************************************************/
PUBLIC INDEX FontDisplayText(FONTHANDLE FontType, char *text, RECT area, ALIGN alignment = TXT_LEFT, bool WithinViewPort = false);

/**********************************************************
;
;	Name:		FontGetHeight
;
;	Purpose:	returns the height of the font
;
;	Input:		the font handle
;
;	Output:		none
;
;	Return:		height of font
;
**********************************************************/
PUBLIC long FontGetHeight(FONTHANDLE FontType);

/**********************************************************
;
;	Name:		FontGetWidth
;
;	Purpose:	returns the cell width of the font
;
;	Input:		the font handle
;
;	Output:		none
;
;	Return:		height of font
;
**********************************************************/
PUBLIC long FontGetWidth(FONTHANDLE FontType);


/************************************************************************************************

Member Access/Set function stuff

************************************************************************************************/

//
// GRAPHICHANDLE
//

/**********************************************************
;
;	Name:		GraphicsGetVidType
;
;	Purpose:	returns the video type that you currently have
;
;	Input:		none
;
;	Output:		none
;
;	Return:		VTYPE
;
**********************************************************/
PUBLIC VTYPE GraphicsGetVidType();

/**********************************************************
;
;	Name:		GraphicsGetScrnSize
;
;	Purpose:	gets the screen size
;
;	Input:		none
;
;	Output:		none
;
;	Return:		the size
;
**********************************************************/
PUBLIC  SIZE GraphicsGetScrnSize();

/**********************************************************
;
;	Name:		GraphicsMoveViewPort
;
;	Purpose:	set the viewport location and size(optional)
;
;	Input:		the x & y increment
;
;	Output:		the viewport of graphics is set
;
;	Return:		none
;
**********************************************************/
PUBLIC void GraphicsMoveViewPort(long x, long y);

/**********************************************************
;
;	Name:		GraphicsMoveViewPort
;
;	Purpose:	set the viewport location and size(optional)
;
;	Input:		the x & y loc and optional width and height
;
;	Output:		the viewport of graphics is set
;
;	Return:		none
;
**********************************************************/
PUBLIC void GraphicsSetViewPort(long x, long y, long width = 0, long height = 0);

/**********************************************************
;
;	Name:		GraphicsGetViewPort
;
;	Purpose:	gets the viewport of the graphics
;
;	Input:		none
;
;	Output:		none
;
;	Return:		the rect
;
**********************************************************/
PUBLIC RECT GraphicsGetViewPort();

/**********************************************************
;
;	Name:		GraphicsGetResolutionList
;
;	Purpose:	grabs the listhandle full of list mode
;
;	Input:		none
;
;	Output:		none
;
;	Return:		the listhandle
;
**********************************************************/
PROTECTED LISTHANDLE GraphicsGetResolutionList();

/**********************************************************************************************
;
;	Name:		GraphicsGetMapSize
;
;	Purpose:	Accesses map size
;
;	Input:		A layer index
;
;	Output:		Access function
;
;	Return:		A size member
;
**********************************************************************************************/

PUBLIC SIZE GraphicsGetMapSize (INDEX layer);

//
// SPRITEHANDLE
//

/**********************************************************
;
;	Name:		SpriteGetMaxState
;
;	Purpose:	returns the member variable NumStates
;
;	Input:		the handle
;
;	Output:		none
;
;	Return:		the NumStates
;
**********************************************************/
PUBLIC  short SpriteGetMaxState(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		SpriteGetCurrentState
;
;	Purpose:	grabs the current state of the Sprite
;
;	Input:		the handle
;
;	Output:		none
;
;	Return:		the current state
;
**********************************************************/
PUBLIC short SpriteGetCurrentState(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		SpriteGetSize
;
;	Purpose:	grabs the frame size of the given sprite
;
;	Input:		the handle
;
;	Output:		none
;
;	Return:		the frame size
;
**********************************************************/
PUBLIC SIZE SpriteGetSize(SPRITEHANDLE Sprite);

//
// LAYER
//

/**********************************************************
;
;	Name:		LayerGetOffset
;
;	Purpose:	to get the offset of the layer
;
;	Input:		the pointer to layer
;
;	Output:		none
;
;	Return:		the offset
;
**********************************************************/
PRIVATE  LOCATION LayerGetOffset(LAYERHANDLE thisLayer);


/************************************************************************************************

Other Private function stuff

************************************************************************************************/

/**********************************************************
;
;	Name:		GraphicsDestroyLayerList
;
;	Purpose:	destroys the sprite list of the graphics handle
;
;	Input:		none
;
;	Output:		the list is destroyed and set to NULL
;
;	Return:		a code indicating mass destruction
;
**********************************************************/
PRIVATE RETCODE GraphicsDestroyLayerList();

/**********************************************************
;
;	Name:		SpriteDestroyData
;
;	Purpose:	destroys the data within the sprite
;
;	Input:		the sprite
;
;	Output:		dead data, but the sprite is still alive
;
;	Return:		a code of torture
;
**********************************************************/
PRIVATE void SpriteDestroyData(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		ClipRect
;
;	Purpose:	clips the given pointer rect with 
;				the given area.
;
;	Input:		The area and the pointer to the rect to 
;				be clipped
;
;	Output:		The pointer to the rect is modified.
;
;	Return:		If the rect is still valid or not.
;
**********************************************************/
PRIVATE  RETCODE ClipRect(const RECT area, RECT *rect);

/**********************************************************
;
;	Name:		TransferLocToRect
;
;	Purpose:	used for transforming a given coordinate 
;				to the area.  This is used for converting 
;				an object location to the screen location 
;				with the viewport.  
;
;	Input:		const RECT area, LOCATION loc, LOCATION *newLoc
;
;	Output:		newLoc is filled with new coordinates
;
;	Return:		A code indicating some result
;
**********************************************************/
PRIVATE  RETCODE TransferLocToRect(const RECT area, LOCATION *loc);

/************************************************************************************************
*************************************************************************************************
*************************************************************************************************

Special FX

*************************************************************************************************
*************************************************************************************************
************************************************************************************************/
#define MAXFXPARAM 10

//For all FX duration
#define INFINITEDURATION -1

//none of your business #define's
#define MAXFLYINGOBJECTSPEED 10


//FX types
typedef enum {
	FX_FLYINGTHINGS
} FXtype;

//message for FX functions
typedef enum {
	FXM_CREATE,
	FXM_DOSTUFF,
	FXM_DESTROY
} FXmessage;

//signals for FX update
typedef enum {
	FXsignal_animate,
	FXsignal_noanimate
} FXsignal;

//FX flags
//typedef enum {
#define	FXF_DEACTIVATE 1
#define	FXF_ACTIVATE 2
#define	FXF_NOINIT 4			//FX has just been created, you must call FXinit
//} FXflag;

//
// FX init struct for each type
//

// Flying things
typedef struct _flyingthing_init {
	long maxobj;	//maximum objects to be displayed on screen
	long delay;     //update delay
	long duration;  //duration of the whole FX
} flyingthing_init, * Pflyingthing_init;

////
//// NOTE: use the function SpriteGetFX(sprite) when passing the FXHANDLE
////

//
// FX Query Interface and update and access function
//

/**********************************************************
;
;	Name:		SpriteQueryFX
;
;	Purpose:	grabs a specified type of FX from the 
;				table of FX.  This will allocate for the new FXHANDLE
;				and initialize the specified type with the given initStruct,
;				*CAUTION*: YOU MUST GIVE IT THE CORRECT data struct depending
;				on type.  For example, FX_FLYINGTHINGS, you must give it
;				an array with size MAXFXPARAM...with correct values as well...
;
;	Input:		sprite handle FX type (see specialFX.h for different types), 
;				init array of size MAXFXPARAM 
;				*NOTE*: you must know what to put inside the array
;				see specialFX.h for each FX's init param comment.
;
;	Output:		the ptrFX is given the data
;
;	Return:		some code
;
**********************************************************/
PUBLIC RETCODE SpriteQueryFX(SPRITEHANDLE Sprite, long type, long *initarray);

/**********************************************************
;
;	Name:		SpriteGetFX
;
;	Purpose:	Grabs the FXHANDLE of the sprite.
;				This function is mainly used for calling the
;				FX functions in specialFX.h
;
;	Input:		the Sprite handle
;
;	Output:		none
;
;	Return:		the handle to FX
;
**********************************************************/
PUBLIC FXHANDLE SpriteGetFX(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		FXupdate
;
;	Purpose:	updates the FX on the screen
;
;	Input:		the Sprite
;
;	Output:		the sprite is displayed in different way
;
;	Return:		A code
;
**********************************************************/
PROTECTED RETCODE FXupdate(SPRITEHANDLE Sprite);

/**********************************************************
;
;	Name:		FXcopy
;
;	Purpose:	copies the special FX of the source sprite(if it has one)
;				to the dest. sprite(overwrites existing FX).
;
;	Input:		the Sprite source and destination
;
;	Output:		FX copied to destination sprite
;
;	Return:		a code of success or failure
;
**********************************************************/
PROTECTED RETCODE FXcopy(SPRITEHANDLE SpriteSrc, SPRITEHANDLE SpriteDest);


//
// FX management
//


/**********************************************************
;
;	Name:		FXdeactivate
;
;	Purpose:	deactivates the FX, therefore it will be 
;				ignored on update.
;
;	Input:		the FX handle
;
;	Output:		the FX status is set to deactivate
;
;	Return:		none
;
**********************************************************/
PUBLIC void FXdeactivate(FXHANDLE FX);

/**********************************************************
;
;	Name:		FXactivate
;
;	Purpose:	activates the FX, therefore it will be 
;				updated on a regular day-to-day basis
;
;	Input:		the FX handle
;
;	Output:		the FX status is set to activate
;
;	Return:		none
;
**********************************************************/
PUBLIC void FXactivate(FXHANDLE FX);

/**********************************************************
;
;	Name:		FXreset
;
;	Purpose:	resets the duration of the FX
;
;	Input:		the FX handle
;
;	Output:		the FX duration is set to maxduration.
;
;	Return:		huh?
;
**********************************************************/
PUBLIC void FXreset(FXHANDLE FX);

/**********************************************************
;
;	Name:		FXdestroy
;
;	Purpose:	destroys the given FX
;
;	Input:		the handle
;
;	Output:		dead FX
;
;	Return:		some CODE
;
**********************************************************/
PUBLIC RETCODE FXdestroy(FXHANDLE FX);


/**********************************************************
***********************************************************

  The methods for all FX and their corresponding data struct
  Don't you dare call these functions.

***********************************************************
**********************************************************/

//For all FX duration
//#define INFINITEDURATION -1

// flying things init params:
typedef enum {
	FlyParamMaxDuration, // max duration
	FlyParamMaxDelay,	 // max delay
	FlyParamNumObjs,     // number of objects
	FlyParamXDirection,  // x direction -1, 0, or 1
	FlyParamYDirection,  // y direction -1, 0, or 1
	MaxFlyParam
} flythingParam;

typedef struct _flything {
	short numObj;		//max number of objects
	PLOCATION objLocs; //locations of each object
	PLOCATION objSpd;  //speed of each object
	long Xdirection;    // 1 goes to the right. 0 does not move. -1 goes to the left.
	long Ydirection;	// 1 goes to the down. 0 does not move. -1 goes to the up.
} flything, * Pflything;

//blits the sprite to the backbuffer in a bunch of places with different speed 
PROTECTED long flyingthings(void *dumbParam, void *otherParam, long message);

//
// ARG?
//

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
PRIVATE long Random(long a1, long a2);


/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

// Boss visuals
#define BOSS_IMAGE			0x01	// Boss image group
#define BOSS_SPRITE			0x02	// Boss sprite group
// Enemy visuals
#define ENEMY_IMAGE			0x03	// Enemy image group
#define ENEMY_SPRITE		0x04	// Enemy sprite group
// Hamster visuals
#define HAMSTER_IMAGE		0x05	// Hamster image group
#define HAMSTER_SPRITE		0x06	// Hamster sprite group
// Item visuals
#define ITEM_IMAGE			0x07	// Item image group
#define ITEM_SPRITE			0x08	// Item sprite group
// Menu visuals
#define MENU_IMAGE			0x09	// Menu image group
#define MENU_SPRITE			0x0A	// Menu sprite group
// Menu item visuals
#define MENUITEM_IMAGE		0x0B	// Menu item image group
#define MENUITEM_SPRITE		0x0C	// Menu item sprite group
// Player visuals
#define PLAYER_IMAGE		0x0D	// Player image group
#define PLAYER_SPRITE		0x0E	// Player sprite group
// Projectile visuals
#define PROJECTILE_IMAGE	0x0F	// Projectile image group
#define PROJECTILE_SPRITE	0x10	// Projectile sprite group
// Scene visuals
#define SCENE_IMAGE			0x11	// Scene image group
#define SCENE_SPRITE		0x12	// Scene sprite group
// Tile visuals
#define TILE_IMAGE			0x13	// Tile image group
#define TILE_SPRITE			0x14	// Tile sprite group
// Weapon visuals
#define WEAPON_IMAGE		0x15	// Weapon image group
#define WEAPON_SPRITE		0x16	// Weapon sprite group

#endif
#ifndef _i_gdi_h
#define _i_gdi_h

/**********************************************************

  Game:		TED
  Team:     Plomero
  Author:   David Dionisio
  Modified: 10/17/00
 
**********************************************************/

#include "gdi.h"

typedef struct __FONT {
	IMAGEHANDLE Image;			//(IMAGEHANDLE) ; The image of the font
	LPBITMAPINFO lpbi;			//(BITMAPINFO *) ; Used for the sake of changing text
    LPVOID lpBits;				//(LPVOID) ; Used for the sake of changing text
	DWORD color;				//(DWORD) ; current color of the image
	TEXTMETRIC TextMetric;		//(TEXTMETRIC) ; The format of the font
	ABC ABCWidths[256];			//(ABC [256]) ; The offsets of each char
	LOGFONT LogFont;			//(LOGFONT) ; Description of the font
	SIZE CellSize;				//(SIZE) ; Size of each frame
	RECT SrcRects[256];			//(RECT [256]) ; Array of rects for each char
	long BPlusC[256];			//(long) ; Array of each increment through the string
} FONT, * PFONT;

typedef struct __DISPLAYMAP {
	FLAGS Status;				//(FLAGS) ; The status for displaying
	SPRITEHANDLE TileSprite;	//(SPRITEHANDLE) ; The tile for the map
	short *tileIndex;			//(short) ; index of each tile
	short numCol;				//(long) ; The number of column in the tileIndex
	short numRow;				//(long) ; The number of row in the tileIndex
} DISPLAYMAP, *PDISPLAYMAP;

typedef struct __LAYER {
	short layernum;			 //(long) ; This is basically the index, but you never know when it's needed
	FLAGS Status;			 //(FLAGS) ; Indicates status of layer
	LISTHANDLE Sprites;		 //<LISTHANDLE> ; Array of sprites in the layer
	PDISPLAYMAP displayMap;	 //(PDISPLAYMAP) ; Used for displaying the map, 
	LOCATION offset;		 //(LOCATION) ; the top most area of the layer.
	LOCATION scrollratio;	 //(LOCATION) ; the amount of scrolling when viewport moves.
} LAYER, * KAPISH;


typedef struct __FX {
	long Type;		  //(long) ; Specifies effect type
	void *Data;		  //(void *) ; Context to which effect is applied
	EFFECT Effect;	  //(EFFECT) ; Callback function for effect
	long Duration;	  //(long) ; Current duration of effect
	long MaxDuration; //(long) ; Maximum duration of effect
	PTIMER ticker;	  //(PTIMER) ; the timer for animation
	DWORD MaxDelay;	  //(DWORD) ; Delay used to update effect
	FLAGS Status;	  //(FLAGS) ; Indicates status of effect
	long initParam[MAXFXPARAM]; //the parameters used to initialize the FX, this is used internally
} FX, * PFX;

typedef struct __SPRITE {
	short ID;			   //(short) ; Sprite identifier
	long index;			   //(long) ; Index within layer list
	short imageID;		   //(short) ; the image ID of the sprite, mainly used for duplication
	LAYERHANDLE Layer;	   //(PLAYER) ; Pointer to the layer
	FLAGS Status;          //(FLAGS) ; Flag variable used to control sprite
	LOCATION ScreenLoc;	   //(LOCATION) ; Location of sprite on screen
	IMAGEHANDLE Image;	   //(IMAGEHANDLE) ; Image data for sprite
	PTIMER ticker;		   //(PTIMER) ; the timer for animation
	DWORD MaxDelay;		   //(long) ; Delay used to update animation
	short CurFrame;		   //(short) ; Index of current frame
	short NumFrames;	   //(short) ; Number of state frames
	short CurState;		   //(short) ; Index of current state
	short NumStates;	   //(short) ; Number of sprite states
	SIZE FrameSize;		   //(SIZE) ; The size of each frame
	short numImgCol;	   //(short) ; Mainly used for duplication
	LOCATION imgOffset;	   //(LOCATION) ; Mainly used for duplication
	PFX FX;                //(PFX) ; The FX 
	PRECT FrameRects;	   //<RECT *> ; Array of rects for each frames
	PFRAME States;         //<FRAME *> ; Array of sprite states
} SPRITE, * PSPRITE;

typedef struct __GRAPHICS {
	VTYPE videotype;		//(VTYPE) ; it's either fullscreen or windowed, no multi-window!!!
	DDRAWHANDLE DDraw;      //(DDRAWHANDLE) ; Direct Draw information
    VIEWPORT Viewport;      //(VIEWPORT) ; Defines the region into which graphics are displayed
    SIZE ScreenSize;        //(SIZE) ; Dimensions of screen in current video mode
	short numLayers;		//(short) ; The number of layer
    LAYER *Layers;          //<LAYER *> ; List of layers that each contains a bunch of sprite
	//IMAGEHANDLE extraBuffer;//(IMAGEHANDLE) ; Used for displaying texts, polygons, etc...
    /*- Tiles:              //<_LIST *> ; List of tiles for display
    - ItemGFX:              //<_LIST *> ; List of item images for display
    - QuestItemGFX:         //<_LIST *> ; List of quest item images for display
    - WeaponGFX:            //<_LIST *> ; List of weapon images for display
    - ProjectileGFX:        //<_LIST *> ; List of projectile images for display */
} GRAPHICS, * PGRAPHICS;

//
// My own global variable that no one can grab
//
PRIVATE PGRAPHICS Graphics;

//
// LIST function callbacks
//

// Sprite ID search function
RETCODE SpriteIDSearch(void *thing1, void *thing2);

#endif
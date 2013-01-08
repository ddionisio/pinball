/**********************************************************

  Game:		TED
  Team:     Plomero
  Author:   David Dionisio
  Modified: 10/19/00
 
**********************************************************/

#include "gdi.h"
#include "i_gdi.h"


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
PROTECTED RETCODE GraphicsInit (WINHANDLE hwnd, SOMEINSTANCE hinstance, VTYPE vidtype, Pdisplaymode mode)
{
	//create the handle
	if(MemAlloc((void **)(&Graphics), sizeof(GRAPHICS)) != RETCODE_SUCCESS)
	{
		assert(!"Unable to allocate GRAPHICSHANDLE, David says: 'My bad'");
		return VD_BAD;
	}

	if(Graphics == NULL)
	{ assert(!"Steve?  Why isn't my Graphics correctly malloced?"); return VD_BAD; }

	memset(Graphics, 0, sizeof(GRAPHICS));

	//Initialize the ddrawhandle
	Graphics->DDraw = ddraw_create();

	if(Graphics->DDraw == NULL)
	{
		assert(!"Unable to allocate DDRAWSTUFF, David says: 'd'oh'");
	}


	//initialize the video handle of the GRAPHICHANDLE
	switch(vidtype)
	{
	case VDTYPE_FULLSCREEN:
		if(create_ddraw_fullscreen(Graphics->DDraw, hwnd, hinstance, mode) != DDRAW_OK)
		{
			assert(!"Unable to create ddraw in fullscreen, David says: 'don't blame me'");
			return VD_BAD;
		}
		break;
	case VDTYPE_WINDOWED:
		if(create_ddraw_windowed(Graphics->DDraw, hwnd, hinstance) != DDRAW_OK)
		{
			assert(!"Unable to create ddraw in window, David says: 'don't blame me'");
			return VD_BAD;
		}
		break;
	case VDTYPE_FULLSCRNCLIPPER:
		if(create_ddraw_fullscrnclipper(Graphics->DDraw, hwnd, hinstance, mode) != DDRAW_OK)
		{
			assert(!"Unable to create ddraw in fullscrnclipper, David says: 'don't blame me'");
			return VD_BAD;
		}
		break;
	default:
		assert(!"You didn't specify a correct VDTYPE, you ...");
		return VD_BAD;
	}

	Graphics->ScreenSize = get_screensize(Graphics->DDraw);
	GraphicsSetViewPort(0, 0, Graphics->ScreenSize.cx, Graphics->ScreenSize.cy);

	//create the extra buffer just for the sake of TEXT DISPLAY!!!
	//SIZE backbuffSize = DDGetSize(Graphics->DDraw);

	//Graphics->extraBuffer = DDCreateEmptyImage(Graphics->DDraw, backbuffSize.cx, backbuffSize.cy, true);
	//if(!Graphics->extraBuffer)
	//{ assert(!"Sorry Mark, not fontdisplaying for you!  Unable to make extra buffer, error in Graphics Creation"); return VD_BAD; }


	/*if(GraphicsMakeImageList(NumImageBuffer) != VD_OK)
	{ assert(!"GraphicsMakeImageList failed on GraphicsInit"); GraphicsTerm(); return VD_BAD; }
	if(GraphicsMakeLayerList(NumLayer, NumSpritePerLayer, ThereCanBeOnlyOne) != VD_OK)
	{ assert(!"GraphicsMakeLayerList failed on GraphicsInit"); GraphicsTerm(); return VD_BAD; }*/

	return VD_OK;
}

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
PROTECTED RETCODE GraphicsTerm()
{
	if(Graphics)
	{
		//if(Graphics->extraBuffer)
		//	if(DDImageDestroy(Graphics->DDraw, Graphics->extraBuffer) != DDRAW_OK)
		//	{ assert(!"Unable to destroy extraBuffer, thanks to somebody who wanted to display texts in fancy ways!  Error in GraphicsTerm"); return VD_BAD; }

		if(ddraw_destroy(Graphics->DDraw) != DDRAW_OK)
		{ assert(!"ddraw_destroy failed on GraphicsTerm"); return VD_BAD; }
		
		if(GraphicsDestroyLayerList() != VD_OK)
		{ assert(!"GraphicsDestroyLayerList failed on GraphicsTerm"); return VD_BAD; }

		MemFree((void**)(&Graphics));

		//Graphics = NULL;
	}

	return VD_OK;
}

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
PUBLIC RETCODE GraphicsSetMode(int width, int height, int colordepth, int refreshrate)
{
	assert(Graphics);

	//call the set_mode of video handle
	if(set_our_display_mode(Graphics->DDraw, width, height, colordepth, refreshrate) != DDRAW_OK)
	{ assert(!"Unable to set display mode, David says: 'What!? It can not be!'"); return VD_BAD; }

	//restore all images
	if(GraphicsRestore() != VD_OK)
	{ assert(!"Unable to restore images, David says: 'There goes my two stars'"); return VD_BAD; }

	Graphics->ScreenSize = get_screensize(Graphics->DDraw);
	GraphicsSetViewPort(0, 0, Graphics->ScreenSize.cx, Graphics->ScreenSize.cy);

	return VD_OK;
}

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
PUBLIC RETCODE GraphicsChangeType(VTYPE vidtype, WINHANDLE hwnd, SOMEINSTANCE hinstance)
{
	assert(Graphics);
	//recreate the video handle and surfaces depending on type

	//initialize the video handle of the GRAPHICHANDLE
	switch(vidtype)
	{
	case VDTYPE_FULLSCREEN:
		if(create_ddraw_fullscreen(Graphics->DDraw, hwnd, hinstance) != DDRAW_BAD)
		{
			assert(!"Unable to create ddraw in fullscreen, David says: 'don't blame me'");
			return VD_BAD;
		}
		break;
	case VDTYPE_WINDOWED:
		if(Graphics->videotype != VDTYPE_WINDOWED)
			restore_display_mode(Graphics->DDraw);
		if(create_ddraw_windowed(Graphics->DDraw, hwnd, hinstance) != DDRAW_BAD)
		{
			assert(!"Unable to create ddraw in window, David says: 'don't blame me'");
			return VD_BAD;
		}
		break;
	case VDTYPE_FULLSCRNCLIPPER:
		if(create_ddraw_fullscrnclipper(Graphics->DDraw, hwnd, hinstance) != DDRAW_OK)
		{
			assert(!"Unable to create ddraw in fullscrnclipper, David says: 'don't blame me'");
			return VD_BAD;
		}
		break;
	default:
		assert(!"You didn't specify a correct VDTYPE, you ...");
		return VD_BAD;
	}

	Graphics->ScreenSize = get_screensize(Graphics->DDraw);
	GraphicsSetViewPort(0, 0, Graphics->ScreenSize.cx, Graphics->ScreenSize.cy);

	return VD_OK;
}

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
PUBLIC RETCODE GraphicsMakeImageList(short numImage)
{
	assert(Graphics);
	if(DDInitImageList(Graphics->DDraw, numImage) != DDRAW_OK)
		return VD_BAD;

	return VD_OK;
}

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
PUBLIC RETCODE GraphicsLoadImage(char *filename, short newID, bool setSrcColorKey, ULONG low_color_val, ULONG high_color_val, IMGTYPE ImgType)
{
	assert(Graphics);
	//call the video load image function
	//nothing more than a wrapper function
	if(!DDCreateImage(Graphics->DDraw, newID, filename, setSrcColorKey, low_color_val, high_color_val, ImgType))
	{
		assert(!"Damn it!  Unable to load image!  David says: 'yeah, damn it!'");
		return VD_BAD;
	}
	

	return VD_OK;
}

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
									   PFRAME states)
{
	assert(Graphics);
	//ack!  Layer list must be initialized!
	assert(Graphics->Layers);

	//check for every single god damn params for dummies

	//make sure the layer is below the maxlayer
	//Graphics->numLayers + LayerMaxReserve means that layer reserve is included
	assert(layer < Graphics->numLayers + LayerMaxReserve);
	//num image column should at least be one
	assert(numImageColumn > 0);
	//number of frames has to at least be one
	assert(newNumFrames > 0);
	//number of states has to at least be one
	assert(newNumStates > 0);
	//given states cannot be NULL
	assert(states);

	//this will be used later to append the sprite
	LAYERHANDLE thisLayer = &(Graphics->Layers[layer]);
	assert(thisLayer);

	//check to see if we have available spaces
	assert(ListGetMaxNodes(thisLayer->Sprites) - ListGetNodeCount(thisLayer->Sprites) > 0);

	//Make sure we are not creating a sprite with an identical ID to any other
	//Only within the sprite reserve layer, we don't care about the rest...
	if(layer == Graphics->numLayers) //numLayers is the layer sprite reserve
		if(GraphicsGetSprite(newID, Graphics->numLayers) != NULL)
		{
			assert(!"Sprite ID already exists, you dummy!");
			return NULL;
		}

	SPRITE newSprite;
	memset(&newSprite, 0, sizeof(newSprite));

	//create a sprite with the junk given
	//insert in the list of sprite

	//insert the junk
	newSprite.ID = newID;
	newSprite.index = ListGetNodeCount(thisLayer->Sprites);
	newSprite.imageID = imageID;
	newSprite.ScreenLoc = newLoc;
	newSprite.Layer = thisLayer;
	newSprite.MaxDelay = newDelay;
	newSprite.NumFrames = newNumFrames;
	newSprite.FrameSize = newFrameSize;
	newSprite.NumStates = newNumStates;
	newSprite.numImgCol = numImageColumn;
	newSprite.imgOffset = offset;

	newSprite.ticker = TimerInit();
	
	//get the pointer to the image
	newSprite.Image = DDGetImage(Graphics->DDraw, imageID);

	if(!newSprite.Image)
	{ assert(!"Bad Image ID or unable to get Image...either way it's BAD!");  return NULL; }

	SETFLAG(newSprite.Status, GFLAG_ACTIVATE);

	//
	//Create the array of rectangles and set it up with the image source
	//
	if(MemAlloc((void**)&newSprite.FrameRects, sizeof(RECT)*newNumFrames) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! Frame init failed"); return NULL; }

	//first make sure that ListInit is successful
	if(newSprite.FrameRects == NULL)
	{ assert(!"Unable to create FrameRects, oh master.  MASTER!? MASTER! NO!!!  ARG! *dead*"); return NULL; }

	// I don't trust MemAlloc
	memset(newSprite.FrameRects, 0, sizeof(RECT)*newNumFrames);

	//...and now we must fill in the rects

	//pre-CALIculate the rectangles for each frames
	for(int i = 0; i < newNumFrames; i++)
	{
		newSprite.FrameRects[i].left   = i % numImageColumn * newFrameSize.cx + offset.x;
		newSprite.FrameRects[i].top    = i / numImageColumn * newFrameSize.cy + offset.y; //offset
		newSprite.FrameRects[i].right  = newSprite.FrameRects[i].left + newFrameSize.cx; //plus the size of frame
		newSprite.FrameRects[i].bottom = newSprite.FrameRects[i].top + newFrameSize.cy; //plus size of frame
	}
	//done with the rects, on with the significant others
	
	//
	//Create the states of the sprite
	//
	if(MemAlloc((void**)&newSprite.States, sizeof(FRAME)*newNumStates) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! States init failed"); return NULL; }

	//make sure that States is initialized correctly
	if(newSprite.States == NULL)
	{ 
		assert(!"Unable to create States, David says: 'It's not my fault, honest!'");
		MemFree((void**)&newSprite.FrameRects);
		TimerTerm(newSprite.ticker);
		return NULL; 
	}

	// I don't trust MemAlloc
	memset(newSprite.States, 0, sizeof(FRAME)*newNumStates);

	for(int j = 0; j < newNumStates; j++)
	{
		newSprite.States[j].firstframe = states[j].firstframe;
		newSprite.States[j].lastframe = states[j].lastframe;
	}
	//done with the states.


	//...and last but not least, insert the sprite to the list
	if(ListAppend(thisLayer->Sprites, &newSprite) != RETCODE_SUCCESS)
	{
		//ah shit
		assert(!"Unable to append sprite to the list, damn!");
		MemFree((void**)&newSprite.FrameRects);
		MemFree((void**)&newSprite.States);
		TimerTerm(newSprite.ticker);
		return NULL;
	}

		
	return (SPRITEHANDLE)ListExtract(thisLayer->Sprites, newSprite.index);
}

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
									   short newID, LOCATION newLoc)
{
	assert(Graphics);
	//ack!  Layer list must be initialized!
	assert(Graphics->Layers);

	//make sure the layer is below the maxlayer
	assert(layer < Graphics->numLayers + LayerMaxReserve);

	//this will be used later to append the sprite
	//this will be used later to append the sprite
	LAYERHANDLE thisLayer = &(Graphics->Layers[layer]);
	assert(thisLayer);

	//check to see if we have available spaces
	assert(ListGetMaxNodes(thisLayer->Sprites) - ListGetNodeCount(thisLayer->Sprites) > 0);


	//Make sure we are not creating a sprite with an identical ID to any other
	//Only within the sprite reserve layer, we don't care about the rest...
	if(layer == Graphics->numLayers) //numLayers is the layer sprite reserve
		if(GraphicsGetSprite(newID, Graphics->numLayers) != NULL)
		{
			assert(!"Sprite ID already exists, you dummy!");
			return NULL;
		}

	SPRITE newSprite;
	memset(&newSprite, 0, sizeof(newSprite));

	//create a sprite with the junk given
	//insert in the list of sprite

	//insert the junk
	newSprite.ID = newID;
	newSprite.index = ListGetNodeCount(thisLayer->Sprites);
	newSprite.imageID = imageID;
	newSprite.ScreenLoc = newLoc;
	newSprite.Layer = thisLayer;
	newSprite.MaxDelay = 0;
	newSprite.NumFrames = 1;
	newSprite.NumStates = 1;
	newSprite.numImgCol = 1;
	newSprite.imgOffset.x = 0;
	newSprite.imgOffset.y = 0;

	newSprite.ticker = TimerInit();
	
	//get the pointer to the image
	newSprite.Image = DDGetImage(Graphics->DDraw, imageID);

	if(!newSprite.Image)
	{ assert(!"Bad Image ID or unable to get Image...either way it's BAD!");  return NULL; }

	newSprite.FrameSize = DDGetSize(newSprite.Image);

	SETFLAG(newSprite.Status, GFLAG_ACTIVATE);

	//
	//Create the array of rectangles and set it up with the image source
	//
	if(MemAlloc((void**)&newSprite.FrameRects, sizeof(RECT)) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! Frame init failed"); return NULL; }

	// I don't trust MemAlloc
	memset(newSprite.FrameRects, 0, sizeof(RECT));

	//first make sure that ListInit is successful
	if(newSprite.FrameRects == NULL)
	{ assert(!"Unable to create FrameRects, oh master.  MASTER!? MASTER! NO!!!  ARG! *dead*"); return NULL; }

	//...and now we must fill in the rects
	
	//pre-CALIculate the rectangles for each frames
		newSprite.FrameRects[0].left   = 0;
		newSprite.FrameRects[0].top    = 0; //offset
		newSprite.FrameRects[0].right  = newSprite.FrameSize.cx; //plus the size of frame
		newSprite.FrameRects[0].bottom = newSprite.FrameSize.cy; //plus size of frame
		

	//done with the rects, on with the significant others
	
	//
	//Create the states of the sprite
	//
	if(MemAlloc((void**)&newSprite.States, sizeof(FRAME)) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! States init failed"); return NULL; }

	// I don't trust MemAlloc
	memset(newSprite.States, 0, sizeof(FRAME));

	//make sure that States is initialized correctly
	if(newSprite.States == NULL)
	{ 
		assert(!"Unable to create States, David says: 'It's not my fault, honest!'");
		MemFree((void**)&newSprite.FrameRects);
		TimerTerm(newSprite.ticker);
		return NULL; 
	}

		newSprite.States[0].firstframe = 0;
		newSprite.States[0].lastframe = 0;

	//done with the states.


	//...and last but not least, insert the sprite to the list
	if(ListAppend(thisLayer->Sprites, &newSprite) != RETCODE_SUCCESS)
	{
		//ah shit
		assert(!"Unable to append sprite to the list, damn!");
		MemFree((void**)&newSprite.FrameRects);
		MemFree((void**)&newSprite.States);
		TimerTerm(newSprite.ticker);
		return NULL;
	}

		
	return (SPRITEHANDLE)ListExtract(thisLayer->Sprites, newSprite.index);
}

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
									   PFRAME states)
{
	assert(Graphics);

	//check for every single god damn params for dummies

	//num image column should at least be one
	assert(numImageColumn > 0);
	//number of frames has to at least be one
	assert(newNumFrames > 0);
	//number of states has to at least be one
	assert(newNumStates > 0);
	//given states cannot be NULL
	assert(states);

	SPRITEHANDLE newSprite = NULL;
	if(MemAlloc((void**)&newSprite, sizeof(SPRITE)) != RETCODE_SUCCESS)
	{ assert(!"Ah shit, GraphicsCreateMyOwnSpriteNotToBeInsertedToALayer failed to allocate newSprite"); return NULL; }
	memset(newSprite, 0, sizeof(SPRITE));

	//create a sprite with the junk given
	//insert in the list of sprite

	//insert the junk
	newSprite->ID = newID;
	newSprite->index = AntiSocialSpriteThatDoesNotBelongToAnyLayer;
	newSprite->imageID = imageID;
	newSprite->ScreenLoc = newLoc;
	newSprite->Layer = NULL;
	newSprite->MaxDelay = newDelay;
	newSprite->NumFrames = newNumFrames;
	newSprite->FrameSize = newFrameSize;
	newSprite->NumStates = newNumStates;
	newSprite->numImgCol = numImageColumn;
	newSprite->imgOffset = offset;

	newSprite->ticker = TimerInit();
	
	//get the pointer to the image
	newSprite->Image = DDGetImage(Graphics->DDraw, imageID);

	if(!newSprite->Image)
	{ assert(!"Bad Image ID or unable to get Image...either way it's BAD!");  return NULL; }

	SETFLAG(newSprite->Status, GFLAG_ACTIVATE);

	//
	//Create the array of rectangles and set it up with the image source
	//
	if(MemAlloc((void**)&newSprite->FrameRects, sizeof(RECT)*newNumFrames) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! Frame init failed"); return NULL; }

	//first make sure that ListInit is successful
	if(newSprite->FrameRects == NULL)
	{ assert(!"Unable to create FrameRects, oh master.  MASTER!? MASTER! NO!!!  ARG! *dead*"); return NULL; }

	// I don't trust MemAlloc
	memset(newSprite->FrameRects, 0, sizeof(RECT)*newNumFrames);

	//...and now we must fill in the rects
	//pre-CALIculate the rectangles for each frames
	for(int i = 0; i < newNumFrames; i++)
	{
		//thisRect = (RECT*)ListExtract(newSprite.FrameRects, i);

		//assert(thisRect);

		newSprite->FrameRects[i].left   = i % numImageColumn * newFrameSize.cx + offset.x;
		newSprite->FrameRects[i].top    = i / numImageColumn * newFrameSize.cy + offset.y; //offset
		newSprite->FrameRects[i].right  = newSprite->FrameRects[i].left + newFrameSize.cx; //plus the size of frame
		newSprite->FrameRects[i].bottom = newSprite->FrameRects[i].top + newFrameSize.cy; //plus size of frame
	}
	//done with the rects, on with the significant others
	
	//
	//Create the states of the sprite
	//
	if(MemAlloc((void**)&newSprite->States, sizeof(FRAME)*newNumStates) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! States init failed"); return NULL; }

	//make sure that States is initialized correctly
	if(newSprite->States == NULL)
	{ 
		assert(!"Unable to create States, David says: 'It's not my fault, honest!'");
		MemFree((void**)&newSprite->FrameRects);
		TimerTerm(newSprite->ticker);
		return NULL; 
	}

	// I don't trust MemAlloc
	memset(newSprite->States, 0, sizeof(FRAME)*newNumStates);

	//fill the new states with the given states
	for(int j = 0; j < newNumStates; j++)
	{
		newSprite->States[j].firstframe = states[j].firstframe;
		newSprite->States[j].lastframe = states[j].lastframe;
	}
	//done with the states.
		
	return newSprite;
}

/**********************************************************
;
;	Name:		GraphicsCreateMyOwnSimpleSprite
;
;	Purpose:	This will create a sprite with no other fuss
;				and hassles.  Although it's quite dull and boring,
;				the sprite will be nothing more than a mere
;				image static.
;
;	Input:		the imageID, newID and location
;
;	Output:		
;
;	Return:		
;
**********************************************************/
PUBLIC SPRITEHANDLE GraphicsCreateMyOwnSimpleSprite(short imageID, 
									   short newID, LOCATION newLoc)
{
	SPRITEHANDLE newSprite = NULL;
	if(MemAlloc((void**)&newSprite, sizeof(SPRITE)) != RETCODE_SUCCESS)
	{ assert(!"Ah shit, GraphicsCreateMyOwnSpriteNotToBeInsertedToALayer failed to allocate newSprite"); return NULL; }
	memset(newSprite, 0, sizeof(SPRITE));

	//create a sprite with the junk given
	//insert in the list of sprite

	//insert the junk
	newSprite->ID = newID;
	newSprite->index = AntiSocialSpriteThatDoesNotBelongToAnyLayer;
	newSprite->imageID = imageID;
	newSprite->ScreenLoc = newLoc;
	newSprite->Layer = NULL;
	newSprite->MaxDelay = 0;
	newSprite->NumFrames = 1;
	newSprite->NumStates = 1;
	newSprite->numImgCol = 1;
	newSprite->imgOffset.x = 0;
	newSprite->imgOffset.y = 0;

	newSprite->ticker = TimerInit();
	
	//get the pointer to the image
	newSprite->Image = DDGetImage(Graphics->DDraw, imageID);

	if(!newSprite->Image)
	{ assert(!"Bad Image ID or unable to get Image...either way it's BAD!");  return NULL; }

	newSprite->FrameSize = DDGetSize(newSprite->Image);

	SETFLAG(newSprite->Status, GFLAG_ACTIVATE);

	//
	//Create the array of rectangles and set it up with the image source
	//
	if(MemAlloc((void**)&newSprite->FrameRects, sizeof(RECT)) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! Frame init failed"); return NULL; }

	//first make sure that ListInit is successful
	if(newSprite->FrameRects == NULL)
	{ assert(!"Unable to create FrameRects, oh master.  MASTER!? MASTER! NO!!!  ARG! *dead*"); return NULL; }
	
	// I don't trust MemAlloc
	memset(newSprite->FrameRects, 0, sizeof(RECT));

	//...and now we must fill in the rects

	//pre-CALIculate the rectangles for each frames
		newSprite->FrameRects[0].left   = 0;
		newSprite->FrameRects[0].top    = 0; //offset
		newSprite->FrameRects[0].right  = newSprite->FrameSize.cx; //plus the size of frame
		newSprite->FrameRects[0].bottom = newSprite->FrameSize.cy; //plus size of frame
	//done with the rects, on with the significant others
	
	//
	//Create the states of the sprite
	//
	if(MemAlloc((void**)&newSprite->States, sizeof(FRAME)) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! States init failed"); return NULL; }

	//make sure that States is initialized correctly
	if(newSprite->States == NULL)
	{ 
		assert(!"Unable to create States, David says: 'It's not my fault, honest!'");
		MemFree((void**)&newSprite->FrameRects);
		TimerTerm(newSprite->ticker);
		return NULL; 
	}

	// I don't trust MemAlloc
	memset(newSprite->States, 0, sizeof(FRAME));

	//fill the new states with the given states
		newSprite->States[0].firstframe = 0;
		newSprite->States[0].lastframe = 0;
	//done with the states.
		
	return newSprite;
}

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
									   short newID, LOCATION newLoc, int surfacesizeX, int surfacesizeY)
{
	assert(Graphics);
	//ack!  Layer list must be initialized!
	assert(Graphics->Layers);

	//make sure the layer is below the maxlayer
	assert(layer < Graphics->numLayers + LayerMaxReserve);

	//this will be used later to append the sprite
	//this will be used later to append the sprite
	LAYERHANDLE thisLayer = &(Graphics->Layers[layer]);
	assert(thisLayer);

	//check to see if we have available spaces
	assert(ListGetMaxNodes(thisLayer->Sprites) - ListGetNodeCount(thisLayer->Sprites) > 0);

	//Make sure we are not creating a sprite with an identical ID to any other
	//Only within the sprite reserve layer, we don't care about the rest...
	if(layer == Graphics->numLayers) //numLayers is the layer sprite reserve
		if(GraphicsGetSprite(newID, Graphics->numLayers) != NULL)
		{
			assert(!"Sprite ID already exists, you dummy!");
			return NULL;
		}

	SPRITE newSprite;
	memset(&newSprite, 0, sizeof(newSprite));

	//create a sprite with the junk given
	//insert in the list of sprite

	//insert the junk
	newSprite.ID = newID;
	newSprite.index = ListGetNodeCount(thisLayer->Sprites);
	newSprite.imageID = ImageDummyID;
	newSprite.ScreenLoc = newLoc;
	newSprite.Layer = thisLayer;
	newSprite.MaxDelay = 0;
	newSprite.NumFrames = 1;
	newSprite.NumStates = 1;
	newSprite.numImgCol = 1;
	newSprite.imgOffset.x = 0;
	newSprite.imgOffset.y = 0;

	newSprite.ticker = TimerInit();
	
	//load the image from file
	if(surfacesizeX == 0 || surfacesizeY == 0)
		newSprite.Image = DDCreateImage(Graphics->DDraw, ImageDummyID, path, setColorkey);
	else
	{
		newSprite.Image =  DDCreateEmptyImage(Graphics->DDraw, surfacesizeX, surfacesizeY, ImageDummyID, setColorkey);

		if(!newSprite.Image)
		{ assert(!"unable to create surface image...sigh");  return NULL; }
		
		DDReloadImage(Graphics->DDraw, newSprite.Image, path);
		
		if(setColorkey)
			DDSetImageColorKey(newSprite.Image);
	}

	if(!newSprite.Image)
	{ assert(!"Bad Image ID or unable to get Image...either way it's BAD!");  return NULL; }

	newSprite.FrameSize = DDGetSize(newSprite.Image);

	SETFLAG(newSprite.Status, GFLAG_ACTIVATE);

	//
	//Create the array of rectangles and set it up with the image source
	//
	if(MemAlloc((void**)&newSprite.FrameRects, sizeof(RECT)) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! Frame init failed"); return NULL; }

	// I don't trust MemAlloc
	memset(newSprite.FrameRects, 0, sizeof(RECT));

	//first make sure that ListInit is successful
	if(newSprite.FrameRects == NULL)
	{ assert(!"Unable to create FrameRects, oh master.  MASTER!? MASTER! NO!!!  ARG! *dead*"); return NULL; }

	//...and now we must fill in the rects
	
	//pre-CALIculate the rectangles for each frames
		newSprite.FrameRects[0].left   = 0;
		newSprite.FrameRects[0].top    = 0; //offset
		newSprite.FrameRects[0].right  = newSprite.FrameSize.cx; //plus the size of frame
		newSprite.FrameRects[0].bottom = newSprite.FrameSize.cy; //plus size of frame
		

	//done with the rects, on with the significant others
	
	//
	//Create the states of the sprite
	//
	if(MemAlloc((void**)&newSprite.States, sizeof(FRAME)) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! States init failed"); return NULL; }

	// I don't trust MemAlloc
	memset(newSprite.States, 0, sizeof(FRAME));

	//make sure that States is initialized correctly
	if(newSprite.States == NULL)
	{ 
		assert(!"Unable to create States, David says: 'It's not my fault, honest!'");
		MemFree((void**)&newSprite.FrameRects);
		TimerTerm(newSprite.ticker);
		return NULL; 
	}

		newSprite.States[0].firstframe = 0;
		newSprite.States[0].lastframe = 0;

	//done with the states.


	//...and last but not least, insert the sprite to the list
	if(ListAppend(thisLayer->Sprites, &newSprite) != RETCODE_SUCCESS)
	{
		//ah shit
		assert(!"Unable to append sprite to the list, damn!");
		MemFree((void**)&newSprite.FrameRects);
		MemFree((void**)&newSprite.States);
		TimerTerm(newSprite.ticker);
		return NULL;
	}

		
	return (SPRITEHANDLE)ListExtract(thisLayer->Sprites, newSprite.index);
}

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
									   short newID, LOCATION newLoc, int surfacesizeX, int surfacesizeY)
{
	SPRITEHANDLE newSprite = NULL;
	if(MemAlloc((void**)&newSprite, sizeof(SPRITE)) != RETCODE_SUCCESS)
	{ assert(!"Ah shit, GraphicsCreateMyOwnSpriteNotToBeInsertedToALayer failed to allocate newSprite"); return NULL; }
	memset(newSprite, 0, sizeof(SPRITE));

	//create a sprite with the junk given
	//insert in the list of sprite

	//insert the junk
	newSprite->ID = newID;
	newSprite->index = AntiSocialSpriteThatDoesNotBelongToAnyLayer;
	newSprite->imageID = ImageDummyID;
	newSprite->ScreenLoc = newLoc;
	newSprite->Layer = NULL;
	newSprite->MaxDelay = 0;
	newSprite->NumFrames = 1;
	newSprite->NumStates = 1;
	newSprite->numImgCol = 1;
	newSprite->imgOffset.x = 0;
	newSprite->imgOffset.y = 0;

	newSprite->ticker = TimerInit();
	
	//load the image from file
	//load the image from file
	if(surfacesizeX == 0 || surfacesizeY == 0)
		newSprite->Image = DDCreateMyOwnImage(Graphics->DDraw, path, setColorkey);
	else
	{
		newSprite->Image =  DDCreateEmptyImage(Graphics->DDraw, surfacesizeX, surfacesizeY, ImageDummyID, setColorkey);

		if(!newSprite->Image)
		{ assert(!"unable to create surface image...sigh");  return NULL; }
		
		DDReloadImage(Graphics->DDraw, newSprite->Image, path);

		if(setColorkey)
			DDSetImageColorKey(newSprite->Image);
	}

	if(!newSprite->Image)
	{ assert(!"Bad Image ID or unable to get Image...either way it's BAD!");  return NULL; }

	newSprite->FrameSize = DDGetSize(newSprite->Image);

	SETFLAG(newSprite->Status, GFLAG_ACTIVATE);

	//
	//Create the array of rectangles and set it up with the image source
	//
	if(MemAlloc((void**)&newSprite->FrameRects, sizeof(RECT)) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! Frame init failed"); return NULL; }

	//first make sure that ListInit is successful
	if(newSprite->FrameRects == NULL)
	{ assert(!"Unable to create FrameRects, oh master.  MASTER!? MASTER! NO!!!  ARG! *dead*"); return NULL; }
	
	// I don't trust MemAlloc
	memset(newSprite->FrameRects, 0, sizeof(RECT));

	//...and now we must fill in the rects

	//pre-CALIculate the rectangles for each frames
		newSprite->FrameRects[0].left   = 0;
		newSprite->FrameRects[0].top    = 0; //offset
		newSprite->FrameRects[0].right  = newSprite->FrameSize.cx; //plus the size of frame
		newSprite->FrameRects[0].bottom = newSprite->FrameSize.cy; //plus size of frame
	//done with the rects, on with the significant others
	
	//
	//Create the states of the sprite
	//
	if(MemAlloc((void**)&newSprite->States, sizeof(FRAME)) != RETCODE_SUCCESS)
	{ assert(!"Mother f*&$%ing piece of shit!  Don't make me hard code this to malloc!!! States init failed"); return NULL; }

	//make sure that States is initialized correctly
	if(newSprite->States == NULL)
	{ 
		assert(!"Unable to create States, David says: 'It's not my fault, honest!'");
		MemFree((void**)&newSprite->FrameRects);
		TimerTerm(newSprite->ticker);
		return NULL; 
	}

	// I don't trust MemAlloc
	memset(newSprite->States, 0, sizeof(FRAME));

	//fill the new states with the given states
		newSprite->States[0].firstframe = 0;
		newSprite->States[0].lastframe = 0;
	//done with the states.
		
	return newSprite;
}

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
PUBLIC void SpriteDestroy(SPRITEHANDLE Sprite)
{
	if(!Sprite) { assert(!"YOU IDIOT!  WHY ARE YOU GIVING ME A NULL SPRITE, dumbass"); return; }

	if(!Sprite->Layer)
	{
		SpriteDestroyData(Sprite);
		if(MemFree((void**)&Sprite) != RETCODE_SUCCESS)
			 assert(!"SOB!  Unable to free the friggin' sprite!");
	}
	else
	{
		SpriteDestroyData(Sprite);
		int nextIndex = Sprite->index;
		LAYERHANDLE theLayer = Sprite->Layer;
		ListDelete(theLayer->Sprites, Sprite->index);
		int maxSprite = ListGetNodeCount(theLayer->Sprites);
		SPRITEHANDLE thisSprite;

		//AAAAARRRRRRRGGGGGGGGG!!!
		for(int i = nextIndex; i < maxSprite; i++)
		{
			thisSprite = (SPRITEHANDLE)ListExtract(theLayer->Sprites, i);
			assert(thisSprite);
			thisSprite->index = i;
		}
	}
}

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
PUBLIC SPRITEHANDLE SpriteCopyToLayer(short layer, SPRITEHANDLE Sprite)
{
	assert(Sprite);
	assert(layer >= 0 && layer < Graphics->numLayers);
	if(DDIsImageDummy(Sprite->Image) || DDIsImageNotInList(Sprite->Image))
	{ assert(!"Cannot copy sprite, image duplication is not possible: Image ID is dummy or is not in list"); return NULL; }

	SPRITEHANDLE newSprite;

	//copy the states!!!
	PFRAME states;
	if(MemAlloc((void**)&states, sizeof(FRAME)*Sprite->NumStates) != RETCODE_SUCCESS)
	{
		assert(!"Unable to allocate clone states!  I guess cloning is never possible...");
		return NULL;
	}

	for(int i = 0; i < Sprite->NumStates; i++)
	{
		states[i].firstframe = Sprite->States[i].firstframe;
		states[i].lastframe = Sprite->States[i].lastframe;
	}

	newSprite = GraphicsCreateSprite(Sprite->imageID, layer, Sprite->ID,
		Sprite->ScreenLoc, Sprite->numImgCol, Sprite->imgOffset, Sprite->FrameSize,
		Sprite->MaxDelay, Sprite->NumFrames, Sprite->NumStates, states);

	if(MemFree((void**)&states) != RETCODE_SUCCESS)
		assert(!"Unable to free states, cloning flush failure...But hey, at least we got the newSprite!");

	//if the sprite that is being copied has an FX, then copy that as well
	if(FXcopy(Sprite, newSprite) != VD_OK)
		assert(!"Oh well...no FX for you, error in SpriteCopyToLayer");

	return newSprite;
}

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
PUBLIC RETCODE SpriteRemoveFromLayer(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	if(DDIsImageDummy(Sprite->Image) || DDIsImageNotInList(Sprite->Image))
	{ assert(!"Cannot copy sprite, image duplication is not possible: Image ID is dummy or is not in list"); return NULL; }

	LAYERHANDLE layer = Sprite->Layer;

	//if the layer of the sprite is null, then the sprite is not in the layer
	//assume success
	if(!layer)
		return DDRAW_OK;
	
	//otherwise, do the dirty work
	//The way this is going to work is quite messy,
	//duplicate the sprite as an individual sprite and destroy the original
	//sprite...then set the original sprite to the newly created individual sprite

	//copy the states!!!
	PFRAME states;
	if(MemAlloc((void**)&states, sizeof(FRAME)*Sprite->NumStates) != RETCODE_SUCCESS)
	{
		assert(!"Unable to allocate clone states!  I guess cloning is never possible...");
		return VD_BAD;
	}

	for(int i = 0; i < Sprite->NumStates; i++)
	{
		states[i].firstframe = Sprite->States[i].firstframe;
		states[i].lastframe = Sprite->States[i].lastframe;
	}

	SPRITEHANDLE newSprite;
	newSprite = GraphicsCreateMyOwnSprite(Sprite->imageID, Sprite->ID,
		Sprite->ScreenLoc, Sprite->numImgCol, Sprite->imgOffset, Sprite->FrameSize,
		Sprite->MaxDelay, Sprite->NumFrames, Sprite->NumStates, states);

	if(MemFree((void**)&states) != RETCODE_SUCCESS)
		assert(!"Unable to free states, cloning flush failure...But hey, at least we got the newSprite!");

	//if the sprite that is being copied has an FX, then copy that as well
	if(FXcopy(Sprite, newSprite) != VD_OK)
		assert(!"Unable to copy FX to new sprite, well that's just great!  Error in SpriteRemoveFromLayer");

	//now delete the sprite from the layer
	//this should delete it.
	SpriteDestroy(Sprite);

	Sprite = newSprite;

	return VD_OK;
}

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
PROTECTED RETCODE SpriteRestoreImage(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	assert(Sprite->Image);

	if(DDRestoreImage(Graphics->DDraw, Sprite->Image) != DDRAW_OK)
	{ assert(!"Unable to restore the sprite image!  Error in SpriteRestoreImage"); return VD_BAD; }

	return VD_OK;
}

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
PROTECTED RETCODE GraphicsRestore()
{
	return DDRestoreImageList(Graphics->DDraw);
}

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
PUBLIC RETCODE GraphicsCreateDisplayMap(short layer, SPRITEHANDLE Sprite, short numCol, short numRow, short *tiles)
{
	assert(layer >= 0 && layer < Graphics->numLayers);
	LAYERHANDLE theLayer = &Graphics->Layers[layer];

	//destroy the display map if there is one, checking is done inside this function
	GraphicsDestroyDisplayMap(layer);

	//Create the display map

	if(MemAlloc((void**)&theLayer->displayMap, sizeof(DISPLAYMAP)) != RETCODE_SUCCESS)
	{ assert(!"Bad MemAlloc!  BAD!!  Error in GraphicsCreateDisplayMap"); return VD_BAD; }
	memset(theLayer->displayMap, 0, sizeof(DISPLAYMAP));

	//add some info
	theLayer->displayMap->TileSprite = Sprite;
	theLayer->displayMap->numCol = numCol;
	theLayer->displayMap->numRow = numRow;
	SETFLAG(theLayer->displayMap->Status, GFLAG_ACTIVATE);

	//allocate the tiles
	if(MemAlloc((void**)&theLayer->displayMap->tileIndex, sizeof(short)*numRow*numCol) != RETCODE_SUCCESS)
	{ assert(!"Unable to allocate the tileIndex, bad MemAlloc!  BAD!!  Error in GraphicsCreateDisplayMap"); MemFree((void**)&theLayer->displayMap); theLayer->displayMap = NULL; return VD_BAD; }

	//copy the tiles
	memcpy(theLayer->displayMap->tileIndex, tiles, sizeof(short)*numRow*numCol);

	//viola! It's done!
	return VD_OK;
}

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
PUBLIC RETCODE GraphicsDestroyDisplayMap(short layer)
{
	assert(layer >= 0 && layer <= Graphics->numLayers);
	LAYERHANDLE theLayer = &Graphics->Layers[layer];

	if(theLayer->displayMap)
	{
		if(theLayer->displayMap->tileIndex)
			MemFree((void**)&theLayer->displayMap->tileIndex); //too lazy to put error checking
		
		MemFree((void**)&theLayer->displayMap); //too lazy to put error checking
		theLayer->displayMap = NULL;
	}

	return VD_OK;
}

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
PUBLIC void GraphicsChangeDisplayMapTiles(short layer, short *tiles)
{
	assert(layer >= 0 && layer < Graphics->numLayers);
	LAYERHANDLE theLayer = &Graphics->Layers[layer];
	//copy the tiles
	memcpy(theLayer->displayMap->tileIndex, tiles, sizeof(short)*theLayer->displayMap->numRow*theLayer->displayMap->numCol);
}

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
PUBLIC void GraphicsDeactivateDisplayMap(short layer)
{
	assert(layer >= 0 && layer < Graphics->numLayers);
	LAYERHANDLE theLayer = &Graphics->Layers[layer];

	if(theLayer->displayMap)
	{
		CLEARFLAG(theLayer->displayMap->Status, GFLAG_ACTIVATE);
		SETFLAG(theLayer->displayMap->Status, GFLAG_DEACTIVATE);
	}
}

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
PUBLIC void GraphicsActivateDisplayMap(short layer)
{
	assert(layer >= 0 && layer < Graphics->numLayers);
	LAYERHANDLE theLayer = &Graphics->Layers[layer];

	if(theLayer->displayMap)
	{
		CLEARFLAG(theLayer->displayMap->Status, GFLAG_DEACTIVATE);
		SETFLAG(theLayer->displayMap->Status, GFLAG_ACTIVATE);
	}
}

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
PUBLIC RETCODE GraphicsClearLayer(short layer)
{
	assert(Graphics);
	assert(layer >= 0 && layer < Graphics->numLayers + LayerMaxReserve);
	SPRITEHANDLE theSprite;
	int maxSprite = ListGetNodeCount(Graphics->Layers[layer].Sprites);

	while(maxSprite > 0)
	{
		theSprite = (SPRITEHANDLE)ListExtract(Graphics->Layers[layer].Sprites, 0);
		SpriteDestroy(theSprite);
		maxSprite = ListGetNodeCount(Graphics->Layers[layer].Sprites);
	}

	ListEmptyList(Graphics->Layers[layer].Sprites);
	return VD_OK;
}

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
PUBLIC RETCODE GraphicsClearAllLayer()
{
		assert(Graphics);
	SPRITEHANDLE theSprite;
	LAYERHANDLE thisLayer;
	for(int index = 0; index < Graphics->numLayers; index++)
	{
		thisLayer = &Graphics->Layers[index];

		int maxSprite = ListGetNodeCount(thisLayer->Sprites);

		while(maxSprite > 0)
		{
			theSprite = (SPRITEHANDLE)ListExtract(thisLayer->Sprites, 0);
			SpriteDestroy(theSprite);
			maxSprite = ListGetNodeCount(thisLayer->Sprites);
		}

		ListEmptyList(thisLayer->Sprites);
	}
	return VD_OK;

}

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
PUBLIC long GraphicsGetReserveLayerIndex()
{
	assert(Graphics);
	return Graphics->numLayers; //I know it sounds wierd, but that is the index of the layer reserve
}

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
PUBLIC RETCODE GraphicsMakeLayerList(short numLayer, short numSpriteReserve, short *numSprite, bool ThereCanBeOnlyOne)
{
	assert(Graphics);
	//make sure the layer is totally destroyed
	if(GraphicsDestroyLayerList() != VD_OK)
	{
		assert(!"Unable to destroy the graphics layer List");
		return VD_BAD;
	}

	//create the layers including the reserve
	MemAlloc((void**)&Graphics->Layers, (numLayer + LayerMaxReserve) * sizeof(LAYER));
	if(!Graphics->Layers)
	{ assert(!"Aw crap!  Unable to make layers"); return VD_BAD; }
	//I don't trust MemAlloc!
	memset(Graphics->Layers, 0, (numLayer + LayerMaxReserve) * sizeof(LAYER));

	Graphics->numLayers = numLayer;

	//time to create those damn bunch o' sprite for the rest of the layers! yup!
	for(int i = 0; i < numLayer; i++)
	{
		if(ThereCanBeOnlyOne)
		{
			//we will assume that all layers will have the same amount of sprite
			Graphics->Layers[i].Sprites = ListInit(*numSprite, sizeof(SPRITE));
			//assert(Graphics->Layers[layernum].Sprites);
		}
		else
		{
			Graphics->Layers[i].Sprites = ListInit(numSprite[i], sizeof(SPRITE));
			//assert(Graphics->Layers[layernum].Sprites);
		}
		
		Graphics->Layers[i].layernum = i;
		SETFLAG(Graphics->Layers[i].Status, GFLAG_ACTIVATE);

	}

	//create the reserve layer
	//MakeLayerSprites(numLayer, numSpriteReserve);
	Graphics->Layers[numLayer].Sprites = ListInit(numSpriteReserve, sizeof(SPRITE));
	//assert(Graphics->Layers[layernum].Sprites);

	return VD_OK;
}

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
PUBLIC SPRITEHANDLE GraphicsCopySprite(short spriteID, short layer, LOCATION newLoc)
{
	SPRITEHANDLE thisSprite, newSprite;

	//get the sprite from the reserve layer, Graphics->numLayers is the last layer of the
	//layer list...
	thisSprite = GraphicsGetSprite(spriteID, Graphics->numLayers);

	if(!thisSprite)
	{
		assert(!"Sprite ID does not exist, you dummy!");
		return NULL;
	}

	//copy the states!!!
	PFRAME states;
	if(MemAlloc((void**)&states, sizeof(FRAME)*thisSprite->NumStates) != RETCODE_SUCCESS)
	{
		assert(!"Unable to allocate clone states!  I guess cloning is never possible...");
		return NULL;
	}

	for(int i = 0; i < thisSprite->NumStates; i++)
	{
		states[i].firstframe = thisSprite->States[i].firstframe;
		states[i].lastframe = thisSprite->States[i].lastframe;
	}

	newSprite = GraphicsCreateSprite(thisSprite->imageID, layer, thisSprite->ID,
		newLoc, thisSprite->numImgCol, thisSprite->imgOffset, thisSprite->FrameSize,
		thisSprite->MaxDelay, thisSprite->NumFrames, thisSprite->NumStates, states);

	if(MemFree((void**)&states) != RETCODE_SUCCESS)
		assert(!"Unable to free states, cloning flush failure...But hey, at least we got the newSprite!");

	//if the sprite that is being copied has an FX, then copy that as well
	if(FXcopy(thisSprite, newSprite) != VD_OK)
		assert(!"Unable to copy FX to new sprite, damn it!  Error in GraphicsCopySprite");

	return newSprite;
}

/**********************************************************
;
;	Name:		GraphicsGetSprite
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
PUBLIC SPRITEHANDLE GraphicsGetSprite(short spriteID)
{
	assert(Graphics);

	long index;
	SPRITEHANDLE theSprite;

	//the <= means that we are including the layer reserve
	for(int i = 0; i <= Graphics->numLayers; i++)
	{
		//d'oh!
		if(ListGetNodeCount(Graphics->Layers[i].Sprites) == 0)
			continue;

		//search through all the sprites in the layer
		index = ListSearch(Graphics->Layers[i].Sprites, &SpriteIDSearch, (void *)spriteID);

		if(index < 0)
			return NULL;

		theSprite = (SPRITEHANDLE) ListExtract(Graphics->Layers[i].Sprites, index);

		//we found it!
		if(theSprite)
			return theSprite;
	}

	return NULL;
}

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
PUBLIC SPRITEHANDLE GraphicsGetSprite(short spriteID, short layer)
{
	assert(Graphics);

	//make sure the layer is within numLayers
	assert(layer < Graphics->numLayers + LayerMaxReserve);

	//d'oh!
	if(ListGetNodeCount(Graphics->Layers[layer].Sprites) == 0)
		return NULL;

	long index;

	//search through all the sprites in the layer
	index = ListSearch(Graphics->Layers[layer].Sprites, &SpriteIDSearch, (void *)spriteID);

	if(index < 0)
		return NULL;

//	PSPRITE sprites = (PSPRITE) ListGrabBuffer (Graphics->Layers [layer].Sprites);
	return (SPRITEHANDLE) ListExtract(Graphics->Layers[layer].Sprites, index);
}

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
PUBLIC  void SpriteDeactivate(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	//set the status with VSTAT_DEACTIVATE
	CLEARFLAG(Sprite->Status, GFLAG_ACTIVATE);
	SETFLAG(Sprite->Status, GFLAG_DEACTIVATE);
}

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
PUBLIC  void SpriteActivate(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	//remove the flag GFLAG_DEACTIVATE
	CLEARFLAG(Sprite->Status, GFLAG_DEACTIVATE);
	SETFLAG(Sprite->Status, GFLAG_ACTIVATE);
}

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
PUBLIC  void SpriteSetLocation(SPRITEHANDLE Sprite, long x, long y)
{
	assert(Sprite);

	Sprite->ScreenLoc.x = x;
	Sprite->ScreenLoc.y = y;
}

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
PUBLIC  PLOCATION SpriteGetLocation(SPRITEHANDLE Sprite)
{
	return &Sprite->ScreenLoc;
}

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
PUBLIC  void GraphicsDeactivateLayer(short layer)
{
	assert(Graphics);
	assert(layer >= 0 && layer < Graphics->numLayers);
	CLEARFLAG(Graphics->Layers[layer].Status, GFLAG_ACTIVATE);
	SETFLAG(Graphics->Layers[layer].Status, GFLAG_DEACTIVATE);
}

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
PUBLIC  void GraphicsActivateLayer(short layer)
{
	assert(Graphics);
	assert(layer >= 0 && layer < Graphics->numLayers);
	CLEARFLAG(Graphics->Layers[layer].Status, GFLAG_DEACTIVATE);
	SETFLAG(Graphics->Layers[layer].Status, GFLAG_ACTIVATE);
}

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
PUBLIC  void GraphicsSetLayerOffset(short layer, long x, long y)
{
	assert(Graphics);
	assert(layer >= 0 && layer < Graphics->numLayers);
	Graphics->Layers[layer].offset.x = x;
	Graphics->Layers[layer].offset.y = y;
}

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
PUBLIC void GraphicsSetLayerScroll(short layer, long x, long y)
{
	assert(Graphics);
	assert(layer >= 0 && layer < Graphics->numLayers);
	Graphics->Layers[layer].scrollratio.x = x;
	Graphics->Layers[layer].scrollratio.y = y;
}

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
PUBLIC bool GraphicsCheckLocToViewport(const LOCATION loc, long Padding, long paddingy)
{
	if(loc.x < Graphics->Viewport.left - Padding)
		return false;
	if(loc.y < Graphics->Viewport.top  - paddingy)
		return false;
	if(loc.x > Graphics->Viewport.right + Padding)
		return false;
	if(loc.y > Graphics->Viewport.bottom + paddingy)
		return false;

	return true;
}

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
PUBLIC  void GraphicsUpdateScreen(RECT *windowarea)
{
	assert(Graphics);

	//call the update screen of vidhandle
	dd_update_screen(Graphics->DDraw, windowarea);
}

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
PUBLIC  RETCODE GraphicsBltSprite(SPRITEHANDLE Sprite, PLOCATION otherLoc, bool viewPortConstraint)
{
	assert(Graphics);
	assert(Sprite);

	LOCATION layerOffset;
	if(Sprite->Layer) {layerOffset = LayerGetOffset(Sprite->Layer);}
	else {layerOffset.x = 0; layerOffset.y = 0;}
	LOCATION bltLoc;

	//Get the location of the sprite with the layer offset
	if(!otherLoc)
	{ bltLoc.x = Sprite->ScreenLoc.x + layerOffset.x;
	  bltLoc.y = Sprite->ScreenLoc.y + layerOffset.y; }
	else
	{ bltLoc.x = otherLoc->x + layerOffset.x;
	  bltLoc.y = otherLoc->y + layerOffset.y; }

	//call blit routine with the rect, loc and image
	RECT rcRect = Sprite->FrameRects[Sprite->CurFrame];

	//transform the location to viewport area, only if viewPortConstraint = true
	if(viewPortConstraint)
	{
		//if the location is off the viewport, don't blt it
		if(TransferLocToRect(Graphics->Viewport, &bltLoc) == VD_RANGEOUTOFBOUND)
			return VD_OK; //assume we succeeded

		//this should clip the image correctly within the viewport
		//but not the screen area, of course.
		//if(TransferLoc(&bltLoc, &rcRect, Graphics->Viewport) == DDRAW_OUTOFBOUND)
		//	return VD_OK;
	}
		
	if(DDBltImage(Graphics->DDraw, Sprite->Image, rcRect, bltLoc) != DDRAW_OK)
	{ assert(!"Oh crap!  GraphicsBltSprite() failed"); return VD_BAD; }

	return VD_OK;
}

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
PUBLIC RETCODE GraphicsBltSpriteLayers()
{
	assert(Graphics);

	LAYERHANDLE layerHolder;
	SPRITEHANDLE spriteHolder;
	int MaxSprite;

	for(int i = 0; i < Graphics->numLayers; i++)
	{
		layerHolder = &Graphics->Layers[i];

		//make sure the layer is not activated
		//if not, extract all sprite and blt it
		if(GETFLAGS(layerHolder->Status, GFLAG_ACTIVATE))
		{
			//Let's first display the map
			if(layerHolder->displayMap)
			{
				PDISPLAYMAP themap = layerHolder->displayMap;
				if(GETFLAGS(themap->Status, GFLAG_ACTIVATE))
					SpriteDisplayMap(themap->TileSprite, layerHolder->offset, themap->tileIndex, themap->numCol, themap->numRow);
			}

			MaxSprite = ListGetNodeCount(layerHolder->Sprites);

			for(int j = 0; j < MaxSprite; j++)
			{
				spriteHolder = (SPRITEHANDLE) ListExtract(layerHolder->Sprites, j);
				assert(spriteHolder);
				if(GETFLAGS(spriteHolder->Status, GFLAG_ACTIVATE))
				{
					if(!spriteHolder->FX)
						GraphicsBltSprite(spriteHolder);
					else
						FXupdate(spriteHolder);
				}
			}
		}
	}

	return VD_OK;
}

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
PUBLIC RETCODE GraphicsBltSpriteLayers(long layerMin, long layerMax)
{
	assert(Graphics);
	assert(layerMin >= 0 && layerMax < Graphics->numLayers && layerMin <= layerMax);
	
	SPRITEHANDLE spriteHolder;
	LAYERHANDLE layerHolder;

	int MaxSprite;

	for(int i = layerMin; i <= layerMax; i++)
	{
		layerHolder = &Graphics->Layers[i];
		
		//make sure the layer is not activated
		//if not, extract all sprite and blt it
		if(GETFLAGS(layerHolder->Status, GFLAG_ACTIVATE))
		{
			//Let's first display the map
			if(layerHolder->displayMap)
			{
				PDISPLAYMAP themap = layerHolder->displayMap;
				if(GETFLAGS(themap->Status, GFLAG_ACTIVATE))
					SpriteDisplayMap(themap->TileSprite, layerHolder->offset, themap->tileIndex, themap->numCol, themap->numRow);
			}

			MaxSprite = ListGetNodeCount(layerHolder->Sprites);

			for(int j = 0; j < MaxSprite; j++)
			{
				spriteHolder = (SPRITEHANDLE)ListExtract(layerHolder->Sprites, j);
				assert(spriteHolder);
				if(GETFLAGS(spriteHolder->Status, GFLAG_ACTIVATE))
				{
					if(!spriteHolder->FX)
						GraphicsBltSprite(spriteHolder);
					else
						FXupdate(spriteHolder);
				}
			}
		}
	}

	return VD_OK;
}

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
PROTECTED RETCODE GraphicsDisplaySpriteTiled(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	//get the starting point to blit on the screen with the given offset
	//blit until end of screen.
	LOCATION offset;
	LOCATION layerOffset;
	if(Sprite->Layer) {LayerGetOffset(Sprite->Layer);}
	else {layerOffset.x = 0; layerOffset.y = 0;}
	LOCATION bltLoc;

	//call blit routine with the rect, loc and image
	RECT rcRect = Sprite->FrameRects[Sprite->CurFrame];
	
	//this will work I tell you!
	offset.x = -(Sprite->FrameSize.cx - (abs(Sprite->ScreenLoc.x + layerOffset.x)%Sprite->FrameSize.cx));
	offset.y = -(Sprite->FrameSize.cy - (abs(Sprite->ScreenLoc.y + layerOffset.y)%Sprite->FrameSize.cy));

	//go through the whole screen and move the sprite
	SIZE scrnsize = GraphicsGetScrnSize();
	for(bltLoc.y = offset.y; bltLoc.y < scrnsize.cy; bltLoc.y += Sprite->FrameSize.cy)
	{
		for(bltLoc.x = offset.x; bltLoc.x < scrnsize.cx; bltLoc.x += Sprite->FrameSize.cx)
		{
			//blt sprite
			if(DDBltImage(Graphics->DDraw, Sprite->Image, rcRect, bltLoc) != DDRAW_OK)
			{ assert(!"Oh crap!  GraphicsBltSprite() failed"); return VD_BAD; }
		}
	}

	return VD_OK;
}

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
PUBLIC RETCODE SpriteDisplayMap(SPRITEHANDLE Sprite, LOCATION layerOffset, short *tiles, short numCol, short numRow)
{
	SIZE index, endIndex;
	LOCATION bltOffset, bltLoc;
//	LOCATION layerOffset;
//	if(Sprite->Layer) {layerOffset = LayerGetOffset(Sprite->Layer);}
//	else {layerOffset.x = 0; layerOffset.y = 0;}



	//get the location in terms of screen location without viewport
	bltOffset.x = Sprite->ScreenLoc.x + layerOffset.x;
	bltOffset.y = Sprite->ScreenLoc.y + layerOffset.y;

	//check to see if the whole map is NOT visible, if not, then assume we succeeded
	if((bltOffset.x >= Graphics->Viewport.right) 
		|| (bltOffset.x + numCol*Sprite->FrameSize.cx <= Graphics->Viewport.left)
		|| (bltOffset.y >= Graphics->Viewport.bottom)
		|| (bltOffset.y + numRow*Sprite->FrameSize.cy <= Graphics->Viewport.top))
		return VD_OK; //nothing is blited, so nothing went wrong


	//get the starting index of the map
	index.cx = (Graphics->Viewport.left - bltOffset.x)/Sprite->FrameSize.cx;
	index.cy = (Graphics->Viewport.top - bltOffset.y)/Sprite->FrameSize.cy;

	//this means that the top-most location of the map is within viewport
	if(index.cx < 0)
		index.cx = 0;
	if(index.cy < 0)
		index.cy = 0;

	//get the bottom-left index of the map
	endIndex.cx = (Graphics->Viewport.right - bltOffset.x)/Sprite->FrameSize.cx;
	endIndex.cy = (Graphics->Viewport.bottom - bltOffset.y)/Sprite->FrameSize.cy;

	//this means that the bottom-most location of the map is within viewport
	if(endIndex.cx >= numCol)
		endIndex.cx = numCol - 1;
	if(endIndex.cy >= numRow)
		endIndex.cy = numRow - 1;

	//get the location to blit on the screen
	bltLoc.x = (bltOffset.x + Sprite->FrameSize.cx*index.cx) - Graphics->Viewport.left;
	bltLoc.y = (bltOffset.y + Sprite->FrameSize.cy*index.cy) - Graphics->Viewport.top;

	//GO BLT IT BABY!!! YEAH!
	long i, j;
	LOCATION scrnLoc;//, doLoc;
	RECT rcRect;

	for(scrnLoc.y = bltLoc.y, j = index.cy; j <= endIndex.cy; scrnLoc.y += Sprite->FrameSize.cy, j++)
	{
		for(scrnLoc.x = bltLoc.x, i = index.cx; i <= endIndex.cx; scrnLoc.x += Sprite->FrameSize.cx, i++)
		{
			//call blit routine with the rect, loc and image
			//rcRect = Sprite->FrameRects[BOARDPOS(tiles, j, i, index.cx)];
			rcRect = Sprite->FrameRects[BOARDPOS(tiles, j, i, numCol)];

			//this should clip it correctly within viewport
			//doLoc = scrnLoc; //*sigh*...minus 1% performance...
			//if(TransferLoc(&doLoc, &rcRect, Graphics->Viewport) != DDRAW_OUTOFBOUND)
				if(DDBltImage(Graphics->DDraw, Sprite->Image, rcRect, scrnLoc) != DDRAW_OK)
				{ assert(!"Oh crap!  GraphicsBltSprite() failed"); return VD_BAD; }
		}
	}

	return VD_OK;
}

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
PUBLIC RETCODE SpriteSetState(SPRITEHANDLE Sprite, short state)
{
	assert(Sprite);

	if((state >= SpriteGetMaxState(Sprite)) || (state < 0))
		return VD_RANGEOUTOFBOUND;

	Sprite->CurState = state;

	if(Sprite->CurFrame < Sprite->States[state].firstframe)
		Sprite->CurFrame = Sprite->States[state].firstframe;
	return VD_OK;
}

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
PUBLIC  void SpriteUpdateAnimation(SPRITEHANDLE Sprite)
{
	assert(Sprite);

	//do the tick count
	TimerCheckTime(Sprite->ticker);

	if(TimerGetEllapsed(Sprite->ticker) > Sprite->MaxDelay)
	{
		TimerUpdateTimer(Sprite->ticker);
		Sprite->CurFrame++;

		if(Sprite->CurFrame > Sprite->States[Sprite->CurState].lastframe)
			Sprite->CurFrame = Sprite->States[Sprite->CurState].firstframe;

	}
}

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
PUBLIC RETCODE GraphicsUpdateAnimationLayers()
{
	assert(Graphics);

	LAYER layerHolder;
	SPRITEHANDLE spriteHolder;
	int MaxSprite;

	for(int i = 0; i < Graphics->numLayers; i++)
	{
		layerHolder = Graphics->Layers[i];
		MaxSprite = ListGetNodeCount(layerHolder.Sprites);

		for(int j = 0; j < MaxSprite; j++)
		{
			spriteHolder = (SPRITEHANDLE)ListExtract(layerHolder.Sprites, j);
			
			if(spriteHolder)
				if(GETFLAGS(spriteHolder->Status, GFLAG_ACTIVATE))
					SpriteUpdateAnimation(spriteHolder);
		}
	}
	return VD_OK;
}

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
PUBLIC RETCODE GraphicsUpdateAnimationLayers(long layerMin, long layerMax)
{
	assert(Graphics);
	assert(layerMin >= 0 && layerMax < Graphics->numLayers && layerMin <= layerMax);
	
	SPRITEHANDLE spriteHolder;
	LAYER layerHolder;

	int MaxSprite;

	for(int i = layerMin; i < layerMax; i++)
	{
		layerHolder = Graphics->Layers[i];
		MaxSprite = ListGetNodeCount(layerHolder.Sprites);

		for(int j = 0; j < MaxSprite; j++)
		{
			spriteHolder = (SPRITEHANDLE)ListExtract(layerHolder.Sprites, j);

			if(spriteHolder)
				if(GETFLAGS(spriteHolder->Status, GFLAG_ACTIVATE))
					SpriteUpdateAnimation(spriteHolder);
		}
	}

	return VD_OK;
}

//
// Surface Manipulation
//

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
PROTECTED IMAGEHANDLE SpriteGetImage(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	return Sprite->Image;
}

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
PUBLIC void GraphicsClearBuffer(ULONG color)
{
	DDClearBuffer(Graphics->DDraw, color);
}

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
PUBLIC  void GraphicsLock()
{
	dd_lock(Graphics->DDraw);
}

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
PUBLIC  void GraphicsUnlock()
{
	dd_unlock(Graphics->DDraw);
}

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
PUBLIC long GraphicsGetBufferPitch()
{
	return DDGetPitch(Graphics->DDraw);
}

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
PUBLIC SIZE GraphicsGetBufferSize()
{
	return DDGetSize(Graphics->DDraw);
}

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
PUBLIC long GraphicsGetNumChannel()
{
	return DDGetChannel(Graphics->DDraw);
}

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
PUBLIC BYTE * GraphicsGetLinePtr(long y)
{
	return DDGetLinePtr(Graphics->DDraw, y);
}

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
PUBLIC BYTE * GraphicsGetPixelPtr(long x, long y)
{
	return DDGetPixelPtr(Graphics->DDraw, x, y);
}


//
// FONT Management and Manipulation
//

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
PUBLIC FONTHANDLE FontCreate(char *filename, DWORD color)
{
	FONTHANDLE newFont = NULL;
	FILEHANDLE TheFile = NULL;

	if(MemAlloc((void**)&newFont, sizeof(FONT)) != RETCODE_SUCCESS)
		return newFont;
	//double check
	if(!newFont)
		return newFont;
	memset(newFont, 0, sizeof(FONT));

	//get a filehandle
	TheFile = FileOpenFile (filename, FILEREAD);
	if(!TheFile)
	{ assert(!"Font create failed!"); MemFree((void**)&newFont); return NULL; }

	//Get the bitmap file info
	BITMAPFILEHEADER bmfh;
	memset(&bmfh, 0, sizeof(bmfh));

	//I hope this works...
	if(!FileRead (TheFile, &bmfh, sizeof(bmfh)))
	{ assert(!"Font create failed!  Error in Reading bitmap file header"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }

	// Check whether it's a valid DDF file:
	//if it's not, then the bitmap might be a stupid MAC version or corrupted
        if ((bmfh.bfType != 19778)  // 'BM'
            || (bmfh.bfReserved1 != 'DD') || (bmfh.bfReserved2 != 'FF'))
		{ assert(!"Font create failed!  Bad bitmap format"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }


	// Read BITMAPINFO and bitmap bits: (I PROMISE TO FREE THEM LATER)
	if(MemAlloc((void**)&newFont->lpbi, (bmfh.bfOffBits - sizeof(bmfh))*sizeof(char)) != RETCODE_SUCCESS)
	{ assert(!"Font create failed!  Unable to allocate bitmap info"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }
    if(!FileRead(TheFile, newFont->lpbi, bmfh.bfOffBits - sizeof(bmfh)))
	{ assert(!"Font create failed!  Unable to read bitmap info"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }

	if(MemAlloc((void**)&newFont->lpBits, (bmfh.bfSize - bmfh.bfOffBits)*sizeof(char)) != RETCODE_SUCCESS)
	{ assert(!"Font create failed!  Unable to allocate bitmap bits"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }
    if(!FileRead(TheFile, newFont->lpBits, bmfh.bfSize - bmfh.bfOffBits))
	{ assert(!"Font create failed!  Unable to read bitmap bits"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }
	//

	//sanity check...or something
	assert(newFont->lpbi->bmiHeader.biSize == sizeof(BITMAPINFOHEADER));

	//get the textmetric
	if(!FileRead(TheFile, &newFont->TextMetric, sizeof(newFont->TextMetric)))
	{ assert(!"Font create failed!  Error in Text Metric"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }

	//get the ABC widths of the font
	if(!FileRead(TheFile, (LPVOID)(((DWORD)newFont->ABCWidths)+ASCIIOFFSET*sizeof(ABC)),
            ASCIILIMIT*sizeof(ABC)))
	{ assert(!"Font create failed!  Error in getting the ABC widths"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }

	//get the LogFont
	if(!FileRead(TheFile, &newFont->LogFont, sizeof(newFont->LogFont)))
	{ assert(!"Font create failed!  Error in getting LogFont"); FontDestroy(newFont); FileCloseFile(TheFile); return NULL; }

	//we don't need the friggin' file handle
	FileCloseFile(TheFile);

	//create the Image
	newFont->Image = DDCreateImage(Graphics->DDraw, ImageFontID, filename, true, 0, 0);
	if(!newFont->Image)
	{ assert(!"Font create failed!  Error in getting Image"); FontDestroy(newFont); return NULL; }

	//get the size of the frame
	SIZE imageSize = DDGetSize(newFont->Image);
	newFont->CellSize.cx = imageSize.cx >> 3; //divided by 8 for offset reason
	newFont->CellSize.cy = imageSize.cy / 28; //there are 28 rows, so...

	//zero-out the non-valid characters
	memset(newFont->SrcRects, 0, sizeof(RECT) * ASCIIOFFSET);
	memset(newFont->ABCWidths, 0, sizeof(ABC) * ASCIIOFFSET);
	memset(newFont->BPlusC, 0, sizeof(long) * ASCIIOFFSET);

	//caliculate all the src rects
	for (int i = ASCIIOFFSET; i < 256; i++)
    {
        newFont->SrcRects[i].left = ((i-ASCIIOFFSET) % 8) * newFont->CellSize.cx; //8 is the number of columns
        newFont->SrcRects[i].top = ((i-ASCIIOFFSET) >> 3) * newFont->CellSize.cy;
        newFont->SrcRects[i].right = newFont->SrcRects[i].left + newFont->ABCWidths[i].abcB;
        newFont->SrcRects[i].bottom = newFont->SrcRects[i].top + newFont->CellSize.cy;
        newFont->BPlusC[i] = newFont->ABCWidths[i].abcB + newFont->ABCWidths[i].abcC;
    }

	//last but not least, set the color
	if(FontChangeColor(newFont, color) != VD_OK)
	{ assert(!"Font create failed!  Error in changing font color"); FontDestroy(newFont); return NULL; }

	//yay!
	return newFont;
}

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
PUBLIC RETCODE FontDestroy(FONTHANDLE FontType)
{
	if(FontType)
	{
		if(FontType->lpbi)
			if(MemFree((void**)&FontType->lpbi) != RETCODE_SUCCESS)
				return VD_BAD;
		if(FontType->lpBits)
			if(MemFree((void**)&FontType->lpBits) != RETCODE_SUCCESS)
				return VD_BAD;
		if(FontType->Image)
			if(DDImageDestroy(Graphics->DDraw, FontType->Image) != DDRAW_OK)
				return VD_BAD;

		if(MemFree((void**)&FontType) != RETCODE_SUCCESS)
			return VD_BAD;
	}

	return VD_OK;
}

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
PUBLIC RETCODE FontChangeColor(FONTHANDLE FontType, DWORD color)
{
	//this should work..

	FontType->color = color;

    // Change foreground palette entry to requested text color:
    // (Assumes that entry 1 is foreground and 0 is background)
    DWORD *palentry = (DWORD*)&FontType->lpbi->bmiColors[1];
    *palentry = color;

	LOCATION loc = {0, 0};

    if(DDImageStretchDIBits(FontType->Image, loc, 
		DDGetSize(FontType->Image), FontType->lpBits, FontType->lpbi) != DDRAW_OK)
		return VD_BAD;

	return VD_OK;
}

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
PUBLIC INDEX FontDisplayText(FONTHANDLE FontType, char *text, RECT area, ALIGN alignment, bool WithinViewPort)
{
	assert(FontType);

	LOCATION bltOffset, bltLoc, bltLocEnd;
	RECT screenArea;

	//get the boundary of the blit area,
	//if WithinViewPort is true, get the viewport
	//otherwise the boundart would be the whole screen
	if(WithinViewPort)
		screenArea = GraphicsGetViewPort();
	else
	{
		SIZE screenSize = GraphicsGetScrnSize();
		screenArea.top = 0;
		screenArea.left = 0;
		screenArea.right = screenSize.cx;
		screenArea.bottom = screenSize.cy;
	}

	//get the starting location & end loc
	bltOffset.x = area.left;
	bltOffset.y = area.top;
	if(TransferLocToRect(screenArea, &bltOffset) == VD_RANGEOUTOFBOUND)
		return 0;
	
	bltLocEnd.x = bltOffset.x + (area.right - area.left);
	bltLocEnd.y = bltOffset.y + (area.bottom - area.top);

	//first check to see if the whole text area is not visible from the screen area
	//if so, return 0
	if((bltLocEnd.x < screenArea.left)
		|| (bltLocEnd.y < screenArea.top))
		return 0;

	UCHAR ch;
	int numchar = strlen(text); //get the number of chars, minus null

	//adjust the location depending on alignment type
	switch(alignment)
	{
	case TXT_LEFT:
		{
			//not much to do here
			bltLoc = bltOffset;
			
			for (int i = 0; i < numchar; i++)
			{
				ch = text[i];
				bltLoc.x += FontType->ABCWidths[ch].abcA;
				DDBltImage(Graphics->DDraw, FontType->Image, FontType->SrcRects[ch], bltLoc);
				bltLoc.x += FontType->BPlusC[ch];
				
				if(bltLoc.x > bltLocEnd.x || text[i] == ASCIIRET)
				{ 
					bltLoc.x = bltOffset.x; bltLoc.y += FontType->CellSize.cy; 
					//If we got outside the boundary, return the next index that will not be blitted
					if(bltLoc.y > bltLocEnd.y)
					{ i++; return ((numchar - i > 0) ? i : 0); }
				}
			}
			break;
		}
	case TXT_CENTER:
		assert(!"Not implemented, sorry...");
		break;
	case TXT_RIGHT:
		assert(!"Not implemented, sorry...");
		break;
	default:
		assert(!"Bad TXT alignment type, error in SpriteDisplayText");
		return VD_BAD;
	}

	return 0; //everything went ok, so just return the first index
}

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
PUBLIC long FontGetHeight(FONTHANDLE FontType)
{
	return FontType->CellSize.cy;
}

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
PUBLIC long FontGetWidth(FONTHANDLE FontType)
{
	return FontType->CellSize.cx;
}


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
PUBLIC VTYPE GraphicsGetVidType()
{
	return Graphics->videotype;
}

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
PUBLIC  SIZE GraphicsGetScrnSize()
{
	assert(Graphics);
	return Graphics->ScreenSize;
}

/**********************************************************
;
;	Name:		GraphicsMoveViewPort
;
;	Purpose:	set the viewport location.  This will also
;				update all layer offset.
;
;	Input:		the x & y increment
;
;	Output:		the viewport of graphics is set
;
;	Return:		none
;
**********************************************************/
PUBLIC void GraphicsMoveViewPort(long x, long y)
{
	assert(Graphics);

	Graphics->Viewport.left += x;
	Graphics->Viewport.top += y;
	Graphics->Viewport.right += x;
	Graphics->Viewport.bottom += y;

	if(Graphics->Layers)
	{
		for(int i = 0; i < Graphics->numLayers; i++)
		{
			Graphics->Layers[i].offset.x += Graphics->Layers[i].scrollratio.x*x;
			Graphics->Layers[i].offset.y += Graphics->Layers[i].scrollratio.y*y;
		}
	}
	
}

/**********************************************************
;
;	Name:		GraphicsSetViewPort
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
PUBLIC void GraphicsSetViewPort(long x, long y, long width, long height)
{
	assert(Graphics);

	if(width > 0 && height > 0)
	{
		Graphics->Viewport.left = x;
		Graphics->Viewport.top  = y;
		Graphics->Viewport.right = x + width;
		Graphics->Viewport.bottom = y + height;
	}
	else
	{
		SIZE vpsize;
		vpsize.cx = Graphics->Viewport.right - Graphics->Viewport.left;
		vpsize.cy = Graphics->Viewport.bottom - Graphics->Viewport.top;
		Graphics->Viewport.left = x;
		Graphics->Viewport.top  = y;
		Graphics->Viewport.right = x + vpsize.cx;
		Graphics->Viewport.bottom = y + vpsize.cy;
	}

	if(Graphics->Layers)
	{
		for(int i = 0; i < Graphics->numLayers; i++)
		{
			Graphics->Layers[i].offset.x += Graphics->Layers[i].scrollratio.x*x;
			Graphics->Layers[i].offset.y += Graphics->Layers[i].scrollratio.y*y;
		}
	}
}

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
PUBLIC RECT GraphicsGetViewPort()
{
	assert(Graphics);
	return Graphics->Viewport;
}

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
PROTECTED LISTHANDLE GraphicsGetResolutionList()
{
	return DDGetListModes(Graphics->DDraw);
}

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

PUBLIC SIZE GraphicsGetMapSize (INDEX layer)
{
	SIZE size, sSize;

	PDISPLAYMAP map = Graphics->Layers [layer].displayMap;

	sSize	= SpriteGetSize (map->TileSprite);

	size.cx = map->numCol * sSize.cx;
	size.cy = map->numRow * sSize.cy;

	return size;
}

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
PUBLIC  short SpriteGetMaxState(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	return Sprite->NumStates;
}

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
PUBLIC short SpriteGetCurrentState(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	return Sprite->CurState;
}

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
PUBLIC SIZE SpriteGetSize(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	return Sprite->FrameSize;
}

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
PRIVATE  LOCATION LayerGetOffset(LAYERHANDLE thisLayer)
{
	assert(thisLayer);
	return thisLayer->offset;
}

/************************************************************************************************

Private function stuff

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
PRIVATE RETCODE GraphicsDestroyLayerList()
{
	if(Graphics->Layers)
	{
		LAYER HolderLayer;
		SPRITEHANDLE HolderSprite;
		int MaxSprite;

		for(int i = 0; i <= Graphics->numLayers; i++)
		{
			HolderLayer = Graphics->Layers[i];

			//destroy the display map
			GraphicsDestroyDisplayMap(i);

			if(HolderLayer.Sprites)
			{
				//destroy all available sprites
				MaxSprite = ListGetNodeCount(HolderLayer.Sprites);
				for(int j = 0; j < MaxSprite; j++)
				{
					HolderSprite = (SPRITEHANDLE)ListExtract(HolderLayer.Sprites, j);
					
					SpriteDestroyData(HolderSprite);
				}
				
				if(ListDestroy(&HolderLayer.Sprites) != RETCODE_SUCCESS)
					assert(!"ListDestroy Sprites failed in GraphicsDestroyLayerList");
			}
		}

		if(MemFree((void**)&Graphics->Layers) != RETCODE_SUCCESS)
			assert(!"MemFree Layers failed in GraphicsDestroyLayerList");

		Graphics->Layers = NULL;
		Graphics->numLayers = 0;
	}

	return VD_OK;
}

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
;	Return:		none
;
**********************************************************/
PRIVATE void SpriteDestroyData(SPRITEHANDLE Sprite)
{
	if(Sprite)
	{
		//destroy the image if it is not in the image list
		//destroy it as well if the image ID is a dummy
		if(DDIsImageNotInList(Sprite->Image) || DDIsImageDummy(Sprite->Image))
			DDImageDestroy(Graphics->DDraw, Sprite->Image);

		//destroy these crap
		if(FXdestroy(Sprite->FX) != VD_OK)
			assert(!"FXdestroy failed in GraphicsDestroyLayerList");
		if(MemFree((void**)&Sprite->FrameRects) != RETCODE_SUCCESS)
			assert(!"MemFree FrameRects failed in GraphicsDestroyLayerList");
		if(MemFree((void**)&Sprite->States) != RETCODE_SUCCESS)
			assert(!"MemFree States failed in GraphicsDestroyLayerList");
		
		if(!TimerTerm(Sprite->ticker))
			assert(!"Unable to kill time, damn!");
	}
}

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
PRIVATE  RETCODE ClipRect(const RECT area, RECT *rect)
{
	if(rect->left < area.left)
		rect->left = area.left;

	if(rect->top < area.top)
		rect->top = area.top;

	//If the rectangle is a little bit out of boundary...
	if(rect->right > area.right)
		rect->right = area.right;

	if(rect->bottom > area.bottom)
		rect->bottom = area.bottom;
	
	//Since we fooled around with the rectange, check if it is invalid, if so, return BAD
	if(rect->left >= rect->right)
		return VD_BAD;
	else if(rect->top >= rect->bottom)
		return VD_BAD;

	return VD_OK;
}

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
PRIVATE  RETCODE TransferLocToRect(const RECT area, LOCATION *loc)
{
	loc->x -= area.left;
	loc->y -= area.top;

	//You don't have to terminate the program because of this, it's just so
	//that you know the location is out of the area
	if((loc->x > area.right)
		|| (loc->y > area.bottom))
		return VD_RANGEOUTOFBOUND;

	return VD_OK;
}

/************************************************************************************************

LIST function callbacks

************************************************************************************************/

// Sprite ID search function
RETCODE SpriteIDSearch(void *thing1, void *thing2)
{
	SPRITEHANDLE sprite = (SPRITEHANDLE)thing1;

	return (sprite->ID == (short)thing2);
}


#include "gdi.h"
#include "i_gdi.h"

PRIVATE EFFECT EffectTable[] = {flyingthings};    //(EFFECT **) ; Pointer to effect interface
PRIVATE FXsignals[] = {FXsignal_animate, FXsignal_noanimate};

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
PUBLIC RETCODE SpriteQueryFX(SPRITEHANDLE Sprite, long type, long *initarray)
{

	assert(Sprite);

	if(Sprite->FX) { FXdestroy(Sprite->FX); Sprite->FX = NULL; }

	FXHANDLE *newFX = &Sprite->FX;

	if(MemAlloc((void**)newFX, sizeof(FX)) != RETCODE_SUCCESS)
	{ assert(!"FX allocation failure!"); return VD_BAD; }

	//double check
	if(!(*newFX)) { assert(!"FX creation failure, MemAlloc totally blew it!"); return VD_BAD; }
	memset(*newFX, 0, sizeof(FX));

	(*newFX)->ticker = TimerInit();
	if(!(*newFX)->ticker) { assert(!"FX creation failure, timer not created"); MemFree((void**)newFX); Sprite->FX = NULL; return VD_BAD; }

	(*newFX)->Type = type;
	(*newFX)->Effect = EffectTable[type];
	SETFLAG((*newFX)->Status, FXF_ACTIVATE);

	//copy the initialization param
	memcpy((*newFX)->initParam, initarray, sizeof(long)*MAXFXPARAM);

	//call the function with the message CREATE
	if((*newFX)->Effect(Sprite, initarray, FXM_CREATE) != VD_OK)
	{ assert(!"FX creation failure, FXM_CREATE failed"); TimerTerm((*newFX)->ticker); MemFree((void**)newFX); Sprite->FX = NULL; return VD_BAD; }
	
	//create a new handle and insert it to the FX list of graphicsObj
	//make the function pointer point to the appropriate function
	//and there you go.
	return VD_OK;
}

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
PUBLIC FXHANDLE SpriteGetFX(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	return Sprite->FX;
}

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
PROTECTED RETCODE FXupdate(SPRITEHANDLE Sprite)
{
	assert(Sprite);
	FXHANDLE theFX = SpriteGetFX(Sprite);

		//do the tick count
		TimerCheckTime(theFX->ticker);
		
		//animate the FX if we reach Delay...also decrement the duration
		if(TimerGetEllapsed(theFX->ticker) > theFX->MaxDelay)
		{
			TimerUpdateTimer(theFX->ticker);
			
			if(theFX->Effect(Sprite, &FXsignals[FXsignal_animate], FXM_DOSTUFF) != VD_OK)
			{ assert(!"Error in FX update"); return VD_BAD; }

			if(theFX->Duration != INFINITEDURATION)
			{
				//destroy the FX if the duration reaches it's limit
				if((theFX->Duration--) == 0)
					if(FXdestroy(theFX) != VD_OK)
					{ assert(!"Piece of shit FXdestroy failed, error in FXupdate"); return VD_BAD; }
			}

		}
		else //just display the FX with out animation
		{
			if(theFX->Effect(Sprite, &FXsignals[FXsignal_noanimate], FXM_DOSTUFF) != VD_OK)
				{ assert(!"Error in FX update"); return VD_BAD; }
		}

	
	return VD_OK;
}

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
PROTECTED RETCODE FXcopy(SPRITEHANDLE SpriteSrc, SPRITEHANDLE SpriteDest)
{
	assert(SpriteSrc);
	assert(SpriteDest);

	if(SpriteSrc->FX)
	{
		if(SpriteDest->FX) //delete the existing FX of the destination sprite
			FXdestroy(SpriteDest->FX);

		//now, copy the FX of the old one to the new one...
		if(SpriteQueryFX(SpriteDest, SpriteSrc->FX->Type, SpriteSrc->FX->initParam) != VD_OK)
		{
			assert(!"Unable to copy sprite FX, error in FXcopy!  G^%$ F$#ing S%$#^$%ch!!");
			return VD_BAD;
		}
	}

	//everything a-ok!
	return VD_OK;
}

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
PUBLIC void FXdeactivate(FXHANDLE FX)
{
	assert(FX);
	CLEARFLAG(FX->Status, FXF_ACTIVATE); 
	SETFLAG(FX->Status, FXF_DEACTIVATE);
}

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
PUBLIC void FXactivate(FXHANDLE FX)
{
	assert(FX);
	CLEARFLAG(FX->Status, FXF_DEACTIVATE); 
	SETFLAG(FX->Status, FXF_ACTIVATE);
}

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
;	Return:		none
;
**********************************************************/
PUBLIC void FXreset(FXHANDLE FX)
{
	assert(FX);
	FX->Duration = FX->MaxDuration;
}

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
PUBLIC RETCODE FXdestroy(FXHANDLE FX)
{
	if(FX)
	{
		//destroy the data of the FX, depending on what kind
		if(FX->Effect(FX, NULL, FXM_DESTROY) != VD_OK)
			assert(!"Unable to destroy FX data");

		if(FX->ticker)
			TimerTerm(FX->ticker);

		if(MemFree((void**)&FX) != RETCODE_SUCCESS)
			assert(!"Unable to free FX");
		FX = NULL;
	}
	return VD_OK;
}


/**********************************************************
***********************************************************

  The methods for all FX
  Don't you dare call these functions.

***********************************************************
**********************************************************/

//blits the sprite to the backbuffer inverted.
PROTECTED long flyingthings(void *dumbParam, void *otherParam, long message)
{
	switch(message)
	{
	case FXM_DOSTUFF:
		{
			SPRITEHANDLE thisSprite = (SPRITEHANDLE)dumbParam;
			FXHANDLE thisFX = SpriteGetFX(thisSprite);
			Pflything thisdata = (Pflything)(thisFX->Data);
			long signal = *((long*)otherParam);
			
			if(signal == FXsignal_animate)
			{

				RECT screenarea = GraphicsGetViewPort();
				PLOCATION bltLoc;
				for(short i = 0; i < thisdata->numObj; i++)
				{
					bltLoc = &thisdata->objLocs[i];
					
					bltLoc->x += thisdata->objSpd[i].x;
					bltLoc->y += thisdata->objSpd[i].y;
					
					if(bltLoc->x > screenarea.right)
					{ bltLoc->x = 1 - thisSprite->FrameSize.cx; bltLoc->y = Random(screenarea.top, screenarea.bottom - thisSprite->FrameSize.cy); }
					else if(bltLoc->x + thisSprite->FrameSize.cx < screenarea.left)
					{ bltLoc->x = screenarea.right - 1; bltLoc->y = Random(screenarea.top, screenarea.bottom - thisSprite->FrameSize.cy); }
					else if(bltLoc->y > screenarea.bottom)
					{ bltLoc->y = 1 - thisSprite->FrameSize.cy; bltLoc->x = Random(screenarea.left, screenarea.right - thisSprite->FrameSize.cx); }
					else if(bltLoc->y + thisSprite->FrameSize.cy < screenarea.top)
					{ bltLoc->y = screenarea.bottom - 1; bltLoc->x = Random(screenarea.left, screenarea.right - thisSprite->FrameSize.cx); }
					
					GraphicsBltSprite(thisSprite, bltLoc);
				}
			}
			else
			{
				for(short i = 0; i < thisdata->numObj; i++)
					GraphicsBltSprite(thisSprite, &thisdata->objLocs[i]);
			}
			
			return VD_OK;
		}
	case FXM_CREATE:
		{
			SPRITEHANDLE thisSprite = (SPRITEHANDLE)dumbParam;
			FXHANDLE thisFX = SpriteGetFX(thisSprite);
			long *initarray = (long *)otherParam;
			
			thisFX->Duration = thisFX->MaxDuration = initarray[FlyParamMaxDuration];
			thisFX->MaxDelay = initarray[FlyParamMaxDelay];

			Pflything thisdata;
			if(MemAlloc((void**)&thisdata, sizeof(flything)) != RETCODE_SUCCESS)
			{
				assert(!"Unable to allocate FX data, damn!");
				return VD_BAD;
			}
			memset(thisdata, 0, sizeof(flything));
			thisFX->Data = (void *)thisdata;
			
			//Pflything thisdata = (Pflything)thisFX->Data;
			
			thisdata->numObj = (short)initarray[FlyParamNumObjs];
			thisdata->Xdirection = initarray[FlyParamXDirection];
			thisdata->Ydirection = initarray[FlyParamYDirection];
			
			
			if(MemAlloc((void**)&thisdata->objLocs, thisdata->numObj * sizeof(LOCATION)) != RETCODE_SUCCESS)
			{ assert(!"Piece of shit MemAlloc, I'm better off doing it by myself! FX flying objs create error."); return VD_BAD; }
			//I don't trust MemAlloc
			memset(thisdata->objLocs, 0, thisdata->numObj * sizeof(LOCATION));
			if(MemAlloc((void**)&thisdata->objSpd, thisdata->numObj * sizeof(LOCATION)) != RETCODE_SUCCESS)
			{ assert(!"Piece of shit MemAlloc, I'm better off doing it by myself! FX flying objs create error."); return VD_BAD; }
			//I don't trust MemAlloc
			memset(thisdata->objSpd, 0, thisdata->numObj * sizeof(LOCATION));
			
			RECT screenarea = GraphicsGetViewPort();

			for(int i = 0; i < thisdata->numObj; i++)
			{
				thisdata->objLocs[i].x = Random(screenarea.left, screenarea.right - thisSprite->FrameSize.cx);
				thisdata->objLocs[i].y = Random(screenarea.top, screenarea.bottom - thisSprite->FrameSize.cx);
			}
			
			for(int j = 0; j < thisdata->numObj; j++)
			{
				thisdata->objSpd[j].x = thisdata->Xdirection*(rand() % MAXFLYINGOBJECTSPEED);
				thisdata->objSpd[j].y = thisdata->Ydirection*(rand() % MAXFLYINGOBJECTSPEED);
			}
			
			return VD_OK;
		}
	case FXM_DESTROY:
		FXHANDLE thisFX = (FXHANDLE)dumbParam;
		Pflything thisdata = (Pflything)(thisFX->Data);
		
		if(thisdata)
		{
			if(thisdata->objLocs)
				if(MemFree((void**)&thisdata->objLocs) != RETCODE_SUCCESS)
					assert(!"Unable to destroy objLocs of FX flyingobjects");
			if(thisdata->objSpd)
				if(MemFree((void**)&thisdata->objSpd) != RETCODE_SUCCESS)
					assert(!"Unable to destroy objSpd of FX flyingobjects");
					
			if(MemFree((void**)&thisdata) != RETCODE_SUCCESS)
				assert(!"Unable to destroy the data of FX flyingobjects");
		}
		
		return VD_OK;
	}
	
	return VD_OK;
}


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
PRIVATE long Random(long a1, long a2)
{
	long tempnum = a2 - a1 + 1;
	long tempnum2;

	tempnum2 = a2 - (rand()%tempnum);

	return tempnum2;
}
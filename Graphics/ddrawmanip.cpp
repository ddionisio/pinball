//#include "ddraw.h"

// You must include this define to use QueryInterface
//#define INITGUID

#include <assert.h>
#include "ddrawstuff.h"
#include "i_ddrawstuff.h"
#include "csbitmap.h"
//#include "mm.h" ???

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
PUBLIC DDRAWHANDLE ddraw_create()
{
	DDRAWHANDLE newddrawstuff;

	//
	//	USE RUNTIME!!!   ALRIGHT! ALRIGHT!
	//
	MemAlloc((void**)(&newddrawstuff), sizeof(ddrawdude));

	//assert(newddrawstuff);

	//set the whole handle stuff to zero
	memset(newddrawstuff, 0, sizeof(ddrawdude));

	return newddrawstuff;
}

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
PUBLIC RETCODE ddraw_destroy(DDRAWHANDLE ddrawstuff)
{
	int retCode = DDRAW_OK;

	if(ddrawstuff)
	{
		if(ddrawstuff->guidlist)
			if(ListDestroy(&ddrawstuff->guidlist) != RETCODE_SUCCESS)
			{ MESSAGE_BOX("Unable to destroy guidlist, damn it!", "Error in ddraw_destroy"); retCode = DDRAW_BAD; }
#if MODEACTIVATE
		if(ddrawstuff->modes)
			if(ListDestroy(&ddrawstuff->modes) != RETCODE_SUCCESS)
			{ MESSAGE_BOX("Unable to destroy modes, damn it!", "Error in ddraw_destroy"); retCode = DDRAW_BAD; }

		/*PMODES holdermodes;
		while(ddrawstuff->modes != NULL)
		{
			holdermodes = ddrawstuff->modes->next;
			free(ddrawstuff->modes);
			ddrawstuff->modes = holdermodes;
		}*/

#endif
		if(ddrawstuff->Images)
		{
			DDClearImageList(ddrawstuff);
			if(ListDestroy(&ddrawstuff->Images) != RETCODE_SUCCESS)
			{ MESSAGE_BOX("Unable to destroy images, damn it!", "Error in ddraw_destroy"); retCode = DDRAW_BAD; }
		}
		
		if(destroy_the_friggin_surfaces(ddrawstuff) != DDRAW_OK)
		{ MESSAGE_BOX("destroy_the_friggin_surfaces failed, damn it!", "Error in ddraw_destroy"); retCode = DDRAW_BAD; }
				
		if(destroy_surface_clipper(ddrawstuff) != DDRAW_OK)
		{ MESSAGE_BOX("destroy_surface_clipper failed, damn it!", "Error in ddraw_destroy"); retCode = DDRAW_BAD; }
		
		destroy_ddrawobj(ddrawstuff);
		
		if(MemFree((void**)(&ddrawstuff)) != RETCODE_SUCCESS)
		{ MESSAGE_BOX("Unable to destroy ddrawstuff, damn it!", "Error in ddraw_destroy"); retCode = DDRAW_BAD; }
		
		//memset(ddrawstuff, 0, sizeof(ddrawdude));
	}

	return retCode;
}

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
PRIVATE  DDSURFACEDESC get_user_made_surfacedesc(DDRAWHANDLE ddrawstuff)
{
	assert(ddrawstuff);

	return ddrawstuff->usersurfacedesc;
}

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
PROTECTED  DDSURFACEDESC get_surfacedesc(DDRAWHANDLE ddrawstuff)
{
	if(!we_have_ddrawobj(ddrawstuff))
		assert(!"We don't have ddraw obj");

	DDSURFACEDESC ddsd;

	memset(&ddsd, 0, sizeof(DDSURFACEDESC));

	if(ddrawstuff->lpDDSurfacePrimary)
		IDirectDrawSurface_GetSurfaceDesc(ddrawstuff->lpDDSurfacePrimary,&ddsd);

	return ddsd;

}

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
PUBLIC  SIZE get_screensize(DDRAWHANDLE ddrawstuff)
{
	assert(ddrawstuff);
	return ddrawstuff->backbuffsize;
}

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
PRIVATE RETCODE ddraw_create_obj(DDRAWHANDLE ddrawstuff, GUID *guid)
{
	destroy_ddrawobj(ddrawstuff);

	//use the primary driver and don't display the dialog
	if(dd_check_error(TRUE, DirectDrawCreate(guid, &ddrawstuff->ddrawobj, NULL)))
	{ assert(!"Can't create direct draw object! Error in ddraw_init!"); return DDRAW_BAD; }
	
	//Do the QueryInterface
	if(dd_check_error(TRUE, IDirectDraw_QueryInterface(ddrawstuff->ddrawobj, IID_IDirectDraw2, (LPVOID *)&ddrawstuff->ddrawobj2)))
	{ assert(!"Can't QueryInterface!  Error in ddraw_init!"); IDirectDraw_Release(ddrawstuff->ddrawobj); ddrawstuff->ddrawobj = NULL; return DDRAW_BAD; }
	else
		//QUERY WORKED!  That means we don't need this worthless object
	{ IDirectDraw_Release(ddrawstuff->ddrawobj); ddrawstuff->ddrawobj = NULL; } 

	return DDRAW_OK;
}

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
PROTECTED RETCODE ddraw_init(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, SOMEINSTANCE hinst, DWORD cooperative_flag)
{
	assert(ddrawstuff);

	//Initialize the GUID list with an arbitrary number
	if((ddrawstuff->guidlist = ListInit(GUIDMAX, sizeof(GUIDS))) == NULL)
	{ assert(!"BAD ListInit! BAD!! Error in ddraw_init"); return DDRAW_BAD; }

	//Get the list of video cards...Thanks to stupid ppl. who have 2 or more video cards
	//in one PC
	if(dd_check_error(TRUE, DirectDrawEnumerate(enumcallback, ddrawstuff)))
	{ assert(!"Direct Draw Enumeration failed! Error in ddraw_init"); return DDRAW_BAD; }

	//piece of shit!!!
	//Check to see if there are more than one video card
	if(ListGetNodeCount(ddrawstuff->guidlist) > 1)
		//that means there are two or more video card
		DialogBoxParam(hinst, MAKEINTRESOURCE(ID_VIDOP),hwnd,vidopt, (LPARAM)ddrawstuff);
	else
	{
		//otherwise we will just use the primary video card...since there is only one
		if(ddraw_create_obj(ddrawstuff, NULL) == DDRAW_BAD)
		{ assert(!"BAD DDRAWOBJ! BAD!  Error in ddraw_init!"); return DDRAW_BAD; }
	}
	
	//cooperation
	if(set_cooperation(ddrawstuff, hwnd, cooperative_flag) == DDRAW_BAD)
	{ assert(!"I will not cooperate with you fools!  Error in ddraw_init!"); destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }

	
	//Get the modes and also current mode
	INIT_DDSD(ddrawstuff->currentmode);
	
	if(dd_check_error(TRUE, IDirectDraw2_GetDisplayMode(ddrawstuff->ddrawobj2, &ddrawstuff->currentmode)))
		assert(!"Cannot Get Mode!!!!  Error in ddraw_init!");


#if MODEACTIVATE	
	//Initialize the GUID list with an arbitrary number
	if((ddrawstuff->modes = ListInit(MODEMAX, sizeof(DDSURFACEDESC))) == NULL)
	{ assert(!"BAD ListInit! BAD!! Error in ddraw_init"); return DDRAW_BAD; }

	if(dd_check_error(TRUE, IDirectDraw2_EnumDisplayModes(ddrawstuff->ddrawobj2, DDEDM_REFRESHRATES, NULL, ddrawstuff, ( LPDDENUMMODESCALLBACK )enumdisplaycallback)))
		MessageBox(hwnd, "Enumeration Malfunction", "do'h", MB_OK);
#endif

	
	//get the capability of the video card
	memset(&ddrawstuff->vidcap, sizeof(DDCAPS), 0);
	memset(&ddrawstuff->hel, sizeof(DDCAPS), 0);
	
	ddrawstuff->vidcap.dwSize = sizeof(DDCAPS);
	ddrawstuff->hel.dwSize = sizeof(DDCAPS);

	if(dd_check_error(TRUE, IDirectDraw2_GetCaps(ddrawstuff->ddrawobj2, &ddrawstuff->vidcap, &ddrawstuff->hel)))
		assert(!"Ddraw caps spy failed!  Error in ddraw_init!");

	//this is done so that we won't have to call this function to initialize all the junk again
	//junk by means of guid and mode list.
	SETFLAG(ddrawstuff->status, DDFLAG_INIT_CALLED);

	return DDRAW_OK;
}

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
PROTECTED RETCODE ddraw_remake(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, DWORD cooperative_flag)
{
	//use the guid the user selected from the start-up, the video card
	if(ddraw_create_obj(ddrawstuff, ddrawstuff->current_adapter) != DDRAW_OK)
	{ assert(!"BAD DDRAWOBJ! BAD!  Error in ddraw_remake!"); return DDRAW_BAD; }
	
	//cooperation
	if(set_cooperation(ddrawstuff, hwnd, cooperative_flag) != DDRAW_OK)
	{ assert(!"I will not cooperate with you fools!  Error in ddraw_remake!"); destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }

	return DDRAW_OK;
}

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
PROTECTED RETCODE set_cooperation(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, DWORD cooperative_flag)
{
	if(we_have_ddrawobj(ddrawstuff))
		if(dd_check_error(TRUE, IDirectDraw2_SetCooperativeLevel(ddrawstuff->ddrawobj2, hwnd, cooperative_flag)))
			return DDRAW_BAD;

	return DDRAW_OK;
}

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
PRIVATE void destroy_ddrawobj(DDRAWHANDLE ddrawstuff)
{
	if(ddrawstuff)
	{
		if(ddrawstuff->ddrawobj)
		{ IDirectDraw_Release(ddrawstuff->ddrawobj); ddrawstuff->ddrawobj = NULL; }
		if(ddrawstuff->ddrawobj2)
		{ IDirectDraw_Release(ddrawstuff->ddrawobj2); ddrawstuff->ddrawobj2 = NULL; }
	}
}

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
PUBLIC RETCODE we_have_ddrawobj(DDRAWHANDLE ddrawstuff)
{
	if(ddrawstuff)
		//Only check for the latest...screw ddrawobj1!
		if(ddrawstuff->ddrawobj2)
			return DDRAW_OK;
	
	return DDRAW_BAD;
}

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
PUBLIC RETCODE create_ddraw_fullscreen(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, SOMEINSTANCE hinst, Pdisplaymode themode)
{
	assert(ddrawstuff);

	if(GETFLAGS(ddrawstuff->status,DDFLAG_INIT_CALLED))
	{
		//just remake the ddrawobject
		if(ddraw_remake(ddrawstuff, hwnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE) != DDRAW_OK)
			return DDRAW_BAD;
	}
	else
	{
		//make the ddrawobject and all the other junk
		if(ddraw_init(ddrawstuff, hwnd, hinst, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE) != DDRAW_OK)
			return DDRAW_BAD;
	}

	//once all that gobble dee gook is done, set the mode baby!
	if(themode)
	{
		if(set_our_display_mode(ddrawstuff, themode->width, themode->height, themode->colorbits, themode->refreshrate,false) != DDRAW_OK)
		{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }
	}
	else //we will just assume that the user wanted 640x480x8x0
	{
		if(set_our_display_mode(ddrawstuff, DEFAULTWIDTH, DEFAULTHEIGHT, DEFAULTCBIT, DEFAULTRR,false) != DDRAW_OK)
		{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }
	}

	DDSURFACEDESC ddsd;

	INIT_DDSD(ddsd);

	//make the description of our surface
	ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	ddsd.dwBackBufferCount = 1;

	if(make_surface_primary(ddrawstuff, ddsd) != DDRAW_OK)
	{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }

	//set the update screen!!!
	dd_update = &dd_update_fullscreen;

	return DDRAW_OK;
}

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
PUBLIC RETCODE create_ddraw_windowed(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, SOMEINSTANCE hinst, long sizeX, long sizeY)
{
	assert(ddrawstuff);

	if(GETFLAGS(ddrawstuff->status,DDFLAG_INIT_CALLED))
	{
		//just remake the ddrawobject
		if(ddraw_remake(ddrawstuff, hwnd, DDSCL_NORMAL) != DDRAW_OK)
			return DDRAW_BAD;
	}
	else
	{
		//make the ddrawobject and all the other junk
		if(ddraw_init(ddrawstuff, hwnd, hinst, DDSCL_NORMAL) != DDRAW_OK)
			return DDRAW_BAD;
	}

	DDSURFACEDESC ddsd;
	INIT_DDSD(ddsd);

	//make the description of our surface
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	
	//
	//  If the user gave us bogus size X,Y (or you could say default)
	//
	if((sizeX == 0) || (sizeY == 0))
	{
		RECT this_dumb_rect;
		GetClientRect(hwnd, &this_dumb_rect);

		//set to the size of the client area, I hope the window is not resizeable :(
		sizeX = this_dumb_rect.right - this_dumb_rect.left;
		sizeY = this_dumb_rect.bottom - this_dumb_rect.top;
	}

	//make the surface primary with a backbuffer as offscreen
	if(make_surface_primary(ddrawstuff, ddsd, true, sizeX, sizeY) != DDRAW_OK)
	{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }

	//create a clipper!  YEP!
	if(make_surface_clipper(ddrawstuff, hwnd) != DDRAW_OK)
	{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }

	//set the update screen!!!
	dd_update = &dd_update_windowed;

	return DDRAW_OK;
}

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
PUBLIC RETCODE create_ddraw_fullscrnclipper(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd, SOMEINSTANCE hinst, Pdisplaymode themode)
{
	assert(ddrawstuff);

	if(GETFLAGS(ddrawstuff->status,DDFLAG_INIT_CALLED))
	{
		//just remake the ddrawobject
		if(ddraw_remake(ddrawstuff, hwnd, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE) != DDRAW_OK)
			return DDRAW_BAD;
	}
	else
	{
		//make the ddrawobject and all the other junk
		if(ddraw_init(ddrawstuff, hwnd, hinst, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE) != DDRAW_OK)
			return DDRAW_BAD;
	}

	DDSURFACEDESC ddsd;
	INIT_DDSD(ddsd);

	//make the description of our surface
	ddsd.dwSize = sizeof(DDSURFACEDESC);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	
	//once all that gobble dee gook is done, set the mode baby!
	if(themode)
	{
		if(set_our_display_mode(ddrawstuff, themode->width, themode->height, themode->colorbits, themode->refreshrate,false) != DDRAW_OK)
		{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }

		//set up the size of backbuffer and resize the window
		MoveWindow(hwnd, 0, 0, themode->width, themode->height, FALSE);
	}
	else //we will just assume that the user wanted 640x480x8x0
	{
		if(set_our_display_mode(ddrawstuff, DEFAULTWIDTH, DEFAULTHEIGHT, DEFAULTCBIT, DEFAULTRR,false) != DDRAW_OK)
		{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }
		
		//set up the size of backbuffer and resize the window
		MoveWindow(hwnd, 0, 0, DEFAULTWIDTH, DEFAULTHEIGHT, FALSE);
	}

	//make the surface primary with a backbuffer as offscreen
	if(make_surface_primary(ddrawstuff, ddsd, true) != DDRAW_OK)
	{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }

	//create a clipper!  YEP!
	if(make_surface_clipper(ddrawstuff, hwnd) != DDRAW_OK)
	{ destroy_ddrawobj(ddrawstuff); return DDRAW_BAD; }

	//set the update screen!!!
	dd_update = &dd_update_windowed;

	return DDRAW_OK;
}

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
PUBLIC RETCODE set_our_display_mode(DDRAWHANDLE ddrawstuff, ULONG width, ULONG height, ULONG colorbits, ULONG refreshrate, bool willrecreatesurface)
{
	if(we_have_ddrawobj(ddrawstuff))
	{
		if(dd_check_error(TRUE, IDirectDraw2_SetDisplayMode(ddrawstuff->ddrawobj2, width, height, colorbits, refreshrate, 0)))
		{ assert(!"error in set_our_display_mode"); return DDRAW_BAD; }
		
		//set the current mode with the new one
		INIT_DDSD(ddrawstuff->currentmode);

		if(dd_check_error(TRUE, IDirectDraw2_GetDisplayMode(ddrawstuff->ddrawobj2, &ddrawstuff->currentmode)))
		{ assert(!"error in set_our_display_mode"); return DDRAW_BAD; }
		//
		//
		//
		
		//we must recreate the surface!  INDEED!!!
		if(willrecreatesurface)
		{
			bool backbuffisoffscrn = false;

			if(GETFLAGS(ddrawstuff->status,DDFLAG_BACKBUFFERISOFFSCRN))
				backbuffisoffscrn = true;

			if(make_surface_primary(ddrawstuff, ddrawstuff->usersurfacedesc, backbuffisoffscrn) != DDRAW_OK)
				return DDRAW_BAD;
		}
	}

	return DDRAW_OK;
}

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
PUBLIC RETCODE restore_display_mode(DDRAWHANDLE ddrawstuff, bool willrecreatesurface)
{
	if(we_have_ddrawobj(ddrawstuff))
	{
		if(dd_check_error(TRUE, IDirectDraw2_RestoreDisplayMode(ddrawstuff->ddrawobj2)))
		{ assert(!"restore_display_mode 1"); return DDRAW_BAD; }
		
		INIT_DDSD(ddrawstuff->currentmode);

		if(dd_check_error(TRUE, IDirectDraw2_GetDisplayMode(ddrawstuff->ddrawobj2, &ddrawstuff->currentmode)))
		{ assert(!"restore_display_mode 2"); return DDRAW_BAD; }
		

		//we must recreate the surface!  INDEED!!!
		if(willrecreatesurface)
		{
			bool backbuffisoffscrn = false;

			if(GETFLAGS(ddrawstuff->status,DDFLAG_BACKBUFFERISOFFSCRN))
				backbuffisoffscrn = true;

			if(make_surface_primary(ddrawstuff, ddrawstuff->usersurfacedesc, backbuffisoffscrn) != DDRAW_OK)
				return DDRAW_BAD;
		}
	}

	return DDRAW_OK;
}

/***********************************************************************************************/
/***********************************************************************************************/
/* ////////////////////////////////////////////////////////////////
These are Surface Stuff and clipper
//////////////////////////////////////////////////////////////// */

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
PROTECTED RETCODE make_surface_primary(DDRAWHANDLE ddrawstuff, DDSURFACEDESC surfacedesc, int WeWantBackBufferAsOffscreen, ULONG backbuffsizeX, ULONG backbuffsizeY)
{
	//LPDIRECTDRAWSURFACE tempsurface;
	//DDPIXELFORMAT pixelformat;
	
	if(!we_have_ddrawobj(ddrawstuff))
		assert(!"We don't have ddraw obj");
	
	//Better not mess around with already existing surfaces
	//So dump those!
	if(destroy_the_friggin_surfaces(ddrawstuff) != DDRAW_OK)
	{ assert(!"cannot destroy the damn surfaces, error in make_surface_primary"); return DDRAW_BAD; }
	
	
	//make the surface
	//if the creation succeeded
	if(!dd_check_error(TRUE, IDirectDraw2_CreateSurface(ddrawstuff->ddrawobj2, &surfacedesc, 
		&ddrawstuff->lpDDSurfacePrimary, NULL)))
	{
		//memset(&ddrawstuff->pixelformat, 0, sizeof(DDPIXELFORMAT));
		//ddrawstuff->pixelformat.dwSize = sizeof(DDPIXELFORMAT);

		//
		// Practically useless...but you never know when you know that you 
		// don't know when to use it
		//
		INIT_STRUCT(ddrawstuff->pixelformat);

		if(dd_check_error(TRUE, IDirectDrawSurface_GetPixelFormat(ddrawstuff->lpDDSurfacePrimary, &ddrawstuff->pixelformat)))
			//we should do something else in here...well???
			assert(!"Cannot Get Pixel Format in make_surface_primary");

		ddrawstuff->Channel = ddrawstuff->pixelformat.dwRGBBitCount >> 3; //divided by 8
		//
		//
		//

		//
		// Create the method for getting the color
		//
		if(ddrawstuff->pixelformat.dwFlags & DDPF_RGB)
		{
		switch(ddrawstuff->pixelformat.dwRGBBitCount)
		{
		case 15:
			SetPixelColorPtr = &SetPixelColor555;
			break;
		case 16:
			SetPixelColorPtr = &SetPixelColor565;
			break;
		case 24:
			SetPixelColorPtr = &SetPixelColor24;
			break;
		case 32:
			SetPixelColorPtr = &SetPixelColor32;
			break;
		}
		}
		else if(ddrawstuff->pixelformat.dwFlags & DDPF_PALETTEINDEXED8)
		{
			//8-bit
			//The GetColorPtr will use the first param as the index of the palette
			SetPixelColorPtr = &SetPixelColor8;
		}
		
		
			/*if(dd_check_error(TRUE, IDirectDraw2_GetAvailableVidMem(dditems->ddrawobj2, &dditems->surfaces.DDSurfacePrimDesc.ddsCaps, &dditems->vidcap.dwVidMemTotal, &dditems->vidcap.dwVidMemFree)))
		MessageBox(hwnd, "GetAvailableVidMem failed!", "do'h", MB_OK);*/
		
		if(WeWantBackBufferAsOffscreen)
			SETFLAG(ddrawstuff->status,DDFLAG_BACKBUFFERISOFFSCRN);
		else
			//just in case the flag is still set
			CLEARFLAG(ddrawstuff->status,DDFLAG_BACKBUFFERISOFFSCRN);
		
		// Get a pointer to the back buffer
		// If the user requested a number of backbuffer, then make one
		if(surfacedesc.dwBackBufferCount)
		{
			DDSURFACEDESC backbufferdesc;
			
			INIT_DDSD(backbufferdesc);
			backbufferdesc.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
			
			if(dd_check_error(TRUE, IDirectDrawSurface_GetAttachedSurface(ddrawstuff->lpDDSurfacePrimary, &backbufferdesc.ddsCaps, &ddrawstuff->backbuffer)))
			{ assert(!"shit, error in make_surface_primary"); destroy_the_friggin_surfaces(ddrawstuff); return DDRAW_BAD; }

			//set the size of the backbuffer
			ddrawstuff->backbuffsize.cx = ddrawstuff->currentmode.dwWidth;
			ddrawstuff->backbuffsize.cy = ddrawstuff->currentmode.dwHeight;
		}
		//create the backbuffer as offscreen if the user wants to...used for window mode
		else if(WeWantBackBufferAsOffscreen)
		{
			if(set_backbuffer_as_offscreen(ddrawstuff, backbuffsizeX, backbuffsizeY) != DDRAW_OK)
			{ assert(!"unable to set backbuffer as offscreen"); destroy_the_friggin_surfaces(ddrawstuff); return DDRAW_BAD; }
		}
#if 0
		//create the clipper of the backbuffer
		RECT screenarea;
		screenarea.top = screenarea.left = 0;
		screenarea.right = ddrawstuff->backbuffsize.cx;
		screenarea.bottom = ddrawstuff->backbuffsize.cy;

		ddrawstuff->lpDDBackBuffClipper = DDAttachClipping(ddrawstuff, ddrawstuff->backbuffer, 1, &screenarea);

		//set the flag indicating that we have a clipper
		if(ddrawstuff->lpDDBackBuffClipper)
			SETFLAG(ddrawstuff->status,DDFLAG_USINGCLIPPER);
#endif
	}
	
	//set the description made by the user, used later if we want to recreate the surfaces
	ddrawstuff->usersurfacedesc = surfacedesc;

	//set the surface as unlocked
	SETFLAG(ddrawstuff->status,DDFLAG_UNLOCKED);

	return DDRAW_OK;
}

/**********************************************************
;
;	Name:		destroy_the_friggin_surfaces
;
;	Purpose:	Only if you are pissed off of the surfaces...careful!
;
;	Input:		none
;
;	Output:		dead surfaces
;
;	Return:		some code
;
**********************************************************/
PRIVATE RETCODE destroy_the_friggin_surfaces(DDRAWHANDLE ddrawstuff)
{
	if(!we_have_ddrawobj(ddrawstuff)) //if there is no ddrawobj, then there is no surface
		return DDRAW_OK;

	if(ddrawstuff->lpDDSurfacePrimary)
	{
		if(dd_check_error(TRUE, IDirectDrawSurface_Release(ddrawstuff->lpDDSurfacePrimary)))
			return DDRAW_BAD;

		ddrawstuff->lpDDSurfacePrimary = NULL;
	}

	if(GETFLAGS(ddrawstuff->status,DDFLAG_BACKBUFFERISOFFSCRN))
	{
		if(ddrawstuff->backbuffer)
			if(dd_check_error(TRUE, IDirectDrawSurface_Release(ddrawstuff->backbuffer)))
				return DDRAW_BAD;
		
		ddrawstuff->backbuffer = NULL;
	}

#if 0
	//release the clipper of the backbuffer
	DDReleaseClipper(ddrawstuff->lpDDBackBuffClipper);
	CLEARFLAG(ddrawstuff->status,DDFLAG_USINGCLIPPER);
#endif

	return DDRAW_OK;
}

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
;	Return:		a freaking code of a sort
;
**********************************************************/
PROTECTED RETCODE make_surface_clipper(DDRAWHANDLE ddrawstuff, WINHANDLE hwnd)
{
	if(!we_have_ddrawobj(ddrawstuff))
		assert(!"We don't have ddraw obj");
	//
	// Create a clipper and attach it to the primary surface.
	//

    if (dd_check_error(TRUE, IDirectDraw_CreateClipper(ddrawstuff->ddrawobj2, 0, &ddrawstuff->lpDDClipper, NULL)))
	{ assert(!"Couldn't create the clipper."); return DDRAW_BAD; }

    // Associate our clipper with our hwnd so it will be updated
    // by Windows.
    if (dd_check_error(TRUE, IDirectDrawClipper_SetHWnd(ddrawstuff->lpDDClipper, 0, hwnd)))
	{ assert(!"Couldn't set the hwnd for clipper."); destroy_surface_clipper(ddrawstuff); return DDRAW_BAD; }

    // Associate our clipper with the primary surface, so Blt 
    // will use it.

	if(ddrawstuff->lpDDSurfacePrimary)
		if(dd_check_error(TRUE, IDirectDrawSurface_SetClipper(ddrawstuff->lpDDSurfacePrimary, ddrawstuff->lpDDClipper)))
		{ assert(!"Couldn't set the clipper to the darn primary surface."); destroy_surface_clipper(ddrawstuff); return DDRAW_BAD; }

	//IDirectDrawClipper_Release(dditems->lpDDClipper);
	return DDRAW_OK;
}

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
PROTECTED RETCODE destroy_surface_clipper(DDRAWHANDLE ddrawstuff)
{
	if(ddrawstuff->lpDDClipper != NULL)
		if(dd_check_error(TRUE, IDirectDrawClipper_Release(ddrawstuff->lpDDClipper)))
			return DDRAW_BAD;

	return DDRAW_OK;
}

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
PROTECTED LPDIRECTDRAWCLIPPER DDAttachClipping(DDRAWHANDLE ddrawstuff, LPDIRECTDRAWSURFACE lpDDSurface, int num_rects, LPRECT clip_list)
{
	LPDIRECTDRAWCLIPPER lpddclipper;
	LPRGNDATA region_data;

	//let the caller decide to display an error or not.
	if(FAILED(IDirectDraw_CreateClipper(ddrawstuff->ddrawobj2, 0, &lpddclipper, NULL)))
		return NULL;

	//create the hit list...err clip list.
	if(MemAlloc((void**)&region_data, sizeof(RGNDATAHEADER)+num_rects*sizeof(RECT)) != RETCODE_SUCCESS)
	{ assert(!"Failure allocating region_data, the heck with this!"); return NULL; }

	memcpy(region_data->Buffer, clip_list, sizeof(RECT)*num_rects);
	
	INIT_STRUCT(region_data->rdh);
	region_data->rdh.iType = RDH_RECTANGLES; //I wonder if they have circles...that'd be cool
	region_data->rdh.nCount = num_rects;
	region_data->rdh.nRgnSize = num_rects*sizeof(RECT);

	//I don't know what this is for, something to do with the limit
	region_data->rdh.rcBound.top = region_data->rdh.rcBound.left = 64000;
	region_data->rdh.rcBound.bottom = region_data->rdh.rcBound.right = -64000;

	//find the bounds of all clipping regions
	for(int i = 0; i < num_rects; i++)
	{
		//test if next rectangle unioned with the currect bound is larger
		if(clip_list[i].left < region_data->rdh.rcBound.left)
			region_data->rdh.rcBound.left = clip_list[i].left;
		if(clip_list[i].right > region_data->rdh.rcBound.right)
			region_data->rdh.rcBound.right = clip_list[i].right;
		if(clip_list[i].top < region_data->rdh.rcBound.top)
			region_data->rdh.rcBound.top = clip_list[i].top;
		if(clip_list[i].bottom > region_data->rdh.rcBound.bottom)
			region_data->rdh.rcBound.bottom = clip_list[i].bottom;
	}

	//now to set up the clipper and we're all set!
	if(FAILED(IDirectDrawClipper_SetClipList(lpddclipper, region_data, 0)))
	{
		MemFree((void**)&region_data);
		return NULL;
	}

	//attach it, baby!
	if(FAILED(IDirectDrawSurface_SetClipper(lpDDSurface, lpddclipper)))
	{
		MemFree((void**)&region_data);
		return NULL;
	}

	//everything A-ok!
	MemFree((void**)&region_data);
	return lpddclipper;
}

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
PROTECTED void DDReleaseClipper(LPDIRECTDRAWCLIPPER lpddclipper)
{
	if(lpddclipper)
		dd_check_error(TRUE, IDirectDrawClipper_Release(lpddclipper));
}

/**********************************************************
;
;	Name:		set_backbuffer_as_offscreen
;
;	Purpose:	not really sure if this is THE WAY for window mode...but it works
;
;	Input:		the size of the buffer you want to make, defaulted to current screen size
;
;	Output:		backbuffer becomes an offscreen surface
;
;	Return:		SOMETHING!!!
;
**********************************************************/
PRIVATE RETCODE set_backbuffer_as_offscreen(DDRAWHANDLE ddrawstuff, long sizeX, long sizeY)
{
	if(!we_have_ddrawobj(ddrawstuff))
		return DDRAW_BAD;

	//we can't have either one zero!  It's just ridiculous
	if((sizeX == 0) || (sizeY == 0))
	{
		//I hope that the current mode is correctly filled :P
		ddrawstuff->backbuffsize.cx = ddrawstuff->currentmode.dwWidth;
		ddrawstuff->backbuffsize.cy = ddrawstuff->currentmode.dwHeight;
	}
	else
	{
		ddrawstuff->backbuffsize.cx = sizeX;
		ddrawstuff->backbuffsize.cy = sizeY;
	}

	//make sure we clean them up first!!!
	if(GETFLAGS(ddrawstuff->status,DDFLAG_BACKBUFFERISOFFSCRN))
	{
		if(ddrawstuff->backbuffer)
			if(dd_check_error(TRUE, IDirectDrawSurface_Release(ddrawstuff->backbuffer)))
				return DDRAW_BAD;

		ddrawstuff->backbuffer = NULL;
	}

	
	//
	// We're going to make the backbuffer as an offscreen for blitting
	//
	DDSURFACEDESC ddsdbackbuff;
	INIT_DDSD(ddsdbackbuff);
	
	ddsdbackbuff.dwSize = sizeof( DDSURFACEDESC );
	ddsdbackbuff.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsdbackbuff.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; //| DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
	ddsdbackbuff.dwHeight = ddrawstuff->backbuffsize.cy;
	ddsdbackbuff.dwWidth = ddrawstuff->backbuffsize.cx;
	
	//NOW WE WILL MAKE THE BACKBUFFER A SURFACE!  HOORAH!
	if(dd_check_error(TRUE, IDirectDraw2_CreateSurface(ddrawstuff->ddrawobj2, &ddsdbackbuff, 
		&ddrawstuff->backbuffer, NULL)))
		return DDRAW_BAD;

	return DDRAW_OK;
}

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
PUBLIC bool DDIsImageValid(IMAGEHANDLE image)
{
	if(image)
	{
		if(image->Buffer)
			return true;
	}

	return false;
}

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
PUBLIC RETCODE DDBltImage(DDRAWHANDLE ddrawstuff, IMAGEHANDLE image, RECT rcRect, LOCATION loc)
{
	assert(ddrawstuff);
	assert(image);
	HRESULT ddrval;

	SIZE rcSize;

	rcSize.cx = rcRect.right - rcRect.left;
	rcSize.cy = rcRect.bottom - rcRect.top;

	//
	// If the location is in negative boundary of the area
	// adjust the rectangle
	//
	if(loc.x < 0)
	{
		rcRect.left -= loc.x;
		loc.x = 0;
	}
	if(loc.y < 0)
	{
		rcRect.top -= loc.y;
		loc.y = 0;
	}
	//If the rectangle to be blitted is a little bit out of boundary...
	//subtract it by that amount
	if(loc.x + rcSize.cx > ddrawstuff->backbuffsize.cx)
		rcRect.right -= (loc.x + rcSize.cx) - ddrawstuff->backbuffsize.cx;
	if(loc.y + rcSize.cy > ddrawstuff->backbuffsize.cy)
		rcRect.bottom -= (loc.y + rcSize.cy) - ddrawstuff->backbuffsize.cy;
	
	//Since we fooled around with the rectange, check if it is invalid, if so, return
	if(rcRect.left >= rcRect.right)
		return DDRAW_OK; //we will just assume we suceeded
	else if(rcRect.top >= rcRect.bottom)
		return DDRAW_OK; //we will just assume we suceeded
	//
	//
	//
			
		//This will blit the offscreen image to the backbuffer...it will be flipped later.		
		while( 1 )
		{
			//surfacebackbuff1->BltFast
				ddrval = IDirectDrawSurface_BltFast(ddrawstuff->backbuffer,
					(DWORD)(loc.x),(DWORD)(loc.y), image->Buffer,
					&rcRect, image->BltFastType);
			
			
			if( ddrval == DDERR_SURFACELOST )
			{
				//this means that we have to restore the image the user passed in, let them do it.
				//but first, attemp to restore the surface
				ddrval = IDirectDrawSurface_Restore(ddrawstuff->backbuffer);

				if(ddrval == DD_OK)
					if(DDRestoreImage(ddrawstuff, image) != DDRAW_OK)
						return DDRAW_SURFACELOST;
				else
					return DDRAW_BAD;
			}
			else //heh
			{
				break;
			}
		}	

		return DDRAW_OK;
}

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
PUBLIC RETCODE DDInitImageList(DDRAWHANDLE ddrawstuff, short numImage)
{
	assert(numImage > 0);

	if(ddrawstuff->Images) //if there is an existing Image list, destroy it
	{
		DDClearImageList(ddrawstuff);
		if(ListDestroy(&ddrawstuff->Images) != RETCODE_SUCCESS)
		{ assert(!"Unable to destroy image list, error in DDInitImageList"); return DDRAW_BAD; }

		ddrawstuff->Images = NULL;
	}

	ddrawstuff->Images = ListInit(numImage, sizeof(IMAGE));

	if(!ddrawstuff->Images)
		return DDRAW_BAD;

	SETFLAG(ddrawstuff->status,DDFLAG_IMAGELISTCREATED);

	return DDRAW_OK;
}

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
PUBLIC RETCODE DDClearImageList(DDRAWHANDLE ddrawstuff)
{
	if(ddrawstuff->Images)
	{
		int MaxNode = ListGetNodeCount(ddrawstuff->Images);
		IMAGEHANDLE ImgHolder;
		
		// First we must release the surfaces associated with all the image
		for(int i = 0; i < MaxNode; i++)
		{
			ImgHolder = (IMAGEHANDLE)ListExtract(ddrawstuff->Images, i);
			
			if(DDKillImageData(ImgHolder) != DDRAW_OK)
				assert(!"DDKillImageData failed in DDClearImageList");
		}
		
		
		if(ListEmptyList(ddrawstuff->Images) != RETCODE_SUCCESS)
			return DDRAW_BAD;		
	}

	return DDRAW_OK;
}

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
PUBLIC IMAGEHANDLE DDGetImage(DDRAWHANDLE ddrawstuff, short ID)
{
	long index;
	assert(ddrawstuff);

	index = ListSearch (ddrawstuff->Images, ImageIDSearch, (void *)ID);

	if(index < 0)
		return NULL;

	return (IMAGEHANDLE)ListExtract(ddrawstuff->Images, index);
}

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
PUBLIC IMAGEHANDLE DDCreateImage(DDRAWHANDLE ddrawstuff, short newID, char *filename, bool setSrcColorKey, ULONG low_color_val, ULONG high_color_val, IMGTYPE imgType)
{
	//First make sure that the image list is created
	if(GETFLAGS(ddrawstuff->status, DDFLAG_IMAGELISTCREATED))
	{
		IMAGE newImage;
		
		memset(&newImage, 0, sizeof(IMAGE));
		
		
		//check to see if there are any available spaces...
		assert(ListGetMaxNodes(ddrawstuff->Images) - ListGetNodeCount(ddrawstuff->Images) > 0);
				
		//start filling the newImage depending on type
		switch(imgType)
		{
		case IMG_BMP:
			newImage.ImageSize = DDLoadBitmap(ddrawstuff, &(newImage.Buffer), filename);
			break;
		default:
			//invalid, so do nothing and return NULL
			return NULL;
		}
		
		// (0,0) means something went wrong
		if((newImage.ImageSize.cx == 0) || (newImage.ImageSize.cy == 0))
		{
			
			return NULL;
		}
		
		//Copy the file path for later use
		strcpy(newImage.filename, filename);
		
		//...as well as the image type
		newImage.imageType = imgType;
		//ooops...sorry
		newImage.ID = newID;
		
		
		//get the number of channels
		DDPIXELFORMAT ddpf;
		INIT_STRUCT(ddpf);
		
		if(dd_check_error(TRUE, IDirectDrawSurface_GetPixelFormat(newImage.Buffer, &ddpf)))
			assert(!"Cannot Get Pixel Format in DDCreateImage");
		
		newImage.Channel = ddpf.dwRGBBitCount >> 3; //divided by 8
		
		//set the color key if specified
		if(setSrcColorKey)
		{
			if(SetColorKeySrc(newImage.Buffer, low_color_val, high_color_val) != DDRAW_OK)
				assert(!"Bad SetColorKeySrc, failure in DDCreateImage");

			//
			// Set the bltfast type for later use
			//
			newImage.BltFastType = DDBLTFAST_SRCCOLORKEY;
		}
		else
		{
			//
			// Set the bltfast type for later use
			//
			newImage.BltFastType = DDBLTFAST_NOCOLORKEY;
		}
			
			//get the index
			newImage.index = ListGetNodeCount(ddrawstuff->Images);
			//append the image to the list
			if(ListAppend(ddrawstuff->Images, &newImage) != RETCODE_SUCCESS)
			{
				//something bad happened, so we have to destroy the image data and return
				//a NULL
				
				DDKillImageData(&newImage);
				return NULL;
			}
			
			//um...
			return (IMAGEHANDLE)ListExtract(ddrawstuff->Images, newImage.index);
			//return NULL; //for now...
	}
	else
	{ assert(!"Image List hasn't been initialized!!!"); return NULL; }
}

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
PUBLIC IMAGEHANDLE DDCreateMyOwnImage(DDRAWHANDLE ddrawstuff, char *filename, bool setSrcColorKey, ULONG low_color_val, ULONG high_color_val, IMGTYPE imgType)
{
		IMAGEHANDLE newImage;

		//allocate the new image
		if(MemAlloc((void**)&newImage, sizeof(IMAGE)) != RETCODE_SUCCESS)
		{ assert(!"Error allocating newImage!  DDCreateMyOwnImage failed"); return NULL; }
		memset(newImage, 0, sizeof(IMAGE));

		//set it as individual image
		newImage->index = INDIVIDUALIMAGE;
		
		
		//start filling the newImage depending on type
		switch(imgType)
		{
		case IMG_BMP:
			newImage->ImageSize = DDLoadBitmap(ddrawstuff, &(newImage->Buffer), filename);
			break;
		default:
			//invalid, so do nothing and return NULL
			DDImageDestroy(ddrawstuff, newImage);
			return NULL;
		}
		
		// (0,0) means something went wrong
		if((newImage->ImageSize.cx == 0) || (newImage->ImageSize.cy == 0))
		{
			DDImageDestroy(ddrawstuff, newImage);
			return NULL;
		}
		
		//Copy the file path for later use
		strcpy(newImage->filename, filename);
		
		//...as well as the image type
		newImage->imageType = imgType;
		//ooops...sorry
		newImage->ID = ImageDummyID;
		
		
		//get the number of channels
		DDPIXELFORMAT ddpf;
		INIT_STRUCT(ddpf);
		
		if(dd_check_error(TRUE, IDirectDrawSurface_GetPixelFormat(newImage->Buffer, &ddpf)))
		{ assert(!"Cannot Get Pixel Format in DDCreateImage"); DDImageDestroy(ddrawstuff, newImage); return NULL; }
		
		newImage->Channel = ddpf.dwRGBBitCount >> 3; //divided by 8
		
		//set the color key if specified
		if(setSrcColorKey)
		{
			if(SetColorKeySrc(newImage->Buffer, low_color_val, high_color_val) != DDRAW_OK)
				assert(!"Bad SetColorKeySrc, failure in DDCreateImage");

			//
			// Set the bltfast type for later use
			//
			newImage->BltFastType = DDBLTFAST_SRCCOLORKEY;
		}
		else
		{
			//
			// Set the bltfast type for later use
			//
			newImage->BltFastType = DDBLTFAST_NOCOLORKEY;
		}
			
			
			//um...
			return newImage;
			//return NULL; //for now...
}

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
PUBLIC RETCODE DDReloadImage(DDRAWHANDLE ddrawstuff, IMAGEHANDLE image, char *filename, IMGTYPE imgType)
{
	//there should be a surface
	assert(image->Buffer);

	if(filename) //if the give a new filename
	{
		strcpy(image->filename, filename);
		image->imageType = imgType;
	}

	//make sure there is a file path
	if(image->filename)
	{
		switch(image->imageType)
		{
		case IMG_BMP:
			image->ImageSize = DDReloadBitmap(image->Buffer, image->filename);
			
			if((image->ImageSize.cx == 0) || (image->ImageSize.cy == 0))
			{ assert(!"Bad Image Size.  Error in DDReloadImage!"); return DDRAW_BAD; }
			break;
		default:
			return DDRAW_BAD;
		}
	}
	else //otherwise, just create the surface with it's size
	{
		//release the old buffer
		if(image->Buffer)
			IDirectDrawSurface_Release(image->Buffer);

		assert(image->ImageSize.cx > 0 && image->ImageSize.cy > 0);
			//
			// Create the surface
			//
			DDSURFACEDESC ddsd;
			INIT_DDSD(ddsd);

			ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; //| DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
			ddsd.dwHeight = image->ImageSize.cy;
			ddsd.dwWidth = image->ImageSize.cx;
			if(dd_check_error(TRUE, IDirectDraw2_CreateSurface(ddrawstuff->ddrawobj2, &ddsd, 
				&image->Buffer, NULL)))
			{ assert(!"Damn!  Creation of surface failed, panic NOW!!!  Unable to create empty Image"); return DDRAW_BAD; }
	}

	return DDRAW_OK;
}

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
PRIVATE SIZE DDLoadBitmap(DDRAWHANDLE ddrawstuff, LPDIRECTDRAWSURFACE *lpDDSurfaceOffScr, char *bmfilename, long dx, long dy)
{
	HBITMAP hbm;
	BITMAP bmInfo;
	DDSURFACEDESC ddsd;
	SIZE bmsize = {0, 0};

	if(!we_have_ddrawobj(ddrawstuff))
		return bmsize;

	if(*lpDDSurfaceOffScr)
		if(FAILED(IDirectDrawSurface_Release(*lpDDSurfaceOffScr))) //throw away this crap
			return bmsize;

	//
    //  try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), bmfilename, IMAGE_BITMAP, dx, dy, LR_CREATEDIBSECTION);

    if (hbm == NULL)
	hbm = (HBITMAP)LoadImage(NULL, bmfilename, IMAGE_BITMAP, dx, dy, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

    if (hbm == NULL)
		return bmsize;

	//
    // get size of the bitmap
    //
    GetObject(hbm, sizeof(bmInfo), &bmInfo);      // get size of bitmap

	bmsize.cx = bmInfo.bmWidth;
	bmsize.cy = bmInfo.bmHeight;

	//
	// Create the surface
	//
	INIT_DDSD(ddsd);

		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; //| DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		ddsd.dwHeight = bmInfo.bmHeight;
		ddsd.dwWidth = bmInfo.bmWidth;
		if(dd_check_error(TRUE, IDirectDraw2_CreateSurface(ddrawstuff->ddrawobj2, &ddsd, 
			lpDDSurfaceOffScr, NULL)))
			goto END;
	//
	//
	//

	//This better work!
	//copy the bitmap to offscreen
	if(dd_check_error(TRUE, DDCopyBitmap(*lpDDSurfaceOffScr, bmfilename, 0, 0, 0, 0)))
		assert(!"error in DDReloadBitmapOffScreen");

END:
	DeleteObject(hbm);
	return bmsize;
}

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
PRIVATE SIZE DDReloadBitmap(LPDIRECTDRAWSURFACE lpDDSurface, char *bmfilename)
{
	SIZE bmsize = {0, 0};

	if (lpDDSurface == NULL)
        return bmsize;

	//for now, this is the format
	if(dd_check_error(TRUE, DDCopyBitmap(lpDDSurface, bmfilename, 0, 0, 0, 0)))
		assert(!"DDCopyBitmap Failed!  Panic!  Now!!!  Error in DDReloadBitmap");

	//
    // get size of surface.
    //
	DDSURFACEDESC ddsd;
    ddsd.dwSize = sizeof(DDSURFACEDESC);
    //ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
	/*ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;*/ //| DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
    IDirectDrawSurface_GetSurfaceDesc(lpDDSurface,(&ddsd));
	bmsize.cx = ddsd.dwWidth;
	bmsize.cy = ddsd.dwHeight;

	return bmsize;
}

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
PRIVATE RETCODE DDKillImageData(IMAGEHANDLE image)
{
	if(GETFLAGS(image->Status, DDFLAG_LOCKED))
		dd_unlock(image);

	if(image->Buffer)
		if(FAILED(IDirectDrawSurface_Release(image->Buffer)))
		{ assert(!"Unable to kill image buffer"); return DDRAW_BAD; }

	image->Buffer = NULL;

	//memset(image, 0, sizeof(IMAGE));

	return DDRAW_OK;
}

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
PUBLIC RETCODE DDRestoreImage(DDRAWHANDLE ddrawstuff, IMAGEHANDLE image)
{
	assert(image);
	HRESULT ddrval;

	//try to restore it first
    ddrval = IDirectDrawSurface_Restore(image->Buffer);
	
    
	if(ddrval != DD_OK)
	{
		if(DDReloadImage(ddrawstuff, image) != DDRAW_OK)
			return DDRAW_BAD;
	}

	return DDRAW_OK;
}

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
PUBLIC RETCODE DDRestoreImageList(DDRAWHANDLE ddrawstuff)
{
	int MaxNode = ListGetNodeCount(ddrawstuff->Images);
	IMAGEHANDLE ImgHolder;
	bool failure_somewhere = false;

	for(int i = 0; i < MaxNode; i++)
	{
		ImgHolder = (IMAGEHANDLE)ListExtract(ddrawstuff->Images, i);		
    
		//if this fails, continue with the other images
		if(DDRestoreImage(ddrawstuff, ImgHolder) != DDRAW_OK)
		{ failure_somewhere = true; continue; }
	}

	if(!failure_somewhere)
		return DDRAW_OK;

	assert(!"There is a failure restoring an image, debug me if you can");
	return DDRAW_BAD;
}

/**********************************************************
;
;	Name:		DDCopyBitmap
;
;	Purpose:	copy the given HBITMAP to an existing surface
;
;	Input:		The surface and the hbitmap and the offset of the bitmap.
;
;	Output:		stuff happens
;
;	Return:		some result
;
**********************************************************/
static HRESULT DDCopyBitmap(LPDIRECTDRAWSURFACE lpDDSurface, char *imgpath, int x, int y, int dx, int dy)
{
	//HDC					winhdc;
    //HDC                 hdcImage;
    HDC                 hdc;
    //BITMAP              bm;
    DDSURFACEDESC       ddsd;
    HRESULT             hr;
	//RECT				rect;
	
    if (lpDDSurface == NULL)
        return DDERR_INVALIDPARAMS;

    //
    // make sure this surface is restored.
    //

    IDirectDrawSurface_Restore(lpDDSurface);

    //
    // get size of surface.
    //
    ddsd.dwSize = sizeof(DDSURFACEDESC);
    //ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
	/*ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;*/ //| DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
    IDirectDrawSurface_GetSurfaceDesc(lpDDSurface,(&ddsd));

	CSBitmap srcImg, destImg;

	int numchannel = ddsd.ddpfPixelFormat.dwRGBBitCount>>3;
	bool result;

	if(numchannel != 2)
		result = LoadBMPFile(imgpath, &srcImg, 0);
	else
		result = false;

	if(!result)
	{ 
		HDC                 hdcImage;
		BITMAP              bm;
		HBITMAP hbm;
		//RECT				rect;

		//
		//  try to load the bitmap as a resource, if that fails, try it as a file
		//
		hbm = (HBITMAP)LoadImage(GetModuleHandle(NULL), imgpath, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION);

		if (hbm == NULL)
		hbm = (HBITMAP)LoadImage(NULL, imgpath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE|LR_CREATEDIBSECTION);

		if (hbm == NULL)
			return DDERR_EXCEPTION;
		
		//
		//  select bitmap into a memoryDC so we can use it.
		//
				
		hdcImage = CreateCompatibleDC(NULL);
		
		if (!hdcImage)
			OutputDebugString("createcompatible dc failed\n");
		
		SelectObject(hdcImage, hbm);
		
		//
		// get size of the bitmap
		//
		GetObject(hbm, sizeof(bm), &bm);    // get size of bitmap
		dx = (dx == 0) ? bm.bmWidth  : dx;    // use the passed size, unless zero
		dy = (dy == 0) ? bm.bmHeight : dy;
				
		hr = IDirectDrawSurface_GetDC(lpDDSurface, &hdc);
		
		if (hr == DD_OK)
		{
			//GetClientRect(hwnd, &rect);
			
			StretchBlt(hdc, 0, 0,
			ddsd.dwWidth,
			ddsd.dwHeight,
			hdcImage,
			x, y, dx, dy, SRCCOPY);
			
			/*BitBlt(hdc, 0, 0,
				dx,//ddsd.dwWidth,
				dy,//ddsd.dwHeight,
				hdcImage,
				x, y, SRCCOPY);*/
			
			IDirectDrawSurface_ReleaseDC(lpDDSurface, hdc);
		}
		
		DeleteDC(hdcImage);
	}
	else
	{
		
		if(!SetSize(&destImg, ddsd.dwWidth, ddsd.dwHeight, numchannel))
		{ assert(!"Unable to set destImg size, error in DDCopyBitmap"); return DDERR_EXCEPTION; }
		
		if(numchannel < 3)
		{
			ScaleBitmap(&srcImg, &destImg, false);
		}
		else
		{
			ScaleBitmap(&srcImg, &destImg, true);
		}
		
		hr = IDirectDrawSurface_GetDC(lpDDSurface, &hdc);
		
		if (hr == DD_OK)
		{
			//GetClientRect(hwnd, &rect);
			
			/*StretchBlt(hdc, 0, 0,
			ddsd.dwWidth,
			ddsd.dwHeight,
			hdcImage,
			x, y, dx, dy, SRCCOPY);*/
			DrawAll(&destImg, hdc);
			
			/*BitBlt(hdc, 0, 0,
			dx,//ddsd.dwWidth,
			dy,//ddsd.dwHeight,
			hdcImage,
			x, y, SRCCOPY);*/
			
			IDirectDrawSurface_ReleaseDC(lpDDSurface, hdc);
		}
		
		
		FreeBitmap(&srcImg);
		FreeBitmap(&destImg);
	}
	//DeleteDC(hdcImage);
	//ReleaseDC(hwnd, winhdc);

    return hr;
}

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
void dd_update_screen(DDRAWHANDLE ddrawstuff, RECT *windowarea)
{
	dd_update(ddrawstuff, windowarea);
}

/***********************************************************************************************/
/***********************************************************************************************/
/* ////////////////////////////////////////////////////////////////
These are Surface Manipulation
//////////////////////////////////////////////////////////////// */

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
PUBLIC void DDClearBuffer(DDRAWHANDLE ddrawstuff, ULONG color)
{
	DDBLTFX ddfx;
	INIT_STRUCT(ddfx);

	ddfx.dwFillColor = color;

	if(dd_check_error(TRUE, IDirectDrawSurface_Blt(ddrawstuff->backbuffer, NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddfx)))
		assert(!"Failure in DDClearBuffer");
}

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
PUBLIC  void dd_lock(DDRAWHANDLE ddrawstuff)
{
	if(GETFLAGS(ddrawstuff->status, DDFLAG_UNLOCKED))
	{
		DDSURFACEDESC ddsd;
		INIT_DDSD(ddsd);
		
		//if all goes well, set up our stuff
		if(SUCCEEDED(IDirectDrawSurface_Lock(ddrawstuff->backbuffer, NULL, &ddsd, 
			DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
		{
			//after calling this function, the user must make sure
			//the status is DDFLAG_LOCKED.
			CLEARFLAG(ddrawstuff->status, DDFLAG_UNLOCKED);
			SETFLAG(ddrawstuff->status, DDFLAG_LOCKED);
			ddrawstuff->VidMem = (BYTE*)ddsd.lpSurface;
			ddrawstuff->Pitch = ddsd.lPitch;
		}
	}
}

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
PUBLIC  void dd_unlock(DDRAWHANDLE ddrawstuff)
{
	if(GETFLAGS(ddrawstuff->status, DDFLAG_LOCKED))
	{
		if(SUCCEEDED(IDirectDrawSurface2_Unlock(ddrawstuff->backbuffer, NULL)))
		{
			CLEARFLAG(ddrawstuff->status, DDFLAG_LOCKED);
			SETFLAG(ddrawstuff->status,DDFLAG_UNLOCKED);
			ddrawstuff->VidMem = NULL;
			ddrawstuff->Pitch = 0;
		}
	}
}

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
PUBLIC  void dd_lock(IMAGEHANDLE image)
{
	if(GETFLAGS(image->Status, DDFLAG_UNLOCKED))
	{
		DDSURFACEDESC ddsd;
		INIT_DDSD(ddsd);
		
		//if all goes well, set up our stuff
		if(SUCCEEDED(IDirectDrawSurface_Lock(image->Buffer, NULL, &ddsd, 
			DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL)))
		{
			//after calling this function, the user must make sure
			//the status is DDFLAG_LOCKED.
			CLEARFLAG(image->Status, DDFLAG_UNLOCKED);
			SETFLAG(image->Status, DDFLAG_LOCKED);
			image->VidMem = (BYTE*)ddsd.lpSurface;
			image->Pitch = ddsd.lPitch;
		}
	}
}

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
PUBLIC  void dd_unlock(IMAGEHANDLE image)
{
	if(GETFLAGS(image->Status, DDFLAG_LOCKED))
	{
		if(SUCCEEDED(IDirectDrawSurface_Unlock(image->Buffer, NULL)))
		{
			CLEARFLAG(image->Status, DDFLAG_LOCKED);
			SETFLAG(image->Status,DDFLAG_UNLOCKED);
			image->VidMem = NULL;
			image->Pitch = 0;
		}
	}
}

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
PUBLIC long DDGetPitch(DDRAWHANDLE ddrawstuff)
{
	return ddrawstuff->Pitch;
}

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
PUBLIC long DDGetPitch(IMAGEHANDLE image)
{
	return image->Pitch;
}

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
PUBLIC SIZE DDGetSize(DDRAWHANDLE ddrawstuff)
{
	assert(ddrawstuff);
	return ddrawstuff->backbuffsize;
}

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
PUBLIC SIZE DDGetSize(IMAGEHANDLE image)
{
	assert(image);
	return image->ImageSize;
}

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
PUBLIC long DDGetChannel(DDRAWHANDLE ddrawstuff)
{
	assert(ddrawstuff);
	return ddrawstuff->Channel;
}

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
PUBLIC long DDGetChannel(IMAGEHANDLE image)
{
	assert(image);
	return image->Channel;
}

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
PUBLIC BYTE * DDGetLinePtr(DDRAWHANDLE ddrawstuff, long y)
{
	assert(ddrawstuff);
	assert(GETFLAGS(ddrawstuff->status,DDFLAG_LOCKED));
	assert(y >= 0 && y < ddrawstuff->backbuffsize.cy);

	return ddrawstuff->VidMem + y*ddrawstuff->Pitch; 
}

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
PUBLIC BYTE * DDGetPixelPtr(DDRAWHANDLE ddrawstuff, long x, long y)
{
	assert(ddrawstuff);
	assert(GETFLAGS(ddrawstuff->status,DDFLAG_LOCKED));
	assert(x >= 0 && x < ddrawstuff->backbuffsize.cx);

	return DDGetLinePtr(ddrawstuff, y) + x*ddrawstuff->Channel;
}

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
PUBLIC BYTE * DDGetLinePtr(IMAGEHANDLE image, long y)
{
	assert(image);
	assert(GETFLAGS(image->Status,DDFLAG_LOCKED));
	assert(y >= 0 && y < image->ImageSize.cy);

	return image->VidMem + y*image->Pitch; 
}

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
PUBLIC BYTE * DDGetPixelPtr(IMAGEHANDLE image, long x, long y)
{
	assert(image);
	assert(GETFLAGS(image->Status,DDFLAG_LOCKED));
	assert(x >= 0 && x < image->ImageSize.cx);

	return DDGetLinePtr(image, y) + x*image->Channel;
}

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
PUBLIC void SetPixelColor(UCHAR *buff, int Red, int Green, int Blue)
{
	SetPixelColorPtr(buff, Red, Green, Blue); //sigh...
}

//
// Stupid Image functions for the sake of the font engine
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
PROTECTED bool DDIsImageNotInList(IMAGEHANDLE image)
{
	if(image->index == INDIVIDUALIMAGE)
		return true;
	return false;
}

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
PROTECTED bool DDIsImageDummy(IMAGEHANDLE image)
{
	if(image->ID == ImageDummyID)
		return true;
	return false;
}

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
PROTECTED IMAGEHANDLE DDCreateEmptyImage(DDRAWHANDLE ddrawstuff, long sizeX, long sizeY, short ImageID, bool setSrcColorKey, ULONG low_color_val, ULONG high_color_val, IMGTYPE imgType)
{
	//First make sure that the image list is created
	if(GETFLAGS(ddrawstuff->status, DDFLAG_IMAGELISTCREATED))
	{
		IMAGE newImage;
		
		memset(&newImage, 0, sizeof(IMAGE));
		
		
		//check to see if there are any available spaces...
		assert(ListGetMaxNodes(ddrawstuff->Images) - ListGetNodeCount(ddrawstuff->Images) > 0);
				
		
		newImage.imageType = imgType; //in here for now
		//ooops...sorry
		newImage.ID = ImageID;
		

		//
		// Create the surface
		//
		DDSURFACEDESC ddsd;
		INIT_DDSD(ddsd);

		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; //| DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
		ddsd.dwHeight = sizeY;
		ddsd.dwWidth = sizeX;
		if(dd_check_error(TRUE, IDirectDraw2_CreateSurface(ddrawstuff->ddrawobj2, &ddsd, 
			&newImage.Buffer, NULL)))
		{ assert(!"Damn!  Creation of surface failed, panic NOW!!!  Unable to create empty Image"); return NULL; }


		//get the number of channels
		DDPIXELFORMAT ddpf;
		INIT_STRUCT(ddpf);
		
		if(dd_check_error(TRUE, IDirectDrawSurface_GetPixelFormat(newImage.Buffer, &ddpf)))
			assert(!"Cannot Get Pixel Format in DDCreateImage");
		
		newImage.Channel = ddpf.dwRGBBitCount >> 3; //divided by 8

		//set up the size
		newImage.ImageSize.cx = sizeX;
		newImage.ImageSize.cy = sizeY;
		
		//set the color key if specified
		if(setSrcColorKey)
		{
			if(SetColorKeySrc(newImage.Buffer, low_color_val, high_color_val) != DDRAW_OK)
				assert(!"Bad SetColorKeySrc, failure in DDCreateImage");

			//
			// Set the bltfast type for later use
			//
			newImage.BltFastType = DDBLTFAST_SRCCOLORKEY;
		}
		else
		{
			//
			// Set the bltfast type for later use
			//
			newImage.BltFastType = DDBLTFAST_NOCOLORKEY;
		}
			
			//get the index
			newImage.index = ListGetNodeCount(ddrawstuff->Images);
			//append the image to the list
			if(ListAppend(ddrawstuff->Images, &newImage) != RETCODE_SUCCESS)
			{
				//something bad happened, so we have to destroy the image data and return
				//a NULL
				
				DDKillImageData(&newImage);
				return NULL;
			}
			
			//um...
			return (IMAGEHANDLE)ListExtract(ddrawstuff->Images, newImage.index);
			//return NULL; //for now...
	}
	
	assert(!"Image List hasn't been initialized!!!");
	return NULL;
}

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
PROTECTED IMAGEHANDLE DDCreateMyOwnEmptyImage(DDRAWHANDLE ddrawstuff, long sizeX, long sizeY, bool setSrcColorKey, ULONG low_color_val, ULONG high_color_val, IMGTYPE imgType)
{
	IMAGEHANDLE newImage;
	
	if(MemAlloc((void**)&newImage, sizeof(IMAGE)) != RETCODE_SUCCESS)
	{ assert(!"OH MAN!!!  MemAlloc failed!  Unable to create empty Image"); return NULL; }

	//memset it
	memset(newImage, 0, sizeof(IMAGE));

	//Set up the ID
	newImage->ID = ImageDummyID;

	//set up the Image type
	newImage->imageType = imgType;

	//set up some more fun stuff
	newImage->index = INDIVIDUALIMAGE;

	//
	// Create the surface
	//
	DDSURFACEDESC ddsd;
	INIT_DDSD(ddsd);

	ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; //| DDSCAPS_VIDEOMEMORY | DDSCAPS_LOCALVIDMEM;
	ddsd.dwHeight = sizeY;
	ddsd.dwWidth = sizeX;
	if(dd_check_error(TRUE, IDirectDraw2_CreateSurface(ddrawstuff->ddrawobj2, &ddsd, 
		&newImage->Buffer, NULL)))
	{ assert(!"Damn!  Creation of surface failed, panic NOW!!!  Unable to create empty Image"); MemFree((void**)&newImage); return NULL; }

	DDPIXELFORMAT ddpf;
	INIT_STRUCT(ddpf);

	if(dd_check_error(TRUE, IDirectDrawSurface_GetPixelFormat(newImage->Buffer, &ddpf)))
			//we should do something else in here...well???
			assert(!"Cannot Get Pixel Format in make_surface_primary");

	newImage->Channel = ddpf.dwRGBBitCount >> 3; //divided by 8

	//set up the size
	newImage->ImageSize.cx = sizeX;
	newImage->ImageSize.cy = sizeY;

	//set the color key if specified
	if(setSrcColorKey)
	{
		if(SetColorKeySrc(newImage->Buffer, low_color_val, high_color_val) != DDRAW_OK)
			assert(!"Bad SetColorKeySrc, failure in DDCreateImage");

		//
		// Set the bltfast type for later use
		//
		newImage->BltFastType = DDBLTFAST_SRCCOLORKEY;
	}
	else
	{
		//
		// Set the bltfast type for later use
		//
		newImage->BltFastType = DDBLTFAST_NOCOLORKEY;
	}

	return newImage;
}

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
PROTECTED RETCODE DDBltImageToImage(DDRAWHANDLE ddrawstuff, IMAGEHANDLE destImage, IMAGEHANDLE srcImage,  RECT rcRect, LOCATION loc)
{
	//assert(ddrawstuff);
	assert(destImage);
	assert(srcImage);

	HRESULT ddrval;
	SIZE rcSize, destSize;

	rcSize.cx = rcRect.right - rcRect.left;
	rcSize.cy = rcRect.bottom - rcRect.top;

	//
	// If the location is in negative boundary of the area
	// adjust the rectangle
	//
	if(loc.x < 0)
	{
		rcRect.left -= loc.x;
		loc.x = 0;
	}
	if(loc.y < 0)
	{
		rcRect.top -= loc.y;
		loc.y = 0;
	}
	//If the rectangle to be blitted is a little bit out of boundary...
	//subtract it by that amount
	destSize = DDGetSize(destImage);
	if(loc.x + rcSize.cx > destSize.cx)
		rcRect.right -= (loc.x + rcSize.cx) - destSize.cx;
	if(loc.y + rcSize.cy > destSize.cy)
		rcRect.bottom -= (loc.y + rcSize.cy) - destSize.cy;
	
	//Since we fooled around with the rectange, check if it is invalid, if so, return
	if(rcRect.left >= rcRect.right)
		return DDRAW_OK; //we will just assume we suceeded
	else if(rcRect.top >= rcRect.bottom)
		return DDRAW_OK; //we will just assume we suceeded
	//
	//
	//
			
		//This will blit the offscreen image to the backbuffer...it will be flipped later.		
		while( 1 )
		{
			//surfacebackbuff1->BltFast
				ddrval = IDirectDrawSurface_BltFast(destImage->Buffer,
					(DWORD)(loc.x),(DWORD)(loc.y), srcImage->Buffer,
					&rcRect, srcImage->BltFastType);
			
			
			if( ddrval == DD_OK )
			{
				//nothing bad happened
				break;
			}
			if( ddrval == DDERR_SURFACELOST )
			{
				//this means that we have to restore the image the user passed in, let them do it.
				//but first, attemp to restore the surface

				if(DDRestoreImage(ddrawstuff, destImage) == DDRAW_OK)
					if(DDRestoreImage(ddrawstuff, srcImage) != DDRAW_OK)
						return DDRAW_SURFACELOST;
				else
					return DDRAW_BAD;
			}
			else //we got unknown error
			{
				dd_check_error(TRUE, ddrval);
					
				assert(!"god damn DDBlitImagetoImage");
				return DDRAW_BAD;
			}
		}	

		return DDRAW_OK;
}

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
PROTECTED RETCODE DDImageDestroy(DDRAWHANDLE ddrawstuff, IMAGEHANDLE image)
{
	if(DDIsImageValid(image))
	{
		if(image->index != INDIVIDUALIMAGE)
		{
			int nextIndex = image->index;
			DDKillImageData(image);
			if(ListDelete(ddrawstuff->Images, image->index) != RETCODE_SUCCESS)
			{ assert(!"Error in DDImageDestroy.  Unable to delete node from the list"); return DDRAW_BAD; }

			int maxImage = ListGetNodeCount(ddrawstuff->Images);
			if(nextIndex < maxImage)
			{
				IMAGEHANDLE thisImage;
				for(int i = nextIndex; i < maxImage; i++)
				{
					thisImage = (IMAGEHANDLE)ListExtract(ddrawstuff->Images, i);
					assert(thisImage);
					thisImage->index = i;
				}
			}
		}
		else
		{
			DDKillImageData(image);
			if(MemFree((void**)&image) != RETCODE_SUCCESS)
			{ assert(!"Error in DDImageDestroy.  Unable to destroy image"); return DDRAW_BAD; }
		}
	}
	
	return DDRAW_OK;
}

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
PROTECTED RETCODE DDKillAllDummyImage(DDRAWHANDLE ddrawstuff)
{
	assert(ddrawstuff);

	int numberofimage = ListGetNodeCount(ddrawstuff->Images);
	IMAGEHANDLE thisImage;
	for(int i = 0; i < numberofimage; i++)
	{
		thisImage = (IMAGEHANDLE)ListExtract(ddrawstuff->Images, i);
		if(DDIsImageDummy(thisImage))
			if(DDImageDestroy(ddrawstuff, thisImage) != DDRAW_OK)
			{ assert(!"Unable to destroy a dumbass image...err I mean dummy image"); return DDRAW_BAD; }

	}
	return DDRAW_OK;
}

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
PROTECTED RETCODE DDImageStretchDIBits(IMAGEHANDLE image, LOCATION loc, SIZE imgSize, const void *bits, const BITMAPINFO *bmi)
{
	HDC SurfDC;
    if (!FAILED(IDirectDrawSurface_GetDC(image->Buffer, &SurfDC)))
    {    StretchDIBits(SurfDC,
            loc.x, loc.y, imgSize.cx, imgSize.cy,
            loc.x, loc.y, imgSize.cx, imgSize.cy,
            bits,
            bmi,
            DIB_RGB_COLORS,
            SRCCOPY);
        
		if(FAILED(IDirectDrawSurface_ReleaseDC(image->Buffer, SurfDC)))
		{ assert(!"Error in DDImageStretchDIBits.  Unable to release DC of image"); return DDRAW_BAD; }
    }
	else
	{ assert(!"Error in DDImageStretchDIBits.  Unable to get DC of image"); return DDRAW_BAD; }

	return DDRAW_OK;
}

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
;	Return:		none
;
**********************************************************/
PROTECTED void DDSetImageColorKey(IMAGEHANDLE image, ULONG low_color_val, ULONG high_color_val)
{
	assert(image);
	if(SetColorKeySrc(image->Buffer, low_color_val, high_color_val) != DDRAW_OK)
				assert(!"Bad SetColorKeySrc, failure in DDCreateImage");

			//
			// Set the bltfast type for later use
			//
			image->BltFastType = DDBLTFAST_SRCCOLORKEY;
}

/***********************************************************************************************/
/***********************************************************************************************/
/* ////////////////////////////////////////////////////////////////
These are Palette Stuff
For 8-bit :(
//////////////////////////////////////////////////////////////// */

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
PUBLIC RETCODE make_surface_palette(DDRAWHANDLE ddrawstuff, PPALETTEENTRY destptPalette)
{
	if(!we_have_ddrawobj(ddrawstuff))
		return DDRAW_BAD;

	//BOOL FILLHERUP = FALSE;
	int total;
	
	//If they pass in a filled palette
	if(destptPalette != NULL)
	{
		//set our palette entry with this blob
		memcpy(ddrawstuff->pPaletteEntry, destptPalette, sizeof(PALETTEENTRY) * 256);
	}
	else
	{
		//
		//  Fill the PALETTEENTRY array with colors.
		//  Only if they passed in a NULL destptPalette
		
		for(total = 1; total < 255; total++)
		{
			ddrawstuff->pPaletteEntry[total].peRed   = (UCHAR)total;
			ddrawstuff->pPaletteEntry[total].peGreen = (UCHAR)total;
			ddrawstuff->pPaletteEntry[total].peBlue  = (UCHAR)total;
			ddrawstuff->pPaletteEntry[total].peFlags = (UCHAR)total;			
		}
	}
	
	//
	//  Create the Palette.
	//
	
	if(dd_check_error(TRUE, IDirectDraw2_CreatePalette(ddrawstuff->ddrawobj2, DDPCAPS_8BIT, 
		ddrawstuff->pPaletteEntry, &ddrawstuff->lpDDPal, NULL)))
		return DDRAW_BAD;
	
	//
	//  Associate the palette to the surface.
	//
	if(ddrawstuff->lpDDSurfacePrimary != NULL)
		if(dd_check_error(TRUE, IDirectDrawSurface_SetPalette(ddrawstuff->lpDDSurfacePrimary,
			ddrawstuff->lpDDPal)))
			return DDRAW_BAD;

	return TRUE;
}

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
PUBLIC RETCODE SetColorKeySrc(LPDIRECTDRAWSURFACE surface, ULONG low_color_val, ULONG high_color_val)
{
	DDSURFACEDESC   surfaceDesc;
	DDCOLORKEY      transparency;
	
	INIT_DDSD(surfaceDesc);
	if(dd_check_error(TRUE, IDirectDrawSurface_GetSurfaceDesc(surface, &surfaceDesc)))
	{ assert(!"Cannot Get Surface Description! Failure in SetTransparency"); return DDRAW_BAD; }
	
	if(dd_check_error(TRUE, IDirectDrawSurface_Lock(surface, NULL, &surfaceDesc, DDLOCK_READONLY | DDLOCK_WAIT, NULL)))
	{ assert(!"Can't get a lock on surface, break out! break out!  Failure in SetTransparency"); return DDRAW_BAD; }
	
	//If the user didn't specify the color for transparency...we make it black
	if(low_color_val <= 0 || high_color_val < low_color_val)
	{
        /*unsigned long   srcColor  = RGB(0, 0, 0);
        
        
        unsigned long   redIndex = 1, greenIndex = 1, blueIndex = 1;
		
        
		
        //DumpSurfacePixelFormat(Hwnd, &surfaceDesc);
		
		//
		//do some alakazam!
		//
        redIndex  <<= (surfaceDesc.ddpfPixelFormat.dwRGBBitCount);
        greenIndex  <<= (surfaceDesc.ddpfPixelFormat.dwRGBBitCount);
        blueIndex  <<= (surfaceDesc.ddpfPixelFormat.dwRGBBitCount);
		
        //
        //  This choice will make black transparent. emmm...ok
        //
		
        srcColor  = RGB(redIndex, greenIndex, blueIndex);
		
        transparency.dwColorSpaceLowValue = srcColor;
        transparency.dwColorSpaceHighValue = srcColor;*/
		IMAGE temp;
		temp.VidMem = (BYTE*)surfaceDesc.lpSurface;
		temp.Pitch = surfaceDesc.lPitch;
		temp.Channel = surfaceDesc.ddpfPixelFormat.dwRGBBitCount>>3;
		temp.ImageSize.cx = surfaceDesc.dwWidth;
		temp.ImageSize.cy = surfaceDesc.dwHeight;
		SETFLAG(temp.Status, DDFLAG_LOCKED);

		transparency.dwColorSpaceLowValue = (ULONG)(*DDGetPixelPtr(&temp, 0, 0));
		transparency.dwColorSpaceHighValue = transparency.dwColorSpaceLowValue;
		
        
	}
	else
	{
		transparency.dwColorSpaceLowValue = low_color_val;
        transparency.dwColorSpaceHighValue = high_color_val;
	}
	
	dd_check_error(TRUE, IDirectDrawSurface_SetColorKey(surface, DDCKEY_SRCBLT, &transparency));
	
	if(dd_check_error(TRUE, IDirectDrawSurface_Unlock(surface, NULL)))
	{ assert(!"Damn, can't unlock surface!  Failure in SetTransparency"); return DDRAW_BAD; }

	return DDRAW_OK;
}

/***********************************************************************************************/
/***********************************************************************************************/
/* ////////////////////////////////////////////////////////////////
Check Error Function
//////////////////////////////////////////////////////////////// */
BOOL dd_check_error(BOOL displaymessage, HRESULT result)
{
	switch(result)
	{
	case DD_OK:
		break;
	case DDERR_INVALIDPARAMS:
		if(displaymessage)
			assert(!"You Got DDERR_INVALIDPARAMS");
		return TRUE;
	case DDERR_DIRECTDRAWALREADYCREATED:
		if(displaymessage)
			assert(!"You Got DDERR_DIRECTDRAWALREADYCREATED");
		return TRUE;
	case DDERR_GENERIC:
		if(displaymessage)
			assert(!"You Got DDERR_GENERIC (There is an undefined error condition)");
		return TRUE;
	case DDERR_INVALIDDIRECTDRAWGUID:
		if(displaymessage)
			assert(!"You Got DDERR_INVALIDDIRECTDRAWGUID");
		return TRUE;
	case DDERR_NODIRECTDRAWHW:
		if(displaymessage)
			assert(!"You Got DDERR_NODIRECTDRAWHW(the driver does not support any hardware)");
		return TRUE;
	case DDERR_OUTOFMEMORY:
		if(displaymessage)
			assert(!"You Got DDERR_OUTOFMEMORY");
		return TRUE;
	case DDERR_INVALIDOBJECT:
		if(displaymessage)
			assert(!"You Got DDERR_INVALIDOBJECT");
		return TRUE;
	case DDERR_INCOMPATIBLEPRIMARY:
		if(displaymessage)
			assert(!"You Got DDERR_INCOMPATIBLEPRIMARY");
		return TRUE;
	case DDERR_INVALIDCAPS:
		if(displaymessage)
			assert(!"You Got DDERR_INVALIDCAPS");
		return TRUE;
	case DDERR_INVALIDPIXELFORMAT:
		if(displaymessage)
			assert(!"You Got DDERR_INVALIDPIXELFORMAT");
		return TRUE;
	case DDERR_NOALPHAHW:
		if(displaymessage)
			assert(!"You Got DDERR_NOALPHAHW");
		return TRUE;
	case DDERR_NOCOOPERATIVELEVELSET:
		if(displaymessage)
			assert(!"You Got DDERR_NOCOOPERATIVELEVELSET");
		return TRUE;
	case DDERR_NOEMULATION:
		if(displaymessage)
			assert(!"You Got DDERR_NOEMULATION");
		return TRUE;
	case DDERR_NOEXCLUSIVEMODE:
		if(displaymessage)
			assert(!"You Got DDERR_NOEXCLUSIVEMODE");
		return TRUE;
	case DDERR_NOFLIPHW:
		if(displaymessage)
			assert(!"You Got DDERR_NOFLIPHW");
		return TRUE;
	case DDERR_NOMIPMAPHW:
		if(displaymessage)
			assert(!"You Got DDERR_NOMIPMAPHW");
		return TRUE;
	case DDERR_NOOVERLAYHW:
		if(displaymessage)
			assert(!"You Got DDERR_NOOVERLAYHW");
		return TRUE;
	case DDERR_NOZBUFFERHW:
		if(displaymessage)
			assert(!"You Got DDERR_NOZBUFFERHW");
		return TRUE;
	case DDERR_OUTOFVIDEOMEMORY:
		if(displaymessage)
			assert(!"You Got DDERR_OUTOFVIDEOMEMORY");
		return TRUE;
	case DDERR_PRIMARYSURFACEALREADYEXISTS:
		if(displaymessage)
			assert(!"You Got DDERR_PRIMARYSURFACEALREADYEXISTS");
		return TRUE;
	case DDERR_UNSUPPORTEDMODE:
		if(displaymessage)
			assert(!"You Got DDERR_UNSUPPORTEDMODE");
		return TRUE;
	case DDERR_EXCEPTION:
		if(displaymessage)
			assert(!"You Got DDERR_EXCEPTION");
		return TRUE;
	case DDERR_INVALIDRECT:
		if(displaymessage)
			assert(!"You Got DDERR_INVALIDRECT");
		return TRUE;
	case DDERR_NOBLTHW:
		if(displaymessage)
			assert(!"You Got DDERR_NOBLTHW");
		return TRUE;
	case DDERR_SURFACEBUSY:
		if(displaymessage)
			assert(!"You Got DDERR_SURFACEBUSY");
		return TRUE;
//DDERR_SURFACELOST  
	case DDERR_UNSUPPORTED:
		if(displaymessage)
			assert(!"You Got DDERR_UNSUPPORTED");
		return TRUE;
//DDERR_WASSTILLDRAWING 

	default:
		if(displaymessage)
			assert(!"You Got Unknown Error :)  You better start crying");
		return TRUE;
	}
	
	return FALSE;
}

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
;	Return:		A struct PMODES
;
**********************************************************/
PROTECTED LISTHANDLE DDGetListModes(DDRAWHANDLE ddrawstuff)
{
	return ddrawstuff->modes;
}

//
// Display stuff
//
void display_mode_opt(DDRAWHANDLE ddrawstuff, HWND hwnd, HINSTANCE hinst)
{
#if MODEACTIVATE
	assert(ddrawstuff);
	DialogBoxParam(hinst, MAKEINTRESOURCE(ID_MODES),hwnd,modeopt, (LPARAM)ddrawstuff);
#endif
}

void display_vidcap(DDRAWHANDLE ddrawstuff, HWND hwnd, HINSTANCE hinst)
{
	assert(ddrawstuff);
	DialogBoxParam(hinst, MAKEINTRESOURCE(IDD_VIDCAP),hwnd,vidstuff, (LPARAM)(&ddrawstuff->vidcap));
}

//useless for now
void display_pixelformat(DDRAWHANDLE ddrawstuff, HWND hwnd, HINSTANCE hinst)
{
	assert(ddrawstuff);
	DialogBoxParam(hinst, MAKEINTRESOURCE(ID_PIXELSTUFF),hwnd,pixels, (LPARAM)ddrawstuff);
}

/***********************************************************************************************/
/***********************************************************************************************/
/* ////////////////////////////////////////////////////////////////
These are methods
//////////////////////////////////////////////////////////////// */

/**********************************************************
;
;	Name:		dd_update_fullscreen
;
;	Purpose:	fullscreen flippin'
;
;	Input:		LPDIRECTDRAWSURFACE  theprimary, LPDIRECTDRAWSURFACE  backbuffer
;
;	Output:		primary surface is updated
;
;	Return:		OK if we did it
;
**********************************************************/
PROTECTED RETCODE dd_update_fullscreen(DDRAWHANDLE ddrawstuff, RECT *windowarea)
{
	assert(ddrawstuff);

	HRESULT ddrval;

	while( 1 )
		{
			ddrval = IDirectDrawSurface_Flip(ddrawstuff->lpDDSurfacePrimary, NULL, DDFLIP_WAIT);
			if( ddrval == DD_OK )
			{
				break;
			}
			else if( ddrval == DDERR_SURFACELOST )
			{
				ddrval = IDirectDrawSurface_Restore(ddrawstuff->lpDDSurfacePrimary);

				if( ddrval != DD_OK )
					return DDRAW_BAD;
			}
			else if( ddrval != DDERR_WASSTILLDRAWING )
			{
				//dd_check_error(TRUE, ddrval);
				break;
			}
		}

	return DDRAW_OK;
}

/**********************************************************
;
;	Name:		dd_update_windowed
;
;	Purpose:	good old backbuffer blitting
;
;	Input:		LPDIRECTDRAWSURFACE  theprimary, LPDIRECTDRAWSURFACE  backbuffer
;
;	Output:		primary surface is updated
;
;	Return:		OK if we did it
;
**********************************************************/
PROTECTED RETCODE dd_update_windowed(DDRAWHANDLE ddrawstuff, RECT *windowarea)
{
	assert(ddrawstuff);
	assert(windowarea); //they better not pass in a null rect

	HRESULT ddrval;
	RECT rcRect;

		rcRect.left = 0;
		rcRect.top = 0;
		rcRect.right = ddrawstuff->backbuffsize.cx;
		rcRect.bottom = ddrawstuff->backbuffsize.cy;
	
	while( 1 )
	{
		ddrval = IDirectDrawSurface_Blt(ddrawstuff->lpDDSurfacePrimary,
			windowarea, ddrawstuff->backbuffer,
			&rcRect, DDBLT_WAIT, FALSE);//DDBLTFAST_SRCCOLORKEY);
		break;
		
		if( ddrval == DD_OK )
		{
			break;
		}
		if( ddrval == DDERR_SURFACELOST )
		{
			ddrval = IDirectDrawSurface_Restore(ddrawstuff->backbuffer);
			
			if( ddrval != DD_OK )
				return DDRAW_BAD;
			else
			{
				ddrval = IDirectDrawSurface_Restore(ddrawstuff->lpDDSurfacePrimary);
			
				if( ddrval != DD_OK )
					return DDRAW_BAD;
			}
		}
		if(dd_check_error(TRUE, ddrval))
		{
			assert(!"Error in dd_update_windowed");
			return DDRAW_BAD;
		}
		if( ddrval != DDERR_WASSTILLDRAWING )
		{
			return DDRAW_BAD;
		}
	}

	return DDRAW_OK;
}

//methods for get color
//in 8-bit, the first param is the only thing used
PROTECTED void SetPixelColor8(UCHAR *buff, int Red, int Green, int Blue)
{ *buff = Red; } //hehe
PROTECTED void SetPixelColor555(UCHAR *buff, int Red, int Green, int Blue)
{ unsigned short * temp = ((unsigned short*)(buff)); *temp = RGB16BIT555(Red,Green,Blue); }
PROTECTED void SetPixelColor565(UCHAR *buff, int Red, int Green, int Blue)
{ unsigned short * temp = ((unsigned short*)(buff)); *temp = RGB16BIT565(Red,Green,Blue); }
PROTECTED void SetPixelColor24(UCHAR *buff, int Red, int Green, int Blue)
{ buff[0] = Blue; buff[1] = Green; buff[2] = Red; }
PROTECTED void SetPixelColor32(UCHAR *buff, int Red, int Green, int Blue)
{ *buff = buff[BLUE_BITMAP_OFFSET] = Blue; buff[GREEN_BITMAP_OFFSET] = Green; buff[RED_BITMAP_OFFSET] = Red; }


// Image ID search function
RETCODE ImageIDSearch(void *thing1, void *thing2)
{
	IMAGEHANDLE image = (IMAGEHANDLE)thing1;

	return (image->ID == (short)thing2);
}

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
PROTECTED RETCODE TransferLoc(PLOCATION loc, PRECT locArea, const RECT otherArea)
{
	SIZE rcSize;

	rcSize.cx = locArea->right - locArea->left;
	rcSize.cy = locArea->bottom - locArea->top;

	//
	// If the location is in negative boundary of the area
	// adjust the rectangle
	//
	if(loc->x < otherArea.left)
	{
		locArea->left -= loc->x;
		loc->x = otherArea.left;
	}
	if(loc->y < otherArea.top)
	{
		locArea->top -= loc->y;
		loc->y = otherArea.top;
	}
	//If the rectangle to be blitted is a little bit out of boundary...
	//subtract it by that amount
	if(loc->x + rcSize.cx > otherArea.right)
		locArea->right -= (loc->x + rcSize.cx) - otherArea.right;
	if(loc->y + rcSize.cy > otherArea.bottom)
		locArea->bottom -= (loc->y + rcSize.cy) - otherArea.bottom;
	
	//Since we fooled around with the rectange, check if it is invalid, if so, return
	if(locArea->left >= locArea->right)
		return DDRAW_OUTOFBOUND; //we will just assume we suceeded
	else if(locArea->top >= locArea->bottom)
		return DDRAW_OUTOFBOUND; //we will just assume we suceeded

	return DDRAW_OK;
}

/***********************************************************************************************/
/***********************************************************************************************/
/* ////////////////////////////////////////////////////////////////
These are the callbacks
//////////////////////////////////////////////////////////////// */
						 
BOOL PASCAL enumcallback(GUID * guid, LPSTR driverdesc, LPSTR drivername, LPVOID context)
{
	DDRAWHANDLE ddrawstuff = (DDRAWHANDLE)context; 

	GUIDS newguid;

	
	if(strcmp(driverdesc, "\0") != 0)
		strcpy(newguid.driverdesc, driverdesc);
	else
		return FALSE; //that means there is no driver, yes?

	if(strcmp(drivername, "\0") != 0)
		strcpy(newguid.drivername, drivername);
	else
		return FALSE; //as well as this, correct?

	newguid.guid = guid;
	
	if(ListAppend (ddrawstuff->guidlist, &newguid) != RETCODE_SUCCESS)
		return FALSE; //shit!
	
	return TRUE;
}

#if MODEACTIVATE
BOOL PASCAL enumdisplaycallback(LPDDSURFACEDESC mode, LPVOID context)
{
	DDRAWHANDLE ddrawstuff = (DDRAWHANDLE)context;
		
		if(ListGetNodeCount(ddrawstuff->modes) < ListGetMaxNodes(ddrawstuff->modes))	//if the head is not null
		{
			ListAppend(ddrawstuff->modes, mode);
		}
		else
			return FALSE;
		
	
	return TRUE;	
}
#endif

/***********************************************************************************************/
/***********************************************************************************************/
/* ////////////////////////////////////////////////////////////////
These are the dialog callbacks....
//////////////////////////////////////////////////////////////// */

BOOL CALLBACK vidopt(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LONG index;
	static DDRAWHANDLE pdditems;
	GUID *holder;
	switch(message)
	{
	case WM_INITDIALOG:		//if we're starting up...
		{
			pdditems = (DDRAWHANDLE)lparam;
			int maxNode = ListGetNodeCount(pdditems->guidlist);
			PGUIDS theguid;

			for(int i = 0; i < maxNode; i++)
			{
				theguid = (PGUIDS)ListExtract(pdditems->guidlist, i);
				index = SendMessage(GetDlgItem(hwnd,IDC_COMBO1),CB_ADDSTRING,0,(LPARAM)theguid->driverdesc);

				SendMessage(GetDlgItem(hwnd,IDC_COMBO1),CB_SETITEMDATA,index,(LPARAM)theguid->guid);
			}
				
				ShowWindow(hwnd,SW_SHOW);
				SendMessage(GetDlgItem(hwnd,IDC_COMBO1),CB_SETCURSEL,index,0);
			return FALSE;
		}
	case WM_COMMAND:
		switch (LOWORD(wparam))
		{
		case ID_ALRIGHTY:  //we have chosen a card
			index = SendMessage(GetDlgItem(hwnd,IDC_COMBO1),CB_GETCURSEL,0,0);
			holder = (GUID *)SendMessage(GetDlgItem(hwnd,IDC_COMBO1),CB_GETITEMDATA,index,0);

			ddraw_create_obj(pdditems, holder);

			pdditems->current_adapter = holder;
			EndDialog(hwnd, TRUE);
			return TRUE;
		case ID_NO: //if no, then go with the primary
			ddraw_create_obj(pdditems, NULL);

			EndDialog(hwnd, FALSE);
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

#if MODEACTIVATE
BOOL CALLBACK modeopt(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	LONG index;
	LPDDSURFACEDESC pddsd;
	char buff[50];
	static DDRAWHANDLE pdditems;
	switch(message)
	{
	case WM_INITDIALOG:
		{
		pdditems = (DDRAWHANDLE)lparam;
		//modewalk = pdditems->modes; //(PMODES)lparam;//ddstuff.modes;

		int maxNode = ListGetNodeCount(pdditems->modes);
		for(int i = 0; i < maxNode; i++)
		{
			pddsd = (DDSURFACEDESC *)ListExtract(pdditems->modes, i);
			
				wsprintf( buff, "%dx%dx%dx%d", 
					pddsd->dwWidth,
					pddsd->dwHeight,
					pddsd->ddpfPixelFormat.dwRGBBitCount,
					pddsd->dwRefreshRate );
				index = SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
				
				SendMessage(GetDlgItem(hwnd, ID_LIST), LB_SETITEMDATA, index, ( LPARAM )pddsd);
		}
		SendMessage(GetDlgItem(hwnd,ID_LIST), LB_SETCURSEL, index, 0);
		ShowWindow(hwnd,SW_SHOW);
		return FALSE;
		}

	case WM_COMMAND:
		switch(LOWORD(wparam))
		{
			case ID_LIST:
				switch(HIWORD(wparam))
				{
				case LBN_SELCHANGE:
					index = SendMessage(GetDlgItem(hwnd, ID_LIST), LB_GETCURSEL, 0, 0);
					break;
				}
				break;

				case IDOK:
					{
						//
						// Set up the mode!
						// We will also modify the current mode data
						//
						index = SendMessage (GetDlgItem(hwnd, ID_LIST), LB_GETCURSEL, 0, 0);
						pddsd = (LPDDSURFACEDESC)SendMessage(GetDlgItem(hwnd, ID_LIST), LB_GETITEMDATA, index, 0);
						set_our_display_mode(pdditems, pddsd->dwWidth, pddsd->dwHeight, pddsd->ddpfPixelFormat.dwRGBBitCount, pddsd->dwRefreshRate);

						//
						//
						//						
						EndDialog(hwnd, FALSE);
						return TRUE;
					}

			case IDNO:
				EndDialog(hwnd, FALSE);
				return TRUE;
				
			case IDRESTORE:
				{
					restore_display_mode(pdditems);
					
					EndDialog(hwnd, FALSE);
					return TRUE;
				}
		}
		return FALSE;

	case WM_DESTROY:
		EndDialog(hwnd, FALSE);
		return TRUE;
	}
	
	return FALSE;
}
#endif

BOOL CALLBACK vidstuff(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	char buff[100];

	switch(message)
	{
	case WM_INITDIALOG:
		//print all 50+ items!!!
		wsprintf(buff, "Capabilities of surface wanted: %u", ((LPDDCAPS)lparam)->ddsCaps.dwCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "destination rectangle alignment: %u", ((LPDDCAPS)lparam)->dwAlignBoundaryDest);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "source rectangle alignment: %u", ((LPDDCAPS)lparam)->dwAlignBoundarySrc);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "dest rectangle byte size: %u", ((LPDDCAPS)lparam)->dwAlignSizeDest);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "source rectangle byte size: %u", ((LPDDCAPS)lparam)->dwAlignSizeSrc);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "stride alignment: %u", ((LPDDCAPS)lparam)->dwAlignStrideAlign);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "AlphaBltConstBitDepths: %u", ((LPDDCAPS)lparam)->dwAlphaBltConstBitDepths);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "AlphaBltPixelBitDepths: %u", ((LPDDCAPS)lparam)->dwAlphaBltPixelBitDepths);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "AlphaBltSurfaceBitDepths: %u", ((LPDDCAPS)lparam)->dwAlphaBltSurfaceBitDepths);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "AlphaOverlayConstBitDepths: %u", ((LPDDCAPS)lparam)->dwAlphaOverlayConstBitDepths);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "AlphaOverlayPixelBitDepths: %u", ((LPDDCAPS)lparam)->dwAlphaOverlayPixelBitDepths);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "AlphaOverlaySurfaceBitDepths: %u", ((LPDDCAPS)lparam)->dwAlphaOverlaySurfaceBitDepths);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		
		wsprintf(buff, "Driver Specific Capabilities: %u", ((LPDDCAPS)lparam)->dwCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Yet More Driver Specific Capabilities: %u", ((LPDDCAPS)lparam)->dwCaps2);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "color key capabilities of the surface: %u", ((LPDDCAPS)lparam)->dwCKeyCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "current number of video ports used: %u", ((LPDDCAPS)lparam)->dwCurrVideoPorts);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "current number of visible overlays: %u", ((LPDDCAPS)lparam)->dwCurrVisibleOverlays);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "alpha driver specific capabilities: %u", ((LPDDCAPS)lparam)->dwFXAlphaCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "driver specific stretching and effects capabilities: %u", ((LPDDCAPS)lparam)->dwFXCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "MaxHwCodecStretch: %u", ((LPDDCAPS)lparam)->dwMaxHwCodecStretch);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "MaxLiveVideoStretch: %u", ((LPDDCAPS)lparam)->dwMaxLiveVideoStretch);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "MaxOverlayStretch: %u", ((LPDDCAPS)lparam)->dwMaxOverlayStretch);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Maximum number of usable video ports: %u", ((LPDDCAPS)lparam)->dwMaxVideoPorts);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Maximum number of visible overlays: %u", ((LPDDCAPS)lparam)->dwMaxVisibleOverlays);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "MinHwCodecStretch: %u", ((LPDDCAPS)lparam)->dwMinHwCodecStretch);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "MinLiveVideoStretch: %u", ((LPDDCAPS)lparam)->dwMinLiveVideoStretch);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "MinOverlayStretch: %u", ((LPDDCAPS)lparam)->dwMinOverlayStretch);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "NLVBCaps: %u", ((LPDDCAPS)lparam)->dwNLVBCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "NLVBCaps2: %u", ((LPDDCAPS)lparam)->dwNLVBCaps2);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "NLVBCKeyCaps: %u", ((LPDDCAPS)lparam)->dwNLVBCKeyCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "NLVBFXCaps: %u", ((LPDDCAPS)lparam)->dwNLVBFXCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "NLVBRops: %u", ((LPDDCAPS)lparam)->dwNLVBRops);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "number of four CC codes: %u", ((LPDDCAPS)lparam)->dwNumFourCCCodes);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Palette Capabilities: %u", ((LPDDCAPS)lparam)->dwPalCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Reserved1: %u", ((LPDDCAPS)lparam)->dwReserved1);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Reserved2: %u", ((LPDDCAPS)lparam)->dwReserved2);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Reserved3: %u", ((LPDDCAPS)lparam)->dwReserved3);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "ROPS supported: %u", ((LPDDCAPS)lparam)->dwRops);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Size of vidcap struct: %u", ((LPDDCAPS)lparam)->dwSize);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SSBCaps: %u", ((LPDDCAPS)lparam)->dwSSBCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SSBCKeyCaps: %u", ((LPDDCAPS)lparam)->dwSSBCKeyCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SSBFXCaps: %u", ((LPDDCAPS)lparam)->dwSSBFXCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SSBRops: %u", ((LPDDCAPS)lparam)->dwSSBRops);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SVBCaps: %u", ((LPDDCAPS)lparam)->dwSVBCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SVBCaps2: %u", ((LPDDCAPS)lparam)->dwSVBCaps2);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SVBCKeyCaps: %u", ((LPDDCAPS)lparam)->dwSVBCKeyCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SVBFXCaps: %u", ((LPDDCAPS)lparam)->dwSVBFXCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SVBRops: %u", ((LPDDCAPS)lparam)->dwSVBRops);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "SVCaps: %u", ((LPDDCAPS)lparam)->dwSVCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "amt. of free video memory: %u", ((LPDDCAPS)lparam)->dwVidMemFree);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "Total amount of video memory: %u", ((LPDDCAPS)lparam)->dwVidMemTotal);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "VSBCaps: %u", ((LPDDCAPS)lparam)->dwVSBCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "VSBCKeyCaps: %u", ((LPDDCAPS)lparam)->dwVSBCKeyCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "VSBFXCaps: %u", ((LPDDCAPS)lparam)->dwVSBFXCaps);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "VSBRops: %u", ((LPDDCAPS)lparam)->dwVSBRops);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		wsprintf(buff, "ZBufferBitDepths: %u", ((LPDDCAPS)lparam)->dwZBufferBitDepths);
		SendMessage(GetDlgItem(hwnd, ID_VIDSTUFF), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		
		ShowWindow(hwnd,SW_SHOW);
		return FALSE;

	case WM_COMMAND:
		switch(LOWORD(wparam))
		{
		case IDOK:
			EndDialog(hwnd, FALSE);
			return TRUE;
		}
		return FALSE;
	}

	return FALSE;
}

BOOL CALLBACK pixels(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	char buff[100];
	DDSURFACEDESC ddsd;
	ddrawdude *mydude;

	switch(message)
	{
	case WM_INITDIALOG:
		mydude = (DDRAWHANDLE)lparam;

		ddsd = get_surfacedesc(mydude);

		wsprintf(buff, "dwAlphaBitDepth(pixel format): %u", ddsd.ddpfPixelFormat.dwAlphaBitDepth);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwBBitMask: %u", ddsd.ddpfPixelFormat.dwBBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwFlags: %u", ddsd.ddpfPixelFormat.dwFlags);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwFourCC: %u", ddsd.ddpfPixelFormat.dwFourCC);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwGBitMask: %u", ddsd.ddpfPixelFormat.dwGBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwRBitMask: %u", ddsd.ddpfPixelFormat.dwRBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwRGBAlphaBitMask: %u", ddsd.ddpfPixelFormat.dwRGBAlphaBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwRGBBitCount: %u", ddsd.ddpfPixelFormat.dwRGBBitCount);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwRGBZBitMask: %u", ddsd.ddpfPixelFormat.dwRGBZBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwSize: %u", ddsd.ddpfPixelFormat.dwSize);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwUBitMask: %u", ddsd.ddpfPixelFormat.dwUBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwVBitMask: %u", ddsd.ddpfPixelFormat.dwVBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwYBitMask: %u", ddsd.ddpfPixelFormat.dwYBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwYUVAlphaBitMask: %u", ddsd.ddpfPixelFormat.dwYUVAlphaBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwYUVBitCount: %u", ddsd.ddpfPixelFormat.dwYUVBitCount);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwYUVZBitMask: %u", ddsd.ddpfPixelFormat.dwYUVZBitMask);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);
		wsprintf(buff, "dwZBufferBitDepth: %u", ddsd.ddpfPixelFormat.dwZBufferBitDepth);
		SendMessage(GetDlgItem(hwnd, ID_LIST), LB_ADDSTRING, 0, (LONG)(LPSTR)buff);

		ShowWindow(hwnd,SW_SHOW);
		return FALSE;

		case WM_COMMAND:
			switch(LOWORD(wparam))
			{
			case IDOK:
				EndDialog(hwnd, FALSE);
				return TRUE;
			}
		return FALSE;
	}

	return FALSE;
}
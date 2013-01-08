#include "game.h"
#include "Graphics\primitive.h"
#include "mywin.h"

PRIVATE GAMELOOP GameLoop[] = {MainLoop, InGameLoop, HighScoreLoop};
PRIVATE PGAMEDATA theGame;
PRIVATE char AsciiStuff[91][2];

PRIVATE int ballsizes[NUMTYPES] = {64, 32, 16, 8, 4};

#define CHECKPADDING 0
#define REFLECTPADDING 1.5
#define HACKDISPLACEMENT 2.5

PRIVATE void init_AsciiStuff()
{
	strcpy(AsciiStuff[kCodeA], "A");
	strcpy(AsciiStuff[kCodeB], "B");
	strcpy(AsciiStuff[kCodeC], "C");
	strcpy(AsciiStuff[kCodeD], "D");
	strcpy(AsciiStuff[kCodeE], "E");
	strcpy(AsciiStuff[kCodeF], "F");
	strcpy(AsciiStuff[kCodeG], "G");
	strcpy(AsciiStuff[kCodeH], "H");
	strcpy(AsciiStuff[kCodeI], "I");
	strcpy(AsciiStuff[kCodeJ], "J");
	strcpy(AsciiStuff[kCodeK], "K");
	strcpy(AsciiStuff[kCodeL], "L");
	strcpy(AsciiStuff[kCodeM], "M");
	strcpy(AsciiStuff[kCodeN], "N");
	strcpy(AsciiStuff[kCodeO], "O");
	strcpy(AsciiStuff[kCodeP], "P");
	strcpy(AsciiStuff[kCodeQ], "Q");
	strcpy(AsciiStuff[kCodeR], "R");
	strcpy(AsciiStuff[kCodeS], "S");
	strcpy(AsciiStuff[kCodeT], "T");
	strcpy(AsciiStuff[kCodeU], "U");
	strcpy(AsciiStuff[kCodeV], "V");
	strcpy(AsciiStuff[kCodeW], "W");
	strcpy(AsciiStuff[kCodeX], "X");
	strcpy(AsciiStuff[kCodeY], "Y");
	strcpy(AsciiStuff[kCodeZ], "Z");

	strcpy(AsciiStuff[kCode0], "0");
	strcpy(AsciiStuff[kCode1], "1");
	strcpy(AsciiStuff[kCode2], "2");
	strcpy(AsciiStuff[kCode3], "3");
	strcpy(AsciiStuff[kCode4], "4");
	strcpy(AsciiStuff[kCode5], "5");
	strcpy(AsciiStuff[kCode6], "6");
	strcpy(AsciiStuff[kCode7], "7");
	strcpy(AsciiStuff[kCode8], "8");
	strcpy(AsciiStuff[kCode9], "9");
}

RETCODE init_game(VTYPE graphicsmode, char *gameCfgFile, DWORD gameDelay, DWORD frameDelay, Pdisplaymode mode)
{
	srand( (unsigned)time( NULL ) );

	if(MemInit() != RETCODE_SUCCESS)
	{ MESSAGE_BOX("Error processing MemInit: init_game failed","Error"); destroy_game(); return RETCODE_FAILURE; }

	if (FileInit () != RETCODE_SUCCESS)	// Ascertain that file initialization succeeds
	{ MESSAGE_BOX("Error processing FileInit: init_game failed","Error"); destroy_game(); return RETCODE_FAILURE;	}

	//VDTYPE_FULLSCREEN,		//use POP UP window
    //VDTYPE_WINDOWED,		//use overlapped window
	//VDTYPE_FULLSCRNCLIPPER	//use POP UP window
	if(GraphicsInit(G_hwnd, G_hinstance, graphicsmode, mode) != RETCODE_SUCCESS)
	{ MESSAGE_BOX("Error processing GraphicsInit: init_game failed","Error"); destroy_game(); return RETCODE_FAILURE; }

	//go create the game data baby! yeah!
	if(MemAlloc((void**)&theGame, sizeof(GAMEDATA)) != RETCODE_SUCCESS)
	{ MESSAGE_BOX("Damn, unable to allocate game, oh well","Oops!"); destroy_game(); return RETCODE_FAILURE; }

	//initialize the timer
	theGame->frameDelay = frameDelay;
	theGame->gameDelay = gameDelay;
	theGame->frameTimer = TimerInit(); if(!theGame->frameTimer) {MESSAGE_BOX("Unable to create the frame timer!", "D'oh!"); destroy_game(); return RETCODE_FAILURE; }
	theGame->gameTimer = TimerInit(); if(!theGame->gameTimer) {MESSAGE_BOX("Unable to create the game timer!", "D'oh!"); destroy_game(); return RETCODE_FAILURE; }

	//load up the game config and do some stuff
	if(load_gamecfg(gameCfgFile) != RETCODE_SUCCESS)
	{ MESSAGE_BOX("Error initializing, damn!", "Error"); destroy_game(); return RETCODE_FAILURE; }

	//do this once...for the sake of high score
	init_AsciiStuff();

	//the keyboard timer
	theGame->keyboardTimer = TimerInit(); if(!theGame->keyboardTimer) {MESSAGE_BOX("Unable to create the keyboard timer!", "D'oh!"); destroy_game(); return RETCODE_FAILURE; }

	//the in-game time ticker
	theGame->ingameTimer = TimerInit(); if(!theGame->ingameTimer) {MESSAGE_BOX("Unable to create the game timer!", "D'oh!"); destroy_game(); return RETCODE_FAILURE; }
	theGame->ingameDelay = INGAMETIMEDELAY;

	return RETCODE_SUCCESS;
}

PRIVATE RETCODE load_gamecfg(char *gameCfgFile)
{
	assert(theGame);
	FILEHANDLE theFile = NULL;
	theFile = FileOpenFile (gameCfgFile, FILEREAD | FILETEXT);
	if(!theFile)
	{ MESSAGE_BOX("Unable to open file: load_gamecfg failed", "Zoink!"); return RETCODE_FAILURE; }

	bool bLoading = false;
	char buff[MAXCHARBUFF];
	char pathBuff[MAXCHARBUFF];

	do
	{
		fscanf(theFile, "%s\n", buff);

		if(strcmp(buff, "START") == 0) //start loading stuff if we found the START
			bLoading = true;
		else if(bLoading)
		{
			if(strcmp(buff, "[FONT]") == 0)
			{
				int r,g,b;
				fscanf(theFile, "font=%s\n", pathBuff);
				fscanf(theFile, "r=%d g=%d b=%d\n", &r, &g, &b);
				theGame->theFont = FontCreate(pathBuff, TEXTCOLOR(r, g, b));
				if(!theGame->theFont)
				{ MESSAGE_BOX("Unable to load font: load_gamecfg failed", "fatal error"); FileCloseFile (theFile); return RETCODE_FAILURE; }
			}
			else if(strcmp(buff, "[IMAGE]") == 0)
			{
				short numImages, numImageBuffer;
				short imageID;
				int isColorKey;

				fscanf(theFile, "numImageBuffer=%hd\n", &numImageBuffer);
				if(GraphicsMakeImageList(numImageBuffer) != RETCODE_SUCCESS)
				{ MESSAGE_BOX("Unable to make image list: load_gamecfg failed", "fatal error"); FileCloseFile (theFile); return RETCODE_FAILURE; }

				fscanf(theFile, "numImages=%hd\n", &numImages);

				bool bUseColorKey;
				for(int i = 0; i < numImages; i++)
				{
					fscanf(theFile, "path=%s imageID=%hd isColorKey=%d\n", pathBuff, &imageID, &isColorKey);

					if(isColorKey)
						bUseColorKey = true;
					else
						bUseColorKey = false;

					if(GraphicsLoadImage(pathBuff, imageID, bUseColorKey) != RETCODE_SUCCESS)
					{ MESSAGE_BOX("Unable to load image: load_gamecfg failed", "fatal error"); FileCloseFile (theFile); return RETCODE_FAILURE; }
				}
			}
			else if(strcmp(buff, "[LAYER]") == 0)
			{
				AMOUNT numLayers, spriteReserve;
				INDEX index;

				int value;

				short * SpritesPerLayer;

				OFFSET offset, scroll;

				fscanf (theFile, "numLayers=%d\n",	 &numLayers);
				fscanf (theFile, "spriteReserve=%d\n", &spriteReserve);

				MemAlloc ((void **) &SpritesPerLayer, sizeof(short) * numLayers);

				for (index = 0; index < numLayers; ++index)
				{
					fscanf (theFile, "%d\n", &value);
					SpritesPerLayer [index] = (short) value;
				}

				GraphicsMakeLayerList ((short) numLayers, (short) spriteReserve, SpritesPerLayer, false);

				MemFree ((void **) &SpritesPerLayer);

				for (index = 0; index < numLayers; ++index)
				{
					fscanf (theFile, "Offset x=%d y=%d\n", &offset.x, &offset.y);
					fscanf (theFile, "Scroll x=%d y=%d\n", &scroll.x, &scroll.y);

					GraphicsSetLayerOffset ((short) index, offset.x, offset.y);
					GraphicsSetLayerScroll ((short) index, scroll.x, scroll.y);
				}
			}
			else if(strcmp(buff, "[SPRITE]") == 0)
			{
				short theSpriteID, theImageID;

				AMOUNT numSprites, numColumns;
				AMOUNT numFrames, numStates;
				INDEX index, loop;

				LOCATION loc;
				OFFSET offset;
				SIZE size;

				PFRAME frame;

				Dword newDelay;

				fscanf (theFile, "numSprites=%d\n", &numSprites);

				int layer = GraphicsGetReserveLayerIndex();

				for (index = 0; index < numSprites; ++index)
				{
					fscanf (theFile, "imageID=%hd\n",			   &theImageID);
					fscanf (theFile, "SpriteID=%hd\n",	   &theSpriteID);
					fscanf (theFile, "numColumns=%d\n",	   &numColumns);
					fscanf (theFile, "Location x=%d y=%d\n", &loc.x, &loc.y);
					fscanf (theFile, "Offset x=%d y=%d\n",   &offset.x, &offset.y);
					fscanf (theFile, "Size x=%d y=%d\n",	   &size.cx, &size.cy);
					fscanf (theFile, "Delay=%d\n",		   &newDelay);
					fscanf (theFile, "numFrames=%d\n",	   &numFrames);
					fscanf (theFile, "numStates=%d\n",	   &numStates);

					MemAlloc ((void **) &frame, numStates * sizeof(FRAME));

					for (loop = 0; loop < numStates; ++loop)
						fscanf (theFile, "Frame firstFrame=%hd lastFrame=%hd\n", &frame [loop].firstframe, &frame [loop].lastframe);

					if(GraphicsCreateSprite (theImageID, layer, theSpriteID, loc, (short) numColumns,
										  offset, size, newDelay, (short) numFrames, (short) numStates,
										  frame) == NULL)
					{ MESSAGE_BOX("Unable to create sprite: load_gamecfg failed", "fatal error"); MemFree ((void **) &frame); FileCloseFile (theFile); return RETCODE_FAILURE; }

					MemFree ((void **) &frame);

					//GraphicsCopySprite (SpriteID, 0, loc);
				}
			}
			else if(strcmp(buff, "[PLAYER]") == 0)
			{

				fscanf(theFile, "SpriteID=%hd\n", &theGame->Ghali.SpriteID);
				fscanf(theFile, "Layer=%hd\n", &theGame->Ghali.layer);

				fscanf(theFile, "moveLeftState=%hd\n", &theGame->Ghali.moveLeftState);
				fscanf(theFile, "moveStandingLeftState=%hd\n", &theGame->Ghali.moveStandingLeftState);
				fscanf(theFile, "moveRightState=%hd\n", &theGame->Ghali.moveRightState);
				fscanf(theFile, "moveStandingRightState=%hd\n", &theGame->Ghali.moveStandingRightState);
				fscanf(theFile, "moveFrontState=%hd\n", &theGame->Ghali.moveFrontState);
				fscanf(theFile, "moveStandingFrontState=%hd\n", &theGame->Ghali.moveStandingFrontState);
				fscanf(theFile, "moveBackState=%hd\n", &theGame->Ghali.moveBackState);
				fscanf(theFile, "moveStandingBackState=%hd\n", &theGame->Ghali.moveStandingBackState);
				fscanf(theFile, "moveDeathState=%hd\n", &theGame->Ghali.moveDeathState);
				fscanf(theFile, "moveWinState=%hd\n", &theGame->Ghali.moveWinState);

				//set up the circle collision of the player
				fscanf(theFile, "CircleCollisionSize=%d\n", &theGame->Ghali.circleCollision.Radius);
				fscanf(theFile, "numLife=%d\n", &theGame->Ghali.maxLife);
				theGame->Ghali.currentnumLife = theGame->Ghali.maxLife;

				fscanf(theFile, "PlayerSpeed=%d\n", &theGame->Ghali.PlayerSpd);
				fscanf(theFile, "hookSpeed=%d\n", &theGame->Ghali.HookShotSpd);
				fscanf(theFile, "numHookPolyPts=%d\n", &theGame->Ghali.HookShot.numPts);
				fscanf(theFile, "r=%d g=%d b=%d\n", &theGame->Ghali.HookShot.r, &theGame->Ghali.HookShot.g, &theGame->Ghali.HookShot.b);
				//initialize the hook shot polygon

				//make two copies
				if(MemAlloc((void**)&theGame->Ghali.HookPts, theGame->Ghali.HookShot.numPts*sizeof(LOCATION)) != RETCODE_SUCCESS)
				{ MESSAGE_BOX("Error initializing hookshot, darn: load_gamecfg failed", "Error"); destroy_game(); return RETCODE_FAILURE; }
				if(MemAlloc((void**)&theGame->Ghali.HookShot.pts, theGame->Ghali.HookShot.numPts*sizeof(LOCATION)) != RETCODE_SUCCESS)
				{ MESSAGE_BOX("Error initializing hookshot, darn: load_gamecfg failed", "Error"); destroy_game(); return RETCODE_FAILURE; }

				for(int i = 0; i < theGame->Ghali.HookShot.numPts; i++)
					fscanf(theFile, "location offset x=%d y=%d\n", &theGame->Ghali.HookPts[i].x, &theGame->Ghali.HookPts[i].y);
			}
			else if(strcmp(buff, "[MAINMENU]") == 0)
			{
				fscanf(theFile, "BackgroundSpriteID=%hd\n", &theGame->theMenu.BackgroundSpriteID);
				fscanf(theFile, "BackgroundLayer=%hd\n", &theGame->theMenu.BackgroundLayer);

				fscanf(theFile, "ButtonLayer=%hd\n", &theGame->theMenu.ButtonLayer);

				fscanf(theFile, "PlayBtnSpriteID=%hd\n", &theGame->theMenu.buttons[PLAYBUTTON].ID);
				fscanf(theFile, "PlayBtnLoc x=%d y=%d\n", &theGame->theMenu.buttons[PLAYBUTTON].BtnLoc.x, &theGame->theMenu.buttons[PLAYBUTTON].BtnLoc.y);
				
				fscanf(theFile, "HighScoreBtnSpriteID=%hd\n", &theGame->theMenu.buttons[HIGHSCOREBUTTON].ID);
				fscanf(theFile, "HighScoreBtnLoc x=%d y=%d\n", &theGame->theMenu.buttons[HIGHSCOREBUTTON].BtnLoc.x, &theGame->theMenu.buttons[HIGHSCOREBUTTON].BtnLoc.y);
				
				fscanf(theFile, "ExitBtnSpriteID=%hd\n", &theGame->theMenu.buttons[EXITBUTTON].ID);
				fscanf(theFile, "ExitBtnLoc x=%d y=%d\n", &theGame->theMenu.buttons[EXITBUTTON].BtnLoc.x, &theGame->theMenu.buttons[EXITBUTTON].BtnLoc.y);
			}
			else if(strcmp(buff, "[BACKGROUNDFX]") == 0)
			{
				fscanf(theFile, "SpriteID=%hd\n", &theGame->FXspriteID);
				
				SPRITEHANDLE theSprite;
				theSprite = GraphicsGetSprite(theGame->FXspriteID);
				if(!theSprite)
				{ MESSAGE_BOX("Unable to grab sprite ID for FX: load_gamecfg failed", "Zoink!"); FileCloseFile (theFile); return RETCODE_FAILURE; }

				int FXtype;
				int maxParam;
				long initarray[MAXFXPARAM];
				fscanf(theFile, "FXtype=%d\n", &FXtype);

				//get the maximum params
				fscanf(theFile, "FXmaxparam=%d\n", &maxParam);
				//then get the rest
				for(int i = 0; i < maxParam; i++)
					fscanf(theFile, "%d\n", &initarray[i]);

				SpriteQueryFX(theSprite, FXtype, initarray);
			}
			else if(strcmp(buff, "[HIGHSCORE]") == 0)
			{
				fscanf(theFile, "high score file=%s\n", theGame->theScores.scorefilepath);
				
				fscanf(theFile, "BackGroundSpriteID=%hd\n", &theGame->theScores.BackgroundSpriteID);
				fscanf(theFile, "BackgroundLayer=%hd\n", &theGame->theScores.BackgroundLayer);

				FILEHANDLE scrFile = NULL;
				scrFile = FileOpenFile (theGame->theScores.scorefilepath, FILEREAD | FILETEXT);
				if(!scrFile)
				{ MESSAGE_BOX("Unable to open file: load_gamecfg failed", "Zoink!"); FileCloseFile (theFile); return RETCODE_FAILURE; }

				for(int i = 0; i < MAXSCORE; i++)
				{
					fscanf(scrFile, "%s\n", theGame->theScores.highscore[i].playerName);
					fscanf(scrFile, "%d\n", &theGame->theScores.highscore[i].score);
				}

				FileCloseFile(scrFile);
			}
			else if(strcmp(buff, "[LEVELS]") == 0)
			{
				fscanf(theFile, "NumLevels=%d\n", &theGame->maxLevel);
				MemAlloc((void**)&theGame->levelpaths, sizeof(char*)*theGame->maxLevel);
				
				for(int i = 0; i < theGame->maxLevel; i++)
				{
					fscanf(theFile, "%s\n", pathBuff);
					MemAlloc((void**)&theGame->levelpaths[i], sizeof(char)*(strlen(pathBuff)+1));
					strcpy(theGame->levelpaths[i], pathBuff);
				}
			}
			else if(strcmp(buff, "END") == 0)
				bLoading = false;
		}
		
	} while(bLoading);

	FileCloseFile (theFile);
	return RETCODE_SUCCESS;
}

void destroy_game()
{
	//destroy the game
	if(theGame)
	{
		//destroy the timer
		if(theGame->frameTimer)
			TimerTerm(theGame->frameTimer);
		if(theGame->gameTimer)
			TimerTerm(theGame->gameTimer);
		if(theGame->keyboardTimer)
			TimerTerm(theGame->keyboardTimer);
		if(theGame->ingameTimer)
			TimerTerm(theGame->ingameTimer);
		
		destroy_level_data();

		if(theGame->theFont)
			FontDestroy(theGame->theFont);
		if(theGame->levelpaths)
		{
			for(int i = 0; i < theGame->maxLevel; i++)
			{
				MemFree((void**)&theGame->levelpaths[i]);
			}

			MemFree((void**)&theGame->levelpaths);
		}

		if(theGame->Ghali.HookShot.pts)
			MemFree((void**)&theGame->Ghali.HookShot.pts);
		if(theGame->Ghali.HookPts)
			MemFree((void**)&theGame->Ghali.HookPts);

		if(MemFree((void**)&theGame) != RETCODE_SUCCESS)
			MESSAGE_BOX("Failed to destroy game data", "Oops!");
	}

	GraphicsTerm();
	FileTerm();
	MemTerm();
}

PRIVATE void destroy_level_data()
{
	if(theGame->polygon)
	{
		int maxnode = ListGetNodeCount(theGame->polygon);
		PPOLYGON thisPoly;
		for(int i = 0; i < maxnode; i++)
		{
			thisPoly = (PPOLYGON)ListExtract(theGame->polygon, i);
			MemFree((void**)&thisPoly->pts);
		}

		ListDestroy(&theGame->polygon);
	}
	if(theGame->circle)
		ListDestroy(&theGame->circle);

	if(theGame->circleImg)
	{
		for(int j = 0; j < theGame->numCircleLoaded; j++)
		{
			for(int k = 0; k < NUMTYPES; k++)
				SpriteDestroy(theGame->circleImg[j].theimg[k]);
		}

		MemFree((void**)&theGame->circleImg);
	}
}

PRIVATE RETCODE load_level_data(char *path)
{
	FILEHANDLE theFile = NULL;
	theFile = FileOpenFile (path, FILEREAD | FILETEXT);
	if(!theFile)
	{ MESSAGE_BOX("Unable to open level", "Zoink!"); return RETCODE_FAILURE; }

	bool bLoading = false;
	char buff[MAXCHARBUFF];

	do
	{
		fscanf(theFile, "%s\n", buff);

		if(strcmp(buff, "START") == 0) //start loading stuff if we found the START
			bLoading = true;
		else if(bLoading)
		{
			if(strcmp(buff, "[BACKGROUND]") == 0)
			{
				char imgpath[MAXCHARBUFF];
				fscanf(theFile, "Path=%s\n", imgpath);
				LOCATION imgloc = {BOUNDARYOFFSIZE, BOUNDARYOFFSIZE};
				GraphicsCreateSimpleSpriteFromFile(imgpath, false, BACKGROUNDLAYER, SpriteDummyID, imgloc, theGame->boundary.right - theGame->boundary.left, theGame->boundary.bottom - theGame->boundary.top);
			}
			else if(strcmp(buff, "[BACKGROUNDFX]") == 0)
			{
				//long initarray[MAXFXPARAM];
				
			}
			else if(strcmp(buff, "[CIRCLE]") == 0)
			{
				fscanf(theFile, "NumberOfCircles=%d\n", &theGame->numCircleLoaded);

				int totalballmem = theGame->numCircleLoaded * (NUMTYPES<<4);
				//create a truckload of space for the circles
				theGame->circle = ListInit(theGame->numCircleLoaded*totalballmem, sizeof(CIRCLE));
				if(!theGame->circle)
				{ MESSAGE_BOX("Memory overflow!  Too many damn circles!  Why don't you go buy more memory!?", "Bad bad error"); FileCloseFile (theFile); return RETCODE_FAILURE; }

				//initialize the spritehandles
				MemAlloc((void**)&theGame->circleImg, sizeof(BALLIMG)*theGame->numCircleLoaded);

				CIRCLE theCircle;
				DOUBLEPT speed;
				LOCATION loc;
				for(int i = 0; i < theGame->numCircleLoaded; i++)
				{
					char circimgpath[MAXCHARBUFF];
					fscanf(theFile, "SizeType=%d\n", &theCircle.sizetype);
					theCircle.Radius = ballsizes[theCircle.sizetype];

					fscanf(theFile, "imgPath=%s\n", circimgpath);

					fscanf(theFile, "location x=%d y=%d\n", &loc.x, &loc.y);
					theCircle.centerLoc.x = (double)loc.x;
					theCircle.centerLoc.y = (double)loc.y;

					//set up the image surfaces
					for(int j = 0; j < NUMTYPES; j++)
					{
						theGame->circleImg[i].theimg[j] = GraphicsCreateMyOwnSimpleSpriteFromFile(circimgpath, true, SpriteDummyID, loc, ballsizes[j]*2, ballsizes[j]*2);
					}

					theCircle.theImg = &theGame->circleImg[i];

					fscanf(theFile, "speed x=%lf y=%lf\n", &speed.x, &speed.y);
					theCircle.Movement.setXYZ(speed.x, speed.y, 0.0);
					theCircle.oldMovementy = speed.y;
					theCircle.oldMovementx = speed.x;

					fscanf(theFile, "gravity=%lf\n", &theCircle.gravity);

					theCircle.ListIndex = i;
					if(ListAppend(theGame->circle, &theCircle) != RETCODE_SUCCESS)
						assert(!"What the heck happened!?  This isn't suppose to happen!  Circle append failed");
				}
			}
			else if(strcmp(buff, "[POLYGON]") == 0)
			{
				int numpoly;

				fscanf(theFile, "NumberOfPolygon=%d\n", &numpoly);

				//create a truckload of space for the circles
				if(numpoly > 0)
				{
					theGame->polygon = ListInit(numpoly, sizeof(POLYGON));
					if(!theGame->polygon)
					{ MESSAGE_BOX("Memory overflow!  Too many damn polygon!  Why don't you go buy more memory!?", "Bad bad error"); FileCloseFile (theFile); return RETCODE_FAILURE; }
				

					POLYGON thePoly;
					for(int i = 0; i < numpoly; i++)
					{
						fscanf(theFile, "NumPts=%d\n", &thePoly.numPts);
						fscanf(theFile, "r=%d g=%d b=%d\n", &thePoly.r, &thePoly.g, &thePoly.b);

						if(MemAlloc((void**)&thePoly.pts, sizeof(LOCATION)*thePoly.numPts) != RETCODE_SUCCESS)
						{ MESSAGE_BOX("Memory overflow!  Polygon has too many damn points!  Why don't you go buy more memory!?", "Super duper bad error"); FileCloseFile(theFile); return RETCODE_FAILURE; }

						for(int j = 0; j < thePoly.numPts; j++)
							fscanf(theFile, "location x=%d y=%d\n", &thePoly.pts[j].x, &thePoly.pts[j].y);
						
						if(ListAppend(theGame->polygon, &thePoly) != RETCODE_SUCCESS)
							assert(!"What the heck happened!?  This isn't suppose to happen!  Polygon append failed");

					}
				}
			}
			else if(strcmp(buff, "END") == 0)
				bLoading = false;
		}
	}while(bLoading);

	FileCloseFile (theFile);
	return RETCODE_SUCCESS;
}

RETCODE ActivateMainMenuDisplay()
{
	GraphicsClearAllLayer();
	LOCATION loc = {0,0};

	//copy all sprites to their appropriate layer
	if(GraphicsCopySprite(theGame->theMenu.BackgroundSpriteID, theGame->theMenu.BackgroundLayer, loc) == NULL)
	{ MESSAGE_BOX("Unable to copy main menu background!  Call 1-800-DAMNITDIDNTWORK for technical support", "HAHA"); return RETCODE_FAILURE; }

	for(int i = 0; i < MAXBUTTON; i++)
	{
		theGame->theMenu.buttons[i].Sprite = GraphicsCopySprite(theGame->theMenu.buttons[i].ID, theGame->theMenu.ButtonLayer, theGame->theMenu.buttons[i].BtnLoc);
		if(theGame->theMenu.buttons[i].Sprite == NULL)
		{ MESSAGE_BOX("Unable to copy button graphics!  Call 1-800-DAMNITDIDNTWORK for technical support", "HAHA"); return RETCODE_FAILURE; }
	}

	theGame->Ghali.Sprite = GraphicsCopySprite(theGame->Ghali.SpriteID, theGame->Ghali.layer, loc);
	if(theGame->Ghali.Sprite == NULL)
	{ MESSAGE_BOX("Unable to copy player image!  Call 1-800-DAMNITDIDNTWORK for technical support", "HAHA"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

RETCODE ActivateInGameDisplay()
{
	GraphicsClearAllLayer();
	LOCATION loc = {0,0};
	theGame->Ghali.Sprite = GraphicsCopySprite(theGame->Ghali.SpriteID, theGame->Ghali.layer, loc);
	if(theGame->Ghali.Sprite == NULL)
	{ MESSAGE_BOX("Unable to copy player image!  Time to rush back to the store and get your refund", "Oh dear..."); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

RETCODE ActivateScoreDisplay()
{
	GraphicsClearAllLayer();
	LOCATION loc = {0,0};
	//copy all sprites to their appropriate layer
	if(GraphicsCopySprite(theGame->theScores.BackgroundSpriteID, theGame->theScores.BackgroundLayer, loc) == NULL)
	{ MESSAGE_BOX("Unable to copy high score background!  So sue me!", "Cripes!"); return RETCODE_FAILURE; }

	if(GraphicsCopySprite(theGame->FXspriteID, theGame->theScores.BackgroundLayer, loc) == NULL)
	{ MESSAGE_BOX("Unable to copy FX background!  So sue me!", "Cripes!"); return RETCODE_FAILURE; }

	return RETCODE_SUCCESS;
}

//
// The one used by main
//
RETCODE update_stuff()
{
	RETCODE ret_msg;
		//Update Timers
		TimerCheckTime(theGame->ingameTimer);
		TimerCheckTime(theGame->keyboardTimer);

		

		//update the ingame timer only during in-game
		if(theGame->currentLoopType == INGAMELOOP && theGame->currentLoopMessage == LOOP_DOSTUFF)
			if(TimerGetEllapsed(theGame->ingameTimer) >= theGame->ingameDelay)
			{ TimerUpdateTimer(theGame->ingameTimer); theGame->Ghali.currenttime--; }
		
		update_keyboard();


		TimerCheckTime(theGame->gameTimer);
		if(TimerGetEllapsed(theGame->gameTimer) >= theGame->gameDelay)
		{
			ret_msg = GameLoop[theGame->currentLoopType]();
			TimerUpdateTimer(theGame->gameTimer);
		}

		
		//if(TimerGetEllapsed(theGame->keyboardTimer) >= KEYBOARDDELAY)
			

		if(TimerGetEllapsed(theGame->frameTimer) >= theGame->frameDelay)
		{
			GraphicsUpdateScreen (&G_clientarea);

			TimerUpdateTimer(theGame->frameTimer);
		}

		return ret_msg;
}

//These are the game loops
PRIVATE RETCODE MainLoop()
{
	switch(theGame->currentLoopMessage)
	{
	case LOOP_INIT:
		//activate the menu!
		if(ActivateMainMenuDisplay() != RETCODE_SUCCESS)
			return RETCODE_FAILURE;

		//set up the boundary
		theGame->boundary.top = 0;
		theGame->boundary.left = 0;
		theGame->boundary.bottom = GraphicsGetScrnSize().cy;
		theGame->boundary.right = GraphicsGetScrnSize().cx;

		//set up player's location
		SIZE framesize = SpriteGetSize(theGame->Ghali.Sprite);
		theGame->Ghali.location = SpriteGetLocation(theGame->Ghali.Sprite);
		theGame->Ghali.location->x = (GraphicsGetScrnSize().cx>>1) - (framesize.cx>>1);
		theGame->Ghali.location->y = (GraphicsGetScrnSize().cy>>1) - (framesize.cy>>1);

		//activate the current one
		SpriteSetState(theGame->theMenu.buttons[theGame->theMenu.currentSelection].Sprite, BUTTON_ON);

		theGame->currentLoopMessage = LOOP_DOSTUFF;
		break;
	
	case LOOP_DOSTUFF:
		
		if(any_key_is_pressed())
		{
			SIZE spritesize = SpriteGetSize(theGame->Ghali.Sprite);

			if(is_key_pressed(kCodeDown))
			{
				if(SpriteGetCurrentState(theGame->Ghali.Sprite) != theGame->Ghali.moveFrontState)
					SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveFrontState);

				theGame->Ghali.location->y+=theGame->Ghali.PlayerSpd;
				if(theGame->Ghali.location->y + spritesize.cy > theGame->boundary.bottom)
					theGame->Ghali.location->y = theGame->boundary.bottom - spritesize.cy;
			}
			else if(is_key_pressed(kCodeUp))
			{
				if(SpriteGetCurrentState(theGame->Ghali.Sprite) != theGame->Ghali.moveBackState)
					SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveBackState);

				theGame->Ghali.location->y-=theGame->Ghali.PlayerSpd;
				if(theGame->Ghali.location->y < theGame->boundary.top)
					theGame->Ghali.location->y = theGame->boundary.top;
			}
			else if(is_key_pressed(kCodeLeft))
			{
				if(SpriteGetCurrentState(theGame->Ghali.Sprite) != theGame->Ghali.moveLeftState)
					SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveLeftState);
				
				theGame->Ghali.location->x-=theGame->Ghali.PlayerSpd;
				if(theGame->Ghali.location->x < theGame->boundary.left)
					theGame->Ghali.location->x = theGame->boundary.left;
			}
			else if(is_key_pressed(kCodeRight))
			{
				if(SpriteGetCurrentState(theGame->Ghali.Sprite) != theGame->Ghali.moveRightState)
					SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveRightState);

				theGame->Ghali.location->x+=theGame->Ghali.PlayerSpd;
				if(theGame->Ghali.location->x + spritesize.cx > theGame->boundary.right)
					theGame->Ghali.location->x = theGame->boundary.right - spritesize.cx;
			}
			else if(is_key_pressed(kCodeRet)) //do our game loop stuff
			{
				switch(theGame->theMenu.currentSelection)
				{
				case PLAYBUTTON:
					theGame->currentLoopType = INGAMELOOP; //change the game loop to in-game loop
					theGame->currentLoopMessage = LOOP_INIT;
					break;
				case HIGHSCOREBUTTON:
					theGame->currentLoopType = HIGHSCORELOOP; //change the game loop to in-game loop
					theGame->currentLoopMessage = LOOP_INIT;
					break;
				case EXITBUTTON:
					return RETCODE_REQUESTTERMINATE;					
				}
			}

			RECT button;
			RECT player;
			player.top = theGame->Ghali.location->y;
			player.left = theGame->Ghali.location->x;
			player.bottom = player.top + spritesize.cy;
			player.right = player.left + spritesize.cx;

			SIZE btnSize;
			for(int i = 0; i < MAXBUTTON; i++)
			{
				btnSize = SpriteGetSize(theGame->theMenu.buttons[i].Sprite);
				button.top = theGame->theMenu.buttons[i].BtnLoc.y;
				button.left = theGame->theMenu.buttons[i].BtnLoc.x;
				button.bottom = button.top + btnSize.cy;
				button.right = button.left + btnSize.cx;
				if(check_collision_box(player, button)) //if the player touches the button area
				{
					//set the last button off
					SpriteSetState(theGame->theMenu.buttons[theGame->theMenu.currentSelection].Sprite, BUTTON_OFF);
					//set the new one and change the current selection
					SpriteSetState(theGame->theMenu.buttons[i].Sprite, BUTTON_ON);
					theGame->theMenu.currentSelection = i;
				}
			}
		}
		else //no input detected
		{
			short current_state = SpriteGetCurrentState(theGame->Ghali.Sprite);
			
			if(current_state == theGame->Ghali.moveFrontState)
				SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveStandingFrontState);
			else if(current_state == theGame->Ghali.moveBackState)
				SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveStandingBackState);
			else if(current_state == theGame->Ghali.moveLeftState)
				SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveStandingLeftState);
			else if(current_state == theGame->Ghali.moveRightState)
				SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveStandingRightState);
		}

		update_display();
		break;
	}

	return RETCODE_SUCCESS;
}

PRIVATE RETCODE InGameLoop()
{
	switch(theGame->currentLoopMessage)
	{
	case LOOP_INIT:
		//if(ActivateInGameDisplay() != RETCODE_SUCCESS)
		//	return RETCODE_FAILURE;
		
		//make the boundary area
		SIZE scrnSize = GraphicsGetScrnSize();
		theGame->boundary.top = BOUNDARYOFFSIZE;
		theGame->boundary.left = BOUNDARYOFFSIZE;
		theGame->boundary.bottom = scrnSize.cy - BOUNDARYOFFSIZE;
		theGame->boundary.right = scrnSize.cx - BOUNDARYOFFSIZE;

		//set up the boundary sides
		theGame->boundarylines[BOUNDARY_TOP].setPoints(theGame->boundary.left,theGame->boundary.top,theGame->boundary.right,theGame->boundary.top);
		theGame->boundarylines[BOUNDARY_RIGHT].setPoints(theGame->boundary.right,theGame->boundary.top,theGame->boundary.right,theGame->boundary.bottom);
		theGame->boundarylines[BOUNDARY_BOTTOM].setPoints(theGame->boundary.right,theGame->boundary.bottom,theGame->boundary.left,theGame->boundary.bottom);
		theGame->boundarylines[BOUNDARY_LEFT].setPoints(theGame->boundary.left,theGame->boundary.bottom,theGame->boundary.left,theGame->boundary.top);
		
		//set up the player's lives and score and status
		theGame->Ghali.scoreholder = 0;
		theGame->Ghali.currentnumLife = theGame->Ghali.maxLife;
		theGame->curlevel = 0;


		theGame->currentLoopMessage = LOOP_INGAMELOADLEVEL;
		break;
	case LOOP_INGAMELOADLEVEL:

		if(theGame->curlevel < theGame->maxLevel)
		{
			if(ActivateInGameDisplay() != RETCODE_SUCCESS)
				return RETCODE_FAILURE;

			destroy_level_data();
			if(load_level_data(theGame->levelpaths[theGame->curlevel]) != RETCODE_SUCCESS)
				return RETCODE_FAILURE;
		}
		else
		{
			theGame->curlevel = 0;

			//set the score to the score holder
			theGame->Ghali.score = theGame->Ghali.scoreholder;

			//set to zero and go to the high score loop
			theGame->Ghali.currentnumLife = 0;
			theGame->currentLoopType = HIGHSCORELOOP;
			theGame->currentLoopMessage = LOOP_INIT;
			return RETCODE_SUCCESS;
		}

		//set up the score
		theGame->Ghali.score = theGame->Ghali.scoreholder;

		//set up the location of the player
		theGame->Ghali.location = SpriteGetLocation(theGame->Ghali.Sprite);

		SIZE spritesize = SpriteGetSize(theGame->Ghali.Sprite);
		theGame->Ghali.location->x = (theGame->boundary.right>>1) - (spritesize.cx>>1);
		theGame->Ghali.location->y = theGame->boundary.bottom - spritesize.cy;
		
		//set up the center location
		theGame->Ghali.circleCollision.centerLoc.x = theGame->Ghali.location->x + (spritesize.cx>>1);
		theGame->Ghali.circleCollision.centerLoc.y = theGame->Ghali.location->y + (spritesize.cy>>1);

		//set up the timer
		theGame->Ghali.currenttime = MAXTIME;

		CLEARFLAG(theGame->Ghali.Status,PLAYER_HOOKSHOTFIRED); //just in case
		SETFLAG(theGame->Ghali.Status,PLAYER_NOHOOKSHOTFIRED);

		SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveFrontState);
		theGame->currentLoopMessage = LOOP_INGAMELVLINTRO;
		break;
	case LOOP_INGAMELVLINTRO:
		if(GETFLAGS(theGame->Ghali.Status, PLAYER_DEATHINIT))
		{ 
			theGame->Ghali.location->y = GraphicsGetScrnSize().cy;

			theGame->Ghali.deathjump = DEATHFORCE; 
			CLEARFLAG(theGame->Ghali.Status, PLAYER_DEATHINIT);
			SETFLAG(theGame->Ghali.Status, PLAYER_DEATHLOOP);
		}
		else if(GETFLAGS(theGame->Ghali.Status, PLAYER_DEATHLOOP))
		{
			SIZE spritesize = SpriteGetSize(theGame->Ghali.Sprite);

			theGame->Ghali.location->y += int(theGame->Ghali.deathjump);
			theGame->Ghali.deathjump += DEATHPULL;

			//that means the player went offscreen
			if(theGame->Ghali.deathjump > 0 && theGame->Ghali.location->y + spritesize.cy >= theGame->boundary.bottom)
			{
				theGame->Ghali.location->y = theGame->boundary.bottom - spritesize.cy;
				
				CLEARFLAG(theGame->Ghali.Status, PLAYER_DEATHLOOP);
				CLEARFLAG(theGame->Ghali.Status,PLAYER_HOOKSHOTFIRED); //just in case
				SETFLAG(theGame->Ghali.Status,PLAYER_NOHOOKSHOTFIRED);
				theGame->currentLoopMessage = LOOP_DOSTUFF;
			}

			//draw all the stuff on the level
			draw_level();
		}
		break;
	case LOOP_DOSTUFF:
		if(any_key_is_pressed())
		{
			if(is_key_pressed(kCodeLeft))
			{
				if(SpriteGetCurrentState(theGame->Ghali.Sprite) != theGame->Ghali.moveLeftState)
					SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveLeftState);
				
				theGame->Ghali.location->x-=theGame->Ghali.PlayerSpd;
				theGame->Ghali.circleCollision.centerLoc.x-=theGame->Ghali.PlayerSpd;
				if(theGame->Ghali.location->x < theGame->boundary.left)
				{ theGame->Ghali.location->x = theGame->boundary.left; theGame->Ghali.circleCollision.centerLoc.x = theGame->Ghali.location->x + (SpriteGetSize(theGame->Ghali.Sprite).cx>>1); }
			}
			else if(is_key_pressed(kCodeRight))
			{
				SIZE spritesize = SpriteGetSize(theGame->Ghali.Sprite);
				if(SpriteGetCurrentState(theGame->Ghali.Sprite) != theGame->Ghali.moveRightState)
					SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveRightState);

				theGame->Ghali.location->x+=theGame->Ghali.PlayerSpd;
				theGame->Ghali.circleCollision.centerLoc.x+=theGame->Ghali.PlayerSpd;
				if(theGame->Ghali.location->x + spritesize.cx > theGame->boundary.right)
				{ theGame->Ghali.location->x = theGame->boundary.right - spritesize.cx; theGame->Ghali.circleCollision.centerLoc.x = theGame->Ghali.location->x + (SpriteGetSize(theGame->Ghali.Sprite).cx>>1); }
			}
			//hookshot fired, one at a time only
			if(is_key_pressed(kCodeSpc) && GETFLAGS(theGame->Ghali.Status,PLAYER_NOHOOKSHOTFIRED))
			{
				CLEARFLAG(theGame->Ghali.Status,PLAYER_NOHOOKSHOTFIRED); //just in case
				SETFLAG(theGame->Ghali.Status,PLAYER_HOOKSHOTFIRED);
				SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveStandingBackState);

				//relocate the hookshot
				//set the first location...or rather the tip
				theGame->Ghali.HookShot.pts[0].x = theGame->Ghali.location->x + (SpriteGetSize(theGame->Ghali.Sprite).cx>>1);
				theGame->Ghali.HookShot.pts[0].y = theGame->Ghali.location->y;
				for(int i = 1; i < theGame->Ghali.HookShot.numPts; i++)
				{
					theGame->Ghali.HookShot.pts[i].x = theGame->Ghali.HookPts[i].x + theGame->Ghali.HookShot.pts[0].x;
					theGame->Ghali.HookShot.pts[i].y = theGame->Ghali.HookPts[i].y + theGame->Ghali.HookShot.pts[0].y;
				}

				//set up the hook line
				theGame->Ghali.HookLine.setPoints(theGame->Ghali.HookShot.pts[0].x,theGame->Ghali.HookShot.pts[0].y,theGame->Ghali.HookShot.pts[0].x,theGame->boundary.bottom);
				
			}
		}
		else
		{
			if(SpriteGetCurrentState(theGame->Ghali.Sprite) != theGame->Ghali.moveStandingFrontState)
				SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveStandingFrontState);
		}
		
		if(is_key_pressed(kCodeEsc))
		{
			//return to the main menu loop
			theGame->currentLoopType = MENULOOP;
			theGame->currentLoopMessage = LOOP_INIT;
		}
		//level skip ;)
		else if(is_key_pressed(kCodeEnd))
		{
			theGame->Ghali.scoreholder = theGame->Ghali.score;
				//increment the level and go back to level loading
			theGame->curlevel++;
			theGame->currentLoopMessage = LOOP_INGAMELOADLEVEL;
		}

		
		if(theGame->Ghali.currenttime <= 0)
		{
			//time ran out, so death to player, mwuahaha!
			theGame->Ghali.currentnumLife--;
			SETFLAG(theGame->Ghali.Status,PLAYER_DEATHINIT);
			SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveDeathState);
			theGame->currentLoopMessage = LOOP_INGAMEDEATH;
		}


		GraphicsClearBuffer(RGB(255, 0, 255));

		update_display(); //blt all the sprites

		//display and update the objects
		if(theGame->circle)
		{
			int numcircles = ListGetNodeCount(theGame->circle);
			if(numcircles == 0) //that means there are no more circles
			{
				//do level complete loop...none for now
				//store the score with the time bonus
				theGame->Ghali.scoreholder = theGame->Ghali.score + (theGame->Ghali.currenttime*(SCOREAMT>>2));
				//increment the level and go back to level loading
				theGame->curlevel++;
				
				SETFLAG(theGame->Ghali.Status,PLAYER_DEATHINIT);
				theGame->currentLoopMessage = LOOP_INGAMEWINNER;
			}

			//otherwise...loop through the circles, move them, check for collision, do reflection
			//and blit them.
			PCIRCLE thisCircle;
			for(int i = 0; i < numcircles; i++)
			{
				thisCircle = (PCIRCLE)ListExtract(theGame->circle, i);
				assert(thisCircle);

				//move the circle
				//if(thisCircle->Movement.getX()==0)
				//	thisCircle->Movement.setX(thisCircle->oldMovementx);

				thisCircle->centerLoc.x += thisCircle->Movement.getX();
				thisCircle->centerLoc.y += thisCircle->Movement.getY();
				thisCircle->Movement.setY(thisCircle->Movement.getY()+thisCircle->gravity);

				//check for ball to ball collision
				PCIRCLE otherCircle;
				for(int j = 0; j < numcircles; j++)
				{
					if(j != i)
					{
						otherCircle = (PCIRCLE)ListExtract(theGame->circle, j);
						if(check_circle_to_circle(*thisCircle, *otherCircle))
							bounce_circle_to_circle(thisCircle, otherCircle);
					}
				}

				//check for polygon collision
				int maxpoly;
				PPOLYGON thisPoly;
				Line theLine;

				if(theGame->polygon)
				{
					maxpoly = ListGetNodeCount(theGame->polygon);
					for(int i = 0; i < maxpoly; i++)
					{
						thisPoly = (PPOLYGON)ListExtract(theGame->polygon, i);

						//if it did collided, bounce it off
						if(check_circle_to_poly(*thisCircle, *thisPoly, &theLine))
						{
							bounce_circle_to_line(thisCircle, theLine);
						}
					}
				}

#if VULNERABLE
				//check for player collision
				//check for both circle collision and if so, death awaits!
				if(check_circle_to_circle(theGame->Ghali.circleCollision, *thisCircle))
				{
					theGame->Ghali.currentnumLife--;
					SETFLAG(theGame->Ghali.Status,PLAYER_DEATHINIT);
					theGame->currentLoopMessage = LOOP_INGAMEDEATH;
					//GraphicsUnlock();
					return RETCODE_SUCCESS;
				}
#endif

				//check for boundary collision and bounce them if so
				//just invert the vector movements...it's much more efficient...and very easy :)
				if((int)thisCircle->centerLoc.x - thisCircle->Radius < theGame->boundary.left)
				{ thisCircle->centerLoc.x = theGame->boundary.left + thisCircle->Radius; thisCircle->Movement.setX(-thisCircle->Movement.getX()); }
				if((int)thisCircle->centerLoc.x + thisCircle->Radius > theGame->boundary.right)
				{ thisCircle->centerLoc.x = theGame->boundary.right - thisCircle->Radius; thisCircle->Movement.setX(-thisCircle->Movement.getX()); }
				if((int)thisCircle->centerLoc.y - thisCircle->Radius < theGame->boundary.top)
				{ thisCircle->centerLoc.y = theGame->boundary.top + thisCircle->Radius; thisCircle->oldMovementy *= -1; thisCircle->Movement.setY(thisCircle->oldMovementy); }
				if((int)thisCircle->centerLoc.y + thisCircle->Radius > theGame->boundary.bottom)
				{ thisCircle->centerLoc.y = theGame->boundary.bottom - thisCircle->Radius; thisCircle->oldMovementy *= -1; thisCircle->Movement.setY(thisCircle->oldMovementy); }

				LOCATION circbltLoc = {(int)(thisCircle->centerLoc.x - thisCircle->Radius), (int)(thisCircle->centerLoc.y - thisCircle->Radius)};
				GraphicsBltSprite(thisCircle->theImg->theimg[thisCircle->sizetype], &circbltLoc);

				//blit the circle
				/*assert(thisCircle->centerLoc.x - thisCircle->Radius > 0);
				assert(thisCircle->centerLoc.x + thisCircle->Radius < GraphicsGetScrnSize().cx);
				assert(thisCircle->centerLoc.y - thisCircle->Radius > 0);
				assert(thisCircle->centerLoc.y + thisCircle->Radius < GraphicsGetScrnSize().cy);
				DrawFilledCircle((int)thisCircle->centerLoc.x, (int)thisCircle->centerLoc.y, thisCircle->Radius, thisCircle->r, thisCircle->g, thisCircle->b);*/
			}
		}

		//Lock the backbuffer before drawing the rest of the objects
		GraphicsLock();

		//move, check for collision and display the hook shot if it is fired
		if(GETFLAGS(theGame->Ghali.Status,PLAYER_HOOKSHOTFIRED))
		{
			int i, j, k;
			int maxpoly, maxcircle;
			PPOLYGON thisPoly;
			PCIRCLE thisCircle;

			//move the entire hookshot polygon
			for(i = 0; i < theGame->Ghali.HookShot.numPts; i++)
				theGame->Ghali.HookShot.pts[i].y -= theGame->Ghali.HookShotSpd;

			//change the top point of the line
			theGame->Ghali.HookLine.setPoint0(theGame->Ghali.HookShot.pts[0]);

			//check for polygon collision
			//piece of shit won't work
			if(theGame->polygon)
			{
				maxpoly = ListGetNodeCount(theGame->polygon);
				
				for(j = 0; j < maxpoly; j++)
				{
					thisPoly = (PPOLYGON)ListExtract(theGame->polygon, j);
					assert(thisPoly);
					if(check_poly_intersect(*thisPoly, theGame->Ghali.HookLine)) //if we hit something
						goto INTERSECTED;
				}
			}

			//check for circle collision
			maxcircle = ListGetNodeCount(theGame->circle);
			
			for(k = 0; k < maxcircle; k++)
			{
				thisCircle = (PCIRCLE)ListExtract(theGame->circle, k);
				assert(thisCircle);
				if(check_circle_to_poly(*thisCircle, theGame->Ghali.HookShot))
				{
					//if it collided, split the circle into two and destroy the hookshot
					theGame->Ghali.score += SCOREAMT*(thisCircle->sizetype+1);
					if(split_circle(thisCircle) != RETCODE_SUCCESS)
					{ MESSAGE_BOX("Error splitting circles in half, oh that's just great!", "Hey Buddy"); return RETCODE_FAILURE; }
					//destroy the hookshot
					goto INTERSECTED;
				}
				else if((thisCircle->centerLoc.x - thisCircle->Radius <= theGame->Ghali.HookLine.getP0().x)
					&&(thisCircle->centerLoc.x + thisCircle->Radius >= theGame->Ghali.HookLine.getP0().x)
					&&(thisCircle->centerLoc.y + thisCircle->Radius >= theGame->Ghali.HookLine.getP0().y))
				{
					//if it collided, split the circle into two and destroy the hookshot
					theGame->Ghali.score += SCOREAMT*(thisCircle->sizetype+1);
					if(split_circle(thisCircle) != RETCODE_SUCCESS)
					{ MESSAGE_BOX("Error splitting circles in half, oh that's just great!", "Hey Buddy"); return RETCODE_FAILURE; }
					//destroy the hookshot
					goto INTERSECTED;
				}
			}

			//check for boundary collision
			//only check for the top
			//if they collided, the hook shot is removed
			if(theGame->Ghali.HookLine.getP0().y <= theGame->boundary.top)
			{
INTERSECTED:
				CLEARFLAG(theGame->Ghali.Status,PLAYER_HOOKSHOTFIRED); //just in case
				SETFLAG(theGame->Ghali.Status,PLAYER_NOHOOKSHOTFIRED);
				SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveStandingFrontState);
			}
			
			//otherwise everything went well, so blit the darn thing
			DrawFilledPolygon(theGame->Ghali.HookShot.pts, theGame->Ghali.HookShot.numPts, theGame->Ghali.HookShot.r, theGame->Ghali.HookShot.g, theGame->Ghali.HookShot.b);
			DrawLine(theGame->Ghali.HookLine.getP0().x,theGame->Ghali.HookLine.getP0().y,theGame->Ghali.HookLine.getP1().x,theGame->Ghali.HookLine.getP1().y, theGame->Ghali.HookShot.r, theGame->Ghali.HookShot.g, theGame->Ghali.HookShot.b);

		}

		if(theGame->polygon)
		{
			PPOLYGON thisPoly;
			int numpoly = ListGetNodeCount(theGame->polygon);
			for(int i = 0; i < numpoly; i++)
			{
				thisPoly = (PPOLYGON)ListExtract(theGame->polygon, i);
				assert(thisPoly);

				//just blit the polygon
				DrawFilledPolygon(thisPoly->pts, thisPoly->numPts, thisPoly->r, thisPoly->g, thisPoly->b);
			}
		}

		//Blit the boundary
		DrawLine(theGame->boundarylines[BOUNDARY_TOP].getP0().x, theGame->boundarylines[BOUNDARY_TOP].getP0().y, theGame->boundarylines[BOUNDARY_TOP].getP1().x, theGame->boundarylines[BOUNDARY_TOP].getP1().y, 255, 255, 255);
		DrawLine(theGame->boundarylines[BOUNDARY_RIGHT].getP0().x, theGame->boundarylines[BOUNDARY_RIGHT].getP0().y, theGame->boundarylines[BOUNDARY_RIGHT].getP1().x, theGame->boundarylines[BOUNDARY_RIGHT].getP1().y, 255, 255, 255);
		DrawLine(theGame->boundarylines[BOUNDARY_BOTTOM].getP0().x, theGame->boundarylines[BOUNDARY_BOTTOM].getP0().y, theGame->boundarylines[BOUNDARY_BOTTOM].getP1().x, theGame->boundarylines[BOUNDARY_BOTTOM].getP1().y, 255, 255, 255);
		DrawLine(theGame->boundarylines[BOUNDARY_LEFT].getP0().x, theGame->boundarylines[BOUNDARY_LEFT].getP0().y, theGame->boundarylines[BOUNDARY_LEFT].getP1().x, theGame->boundarylines[BOUNDARY_LEFT].getP1().y, 255, 255, 255);

		//DrawFilledCircle((int)theGame->Ghali.circleCollision.centerLoc.x, (int)theGame->Ghali.circleCollision.centerLoc.y, theGame->Ghali.circleCollision.Radius, 255, 255, 255);
		//done displaying the objects
		GraphicsUnlock();

		//display the current level, lives and score
		char buff[MAXCHARBUFF];
		sprintf(buff, "   Level: %d   Lives: %d   Time Left: %d   Score: %d", theGame->curlevel+1, theGame->Ghali.currentnumLife, theGame->Ghali.currenttime, theGame->Ghali.score);

		RECT txtArea;
		txtArea.top = txtArea.left = 0;
		txtArea.right = GraphicsGetScrnSize().cx;
		txtArea.bottom = BOUNDARYOFFSIZE;
		FontDisplayText(theGame->theFont, buff, txtArea);
		break;
		
	case LOOP_INGAMEDEATH:
		if(GETFLAGS(theGame->Ghali.Status, PLAYER_DEATHINIT))
		{ 
			SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveDeathState);
			theGame->Ghali.deathjump = DEATHFORCE; 
			CLEARFLAG(theGame->Ghali.Status, PLAYER_DEATHINIT);
			SETFLAG(theGame->Ghali.Status, PLAYER_DEATHLOOP);
		}
		else if(GETFLAGS(theGame->Ghali.Status, PLAYER_DEATHLOOP))
		{
			theGame->Ghali.location->y += int(theGame->Ghali.deathjump);
			theGame->Ghali.deathjump += DEATHPULL;

			//that means the player went offscreen
			if(theGame->Ghali.location->y >= GraphicsGetScrnSize().cy)
			{
				CLEARFLAG(theGame->Ghali.Status, PLAYER_DEATHLOOP);
				
				if(theGame->Ghali.currentnumLife > 0)
					theGame->currentLoopMessage = LOOP_INGAMELOADLEVEL;
				else //that means it is game over
				{
					//add in the bonus time
					theGame->Ghali.score += theGame->Ghali.currenttime*(SCOREAMT>>2);
					theGame->currentLoopType = HIGHSCORELOOP;
					theGame->currentLoopMessage = LOOP_INIT;
				}
			}

			//draw all the stuff on the level
			draw_level();
		}
		break;
	case LOOP_INGAMEWINNER:
		if(GETFLAGS(theGame->Ghali.Status, PLAYER_DEATHINIT))
		{ 
			theGame->ingameDelay = INGAMETIMEDELAY<<2; //jump happily for 4 seconds
			TimerUpdateTimer(theGame->ingameTimer);
			GraphicsDeactivateLayer(BACKGROUNDLAYER);
			
			theGame->ha = rand()%100000;
			theGame->haha = Random(65, 70);

			//theGame->Ghali.location->y = GraphicsGetScrnSize().cy;
			SpriteSetState(theGame->Ghali.Sprite, theGame->Ghali.moveWinState);
			theGame->Ghali.deathjump = DEATHFORCE>>1; 
			CLEARFLAG(theGame->Ghali.Status, PLAYER_DEATHINIT);
			SETFLAG(theGame->Ghali.Status, PLAYER_DEATHLOOP);
		}
		else if(GETFLAGS(theGame->Ghali.Status, PLAYER_DEATHLOOP))
		{
			SIZE spritesize = SpriteGetSize(theGame->Ghali.Sprite);

			theGame->Ghali.location->y += int(theGame->Ghali.deathjump);
			theGame->Ghali.deathjump += DEATHPULL;

			//that means the player went offscreen
			if(theGame->Ghali.location->y + spritesize.cy >= theGame->boundary.bottom)
			{
				theGame->Ghali.location->y = theGame->boundary.bottom - spritesize.cy;

				theGame->Ghali.deathjump = DEATHFORCE>>1; 
			}

			if((TimerGetEllapsed(theGame->ingameTimer) >= theGame->ingameDelay))
			{
				theGame->ingameDelay = INGAMETIMEDELAY;
				GraphicsActivateLayer(BACKGROUNDLAYER);

				CLEARFLAG(theGame->Ghali.Status, PLAYER_DEATHLOOP);
				CLEARFLAG(theGame->Ghali.Status,PLAYER_HOOKSHOTFIRED); //just in case
				SETFLAG(theGame->Ghali.Status,PLAYER_NOHOOKSHOTFIRED);
				theGame->currentLoopMessage = LOOP_INGAMELOADLEVEL;
			}

			//draw all the stuff on the level
			draw_level();

			RECT txtArea = theGame->boundary;

			txtArea.top += txtArea.bottom>>3;
			txtArea.left += txtArea.right>>4;

			if(theGame->curlevel < theGame->maxLevel)
			{
				FontDisplayText(theGame->theFont, "Well I'll be damned, you win!", txtArea);
			}
			else
			{
				char buff[MAXCHARBUFF];
				FontDisplayText(theGame->theFont, "You have beaten all the levels, my friend", txtArea);
				txtArea.top += FontGetHeight(theGame->theFont);
				FontDisplayText(theGame->theFont, "For your reward,", txtArea);
				txtArea.top += FontGetHeight(theGame->theFont);
				FontDisplayText(theGame->theFont, "I will give you answer from the exam:", txtArea);
				txtArea.top += FontGetHeight(theGame->theFont);

				sprintf(buff, "The answer to #%d is %c", theGame->ha, theGame->haha);
				FontDisplayText(theGame->theFont, buff, txtArea);

			}
		}
		break;
	}
	return RETCODE_SUCCESS;
}

PRIVATE RETCODE HighScoreLoop()
{
	switch(theGame->currentLoopMessage)
	{
	case LOOP_INIT:
		if(ActivateScoreDisplay() != RETCODE_SUCCESS)
			return RETCODE_FAILURE;

		//check to see if the player lost all their life
		if(theGame->Ghali.currentnumLife == 0) //that means it was a game over
		{
			//memset(theGame->theScores.buffer, 0, sizeof(char)*MAXNAMEBUFF);
			strcpy(theGame->theScores.buffer, " ");
			//check to see if the player qualified
			for(int i = 0; i < MAXSCORE; i++)
			{
				if(theGame->Ghali.score > theGame->theScores.highscore[i].score) //that means the player qualifies
				{
					theGame->theScores.win_index = i; //used later to insert the winner
					theGame->currentLoopMessage = LOOP_HIGHSCOREGETNAME;
					return RETCODE_SUCCESS;
				}
			}
		}

		theGame->currentLoopMessage = LOOP_DOSTUFF;
		break;
	case LOOP_DOSTUFF:
		{
			update_display();

			RECT textArea;
			SIZE screenSize = GraphicsGetScrnSize();
			int txtHeight = FontGetHeight(theGame->theFont);
			textArea.left = BOUNDARYOFFSIZE;
			textArea.right = screenSize.cx;
			textArea.top = (screenSize.cy>>1) - ((MAXSCORE>>1)*txtHeight);
			textArea.bottom = screenSize.cy;

			char buff[MAXCHARBUFF];

			for(int i=0; i<MAXSCORE; i++)
			{
				sprintf(buff, "%02d. %s    %d", i+1, theGame->theScores.highscore[i].playerName, theGame->theScores.highscore[i].score);
				FontDisplayText(theGame->theFont, buff, textArea);
				textArea.top += txtHeight;
			}

			if(is_key_pressed(kCodeEsc))
			{
				//return to the main menu loop
				theGame->currentLoopType = MENULOOP;
				theGame->currentLoopMessage = LOOP_INIT;
			}
			break;
		}
	case LOOP_HIGHSCOREGETNAME:
		{
			update_display();

			RECT textArea;
			SIZE screenSize = GraphicsGetScrnSize();
			int txtHeight = FontGetHeight(theGame->theFont);
			textArea.left = BOUNDARYOFFSIZE;
			textArea.right = screenSize.cx;
			textArea.top = (screenSize.cy>>1) - ((MAXSCORE>>1)*txtHeight);
			textArea.bottom = screenSize.cy;

			char buff[MAXCHARBUFF];
			sprintf(buff, "You made it to %dth place, well good for you!", theGame->theScores.win_index+1);
			FontDisplayText(theGame->theFont, buff, textArea);
			textArea.top += txtHeight;
			FontDisplayText(theGame->theFont, "Type in your name (press 'enter' when done):", textArea);
			textArea.top += txtHeight;

			FontDisplayText(theGame->theFont, theGame->theScores.buffer, textArea);

			if((TimerGetEllapsed(theGame->keyboardTimer) >= KEYBOARDDELAY) && any_key_is_pressed())
			{
				if(is_key_pressed(kCodeEsc))
				{
					//that means the player doesn't want his score to be recorded
					theGame->currentLoopMessage = LOOP_DOSTUFF;
					return RETCODE_SUCCESS;
				}
				if(is_key_pressed(kCodeRet) || strlen(theGame->theScores.buffer)+2==MAXNAMEBUFF)
				{
					//update the high score list, as well as the file
					HIGHSCOREDATA lastscoredata[MAXSCORE];
					int index;

					for(index = theGame->theScores.win_index+1; index < MAXSCORE; index++)
					{
						strcpy(lastscoredata[index].playerName, theGame->theScores.highscore[index-1].playerName);
						lastscoredata[index].score = theGame->theScores.highscore[index-1].score;
					}
					
					strcpy(theGame->theScores.highscore[theGame->theScores.win_index].playerName, theGame->theScores.buffer);
					theGame->theScores.highscore[theGame->theScores.win_index].score = theGame->Ghali.score;

					for(index = theGame->theScores.win_index+1; index < MAXSCORE; index++)
					{
						strcpy(theGame->theScores.highscore[index].playerName, lastscoredata[index].playerName);
						theGame->theScores.highscore[index].score = lastscoredata[index].score;
					}

					//save it to file, baby!
					save_scores(theGame->theScores.scorefilepath);

					//better make sure we don't get here again
					theGame->Ghali.currentnumLife = theGame->Ghali.maxLife;

					//display high scores
					theGame->currentLoopMessage = LOOP_DOSTUFF;
				}

				if(is_key_pressed(kCodeBkspc))
				{
					//eek
					char oldbuff[MAXNAMEBUFF];
					strcpy(oldbuff, theGame->theScores.buffer);
					int newstrlen = strlen(theGame->theScores.buffer)-1;

					memset(theGame->theScores.buffer, 0, sizeof(char)*MAXNAMEBUFF);

					for(int i = 0; i < newstrlen; i++)
						theGame->theScores.buffer[i] = oldbuff[i];
				}

				for(BYTE thebyte = kCodeA; thebyte <= kCodeZ; thebyte++)
				{
					if(is_key_pressed(thebyte))
						strcat(theGame->theScores.buffer, AsciiStuff[thebyte]);
				}

				for(BYTE thebyte2 = kCode0; thebyte2 <= kCode9; thebyte2++)
				{
					if(is_key_pressed(thebyte2))
						strcat(theGame->theScores.buffer, AsciiStuff[thebyte2]);
				}

				TimerUpdateTimer(theGame->keyboardTimer);
			}
			break;
		}
	}
	return RETCODE_SUCCESS;
}

PRIVATE void draw_level()
{
	GraphicsClearBuffer(RGB(255, 0, 255));
	
	update_display(); //blt all the sprites
	
	//display and update the objects
	if(theGame->circle)
	{
		int numcircles = ListGetNodeCount(theGame->circle);
		
		PCIRCLE thisCircle;
		for(int i = 0; i < numcircles; i++)
		{
			thisCircle = (PCIRCLE)ListExtract(theGame->circle, i);
			assert(thisCircle);

			LOCATION circbltLoc = {(int)(thisCircle->centerLoc.x - thisCircle->Radius), (int)(thisCircle->centerLoc.y - thisCircle->Radius)};
				GraphicsBltSprite(thisCircle->theImg->theimg[thisCircle->sizetype], &circbltLoc);
			
			//blit the circle
			/*assert(thisCircle->centerLoc.x - thisCircle->Radius > 0);
			assert(thisCircle->centerLoc.x + thisCircle->Radius < GraphicsGetScrnSize().cx);
			assert(thisCircle->centerLoc.y - thisCircle->Radius > 0);
			assert(thisCircle->centerLoc.y + thisCircle->Radius < GraphicsGetScrnSize().cy);
			DrawFilledCircle((int)thisCircle->centerLoc.x, (int)thisCircle->centerLoc.y, thisCircle->Radius, thisCircle->r, thisCircle->g, thisCircle->b);*/
		}
	}

	//Lock the backbuffer before drawing all objects
	GraphicsLock();
	
	if(theGame->polygon)
	{
		PPOLYGON thisPoly;
		int numpoly = ListGetNodeCount(theGame->polygon);
		for(int i = 0; i < numpoly; i++)
		{
			thisPoly = (PPOLYGON)ListExtract(theGame->polygon, i);
			assert(thisPoly);
			
			//just blit the polygon
			DrawFilledPolygon(thisPoly->pts, thisPoly->numPts, thisPoly->r, thisPoly->g, thisPoly->b);
		}
	}
	
	//Blit the boundary
	DrawLine(theGame->boundarylines[BOUNDARY_TOP].getP0().x, theGame->boundarylines[BOUNDARY_TOP].getP0().y, theGame->boundarylines[BOUNDARY_TOP].getP1().x, theGame->boundarylines[BOUNDARY_TOP].getP1().y, 255, 255, 255);
	DrawLine(theGame->boundarylines[BOUNDARY_RIGHT].getP0().x, theGame->boundarylines[BOUNDARY_RIGHT].getP0().y, theGame->boundarylines[BOUNDARY_RIGHT].getP1().x, theGame->boundarylines[BOUNDARY_RIGHT].getP1().y, 255, 255, 255);
	DrawLine(theGame->boundarylines[BOUNDARY_BOTTOM].getP0().x, theGame->boundarylines[BOUNDARY_BOTTOM].getP0().y, theGame->boundarylines[BOUNDARY_BOTTOM].getP1().x, theGame->boundarylines[BOUNDARY_BOTTOM].getP1().y, 255, 255, 255);
	DrawLine(theGame->boundarylines[BOUNDARY_LEFT].getP0().x, theGame->boundarylines[BOUNDARY_LEFT].getP0().y, theGame->boundarylines[BOUNDARY_LEFT].getP1().x, theGame->boundarylines[BOUNDARY_LEFT].getP1().y, 255, 255, 255);
	
	//done displaying the objects
	GraphicsUnlock();
	
	//display the current level, lives and score
	char buff[MAXCHARBUFF];

	//draw the status only if the player has at least one life
	if(theGame->Ghali.currentnumLife > 0)
		sprintf(buff, "   Level: %d   Lives: %d   Time Left: %d   Score: %d", theGame->curlevel+1, theGame->Ghali.currentnumLife, theGame->Ghali.currenttime, theGame->Ghali.score);
	else
		sprintf(buff, "Game Over, My Friend");
	
	RECT txtArea;
	txtArea.top = txtArea.left = 0;
	txtArea.right = GraphicsGetScrnSize().cx;
	txtArea.bottom = BOUNDARYOFFSIZE;
	FontDisplayText(theGame->theFont, buff, txtArea);
}


PRIVATE RETCODE update_display()
{
	TimerCheckTime(theGame->frameTimer);
	if(TimerGetEllapsed(theGame->frameTimer) >= theGame->frameDelay)
		{
			GraphicsUpdateAnimationLayers();
			GraphicsBltSpriteLayers();

	//		TimerUpdateTimer(theGame->frameTimer);
		}

	return RETCODE_SUCCESS;
}

//
// Collision stuff
//

bool check_collision_box(const RECT rect1, const RECT rect2)
{
	int width1, height1, width2, height2;
	int cx1, cy1, cx2, cy2;
	int dx, dy;

	// get the radi of each rect
	width1  = ((rect1.right - rect1.left)>>1) - ((rect1.right - rect1.left)>>3);
	height1 = ((rect1.bottom - rect1.top)>>1) - ((rect1.bottom - rect1.top)>>3);

	width2  = ((rect2.right - rect2.left)>>1) - ((rect2.right - rect2.left)>>3);
	height2 = ((rect2.bottom - rect2.top)>>1) - ((rect2.bottom - rect2.top)>>3);

	// compute center of each rect
	cx1 = rect1.left + width1;
	cy1 = rect1.top + height1;

	cx2 = rect2.left + width2;
	cy2 = rect2.top + height2;

	// compute deltas
	dx = abs(cx2 - cx1);
	dy = abs(cy2 - cy1);

	// test if rects overlap
	if (dx < (width1+width2) && dy < (height1+height2))
	   return true;
	// else no collision
	else
	   return false;
}

bool check_line_intersect(const POINT l1p0, const POINT l1p1, const POINT l2p0, const POINT l2p1, POINT *intersect)
{
	POINT s, t, b; //the scalar vector of the two line

	s.x = l1p1.x - l1p0.x;
	s.y = l1p1.y - l1p0.y;
	t.x = -(l2p1.x - l2p0.x);
	t.y = -(l2p1.y - l2p0.y);
	b.x = l2p0.x - l1p0.x;
	b.y = l2p0.y - l1p0.y;

	double det1, det2; //the determinants
	int divide;
	divide = (s.x*t.y)-(t.x*s.y);

	//do Cramer's rule
	if(divide)
	{
		det1 = ((b.x*t.y) - (t.x*b.y))/divide;
		det2 = ((s.x*b.y) - (b.x*s.y))/divide;
	}
	else
		return false;

	POINT int1; //the intersecting point
	int1.x = int(l1p0.x + det1*s.x);
	int1.y = int(l1p0.y + det1*s.y);

	//they intersect, so return true and fill in the given intersect if it is not null
	if((det1 >= 0.0 && det1 <= 1.0) && (det2 >= 0.0 && det2 <= 1.0))
	{
		if(intersect)
			*intersect = int1; //any intersection will do
		return true;
	}
	
	//otherwise they didn't
	return false;
}


//this function is only used by hookshot
bool check_poly_intersect(const POLYGON &poly, Line line)
{
	int maxloop = poly.numPts - 1;
	for(int i = 0; i < maxloop; i++)
	{
		//polyline.setPoints(poly.pts[i].x, poly.pts[i].y, poly.pts[i+1].x, poly.pts[i+1].y);
		if(line.getP0().x >=poly.pts[i].x && line.getP0().x<=poly.pts[i+1].x)
		{
		Vector2D v1(poly.pts[i].x-poly.pts[i+1].x, poly.pts[i].y-poly.pts[i+1].y);
		Vector2D v2(line.getP0().x-poly.pts[i+1].x, line.getP0().y-poly.pts[i+1].y);
		if(v1.CrossProd(v2)>0)
				return true;
		}
	}

	//check for the last side of the polygon
	//polyline.setPoints(poly.pts[poly.numPts - 1].x, poly.pts[poly.numPts - 1].y, poly.pts[0].x, poly.pts[0].y);
	//if(line.isIntersecting(polyline, intersect))
	//	return true;

	if(line.getP0().x >=poly.pts[poly.numPts - 1].x && line.getP0().x<=poly.pts[0].x)
		{
		Vector2D v1(poly.pts[poly.numPts - 1].x-poly.pts[0].x, poly.pts[poly.numPts - 1].y-poly.pts[0].y);
		Vector2D v2(line.getP0().x-poly.pts[0].x, line.getP0().y-poly.pts[0].y);
		if(v1.CrossProd(v2)>0)
				return true;
		}
	
	//otherwise it's cool
	return false;
}

bool check_circle_to_line(const CIRCLE &thecircle, const Line &theline, Vector3D *theWeight)
{
	/*DOUBLEPT LineVect = {theline.getP1().x-theline.getP0().x, theline.getP1().y-theline.getP0().y};
	DOUBLEPT DVect = {theline.getP0().x-thecircle.centerLoc.x, theline.getP0().y-thecircle.centerLoc.y};
	DOUBLEPT circlescale = {0,0};


	if(((thecircle.centerLoc.x <= theline.getP0().x) && (thecircle.centerLoc.x + thecircle.Radius >= theline.getP0().x))
		|| ((thecircle.centerLoc.x <= theline.getP1().x) && (thecircle.centerLoc.x + thecircle.Radius >= theline.getP1().x)))
	{
		//DVect.x = theline.getP0().x-(thecircle.centerLoc.x+thecircle.Radius);
		circlescale.x = thecircle.Radius<<1;
	}
	else if(((thecircle.centerLoc.x-thecircle.Radius <= theline.getP0().x) && (thecircle.centerLoc.x >= theline.getP0().x))
		|| ((thecircle.centerLoc.x-thecircle.Radius <= theline.getP1().x) && (thecircle.centerLoc.x >= theline.getP1().x)))
	{
		//DVect.x = theline.getP0().x-(thecircle.centerLoc.x-thecircle.Radius);
		//circlescale.x = -(thecircle.Radius<<1);
	}

	if(((thecircle.centerLoc.y <= theline.getP0().y) && (thecircle.centerLoc.y + thecircle.Radius >= theline.getP0().y))
		|| ((thecircle.centerLoc.y <= theline.getP1().y) && (thecircle.centerLoc.y + thecircle.Radius >= theline.getP1().y)))
	{
		//DVect.y = theline.getP0().y-(thecircle.centerLoc.y+thecircle.Radius);
		//circlescale.y = thecircle.Radius<<1;
	}
	else if(((thecircle.centerLoc.y-thecircle.Radius <= theline.getP0().y) && (thecircle.centerLoc.y >= theline.getP0().y))
		|| ((thecircle.centerLoc.y-thecircle.Radius <= theline.getP1().y) && (thecircle.centerLoc.y >= theline.getP1().y)))
	{
		//DVect.y = theline.getP0().y-(thecircle.centerLoc.y-thecircle.Radius);
		//circlescale.y = -thecircle.Radius<<1;
	}

	//DVect.x = thecircle.Movement.getX() < 0 ? theline.getP0().x-(thecircle.centerLoc.x-thecircle.Radius) : theline.getP0().x-(thecircle.centerLoc.x+thecircle.Radius);
	//DVect.y = thecircle.Movement.getY() < 0 ? theline.getP0().y-(thecircle.centerLoc.y-thecircle.Radius) : theline.getP0().y-(thecircle.centerLoc.y+thecircle.Radius);
	circlescale.x = thecircle.Movement.getX() < 0 ? -(thecircle.Radius<<2) : (thecircle.Radius<<2);	
	circlescale.y = thecircle.Movement.getY() < 0 ? -(thecircle.Radius<<2) : (thecircle.Radius<<2);
	//circlescale.x = thecircle.Movement.getX() < 0 ? -(thecircle.Radius) : (thecircle.Radius);
	//circlescale.y = thecircle.Movement.getY() < 0 ? -(thecircle.Radius) : (thecircle.Radius);

	//do Cramer's rule, yes, the one in Sienfeld...
	double divide = (circlescale.x*LineVect.y) - (LineVect.x*circlescale.y);
	double s, t;
	if(divide)
	{
		s = (DVect.x*LineVect.y) - (LineVect.x*DVect.y);
		t = (DVect.x*circlescale.y) - (circlescale.x*DVect.y);
	}
	else
		return false;

	double det1, det2;
	det1 = s/divide;
	det2 = t/divide;

	if((det1 < 0.0 || det1 > 1.0) 
		|| (det2 < 0.0 || det2 > 1.0))
		return false;*/

	double ptchecklow, ptcheckhi;
	if(theline.getP0().x < theline.getP1().x)
	{ ptchecklow = theline.getP0().x - CHECKPADDING; ptcheckhi = theline.getP1().x + CHECKPADDING; }
	else
	{ptchecklow = theline.getP1().x - CHECKPADDING; ptcheckhi = theline.getP0().x + CHECKPADDING; }

	if((thecircle.centerLoc.x + thecircle.Radius < ptchecklow) || (thecircle.centerLoc.x - thecircle.Radius > ptcheckhi))
		return false;
	else
	{
		if(theline.getP0().y < theline.getP1().y)
		{ ptchecklow = theline.getP0().y - CHECKPADDING; ptcheckhi = theline.getP1().y + CHECKPADDING; }
		else
		{ptchecklow = theline.getP1().y - CHECKPADDING; ptcheckhi = theline.getP0().y + CHECKPADDING; }

		if((thecircle.centerLoc.y + thecircle.Radius < ptchecklow) || (thecircle.centerLoc.y - thecircle.Radius > ptcheckhi))
		return false;
	}

	Vector3D d(theline.getP0().x - thecircle.centerLoc.x, theline.getP0().y - thecircle.centerLoc.y);
	Vector3D lineVect(theline.getP1().x-theline.getP0().x,theline.getP1().y-theline.getP0().y);
	
	//Vector3D lineVect(double(theline.getP0().x-theline.getP1().x),double(theline.getP0().y-theline.getP1().y));

	double a, b, c;

	a = lineVect.DotProd(lineVect);
	b = 2*(lineVect.DotProd(d));
	c = d.DotProd(d) - (thecircle.Radius*thecircle.Radius);

	double insideSqrt = b*b - (4*a*c);
	
	if(insideSqrt == 0) //there is collision on an exact point
	{
		if(theWeight)
		{
			theWeight->setXYZ(d.getX()/thecircle.Movement.getX(),
				d.getY()/thecircle.Movement.getY(), 0.0);
		}
		return true;
	}
	else if(insideSqrt > 0) //d'oh!
	{
		if(theWeight)
		{
			double quadthing;
			quadthing = (-b+sqrt(insideSqrt))/(2*a);
			lineVect *= quadthing;
			theWeight->setXYZ((d.getX()+lineVect.getX())/thecircle.Movement.getX(),
				(d.getY()+lineVect.getY())/thecircle.Movement.getY(), 0.0);
		}
		return true;
	}
	
	return false;
}

bool check_circle_to_poly(const CIRCLE &thecircle, const POLYGON &thepoly, Line *theLine, Vector3D *theWeight)
{
	Line polyline;

	int maxpts = thepoly.numPts - 1;
	for(int i = 0; i < maxpts; i++)
	{
		polyline.setPoints(thepoly.pts[i].x, thepoly.pts[i].y, thepoly.pts[i+1].x, thepoly.pts[i+1].y);

		if(check_circle_to_line(thecircle, polyline, theWeight))
			{ if(theLine) {*theLine = polyline;} return true; }
	}

	polyline.setPoints(thepoly.pts[thepoly.numPts - 1].x, thepoly.pts[thepoly.numPts - 1].y, thepoly.pts[0].x, thepoly.pts[0].y);
	if(check_circle_to_line(thecircle, polyline, theWeight))
	{ if(theLine) {*theLine = polyline;} return true; }

	return false;
}

bool check_circle_to_circle(const CIRCLE &circle1, const CIRCLE &circle2)
{
	//this is simple enough...
	double dx = circle1.centerLoc.x - circle2.centerLoc.x;
	double dy = circle1.centerLoc.y - circle2.centerLoc.y;
	double dr = circle1.Radius+circle2.Radius;

	if((dx*dx) + (dy*dy) <= dr*dr)
		return true;

	return false;
}

//
// Other stuff
//
RETCODE split_circle(PCIRCLE thisCircle)
{
	int nextIndex = thisCircle->ListIndex;
	if(thisCircle->sizetype == NUMTYPES - 1) 
	{
		//that means we are just going to destroy the circle because it is the smallest
		ListDelete(theGame->circle, thisCircle->ListIndex);
		goto FIXOTHERINDEX;
	}
	else
	{
		CIRCLE newCircle1, newCircle2;
		//change the radius and the size type
		newCircle1.sizetype = newCircle2.sizetype = thisCircle->sizetype + 1;
		newCircle1.Radius = newCircle2.Radius = thisCircle->Radius>>1;

		//set the color
		newCircle1.r = newCircle2.r = thisCircle->r;
		newCircle1.g = newCircle2.g = thisCircle->g;
		newCircle1.b = newCircle2.b = thisCircle->b;

		//set their gravity
		newCircle1.gravity = newCircle2.gravity = thisCircle->gravity;

		//set their location
		newCircle1.centerLoc.x = thisCircle->centerLoc.x - newCircle1.Radius;
		newCircle1.centerLoc.y = thisCircle->centerLoc.y;
		
		newCircle2.centerLoc.x = thisCircle->centerLoc.x + newCircle2.Radius;
		newCircle2.centerLoc.y = thisCircle->centerLoc.y;

		//do this so that they will bounce off correctly away from each other
		if(thisCircle->Movement.getX() < 0.0)
		{
			newCircle1.Movement.setX(thisCircle->Movement.getX());
			newCircle2.Movement.setX(-thisCircle->Movement.getX());
		}
		else
		{
			newCircle1.Movement.setX(-thisCircle->Movement.getX());
			newCircle2.Movement.setX(thisCircle->Movement.getX());
		}

		//do this so that it is garanteed that they will bounce up
		newCircle1.oldMovementy = newCircle2.oldMovementy = thisCircle->oldMovementy < 0 ? -thisCircle->oldMovementy : thisCircle->oldMovementy;

		newCircle1.Movement.setY(-newCircle1.oldMovementy);
		newCircle2.Movement.setY(-newCircle2.oldMovementy);

		//last but not least, the image they will use
		newCircle1.theImg = thisCircle->theImg;
		newCircle2.theImg = thisCircle->theImg;

		//we don't need the old circle
		ListDelete(theGame->circle, thisCircle->ListIndex);

		//append both circle
		ListAppend(theGame->circle, &newCircle1);
		ListAppend(theGame->circle, &newCircle2);
		//their index will be fixed later
	}

FIXOTHERINDEX:
	int maxCircle = ListGetNodeCount(theGame->circle);
		PCIRCLE otherCircle;

		for(int i = nextIndex; i < maxCircle; i++)
		{
			otherCircle = (PCIRCLE)ListExtract(theGame->circle, i);
			assert(otherCircle);
			otherCircle->ListIndex = i;
		}
	
	return RETCODE_SUCCESS;
}

void bounce_circle_to_circle(PCIRCLE circle1, PCIRCLE circle2)
{
	//bounce the circles away from each other depending on their movement vector direction
	if((circle1->Movement.getX() < 0 && circle2->Movement.getX() < 0)
		|| (circle1->Movement.getX() > 0 && circle2->Movement.getX() > 0))
	{
		if(circle1->Movement.getY() < 0 && circle2->Movement.getY() < 0)
		{
			//check to see who is on top, hehe
			if(circle1->centerLoc.y < circle2->centerLoc.y)
			{
				circle1->Movement.setXYZ(-circle1->Movement.getX(), circle1->Movement.getY(), 0.0);
				circle2->Movement.setXYZ(circle2->Movement.getX(), -circle2->Movement.getY(), 0.0);
			}
			else
			{
				circle1->Movement.setXYZ(circle1->Movement.getX(), -circle1->Movement.getY(), 0.0);
				circle2->Movement.setXYZ(-circle2->Movement.getX(), circle2->Movement.getY(), 0.0);
			}
		}
		else if(circle1->Movement.getY() > 0 && circle2->Movement.getY() > 0)
		{
			//check to see who is on top, hehe
			if(circle1->centerLoc.y < circle2->centerLoc.y)
			{
				circle1->Movement.setXYZ(circle1->Movement.getX(), -circle1->Movement.getY(), 0.0);
				circle2->Movement.setXYZ(-circle2->Movement.getX(), circle2->Movement.getY(), 0.0);
			}
			else
			{
				circle1->Movement.setXYZ(-circle1->Movement.getX(), circle1->Movement.getY(), 0.0);
				circle2->Movement.setXYZ(circle2->Movement.getX(), -circle2->Movement.getY(), 0.0);
			}
		}
		else
		{
			//inverse both y-direction
			circle1->Movement.setXYZ(circle1->Movement.getX(), -circle1->Movement.getY(), 0.0);
			circle2->Movement.setXYZ(circle2->Movement.getX(), -circle2->Movement.getY(), 0.0);
		}
	}
	else
	{
		if((circle1->Movement.getY() < 0 && circle2->Movement.getY() < 0)
			|| (circle1->Movement.getY() > 0 && circle2->Movement.getY() > 0))
		{
			//inverse both y-direction
			circle1->Movement.setXYZ(-circle1->Movement.getX(), circle1->Movement.getY(), 0.0);
			circle2->Movement.setXYZ(-circle2->Movement.getX(), circle2->Movement.getY(), 0.0);
		}
		else
		{
			//inverse them all
			circle1->Movement.setXYZ(-circle1->Movement.getX(), -circle1->Movement.getY(), 0.0);
			circle2->Movement.setXYZ(-circle2->Movement.getX(), -circle2->Movement.getY(), 0.0);
		}
	}

	//move the circle if they still collide
	while(check_circle_to_circle(*circle1, *circle2))
	{
		circle1->centerLoc.x += circle1->Movement.getX();
		circle1->centerLoc.y += circle1->Movement.getY();
		circle2->centerLoc.x += circle2->Movement.getX();
		circle2->centerLoc.y += circle2->Movement.getY();
	}
}

void bounce_circle_to_line(PCIRCLE theCircle, const Line &theLine)
{
	Vector3D invMovt(-theCircle->Movement.getX(), -theCircle->Movement.getY(), 0.0);
	Vector3D normal;
	Vector3D Lvect(theLine.getP1().x-theLine.getP0().x, theLine.getP1().y-theLine.getP0().y, 0.0);

	//if(theCircle->centerLoc.x >= theLine.getLeftMostX() && theCircle->centerLoc.x <= theLine.getRightMostX())
	//{
	//get the normal depending on the cross product
	Vector3D v1(theLine.getP0().x-theCircle->centerLoc.x, theLine.getP0().y-theCircle->centerLoc.y, 0.0);
	Vector3D v2(theLine.getP1().x-theCircle->centerLoc.x, theLine.getP1().y-theCircle->centerLoc.y, 0.0);
	
		if((v1.CrossProd(v2)).getZ() > 0)
			normal.setXYZ(-Lvect.getY(), Lvect.getX(), 0.0);
		else
			normal.setXYZ(Lvect.getY(), -Lvect.getX(), 0.0);
	/*}
	else
	{
		normal.setXYZ(Lvect.getY(), -Lvect.getX(), 0.0);
		if((normal.getX()*(theCircle->Movement.getY()*theCircle->Radius) - (theCircle->Movement.getX()*theCircle->Radius)*normal.getY()) > 0)
			normal.setXYZ(-Lvect.getY(), Lvect.getX(), 0.0);
	}*/

	//now get the new vector of the circle
	theCircle->Movement += (normal.ProjOnto(invMovt)*2);

	//beginning of hack code that doesn't work quite well
	if(theCircle->Movement.getX() < 0)
	{
		if((theCircle->Movement.getX()*-1) < 1)
			theCircle->Movement.setX(theCircle->Movement.getX()-REFLECTPADDING);
	}
	else if(theCircle->Movement.getX() > 0)
	{
		if(theCircle->Movement.getX() < 1)
			theCircle->Movement.setX(theCircle->Movement.getX()+REFLECTPADDING);
	}

	if(theCircle->Movement.getY() < 0)
	{
		if((theCircle->Movement.getY()*-1) < 1)
			theCircle->Movement.setY(theCircle->Movement.getY()-REFLECTPADDING);
	}
	else if(theCircle->Movement.getY() > 0)
	{
		if(theCircle->Movement.getY() < 1)
			theCircle->Movement.setY(theCircle->Movement.getY()+REFLECTPADDING);
	}

	//move them out of the god damn line!!!
	while(check_circle_to_line(*theCircle, theLine))
	{
		theCircle->centerLoc.x += invMovt.getX()*HACKDISPLACEMENT;
		theCircle->centerLoc.y += invMovt.getY()*HACKDISPLACEMENT;
	}
	/*while(check_circle_to_line(*theCircle, theLine))
	{
		theCircle->centerLoc.x += theCircle->Movement.getX();
		theCircle->centerLoc.y += theCircle->Movement.getY();
	}*/
	//end of hack code

	if(theCircle->Movement.getX() > MAXBALLSPEED)
		theCircle->Movement.setX(theCircle->oldMovementx < 0 ? -theCircle->oldMovementx : theCircle->oldMovementx);
	else if(theCircle->Movement.getX() < -MAXBALLSPEED)
		theCircle->Movement.setX(theCircle->oldMovementx > 0 ? -theCircle->oldMovementx : theCircle->oldMovementx);

	if(theCircle->Movement.getY() > MAXBALLSPEED)
		theCircle->Movement.setY(theCircle->oldMovementy < 0 ? -theCircle->oldMovementy : theCircle->oldMovementy);
	else if(theCircle->Movement.getY() < -MAXBALLSPEED)
		theCircle->Movement.setY(theCircle->oldMovementy > 0 ? -theCircle->oldMovementy : theCircle->oldMovementy);

	/*while(check_circle_to_line(*theCircle, theLine))
	{
		theCircle->centerLoc.x += theCircle->Movement.getX();
		theCircle->centerLoc.y += theCircle->Movement.getY();
	}*/
}

//
// highscore saving
//
RETCODE save_scores(char *path)
{
	FILEHANDLE thefile = NULL;

	thefile = FileOpenFile (path, FILEWRITE | FILETEXT);

	if(!thefile)
	{ MESSAGE_BOX("Unable to open high score file!", "Error saving scores"); return RETCODE_FAILURE; }
	for(int i = 0; i < MAXSCORE; i++)
	{
		fprintf(thefile, "%s\n", theGame->theScores.highscore[i].playerName);
		fprintf(thefile, "%d\n", theGame->theScores.highscore[i].score);
	}

	FileCloseFile (thefile);
	return RETCODE_SUCCESS;
}

/*

  	DOUBLEPT LineVect = {theline.getP1().x-theline.getP0().x, theline.getP1().y-theline.getP0().y};
	DOUBLEPT DVect = {theline.getP0().x-thecircle.centerLoc.x, theline.getP0().y-thecircle.centerLoc.y};

	//do Cramer's rule, yes, the one in Sinfeld...
	double divide = (thecircle.Movement.getX()*LineVect.y) - (LineVect.x*thecircle.Movement.getY());
	double s, t;
	if(divide)
	{
		s = (DVect.x*LineVect.y) - (LineVect.x*DVect.y);
		t = (DVect.x*thecircle.Movement.getY()) - (thecircle.Movement.getX()*DVect.y);
	}
	else
		return false;

	double det1, det2;
	det1 = s/divide;
	det2 = t/divide;

	if((det1 >= 0.0 && det1 <= 1.0) 
		|| (det2 >= 0.0 && det2 <= 1.0))
	{
		theWeight->setXYZ(thecircle.centerLoc.x + (thecircle.Movement.getX()*det1), thecircle.centerLoc.y + (thecircle.Movement.getY()*det1), 0.0);
		return true;
	}

	return false;

	*/
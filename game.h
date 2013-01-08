#ifndef _game_h
#define _game_h

#include "File\filetool.h"
#include "List\list.h"
#include "Memory\memory.h"
#include "Timer\timer.h"
#include "Graphics\gdi.h"
#include "MyInput\input.h"
#include "vector.h"

#define VULNERABLE 1

#define BOUNDARYOFFSIZE 32

#define BACKGROUNDLAYER 0
#define PLAYERLAYER 1

#define SCOREAMT 50

#define NUMPTSHOOKSHOT 4
#define HOOKSIZE 5

#define KEYBOARDDELAY 40

#define BUTTON_OFF 0
#define BUTTON_ON 1

#define RETCODE_REQUESTTERMINATE 3

typedef struct _DOUBLEPT {
	double x, y;
} DOUBLEPT, * PDOUBLEPT;

/*#define VERYBIGSIZE 64
#define BIGSIZE VERYBIGSIZE>>1
#define MEDIUMSIZE BIGSIZE>>1
#define SMALLSIZE MEDIUMSIZE>>1
#define VERYSMALLSIZE SMALLSIZE>>1*/

typedef enum {
	VERYBIG,
	BIG,
	MEDIUM,
	SMALL,
	VERYSMALL,
	NUMTYPES
} CIRCLESIZETYPE;

#define MAXBALLSPEED 10 //when the ball is travelling too darn

typedef struct _BALLIMG {
	SPRITEHANDLE theimg[NUMTYPES];
} BALLIMG, *PBALLIMG;

typedef struct _CIRCLE {
	int ListIndex; //index within the list
	PBALLIMG theImg; //the sprite to be blitted
	DOUBLEPT centerLoc; //center of circle
	long Radius;		//the size of circle
	int sizetype; //If the circle is big, medium, small or very small.
	Vector3D Movement; //the speed
	double gravity;		//the pull of gravity downward
	double oldMovementy; //the original movement vector y modifier
	double oldMovementx; //the original movement vector x modifier
	int r,g,b;			//r=Red g=Green b=Blue
} CIRCLE, * PCIRCLE;

typedef struct _POLYGON {
	LOCATION *pts;	//a collection of points
	long numPts;	//number of points
	int r,g,b;		//r=Red g=Green b=Blue
} POLYGON, * PPOLYGON;

typedef enum {
	PLAYER_HOOKSHOTFIRED = 1,
	PLAYER_NOHOOKSHOTFIRED,
	PLAYER_DEATHINIT,
	PLAYER_DEATHLOOP
} PLAYERSTATUS;

#define MAXTIME 120
#define INGAMETIMEDELAY 1000

#define DEATHFORCE -10
#define DEATHPULL .25

typedef struct _PRASANNA {
	PLOCATION location;		 //the upper left boundary location
	SPRITEHANDLE Sprite;  //this is what makes this data structure called "PRASANNA" (this is filled when inGameActivate is called)
	short SpriteID; //For copying the specified sprite ID to the appropriate layer when the game begins
	short layer;	//the layer the sprite ID will be copied when game begins
	int maxLife;	//max number of life
	int currentnumLife;	//current number of life
	int currenttime; //the current time
	unsigned int score; //current score
	unsigned int scoreholder; //used when advancing level and when the player dies
	short moveLeftState; //the sprite state when moving left
	short moveStandingLeftState; //the sprite state when standing facing left
	short moveRightState; //the sprite state when moving right
	short moveStandingRightState; //the sprite state when standing facing right
	short moveFrontState; //the sprite state for moving down
	short moveStandingFrontState; //the sprite state when standing facing front
	short moveBackState; //the sprite state for moving up
	short moveStandingBackState; //the sprite state when standing facing back
	short moveDeathState; //the death state
	short moveWinState; //the winning state
	double deathjump; //used only in LOOP_INGAMEDEATH, muwahahaha!
	POLYGON HookShot; //The hookshot that Prasanna shoots around
	Line HookLine; //the hookshot chord
	LOCATION *HookPts;	//the points that are loaded from the config
	int HookShotSpd; //The vertical speed of the hook shot
	int PlayerSpd; //the speed of the player when moving
	CIRCLE circleCollision; //the circle collision
	FLAGS Status;		 //Status such as alive, sick, is HookShot released...etc.
} PRASANNA, * PPRASANNA;

typedef enum {
	PLAYBUTTON,
	HIGHSCOREBUTTON,
	EXITBUTTON,
	MAXBUTTON
} eMAINMENU;

typedef struct _MAINMENUBUTTON {
	SPRITEHANDLE Sprite; //the play button sprite
	short ID; //the play button sprite ID
	LOCATION BtnLoc; //location to blit the button
} MAINMENUBUTTON, * PMAINMENUBUTTON;

typedef struct _MAINMENU {
	short BackgroundSpriteID; //the cool looking background sprite ID for the main menu
	short BackgroundLayer; //the layer of the background
	short ButtonLayer; //the layer where all buttons will be copied to
	MAINMENUBUTTON buttons[MAXBUTTON]; //the buttons
	int currentSelection; //the current button selected
	DWORD keyboardDelay;
} MAINMENU, *PMAINMENU;

#define MAXSCORE 10
#define MAXNAMEBUFF 12

typedef struct _HIGHSCOREDATA {
	char playerName[MAXCHARBUFF]; //the player name
	unsigned int score;	//the corresponding score
} HIGHSCOREDATA, *PHIGHSCOREDATA;

typedef struct _HIGHSCORE {
	short BackgroundSpriteID; //the cool looking background sprite ID for high score
	short BackgroundLayer; //the layer of the background
	char scorefilepath[MAXCHARBUFF]; //for saving the scores
	char buffer[MAXNAMEBUFF]; //used for typing in the name
	int win_index;			  //the index to insert the new person on the top-ten
	HIGHSCOREDATA highscore[MAXSCORE]; //the top-ten list of players and their score
} HIGHSCORE, * PHIGHSCORE;

typedef enum {
	BOUNDARY_TOP,
	BOUNDARY_RIGHT,
	BOUNDARY_BOTTOM,
	BOUNDARY_LEFT,
	MAXBOUNDARYSIDE
} BOUNDARYSIDES;

typedef struct _GAMEDATA {
	TIMERHANDLE gameTimer;	//the game delay timer
	TIMERHANDLE frameTimer;	//the display delay timer
	TIMERHANDLE keyboardTimer; //used in certain places
	TIMERHANDLE ingameTimer; //the time decrementer for the level
	char **levelpaths; //array of level path to load
	FONTHANDLE theFont;	//font for text display
	int maxLevel;		//maximum level
	int curlevel;		//the current level
	RECT boundary;		//Boundary is so that nothing goes off the screen...hehe
	Line boundarylines[MAXBOUNDARYSIDE]; //the four sides of the boundary
	LISTHANDLE polygon;	//The list of polygons
	int numCircleLoaded; //number of circles loaded from file
	LISTHANDLE circle;  //Collection of moving circles
	PBALLIMG circleImg; //the images for each ball loaded from level, corresponds to the circle list
	PRASANNA Ghali;		//This is Prasanna Ghali
	MAINMENU theMenu;	//Contains necessary things for main menu
	HIGHSCORE theScores; //Contains necessary things for high score
	DWORD gameDelay, frameDelay, ingameDelay; //delay
	int currentLoopType; //Used for accessing the function table
	int currentLoopMessage; //Used for the function table
	int ha; //ha?
	char haha; //haha!
	short FXspriteID; //in here for testing
} GAMEDATA, *PGAMEDATA;

//
//Initialization and Deinitialization
//
RETCODE init_game(VTYPE graphicsmode, char *gameCfgFile, DWORD gameDelay = 10, DWORD frameDelay = 30, Pdisplaymode mode = NULL);
PRIVATE RETCODE load_gamecfg(char *gameCfgFile);
void destroy_game();

PRIVATE void destroy_level_data();
PRIVATE RETCODE load_level_data(char *path);

//
// The one used by main
//
RETCODE update_stuff();

//
//Management
//

//these are for activating display for each loops
RETCODE ActivateMainMenuDisplay();
RETCODE ActivateInGameDisplay();
RETCODE ActivateScoreDisplay();

typedef enum {
	LOOP_INIT,
	LOOP_DOSTUFF,
	LOOP_INGAMELVLINTRO,
	LOOP_INGAMELOADLEVEL,
	LOOP_INGAMEDEATH,
	LOOP_INGAMEWINNER,
	LOOP_HIGHSCOREGETNAME
} LOOPMESSAGE;

typedef enum {
	MENULOOP,
	INGAMELOOP,
	HIGHSCORELOOP,
	MAXLOOPTYPE
} LOOPTYPE;


//used for game loop function table
//the type pointer and message pointer is changed by the loop themselves.
typedef long (* GAMELOOP) ();

//These are the game loops
PRIVATE RETCODE MainLoop(); //this is the one that holds all the other loop
PRIVATE RETCODE InGameLoop();
PRIVATE RETCODE HighScoreLoop();

//used only within in-game loop
PRIVATE void draw_level();

//updates for screen and buffer
//these are called by all game loops
//PRIVATE inline RETCODE update_screen() { GraphicsUpdateScreen (&G_clientarea); }
PRIVATE RETCODE update_display();

//
// Collision stuff
//

//returns true if collided

bool check_collision_box(const RECT rect1, const RECT rect2);

bool check_line_intersect(const POINT l1p0, const POINT l1p1, const POINT l2p0, const POINT l2p1, POINT *intersect);
bool check_poly_intersect(const POLYGON &poly, Line line);

bool check_circle_to_line(const CIRCLE &thecircle, const Line &theline, Vector3D *theWeight = NULL);

bool check_circle_to_poly(const CIRCLE &thecircle, const POLYGON &thepoly, Line *theLine = NULL, Vector3D *theWeight = NULL);

bool check_circle_to_circle(const CIRCLE &circle1, const CIRCLE &circle2); 

//
// other stuff
//

RETCODE split_circle(PCIRCLE thisCircle);
void bounce_circle_to_circle(PCIRCLE circle1, PCIRCLE circle2);
void bounce_circle_to_line(PCIRCLE theCircle, const Line &theLine);

//
// highscore saving
//
RETCODE save_scores(char *path);


#endif
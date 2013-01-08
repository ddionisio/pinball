#ifndef COMMON_H 
#define COMMON_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/
/*#ifndef	_MSC_VER
	// The Borland compiler gives lots of warnings if you redefine the assert
	// macro - I haven't explored how to avoid them so I'll avoid redefining it.
	//yeah whatever, Bruce...
	#define	USEMICROSOFTASSERTDIALOG
#endif
#ifdef	USEMICROSOFTASSERTDIALOG*/
	#include <assert.h>
/*#else
	#undef	assert	// Make sure we've wiped out any previous definitions.
	#ifdef  NDEBUG
		// In release mode, asserts do nothing.
		#define assert(exp)     ((void)0)
	#else
		// In debug mode we need an assert macro and an assert function.
		// _csassert() returns true if the user requests debugging.
		bool _csassert(const char* expr, const char* filename, unsigned int lineno);
		#define assert(exp) \
				do { \
					if (!(exp)) { \
						dprintf("%s(%d): Assertion failed\n%s\n", __FILE__, __LINE__, #exp); \
						if (_csassert(#exp, __FILE__, __LINE__)) \
							DEBUG_BREAK(); \
						} \
				} while (0)
	#endif
#endif*/

#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
// For SelectBitmap(), etc.
#include <windowsx.h>

/********************************************************************
*																	*
*							Qualifiers								*
*																	*
********************************************************************/

#define PUBLIC				// Semantic used to indicate publicly exposed methods
#define PROTECTED			// Semantic used to indicate methods shared among implementations
#define PRIVATE		static	// Semantic used to indicate implementation-specific attributes and methods

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

#define PATH_SIZE	80
#define MAX_FILES   15
#define MAXCHARBUFF 255

#define BASE_AMOUNT		1	// Count of elements of base array
#define BASE_EXTENDED	1	// Index of element beyond base array

//
// More fun stuff
//
#define MAXPARAM -1
#define MINPARAM -1

/********************************************************************
*																	*
*							Return codes							*
*																	*
********************************************************************/

#define RETCODE_FAILURE	0x0	// General-purpose failure
#define RETCODE_SUCCESS	0x1	// General-purpose success

#define COMPARE_LT	-1	// Less than result of compare method
#define COMPARE_EQ	 0	// Equal result of compare method
#define COMPARE_GT	 1	// Greater than result of compare method

#define EQUIVALENCE_FALSE	0	// False result of equivalence method
#define EQUIVALENCE_TRUE	1	// True result of equivalence method

#define SEARCH_NOTFOUND	-1	// Indicates search without a find

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

typedef BYTE  Byte,  * Pbyte;
typedef WORD  Word,  * Pword;
typedef DWORD Dword, * Pdword;

typedef long AMOUNT;	// General quantity type
typedef long FLAGS;		// General flags type
typedef long INDEX;		// General index type
typedef long MESSAGE;	// Callback procedure message
typedef long RETCODE;	// Return code type

typedef POINT LOCATION, * PLOCATION;
typedef POINT OFFSET,   * POFFSET;
typedef RECT  VIEWPORT, * PVIEWPORT;

/********************************************************************
*																	*
*							Handles									*
*																	*
********************************************************************/

typedef struct _LIST  * LISTHANDLE;	// Public handle to linked list object
typedef struct _TIMER * TIMERHANDLE;// Public handle to timer object

typedef HWND WINHANDLE, WINDOW_CONTEXT,		 * PWINDOW_CONTEXT;
typedef HINSTANCE SOMEINSTANCE, APP_CONTEXT, * PAPP_CONTEXT;
typedef HDC DEVICECONTEXT, DEVICE_CONTEXT,   * PDEVICE_CONTEXT;

/********************************************************************
*																	*
*							Methods									*
*																	*
********************************************************************/
													
typedef RETCODE (* COMPARE)		(void *, void *);	// Callback function for sorting comparisons 
typedef RETCODE (* EQUIVALENCE) (void *, void *);	// Callback function for searching comparisons

/********************************************************************
*																	*
*							Access functions						*
*																	*
********************************************************************/

PUBLIC Word GetMaxBullets (void);
PUBLIC Word GetMaxLayers  (void);

PUBLIC Dword GetMemPoolSize (void);

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define MESSAGE_BOX(Message,Caption) MessageBox (NULL, Message, Caption, MB_OK)	// User messages

#define NUM_ELEMENTS(array)	((sizeof(array)) / (sizeof((array) [0]))) // Get the element count of a static array

#define BYTE_EXTRACT(data)		*(Pbyte) (data)	// Extract byte data from a pointer
#define TYPE_EXTRACT(type,data)	*(type*) (data)	// Extract typed data from a pointer

#define BYTE_CAST(data)			((Byte) (data))	// Cast a data item to a Byte
#define TYPE_CAST(type,data)	((type) (data))	// Cast a data item to a given type

#define SETFLAG(data,flag)   (data) |= (flag)	// Set a flag value in a datum
#define CLEARFLAG(data,flag) (data) &= ~(flag)	// Clear a flag value in a datum
#define FLIPFLAG(data,flag)	 (data) ^= (flag)	// Flip a flag value in a datum

#define GETFLAGS(data,mask)	 ((data) & (mask))	// Grab flags from a datum

#define MAX(one,two) (one) > (two) ? (one) : (two)
#define MIN(one,two) (one) < (two) ? (one) : (two)

#define MAKETAG_4(c1,c2,c3,c4) ((((c1) & 0xFF) << 24) | (((c2) & 0xFF) << 16) | (((c3) & 0xFF) << 8) | ((c4) & 0xFF))
// Constructs an integer tag from four characters
#define MAKE_ID(group,member) ((((group) & 0xFFFF) << 16) | ((member) & 0xFFFF))
// Constructs an integer ID from two shorts

#define CORRECT_FROM_ASCII(ch) ((ch) -= '0')
// Used to convert an ASCII character to a byte

//Gets the index of an array as if it was a double array
//Rip-off from the CS170 sliding puzzle assignment, hehehe
#define BOARDPOS(board, row, col, numcols) (board[((row) * (numcols)) + (col)])

//
// Image Types
//
typedef enum {
	IMG_BMP = 1,
	IMG_BUFFER //this means that it is created within the program
} IMGTYPE;

//
// In here for now...Used for creating full screen with a non-default mode
//

typedef struct _displaymode {
	ULONG width; //width of screen
	ULONG height; //height of screen
	ULONG colorbits; //amt. of color
	ULONG refreshrate; //the refresh rate
} displaymode, * Pdisplaymode;

#endif // COMMON_H
#ifndef	CSTYPES_H
#define	CSTYPES_H

// Copyright © 1999-2000 Bruce Dawson.
//Modified by David Dionisio

#include "..\common.h"


// This is a set of typedefs that I like to have available.
// It is appropriate to use these whenever the precise
// number of bits genuinely _needs_ to be specified. All
// other times you should use int or unsigned int so that
// the compiler can use whatever size is most efficient.
//
// These typedef names were chosen because they are part of
// the new ISO C standard.

typedef signed char int8_t;
typedef signed short int16_t;
typedef signed int int32_t;

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

// These types are part of C++ and they are useful for making it
// clear when you are storing true/false values instead of ints.
// The #ifndef is to define them if you are compiling this file
// as a C source file - when compiling as C++ they don't need
// to be defined.
#ifndef	__cplusplus
typedef unsigned char bool;
#define	true	1
#define	false	0
#endif

#ifdef	_MSC_VER	// True when compiling with VC++
	// This extremely wacky and scary #define is used to force VC++ to
	// use the C++ standard scoping rules for variables declared inside
	// of for loops. Trust me - it's a good thing.
	#define for if (0) ; else for
	#define	DEBUG_BREAK()	__asm { int 3 }
#else
#error
	#define	DEBUG_BREAK() DebugBreak()
#endif

// Asserts are good. However Microsoft's implementation is not always good.
// It can very easily cause infinitely recursive asserts, because when the
// assert dialog appears it runs a message pump, which can cause messages
// to be delivered to your window - which can cause your assert to trigger
// again.
// Recursion: n. see recursion.
// Also, if you click 'Debug' on one of Microsoft's assert you get dropped
// into the assert.c source file, instead of into your own source code.
// This assert macro fixes these problems.
// Define this if you don't want the fixed assert dialogs.
//#define	USEMICROSOFTASSERTDIALOG


// This is a tremendously useful macro that calculates how many
// elements are in an array. The calculations are done at compile
// time, so it is very efficient.
#define	NUMELEMENTS(x)	(sizeof(x) / sizeof(x[0]))

// rassert is an assert macro that stays in in release builds.
// It doesn't do much - just crash, or break into the debugger.
// However that is frequently much better than continuing on
// in a bad state.
// Also, with a .pdb file that's all you need to trace where
// the problem occurred. Use these for important checks where
// the modest overhead won't be too great. If an exception
// handler is initialized then a full error log will be recorded
// if a release assert is triggered outside of the debugger.

#ifdef	_DEBUG
	// Use the normal assert macro in debug builds.
	#define rassert(exp)	assert(exp)
#else
	// Define a custom assert macro in release builds.
	#define rassert(exp) \
				do { \
					if (!(exp)) { \
						DEBUG_BREAK(); \
					} \
				} while (0)
#endif

char *GetFilePart(const char *PathName);
char *GetExtension(const char *PathName);
bool GetFileName(HWND Window, char* FileName, size_t FileNameSize,
				const char* Filter, const char* Title, const char* Extension,
				DWORD Flags, bool OpenFileDialog);

double CS_GetTime();

// rdprintf is always compiled in. dprintf only exists in debug builds.
// rdprintf and dprintf are for printing to the debugger with
// OutputDebugString.
void __cdecl rdprintf( const char *buf, ... );

#if	defined(_DEBUG) || defined(ALLOWDPRINTF)
	#define	dprintf	rdprintf
#else
	// Wacky macro to completely remove dprintf from release mode
	// programs. Don't try this at home...
	#define	dprintf	0 &&
#endif

#define	MAXIT(x, y)	(x > y ? x : y)
#define	MINIT(x, y)	(x < y ? x : y)

#endif	// CSTYPES_H

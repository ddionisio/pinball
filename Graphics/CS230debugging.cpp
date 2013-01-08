#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

// Copyright © 1999 Bruce Dawson.

void __cdecl rdprintf( const char *buf, ... )
{
	char buffer[1024];

	va_list arglist;

	va_start( arglist, buf );

	// It is possible that all bytes will not be printed. Oh well - 
	// we're not going to try to handle that.
	_vsnprintf( buffer, sizeof(buffer), buf, arglist );
	buffer[sizeof(buffer)-1] = 0;	// Make sure the string is always null terminated.

	va_end( arglist );

	OutputDebugString(buffer);
}

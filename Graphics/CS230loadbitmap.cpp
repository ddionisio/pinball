#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

// Copyright © 1999 Bruce Dawson.

#include "csbitmap.h"

bool LoadBitmapFile(const char* FileName, CSBitmap* DestBitmap, char** ErrorMessage)
{
	char* LastPeriod = strrchr(FileName, '.');
	if (!LastPeriod)
	{
		if (ErrorMessage)
			*ErrorMessage = "No file extension found.";
		return false;
	}

	if (stricmp(LastPeriod, ".pcx") == 0)
		return LoadPCXFile(FileName, DestBitmap, ErrorMessage);

	if (stricmp(LastPeriod, ".bmp") == 0)
		return LoadBMPFile(FileName, DestBitmap, ErrorMessage);

	if (ErrorMessage)
		*ErrorMessage = "Unsupported file format.";
	return false;
}

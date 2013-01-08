#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

// Copyright © 1999 Bruce Dawson.

#include "csbitmap.h"
#include <math.h>	// For pow

// Pass in the desired number of levels, between two and 255.
// Two is 'monochrome' (actually eight colours) and 255 is
// normal.
void QuantizeBitmap(CSBitmap* pBitmap, int Levels)
{
	int	x, y;
	// Quantizing 8-bit bitmaps doesn't really work...
	if (GetChannels(pBitmap) < 3)
		return;
	assert(Levels <= 255);
	if (Levels >= 255)
		return;
	assert(Levels >= 2);
	Levels--;	// Makes the math eaiser...
	for (y = 0; y < GetHeight(pBitmap); y++)
	{
		uint8_t* pLine = GetLinePtr(pBitmap, y);
		for (x = 0; x < GetWidth(pBitmap); x++)
		{
			// Weird math eh?
			// Multiply each pixel by Levels/255, with appropriate rounding.
			// Then multiply that by 255/Levels, with appropriate rounding.
			// This leaves the numbers in the same range, but quantizes them
			// in the desired manner (to the zero to Levels range) because
			// it is all done with integer math.
			pLine[0] = (((pLine[0] * Levels + 127) / 255) * 255 + Levels / 2) / Levels;
			pLine[1] = (((pLine[1] * Levels + 127) / 255) * 255 + Levels / 2) / Levels;
			pLine[2] = (((pLine[2] * Levels + 127) / 255) * 255 + Levels / 2) / Levels;
			pLine += GetChannels(pBitmap);
		}
	}
}

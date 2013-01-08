#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

#include "csbitmap.h"

// Copyright © 2000 Bruce Dawson.

void DrawPattern(CSBitmap* pBitmap, int Width, int Height, int Channels)
{
	// This code assumes that we have at least three channels available.
	assert(Channels >= 3);
	// Try to make the bitmap the requested size.
	if (!SetSize(pBitmap, Width, Height, Channels))
		return;

	// Loop over all of the lines in the image.
	for (int y = 0; y < GetHeight(pBitmap); y++)
	{
		// Get the address of the current line.
		uint8_t* pLine = GetLinePtr(pBitmap, y);
		// Loop over all pixels in the image.
		for (int x = 0; x < GetWidth(pBitmap); x++)
		{
			// Set the red and blue components to zero, and set
			// the green component to the sum of x and y, to give
			// it a nice pattern.
			pLine[RED_BITMAP_OFFSET] = 0;
			pLine[GREEN_BITMAP_OFFSET] = (uint8_t)(x + y);
			pLine[BLUE_BITMAP_OFFSET] = 0;
			// Skip to the next pixel.
			pLine += GetChannels(pBitmap);
		}
	}
}

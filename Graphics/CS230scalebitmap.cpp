#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

// Copyright © 1999 Bruce Dawson.

#include "csbitmap.h"

// Adjust this to get optimized and non-optimized versions.
bool gScalingOptimization = true;

static void FilterScaleBitmap(CSBitmap* SourceBitmap, CSBitmap* DestBitmap)
{
	const int NumSourceChannels = GetChannels(SourceBitmap);
	int NumSourceSkipChannels = NumSourceChannels;
	const int NumDestChannels = GetChannels(DestBitmap);
	const int kNumFilterChannels = 3;	// Number of channels to filter.
	// Bilinear filtering.
	// Simple floating point code, with precalculated x-locations.
	double YRatio = GetHeight(SourceBitmap) / (double)GetHeight(DestBitmap);
	double XRatio = GetWidth(SourceBitmap) / (double)GetWidth(DestBitmap);

	RGBQUAD	paletteRGBQUAD[256];
	uint8_t	bytePalette[256 * kNumFilterChannels];
	if (NumSourceChannels == 1)
	{
		GetPalette(SourceBitmap, 0, 256, paletteRGBQUAD);
		NumSourceSkipChannels = kNumFilterChannels;
		// After getting the palette, copy it into a layout that is guaranteed to match that
		// of pixels in a bitmap.
		for (int i = 0; i < NUMELEMENTS(paletteRGBQUAD); ++i)
		{
			bytePalette[i * kNumFilterChannels + RED_BITMAP_OFFSET] = paletteRGBQUAD[i].rgbRed;
			bytePalette[i * kNumFilterChannels + GREEN_BITMAP_OFFSET] = paletteRGBQUAD[i].rgbGreen;
			bytePalette[i * kNumFilterChannels + BLUE_BITMAP_OFFSET] = paletteRGBQUAD[i].rgbBlue;
		}
	}
	// Optimized version of filtered scaling.
	// Should probably use new[], but I'm trying to stick to C style code.
	float* SrcXBuffer = (float*)malloc(sizeof(float) * GetWidth(DestBitmap));
	if (!SrcXBuffer)
		return;
	// Precalculate the sourceX values, including left edge clipping them.
	for (int x = 0; x < GetWidth(DestBitmap); ++x)
	{
		double SourceX = (x + 0.5) * XRatio;
		assert(SourceX >= 0 && SourceX < GetWidth(SourceBitmap));
		SourceX -= 0.5;	// Adjust into a more convenient range. Now represents
		// the left edge of the pixel.
		// Deal with the unfortunate border cases. We are clamping the pixels.
		// Other possibilities include wrapping, or using a border colour.
		if (SourceX < 0)
			SourceX = 0;
		if (SourceX >= GetWidth(SourceBitmap) - 1)
			SourceX = GetWidth(SourceBitmap) - 1;
		SrcXBuffer[x] = (float)SourceX;
	}
	for (int y = 0; y < GetHeight(DestBitmap); ++y)
	{
		double SourceY = (y + 0.5) * YRatio;
		assert(SourceY >= 0 && SourceY < GetHeight(SourceBitmap));
		SourceY -= 0.5;	// Adjust into a more convenient range. Now represents
		// the left edge of the pixel.
		// Deal with the unfortunate border cases. We are clamping the pixels.
		// Other possibilities include wrapping, or using a border colour.
		if (SourceY < 0)
			SourceY = 0;
		if (SourceY > GetHeight(SourceBitmap) - 1)
			SourceY = GetHeight(SourceBitmap) - 1;
		// If SourceY might be negative you have to use floor() instead of just casting to int.
		int FirstLine = (int)SourceY;
		double SecondLineWeight = SourceY - FirstLine;

		// We don't need asserts here because GetLinePtr() will check the y-coordinate.
		uint8_t* pDestLine = GetLinePtr(DestBitmap, y);
		uint8_t* pSourceLine1 = GetLinePtr(SourceBitmap, FirstLine);
		// Make sure we don't try getting the address of a non-existent line.
		// Make sure this always gets initialized to something.
		uint8_t* pSourceLine2 = pSourceLine1;
		if (FirstLine + 1 < GetHeight(SourceBitmap))
			pSourceLine2 = GetLinePtr(SourceBitmap, FirstLine + 1);
		uint8_t* pSourcePixel1Left;	// Top line, left pixel.
		uint8_t* pSourcePixel2Left;	// Second line, left pixel.
		for (int x = 0; x < GetWidth(DestBitmap); ++x)
		{
			double SourceX = SrcXBuffer[x];
			int FirstPixel = int(SourceX);
			// Set up pointers into the bitmap or the palette, for the left sample pixels.
			if (NumSourceChannels == 1)
			{
				pSourcePixel1Left = bytePalette + pSourceLine1[FirstPixel] * kNumFilterChannels;
				pSourcePixel2Left = bytePalette + pSourceLine2[FirstPixel] * kNumFilterChannels;
			}
			else
			{
				pSourcePixel1Left = pSourceLine1 + FirstPixel * NumSourceChannels;
				pSourcePixel2Left = pSourceLine2 + FirstPixel * NumSourceChannels;
			}
			uint8_t* pSourcePixel1Right;	// Top line, right pixel.
			uint8_t* pSourcePixel2Right;	// Second line, right pixel.
			// Now setup pointers into the bitmap or the palette, for the right
			// sample pixels. If there are no right sample pixels (right edge)
			// then point at the left sample pixels.
			if (SourceX >= GetWidth(SourceBitmap) - 1)
			{
				pSourcePixel1Right = pSourcePixel1Left;
				pSourcePixel2Right = pSourcePixel2Left;
			}
			else
			{
				if (NumSourceChannels == 1)
				{
					pSourcePixel1Right = bytePalette + pSourceLine1[FirstPixel+1] * kNumFilterChannels;
					pSourcePixel2Right = bytePalette + pSourceLine2[FirstPixel+1] * kNumFilterChannels;
				}
				else
				{
					pSourcePixel1Right = pSourcePixel1Left + NumSourceChannels;
					pSourcePixel2Right = pSourcePixel2Left + NumSourceChannels;
				}
			}
			double SecondPixelWeight = SourceX - FirstPixel;
			for (int channel = 0; channel < kNumFilterChannels; ++channel)
			{
				double FirstLine = pSourcePixel1Left[channel] + (pSourcePixel1Right[channel] - pSourcePixel1Left[channel]) * SecondPixelWeight;
				double SecondLine = pSourcePixel2Left[channel] + (pSourcePixel2Right[channel] - pSourcePixel2Left[channel]) * SecondPixelWeight;
				pDestLine[channel] = (uint8_t)(FirstLine + (SecondLine - FirstLine) * SecondLineWeight);
			}
			pDestLine += NumDestChannels;
		}
	}
	free(SrcXBuffer);
}

static void PointScaleBitmap(CSBitmap* SourceBitmap, CSBitmap* DestBitmap)
{
	int NumChannels = GetChannels(SourceBitmap);
	assert(GetChannels(DestBitmap) == GetChannels(SourceBitmap));

	if (gScalingOptimization)
	{
		// Point sampling.
		// Pre calculated with line copy of identical lines.
		// Should probably use new[], but I'm trying to stick to C style code.
		int* SrcXBuffer = (int*)malloc(sizeof(int) * GetWidth(DestBitmap));
		int LastSourceY = -1;
		uint8_t* pLastLine = 0;
		if (!SrcXBuffer)
			return;
		for (int x = 0; x < GetWidth(DestBitmap); ++x)
		{
			int SourceX = (x * GetWidth(SourceBitmap) + GetWidth(SourceBitmap) / 2) / GetWidth(DestBitmap);
			// Make darned sure our input coordinates are valid.
			assert(SourceX >= 0 && SourceX < GetWidth(SourceBitmap));
			SrcXBuffer[x] = SourceX * NumChannels;
		}
		// Loop over all destination lines.
		for (int y = 0; y < GetHeight(DestBitmap); ++y)
		{
			// I don't bother optimizing the outer loop.
			// Calculate the line number we want to read from.
			int SourceY = (y * GetHeight(SourceBitmap) + GetHeight(SourceBitmap) / 2) / GetHeight(DestBitmap);
			// Get the destination line pointer.
			uint8_t* pDestLine = GetLinePtr(DestBitmap, y);
			if (SourceY == LastSourceY)
			{
				// If we're still reading from the same source line then the results
				// are going to be identical - so just copy them over.
				memcpy(pDestLine, pLastLine, GetWidth(DestBitmap) * NumChannels);
			}
			else
			{
				uint8_t* pSourceLine = GetLinePtr(SourceBitmap, SourceY);
				// We must update pLastLine before the x-loop because the x-loop
				// modifies it.
				pLastLine = pDestLine;
				LastSourceY = SourceY;
				// For each pixel in the line...
				for (int x = 0; x < GetWidth(DestBitmap); ++x)
				{
					// Calculate the address of the pixel we want to read from.
					uint8_t* pSourcePixel = pSourceLine + SrcXBuffer[x];
					// Use a switch statement instead of a loop to copy one to four bytes.
					// Notice the missing break statements so that it will fall through.
					switch (NumChannels)
					{
						case 4:
							pDestLine[3] = pSourcePixel[3];
						case 3:
							pDestLine[2] = pSourcePixel[2];
						case 2:
							pDestLine[1] = pSourcePixel[1];
						case 1:
							pDestLine[0] = pSourcePixel[0];
					}
					// Update our destination pointer to the next pixel.
					pDestLine += NumChannels;
				}
			}
		}
		free(SrcXBuffer);
	}
	else
	{
		// Point sampling.
		// Simplest implementation - no optimizations.
		// Loop over all destination lines.
		const unsigned int sWidth = GetWidth(SourceBitmap);
		const unsigned int sWidthDiv2 = sWidth / 2;
		const unsigned int dWidth = GetWidth(DestBitmap);
		for (int y = 0; y < GetHeight(DestBitmap); ++y)
		{
			// Calculate the line number we want to read from.
			int SourceY = (y * GetHeight(SourceBitmap) + GetHeight(SourceBitmap) / 2) / GetHeight(DestBitmap);
			// We don't need asserts here because GetLinePtr() will check the y-coordinate.
			// Get the source and destination line pointers.
			uint8_t* pDestLine = GetLinePtr(DestBitmap, y);
			uint8_t* pSourceLine = GetLinePtr(SourceBitmap, SourceY);
			// For each pixel in the line...
			for (unsigned int x = 0; x < dWidth; ++x)
			{
				// Calculate the column we want to read from.
				int SourceX = (x * sWidth + sWidthDiv2) / dWidth;
				// Make sure SourceX is in valid range.
				assert(SourceX >= 0 && SourceX < GetWidth(SourceBitmap));
				// And loop over all of the bytes in that pixel.
				uint8_t* pSourcePixel = pSourceLine + SourceX * NumChannels;
				for (int channel = 0; channel < NumChannels; ++channel)
					pDestLine[channel] = pSourcePixel[channel];
				pDestLine += NumChannels;
			}
		}
	}
}

void ScaleBitmap(CSBitmap* SourceBitmap, CSBitmap* DestBitmap, bool Filtered)
{
	assert(HasBitmap(SourceBitmap));
	assert(HasBitmap(DestBitmap));

	if (GetChannels(DestBitmap) >= 3 && Filtered)
		FilterScaleBitmap(SourceBitmap, DestBitmap);
	else
		PointScaleBitmap(SourceBitmap, DestBitmap);
}

#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

#include "csbitmap.h"

// Copyright © 1999 Bruce Dawson.

static void RandomDitherBitmap(CSBitmap* pBitmap, int ditherAmount)
{
	const int width = GetWidth(pBitmap);
	const int height = GetHeight(pBitmap);
	const int numChannels = GetChannels(pBitmap);
	if (numChannels < 3)
		return;
	for (int y = 0; y < height; ++y)
	{
		uint8_t* pPixel = GetLinePtr(pBitmap, y);
		for (int x = 0; x < width; ++x)
		{
			int offset = (rand() % ditherAmount) - (ditherAmount / 2);
			for (int chan = 0; chan < 3; ++chan)
			{
				int Value = pPixel[chan] + offset;
				if (Value < 0)
					Value = 0;
				if (Value > 255)
					Value = 255;
				pPixel[chan] = Value;
			}
			pPixel += numChannels;
		}
	}
}

static void OrderedDitherBitmap(CSBitmap* pBitmap, int ditherAmount)
{
	const int DITHERSIZE = 16;
	// A sixteen by sixteen dither table holds 256 values,
	// which is enough to give us dither values from zero to 255.
	int DitherArray[DITHERSIZE][DITHERSIZE] =
	{
		{ 0, 128, 32, 160,  8, 136, 40, 168,  2, 130, 34, 162, 10, 138, 42, 170, },
		{192, 64, 224, 96, 200, 72, 232, 104, 194, 66, 226, 98, 202, 74, 234, 106, },
		{48, 176, 16, 144, 56, 184, 24, 152, 50, 178, 18, 146, 58, 186, 26, 154, },
		{240, 112, 208, 80, 248, 120, 216, 88, 242, 114, 210, 82, 250, 122, 218, 90, },
		{12, 140, 44, 172,  4, 132, 36, 164, 14, 142, 46, 174,  6, 134, 38, 166, },
		{204, 76, 236, 108, 196, 68, 228, 100, 206, 78, 238, 110, 198, 70, 230, 102, },
		{60, 188, 28, 156, 52, 180, 20, 148, 62, 190, 30, 158, 54, 182, 22, 150, },
		{252, 124, 220, 92, 244, 116, 212, 84, 254, 126, 222, 94, 246, 118, 214, 86, },
		{ 3, 131, 35, 163, 11, 139, 43, 171,  1, 129, 33, 161,  9, 137, 41, 169, },
		{195, 67, 227, 99, 203, 75, 235, 107, 193, 65, 225, 97, 201, 73, 233, 105, },
		{51, 179, 19, 147, 59, 187, 27, 155, 49, 177, 17, 145, 57, 185, 25, 153, },
		{243, 115, 211, 83, 251, 123, 219, 91, 241, 113, 209, 81, 249, 121, 217, 89, },
		{15, 143, 47, 175,  7, 135, 39, 167, 13, 141, 45, 173,  5, 133, 37, 165, },
		{207, 79, 239, 111, 199, 71, 231, 103, 205, 77, 237, 109, 197, 69, 229, 101, },
		{63, 191, 31, 159, 55, 183, 23, 151, 61, 189, 29, 157, 53, 181, 21, 149, },
		{255, 127, 223, 95, 247, 119, 215, 87, 253, 125, 221, 93, 245, 117, 213, 85, },
	};
	assert(GetChannels(pBitmap) >= 3);
	assert(ditherAmount > 0);
	if (ditherAmount == 1)
		return;	// There's nothing to do.
	// ditherAmount is the number of distinct values we want to be adding
	// to our bitmap. We want these values to range between zero and
	// ditherAmount - 1, so we subtract one here.
	ditherAmount -= 1;

	int NumChannels = GetChannels(pBitmap);
	// Before we do the dithering we have to get the values into the right
	// range. We then have to subtract off ditherAmount / 2 which is half of
	// the maximum value so that the values are centered about zero.
	for (int y = 0; y < DITHERSIZE; y++)
	{
		for (int x = 0; x < DITHERSIZE; x++)
		{
			int Temp  = (DitherArray[y][x] * ditherAmount + 127) / 255;
			assert(Temp >= 0 && Temp <= ditherAmount);
			DitherArray[y][x] = Temp - ditherAmount / 2;
		}
	}

	// Now we can dither our image.
	for (int y = 0; y < GetHeight(pBitmap); y++)
	{
		unsigned char* pLine = GetLinePtr(pBitmap, y);
		int* pDitherLine = DitherArray[y & (DITHERSIZE - 1)];
		for (int x = 0; x < GetWidth(pBitmap); x++)
		{
			int DitherAmount = pDitherLine[x & (DITHERSIZE - 1)];
			for (int channels = 0; channels < NumChannels; channels++)
			{
				int DitheredPixel = (int)pLine[channels] + DitherAmount;
				if (DitheredPixel < 0)
					DitheredPixel = 0;
				if (DitheredPixel > 255)
					DitheredPixel = 255;
				pLine[channels] = DitheredPixel;
			}
			pLine += NumChannels;
		}
	}
}

void DitherBitmap(CSBitmap* pBitmap, EDitherMethod ditherMethod, int ditherAmount)
{
	switch (ditherMethod)
	{
		case kDitherMethodRandom:
			RandomDitherBitmap(pBitmap, ditherAmount);
			break;
		case kDitherMethodOrdered:
			OrderedDitherBitmap(pBitmap, ditherAmount);
			break;
		case kDitherMethodErrorDiffusion:
		default:
			assert(!"Error - dither method not implemented.");
			break;
	}
}

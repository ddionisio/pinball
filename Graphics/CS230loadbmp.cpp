#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

// Copyright © 1999 Bruce Dawson.

#include "csbitmap.h"

// Reads in the colour map, returns the number of bytes of colour map read, or -1 for error.
static int ReadColorMap(FILE* pFile, CSBitmap *DestBitmap, BITMAPINFOHEADER *infoheader)
{
	RGBQUAD ColorMap[256];

	/* Calculate the number of colours there should be in the colour map. */
	/* This will be in ClrUsed.  If ClrUsed is zero then the number of */
	/* colours is just 1 ^ bitsperpixel. */

	unsigned int ColorMapSize = infoheader->biClrUsed;
	if (infoheader->biClrUsed == 0)
		ColorMapSize = 1 << infoheader->biBitCount;
	if (ColorMapSize > 256)
		return -1;

	if (ColorMapSize)
	{
		if (!fread(ColorMap, sizeof(ColorMap[0]) * ColorMapSize, 1, pFile))
			return -1;
		// Set the palette in the bitmap, or something like that.
		SetPalette(DestBitmap, 0, ColorMapSize, ColorMap);
	}
	return ColorMapSize * sizeof(ColorMap[0]);
}

static int DecompressToBitmap(CSBitmap *DestBitmap, const char *CompressedData, int DataLength)
{
	const char *DataEnd = CompressedData + DataLength;
	const char *NearEnd = DataEnd - 2;	// Handy.
	for (int y = 0; y < GetHeight(DestBitmap); y++)
	{
		int x = 0;
		uint8_t *Output = GetLinePtr(DestBitmap, GetHeight(DestBitmap) - y - 1);
		while (x < GetWidth(DestBitmap))
		{
			// Make sure I have at least two bytes left.
			if (CompressedData > NearEnd)
			{
				assert(0);
				return 0;	// Not enough data left.
			}
			uint8_t Input = *CompressedData++;
			if (Input == 0)
			{
				Input = *CompressedData++;
				switch (Input)
				{
					case 0:
						// End of line. I watch for this at the end of lines
						// and I have no interest in handling it in the middle
						// of lines.
						assert(0);
						return 0;
					case 1:
						// End of image. I watch for this at the end of the image
						// and I have no interest in handling it in the middle
						// of the image.
						assert(!"I don't handle this.");
						return 0;
					case 2:
						// Skip to new location! This is ill defined in on disk
						// bitmaps and I have not tested it. Therefore I will not
						// try to handle it.
						assert(!"I don't handle this!!!");
						return 0;
					default:
						if (x + Input > GetWidth(DestBitmap))
						{
							assert(0);
							return 0;
						}
						if (CompressedData + Input >= DataEnd)
						{
							assert(0);
							return 0;
						}
						memcpy(Output + x, CompressedData, Input);
						x += Input;
						CompressedData += Input;
						if ((Input & 1) != 0)
							CompressedData++;
						break;
				}
			}
			else
			{
				if (x + Input > GetWidth(DestBitmap))
				{
					assert(0);
					return 0;
				}
				memset(Output + x, *CompressedData++, Input);
				x += Input;
			}
		}
		if (CompressedData <= NearEnd && CompressedData[0] == 0 && CompressedData[1] == 0)
			CompressedData += 2;	// Skip over the end of line data.
		else
		{
			// Out of data or missing end of line stuff.
			assert(0);
			// After complaining we will return a failure code - unless we just finished
			// the last line, in which case we'll cut them some slack.
			if (y != GetHeight(DestBitmap) - 1)
				return 0;
		}
	}
	if (CompressedData <= NearEnd && CompressedData[0] == 0 && CompressedData[1] == 1)
		CompressedData += 2;
	else
	{
		// Out of data or missing end of bitmap stuff.
		assert(0);
		return 0;
	}
	assert(CompressedData == DataEnd);
	return GetHeight(DestBitmap);
}

static bool InternalLoadBMPFile(FILE* pFile, CSBitmap *DestBitmap, char** ErrorMessage)
{
	BITMAPFILEHEADER fileheader;
	assert(ErrorMessage);

	// Read the file header.
	if (!fread(&fileheader, sizeof(fileheader), 1, pFile))
	{
		*ErrorMessage = "Read failure.";
		return false;
	}

	// Check the type field to make sure we have a BMP file.
	if (memcmp(&fileheader.bfType, "BM", 2))
	{
		*ErrorMessage = "Not a BMP file.";
		return false;
	}

	BITMAPINFOHEADER infoheader;
	// Read the info header.
	if (!fread(&infoheader, sizeof(infoheader), 1, pFile))
	{
		*ErrorMessage = "Read failure.";
		return false;
	}

	// Sanity check the info header.
	if (infoheader.biSize != sizeof(infoheader))
	{
		*ErrorMessage = "Corrupt file or not a BMP file.";
		return false;
	}

	if (infoheader.biPlanes != 1)
	{
		*ErrorMessage = "Unsupported format.";
		return false;
	}

	// These are the only depths I'm interested in handling. 1 and 4 bits per pixel
	// are also legal, but too much hassle.
	if (infoheader.biBitCount != 8 && infoheader.biBitCount != 24 && infoheader.biBitCount != 32)
	{
		*ErrorMessage = "Unsupported or illegal format.";
		return false;
	}

	if (!SetSize(DestBitmap, infoheader.biWidth, infoheader.biHeight, infoheader.biBitCount / 8))
	{
		*ErrorMessage = "Couldn't create requested bitmap.";
		return false;
	}

	// Read the color map, if any.
	if (infoheader.biBitCount == 8 && ReadColorMap(pFile, DestBitmap, &infoheader) <= 0)
	{
		// Illegal ColorMap value
		*ErrorMessage = "Couldn't read color map.";
		return false;
	}

	// Jump to the location where the bitmap data is stored.
	if (fseek(pFile, fileheader.bfOffBits, SEEK_SET))
	{
		*ErrorMessage = "Couldn't read bitmap data.";
		return false;
	}

	// Read in the bitmap data.
	if (infoheader.biCompression == BI_RLE8)
	{
		// We only support 8-bit RLE8 files - logically enough.
		if (infoheader.biBitCount != 8)
		{
			*ErrorMessage = "Illegal format.";
			return false;
		}
		// Allocate enough space for all of the compressed data.
		void* CompressedData = malloc(infoheader.biSizeImage);
		if (!CompressedData)
		{
			*ErrorMessage = "Out of memory.";
			return false;
		}
		// Read all of the compressed data - this is much easier than constantly
		// worrying about fread() failures.
		if (!fread(CompressedData, infoheader.biSizeImage, 1, pFile))
		{
			free(CompressedData);
			return false;
		}
		// Decompress.
		int NumLines = DecompressToBitmap(DestBitmap, (char *)CompressedData, infoheader.biSizeImage);
		free(CompressedData);
		if (NumLines != GetHeight(DestBitmap))
		{
			*ErrorMessage = "Couldn't read bitmap data.";
			return false;
		}
	}
	else if (infoheader.biCompression == BI_RGB)
	{
		unsigned int UsedBytes = GetWidth(DestBitmap) * GetChannels(DestBitmap);
		unsigned int BytesPerLine = (UsedBytes + 3) & ~3;
		unsigned int Padding = BytesPerLine - UsedBytes;
		for (int y = 0; y < infoheader.biHeight; y++)
		{
			// We have to flip the y-coordinate because our bitmap class
			// puts line zero at the top of the screen, and the BMP format
			// on disk puts the first bytes at the bottom of the screen.
			uint8_t *LinePtr = GetLinePtr(DestBitmap, GetHeight(DestBitmap) - 1 - y);
			// Read the precise number of bytes that the line requires into the bitmap.
			// Don't read the padding bytes, because the in memory alignment requirements
			// may vary - we don't want our reading code to depend on our bitmap class
			// implementation.
			if (!fread(LinePtr, UsedBytes, 1, pFile))
			{
				*ErrorMessage = "Couldn't read bitmap data.";
				return false;
			}
			// Skip over any padding bytes.
			if (fseek(pFile, Padding, SEEK_CUR))
			{
				*ErrorMessage = "Couldn't read bitmap data.";
				return false;
			}
		}
	}
	else
	{
		*ErrorMessage = "Unsupported or illegal format.";
		return false;
	}

	return true;
}

bool LoadBMPFile(const char* FileName, CSBitmap* DestBitmap, char** ErrorMessage)
{
	assert(FileName);
	assert(DestBitmap);
	FILE* pFile = fopen(FileName, "rb");

	// This is the cleanest way of making sure that the file always gets
	// closed, and making sure that we can count on the ErrorMessage pointer.
	// It also gives us a single place to call FreeBitmap() if the bitmap
	// loading fails.
	char* ErrorMessagePrivate = 0;
	bool	Result = false;
	if (pFile)
	{
		Result = InternalLoadBMPFile(pFile, DestBitmap, &ErrorMessagePrivate);
		fclose(pFile);
	}
	if (!Result)
		FreeBitmap(DestBitmap);
	if (ErrorMessage)
		*ErrorMessage = ErrorMessagePrivate;
	return Result;
}

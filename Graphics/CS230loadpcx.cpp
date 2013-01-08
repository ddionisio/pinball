#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

// Copyright © 1999 Bruce Dawson.

//#define	USEIOSTREAMS

#include "csbitmap.h"

#pragma pack(push, 1)
struct PCXHeader
{
	char	Manufacturer;	// Always 10.
	char	Version;		// 5 - 0 = Ver2.5, 2 = Ver2.8 w/palette, 3 = Ver2.8 w/o palette, 5 = Ver 3.0
	char	Encoding;		// 1 - PCX RLE
	char	BitsPerPixel;	// per plane
	short	xMin, yMin, xMax, yMax;
	short	HDPI, VDPI;
	char	Colormap[48];	// Huh?
	char	Reserved1;
	char	NPlanes;
	short	BytesPerLine;
};
#pragma pack(pop)

#ifdef	USEIOSTREAMS

// Disable the warning about C++ exceptions being disabled - I know they're
// disabled and it's okay.
#pragma warning(disable : 4530)
#include <fstream>
#include <vector>
using namespace std;

// Support function for internal use by the LoadPCXFile function. This
// function is marked static so it won't affect any other source files.
static bool InternalLoadPCXFile(istream& inputfile, CSBitmap* DestBitmap, char** ErrorMessage)
{
	// I guaranteed in the public function that ErrorMessage would always be non-zero.
	assert(ErrorMessage);
	// If the structure size changes, I want to know about it!
	assert(sizeof(PCXHeader) == 68);
	PCXHeader	header;
	if (!inputfile.read((char *)&header, sizeof(header)))
	{
		*ErrorMessage = "Couldn't read header.";
		return false;
	}
	if (header.Manufacturer != 10)
	{
		*ErrorMessage = "Unsupported manufacturer.";
		return false;
	}
	if (header.Version != 5)
	{
		*ErrorMessage = "Unsupported version.";
		return false;
	}
	int Width = header.xMax + 1 - header.xMin;
	int Height = header.yMax + 1 - header.yMin;
	if (Width <= 0 || Height <= 0)
	{
		*ErrorMessage = "Illegal size.";
		return false;
	}
	// We only read 8 or 24-bit images.
	if (header.BitsPerPixel != 8 || (header.NPlanes != 1 && header.NPlanes != 3))
	{
		*ErrorMessage = "Illegal or unsupported option.";
		return false;
	}
	if (header.BytesPerLine < Width)
	{
		*ErrorMessage = "Corrupt file.";
		return false;
	}
	// Seek past the rest of the 128 byte header.
	if (!inputfile.seekg(128))
	{
		*ErrorMessage = "Error seeking to pixel data.";
		return false;
	}
	if (!SetSize(DestBitmap, Width, Height, header.NPlanes))
	{
		*ErrorMessage = "Error allocating bitmap.";
		return false;
	}

	// Using auto_ptr means that I don't have to worry about freeing my
	// decoding buffer - it will get freed for me, no matter how I
	// exit this function.
	auto_ptr<uint8_t> decodebuffer(new uint8_t[header.BytesPerLine]);
	if (!decodebuffer.get())
	{
		*ErrorMessage = "Out of memory.";
		return false;
	}
/*
The encoding method is:
    FOR  each  byte,  X,  read from the file
        IF the top two bits of X are  1's then
            count = 6 lowest bits of X
            data = next byte following X
        ELSE
            count = 1
            data = X
Since the overhead this technique requires is, on average,  25% 
of the non-repeating data and is at least offset whenever bytes are 
repeated, the file storage savings are usually considerable.
*/
/*
Well, that's what they say. What they downplay is that, for no good reason,
this 'compression' method can actually cause the file size to double!
Charming.

It's really not necessary to have such a lousy worst case scenario.
*/
	for (int y = 0; y < Height && inputfile; y++)
	{
		for (int channel = 0; channel < header.NPlanes; ++channel)
		{
			int x = 0;
			while (x < header.BytesPerLine)
			{
				char	data;
				int		count = 1;
				if (!inputfile.read(&data, sizeof(data)))
				{
					*ErrorMessage = "Error reading pixel data.";
					return false;
				}
				if ((data & 0xC0) == 0xC0)
				{
					count = data & 0x3F;
					if (!inputfile.read(&data, sizeof(data)))
					{
						*ErrorMessage = "Error reading pixel data.";
						return false;
					}
				}
				if (count + x > header.BytesPerLine)
				{
					*ErrorMessage = "Corrupt pixel data.";
					return false;
				}
				// Copy count copies of data to the current buffer location.
				memset(decodebuffer.get() + x, data, count);
				x += count;
			}
			// Copy the active portion of the line (Width bytes - not BytesPerLine bytes)
			// to the bitmap.
			int lineMappings[3] = {RED_BITMAP_OFFSET, GREEN_BITMAP_OFFSET, BLUE_BITMAP_OFFSET};
			int destChannel = lineMappings[channel];
			if (header.NPlanes == 1)
				destChannel = 0;
			unsigned char* pLine = GetLinePtr(DestBitmap, y) + destChannel;
			for (x = 0; x < Width; ++x)
			{
				pLine[0] = decodebuffer.get()[x];
				pLine += header.NPlanes;
			}
		}
	}
	if (header.NPlanes == 1)
	{
		if (!inputfile.seekg(-769, ios_base::end))
		{
			*ErrorMessage = "Couldn't seek to palette.";
			return false;
		}
		char	magicNumber;
		if (!inputfile.read(&magicNumber, sizeof(magicNumber)))
		{
			*ErrorMessage = "Couldn't read magic number.";
			return false;
		}
		if (magicNumber != 12)
		{
			*ErrorMessage = "No palette found.";
			return false;
		}
		char	Palette[768];
		if (!inputfile.read(Palette, sizeof(Palette)))
		{
			*ErrorMessage = "Couldn't read palette.";
			return false;
		}
		RGBQUAD	Colors[256];
		for (int i = 0; i < 256; i++)
		{
			Colors[i].rgbRed = Palette[i * 3 + 0];
			Colors[i].rgbGreen = Palette[i * 3 + 1];
			Colors[i].rgbBlue = Palette[i * 3 + 2];
		}
		SetPalette(DestBitmap, 0, 256, Colors);
	}
	return true;
}

bool LoadPCXFile(const char* FileName, CSBitmap* DestBitmap, char** ErrorMessage)
{
	bool	Result = false;
	assert(FileName);
	assert(DestBitmap);
	ifstream	inputfile(FileName, ios_base::binary);

	// This is the cleanest way of making sure that the file always gets
	// closed, and making sure that we can count on the ErrorMessage pointer.
	// It also gives us a single place to call FreeBitmap() if the bitmap
	// loading fails.
	char* ErrorMessagePrivate = 0;
	if (inputfile)
		Result = InternalLoadPCXFile(inputfile, DestBitmap, &ErrorMessagePrivate);
	if (!Result)
		FreeBitmap(DestBitmap);
	if (ErrorMessage)
		*ErrorMessage = ErrorMessagePrivate;
	return Result;
}

#else
// Support function for internal use by the LoadPCXFile function. This
// function is marked static so it won't affect any other source files.
static bool InternalLoadPCXFile(FILE* inputfile, CSBitmap* DestBitmap, char** ErrorMessage)
{
	// I guaranteed in the public function that ErrorMessage would always be non-zero.
	assert(ErrorMessage);
	// If the structure size changes, I want to know about it!
	assert(sizeof(PCXHeader) == 68);
	PCXHeader	header;
	if (!fread(&header, sizeof(header), 1, inputfile))
	{
		*ErrorMessage = "Couldn't read header.";
		return false;
	}
	if (header.Manufacturer != 10)
	{
		*ErrorMessage = "Unsupported manufacturer.";
		return false;
	}
	if (header.Version != 5)
	{
		*ErrorMessage = "Unsupported version.";
		return false;
	}
	int Width = header.xMax + 1 - header.xMin;
	int Height = header.yMax + 1 - header.yMin;
	if (Width <= 0 || Height <= 0)
	{
		*ErrorMessage = "Illegal size.";
		return false;
	}
	// We only read 8 or 24-bit images.
	if (header.BitsPerPixel != 8 || (header.NPlanes != 1 && header.NPlanes != 3))
	{
		*ErrorMessage = "Illegal or unsupported option.";
		return false;
	}
	if (header.BytesPerLine < Width)
	{
		*ErrorMessage = "Corrupt file.";
		return false;
	}
	// Seek past the rest of the 128 byte header.
	if (fseek(inputfile, 128, SEEK_SET))
	{
		*ErrorMessage = "Error seeking to pixel data.";
		return false;
	}
	if (!SetSize(DestBitmap, Width, Height, header.NPlanes))
	{
		*ErrorMessage = "Error allocating bitmap.";
		return false;
	}

	uint8_t* decodebuffer = (uint8_t*)malloc(header.BytesPerLine);
	if (!decodebuffer)
	{
		*ErrorMessage = "Out of memory.";
		return false;
	}
/*
The encoding method is:
    FOR  each  byte,  X,  read from the file
        IF the top two bits of X are  1's then
            count = 6 lowest bits of X
            data = next byte following X
        ELSE
            count = 1
            data = X
Since the overhead this technique requires is, on average,  25% 
of the non-repeating data and is at least offset whenever bytes are 
repeated, the file storage savings are usually considerable.
*/
/*
Well, that's what they say. What they downplay is that, for no good reason,
this 'compression' method can actually cause the file size to double!
Charming.

It's really not necessary to have such a lousy worst case scenario.
*/
	for (int y = 0; y < Height && inputfile; y++)
	{
		for (int channel = 0; channel < header.NPlanes; ++channel)
		{
			int x = 0;
			while (x < header.BytesPerLine)
			{
				char	data;
				int		count = 1;
				if (!fread(&data, sizeof(data), 1, inputfile))
				{
					*ErrorMessage = "Error reading pixel data.";
					free(decodebuffer);
					return false;
				}
				if ((data & 0xC0) == 0xC0)
				{
					count = data & 0x3F;
					if (!fread(&data, sizeof(data), 1, inputfile))
					{
						free(decodebuffer);
						*ErrorMessage = "Error reading pixel data.";
						return false;
					}
				}
				if (count + x > header.BytesPerLine)
				{
					*ErrorMessage = "Corrupt pixel data.";
					return false;
				}
				// Copy count copies of data to the current buffer location.
				memset(decodebuffer + x, data, count);
				x += count;
			}
			// Copy the active portion of the line (Width bytes - not BytesPerLine bytes)
			// to the bitmap.
			int lineMappings[3] = {RED_BITMAP_OFFSET, GREEN_BITMAP_OFFSET, BLUE_BITMAP_OFFSET};
			int destChannel = lineMappings[channel];
			if (header.NPlanes == 1)
				destChannel = 0;
			unsigned char* pLine = GetLinePtr(DestBitmap, y) + destChannel;
			for (x = 0; x < Width; ++x)
			{
				pLine[0] = decodebuffer[x];
				pLine += header.NPlanes;
			}
		}
	}
	free(decodebuffer);
	if (header.NPlanes == 1)
	{
		if (fseek(inputfile, -769, SEEK_END))
		{
			*ErrorMessage = "Couldn't seek to palette.";
			return false;
		}
		char	magicNumber;
		if (!fread(&magicNumber, sizeof(magicNumber), 1, inputfile))
		{
			*ErrorMessage = "Couldn't read magic number.";
			return false;
		}
		if (magicNumber != 12)
		{
			*ErrorMessage = "No palette found.";
			return false;
		}
		char	Palette[768];
		if (!fread(Palette, sizeof(Palette), 1, inputfile))
		{
			*ErrorMessage = "Couldn't read palette.";
			return false;
		}
		RGBQUAD	Colors[256];
		for (int i = 0; i < 256; i++)
		{
			Colors[i].rgbRed = Palette[i * 3 + 0];
			Colors[i].rgbGreen = Palette[i * 3 + 1];
			Colors[i].rgbBlue = Palette[i * 3 + 2];
		}
		SetPalette(DestBitmap, 0, 256, Colors);
	}
	return true;
}

bool LoadPCXFile(const char* FileName, CSBitmap* DestBitmap, char** ErrorMessage)
{
	bool	Result = false;
	assert(FileName);
	assert(DestBitmap);
	FILE* inputfile = fopen(FileName, "rb");

	// This is the cleanest way of making sure that the file always gets
	// closed, and making sure that we can count on the ErrorMessage pointer.
	// It also gives us a single place to call FreeBitmap() if the bitmap
	// loading fails.
	char* ErrorMessagePrivate = 0;
	if (inputfile)
	{
		Result = InternalLoadPCXFile(inputfile, DestBitmap, &ErrorMessagePrivate);
		fclose(inputfile);
	}
	if (!Result)
		FreeBitmap(DestBitmap);
	if (ErrorMessage)
		*ErrorMessage = ErrorMessagePrivate;
	return Result;
}

#endif

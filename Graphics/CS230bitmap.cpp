#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

// Copyright © 1999 Bruce Dawson.

#include "csbitmap.h"

static void DisplayError(long LastError, const char *ErrorText /*= 0*/)
{
	char	ErrorBuffer[1000];
	if (ErrorText)
		sprintf(ErrorBuffer, "%s\nError code is %d.", ErrorText, LastError);
	else
		sprintf(ErrorBuffer, "Error code %d encountered.", LastError);
	MessageBox(0, ErrorBuffer, "Message", MB_OK);
}

static void DisplayLastError(const char *ErrorText /*= 0*/)
{
	DisplayError(GetLastError(), ErrorText);
}


void InitBitmap(CSBitmap* pBitmap)
{
	// Set mWidth, mHeight and mChannels to -1 so that we can tell
	// whether this initialization function has been called. This
	// helps us detect bugs.
	pBitmap->mWidth = pBitmap->mHeight = pBitmap->mChannels = -1;
	pBitmap->mSurfaceBits = 0;
	pBitmap->mBitmapNew = 0;
	pBitmap->mBitmapDC = 0;
	pBitmap->mBitmapMonochrome = 0;
}

void FreeBitmap(CSBitmap* pBitmap)
{
	// Carefully free up the resources we allocated.
	if (pBitmap->mBitmapDC)
	{
		if (pBitmap->mBitmapMonochrome)
		{
			// Select the stock 1x1 monochrome bitmap back in. It is critically important
			// that you do that before deleting the bitmap and device context, or else
			// the DeleteObject() and DeleteDC() commands may silently fail.
			HBITMAP hBitmapOld = SelectBitmap(pBitmap->mBitmapDC, pBitmap->mBitmapMonochrome);
			assert(hBitmapOld == pBitmap->mBitmapNew);
		}
		if (pBitmap->mBitmapNew)
			DeleteObject(pBitmap->mBitmapNew);
		if (pBitmap->mBitmapDC)
			DeleteDC(pBitmap->mBitmapDC);
	}
	// Zero all of the resource handles, so that we don't
	// free resources multiple times.
	InitBitmap(pBitmap);
}

HDC GetDrawSurface(CSBitmap* pBitmap)
{
	assert(HasBitmap(pBitmap));
	return pBitmap->mBitmapDC;
}

HBITMAP GetDrawBitmap(CSBitmap* pBitmap)
{
	assert(HasBitmap(pBitmap));
	return pBitmap->mBitmapNew;
}

// Theoretically we should be paying attention to this:
// From the CreateDIBSection help:
// Windows NT: You need to guarantee that the GDI subsystem has completed any
// drawing to a bitmap created by CreateDIBSection before you draw to the
// bitmap yourself. Access to the bitmap must be synchronized. Do this by
// calling the GdiFlush function. This applies to any use of the pointer
// to the bitmap’s bit values, including passing the pointer in calls to
// functions such as SetDIBits. 
uint8_t *GetLinePtr(CSBitmap* pBitmap, int y)
{
	assert(HasBitmap(pBitmap));
	assert(y >= 0 && y < pBitmap->mHeight);
	return pBitmap->mLineZeroPointer + y * pBitmap->mStride;
}

uint8_t *GetPixelPtr(CSBitmap* pBitmap, int x, int y)
{
	assert(HasBitmap(pBitmap));
	assert(y >= 0 && y < GetHeight(pBitmap));
	assert(x >= 0 && x < GetWidth(pBitmap));
	return GetLinePtr(pBitmap, y) + x * GetChannels(pBitmap);
}

void SetPalette(CSBitmap* pBitmap, int StartColor, int NumColors, RGBQUAD* pColors)
{
	assert(HasBitmap(pBitmap));
	assert(GetChannels(pBitmap) == 1);
	assert(StartColor >= 0);
	assert(StartColor + NumColors <= 256);
	SetDIBColorTable(GetDrawSurface(pBitmap), StartColor, NumColors, pColors);
}

bool GetPalette(CSBitmap* pBitmap, int StartColor, int NumColors, RGBQUAD* pColors)
{
	assert(HasBitmap(pBitmap));
	assert(GetChannels(pBitmap) == 1);
	assert(StartColor >= 0);
	assert(StartColor + NumColors <= 256);
	// Zero the destination array to make sure all bytes get cleared, even if
	// the function fails.
	memset(pColors, 0, sizeof(RGBQUAD) * NumColors);
	int GotCount = GetDIBColorTable(GetDrawSurface(pBitmap), StartColor, NumColors, pColors);
	return GotCount == NumColors;
}

struct DIBData
{
	BITMAPINFOHEADER InfoHeader;
	RGBQUAD ColorTable[256];
};

bool SetSize(CSBitmap* pBitmap, int width, int height, int channels)
{
	// Make sure that the InitBitmap() function was called before calling
	// SetSize() for the first time.
	assert(HasBitmap(pBitmap) || pBitmap->mWidth == -1);
	assert(HasBitmap(pBitmap) || pBitmap->mHeight == -1);
	assert(HasBitmap(pBitmap) || pBitmap->mChannels == -1);
	// This function doesn't support allocating zero size or negative
	// size bitmaps.
	rassert(width > 0);
	rassert(height > 0);
	rassert(channels == 1 || channels == 3 || channels == 4);
	FreeBitmap(pBitmap);
	pBitmap->mWidth = width;
	pBitmap->mHeight = height;
	pBitmap->mChannels = channels;

	// Define a DIBData object, and clear it to zero.
	DIBData Info = {0};

	// Initialize the parameters that we care about, based on the
	// documentation and our needs. 
	Info.InfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	Info.InfoHeader.biWidth = width;
	Info.InfoHeader.biHeight = height;
	Info.InfoHeader.biPlanes = 1;
	Info.InfoHeader.biBitCount = channels * 8;
	Info.InfoHeader.biCompression = BI_RGB;

	// Support for 256 colour bitmaps.
	Info.InfoHeader.biClrUsed = (channels == 1 ? 256 : 0);

	// Create a temporary DC. Alternately we could pass one in. 
	// This is needed for the call to CreateDIBSection.
	HDC TempDC = CreateDC("Display", 0, 0, 0);
	if (TempDC)
	{
		// Create a DIBSection. Note that this returns us two
		// things - an HBITMAP handle and a memory pointer, in
		// mSurfaceBits.
		pBitmap->mBitmapNew = CreateDIBSection(TempDC, (BITMAPINFO *)&Info,
				DIB_RGB_COLORS, (void **)&pBitmap->mSurfaceBits, 0, 0L);
		if (pBitmap->mBitmapNew)
		{
			// Create a copy of our DC that we can keep around
			pBitmap->mBitmapDC = CreateCompatibleDC(TempDC);
			if (pBitmap->mBitmapDC)
			{
				// Select our DIBSection bitmap into our DC. This
				// allows us to draw to the bitmap using GDI functions.
				pBitmap->mBitmapMonochrome = SelectBitmap(
							pBitmap->mBitmapDC,
							pBitmap->mBitmapNew);
			}
			else
				DisplayLastError("CreateCompatibleDC failed.");
		}
		else
			DisplayLastError("CreateDIBSection failed.");
		// Delete our temporary device context - we don't need it any more.
		DeleteDC(TempDC);
	}
	else
		DisplayLastError("CreateDC failed.");
 
	// If the allocation failed, clean up.
	if (!pBitmap->mBitmapMonochrome)
		FreeBitmap(pBitmap);

	// Initialize derived data that GetLinePtr() and other functions need to run efficiently.
	// Here is where we have to calculate the actual number of bytes per line,
	// respecting the Win32 rules for bitmap allocations. When we requested
	// the bitmap we specified a width, which Windows will have padded to
	// a multiple of four bytes - so we'd better allow for that.
	pBitmap->mBytesPerLine = (pBitmap->mWidth * pBitmap->mChannels + 3) & ~3;
	// Calculate the address of line zero - defined here as the line that is
	// at the top when the bitmap is copied to the screen. This is the last
	// line in memory.
	pBitmap->mLineZeroPointer = pBitmap->mSurfaceBits + (pBitmap->mHeight - 1) * pBitmap->mBytesPerLine;
	// Calculate the distance from line to line to be consistent with
	// mLineZeroPointer.
	pBitmap->mStride = -pBitmap->mBytesPerLine;
	return HasBitmap(pBitmap);
}

void Draw(CSBitmap* pBitmap, HDC DestDC, int destx, int desty, int destwidth, int destheight, int sourcex, int sourcey)
{
	if (HasBitmap(pBitmap))
	{
		StretchBlt(DestDC, destx, desty, destwidth, destheight, GetDrawSurface(pBitmap), sourcex, sourcey, destwidth, destheight, SRCCOPY);
	}
}

void DrawAll(CSBitmap* pBitmap, HDC DestDC)
{
	Draw(pBitmap, DestDC, 0, 0, GetWidth(pBitmap), GetHeight(pBitmap), 0, 0);
}

bool HasBitmap(CSBitmap* pBitmap)
{
	assert(pBitmap);
	if (pBitmap->mBitmapDC)
	{
		// Put in all sorts of bitmap validity checks here.
		assert(pBitmap->mWidth > 0);
		assert(pBitmap->mHeight > 0);
		assert(pBitmap->mChannels > 0 && pBitmap->mChannels <= 4);
		return true;
	}
	return false;
}

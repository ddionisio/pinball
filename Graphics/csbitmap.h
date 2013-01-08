#ifndef	CSBITMAP_H
#define	CSBITMAP_H

// Copyright © 1999-2000 Bruce Dawson.
// David Dionisio was here

#include "csdefs.h"

#ifdef	WIN32
// Windows true-colour bitmaps are in BGR format instead
// of RGB. These defines help to avoid depending on the
// order, and make the intent of code clearer.
#define	BLUE_BITMAP_OFFSET	0
#define	GREEN_BITMAP_OFFSET	1
#define	RED_BITMAP_OFFSET	2
#define	ALPHA_BITMAP_OFFSET	3
#else
#error	Unknown platform. Please specify!
#endif

// All data in this struct should be treated as private. DON'T MESS
// WITH IT!!!! Use the bitmap functions below for EVERYTHING!
struct CSBitmap
{
	// The constructor and destructor automatically call
	// InitBitmap() and FreeBitmap(), so you usually
	// don't have to.
	CSBitmap()
	{
		InitBitmap(this);
	}
	~CSBitmap()
	{
		FreeBitmap(this);
	}
	// Private is a C++ keyword that marks data members as accessible
	// to member functions and 'friend' functions only. Since we don't
	// have any member functions (because we're not really programming
	// in C++) we have to mark a bunch of our functions as friends.
private:
	int		mWidth, mHeight, mChannels;

	// Various Win32 specific items.
	HDC		mBitmapDC;
	uint8_t	*mSurfaceBits;
	HBITMAP mBitmapNew, mBitmapMonochrome;

	// Number of bytes per line, always positive.
	int		mBytesPerLine;
	// These two values are tweaked for easier calculations.
	// They handle the messy upside down bitmaps.
	uint8_t	*mLineZeroPointer;
	int		mStride;
	friend bool	SetSize(CSBitmap* pBitmap, int width, int height, int channels);	// Returns true for success.
	friend int	GetWidth(CSBitmap* pBitmap);
	friend int	GetHeight(CSBitmap* pBitmap);
	friend int	GetChannels(CSBitmap* pBitmap);
	friend void	InitBitmap(CSBitmap* pBitmap);
	friend void	FreeBitmap(CSBitmap* pBitmap);
	friend bool	HasBitmap(CSBitmap* pBitmap);
	friend int	GetStride(CSBitmap* pBitmap);
	friend HDC	GetDrawSurface(CSBitmap* pBitmap);
	friend HBITMAP GetDrawBitmap(CSBitmap* pBitmap);
	friend uint8_t*	GetLinePtr(CSBitmap* pBitmap, int y);
};

// Methods to get and set the size of the bitmap. SetSize() will free
// previously allocated bitmaps if there are any.
bool		SetSize(CSBitmap* pBitmap, int width, int height, int channels);	// Returns true for success.
inline int	GetWidth(CSBitmap* pBitmap) {return pBitmap->mWidth;}
inline int	GetHeight(CSBitmap* pBitmap) {return pBitmap->mHeight;}
inline int	GetChannels(CSBitmap* pBitmap) {return pBitmap->mChannels;}
// You should call the Init() function before using the bitmap.
void		InitBitmap(CSBitmap* pBitmap);
// Frees the bitmap data. Be sure to call when finished with the bitmap.
void		FreeBitmap(CSBitmap* pBitmap);
// Returns true if bitmap data is allocated.

// Asserts if the color table cannot be set, for instance if the bitmap
// is not a paletted bitmap or if there is no bitmap.
void SetPalette(CSBitmap* pBitmap, int StartColor, int NumColors, RGBQUAD* pColors);
// Returns true if getting the palette succeeds. Asserts if used illegally.
bool GetPalette(CSBitmap* pBitmap, int StartColor, int NumColors, RGBQUAD* pColors);


// Methods to get access to the bitmap data. Some types of
// Win32 bitmaps allow you to treat them either as an HDC,
// an HBITMAP or as an array of bytes. While not portable,
// the HDC and HBITMAP options are too useful to ignore.
HDC			GetDrawSurface(CSBitmap* pBitmap);
HBITMAP		GetDrawBitmap(CSBitmap* pBitmap);
uint8_t*	GetLinePtr(CSBitmap* pBitmap, int y);
uint8_t*	GetPixelPtr(CSBitmap* pBitmap, int x, int y);
inline int	GetStride(CSBitmap* pBitmap) {return pBitmap->mStride;}

// Get a pixel from arbitrary floating point pixel coordinates, presumably bilinear filtered.
void		GetFilteredPixel(CSBitmap* pBitmap, double SourceX, double SourceY, unsigned char* pDestLine);

// Various methods to draw the bitmap to an HDC.
void		DrawAll(CSBitmap* pBitmap, HDC DestDC);
void		Draw(CSBitmap* pBitmap, HDC DestDC, int destx, int desty,
						int destwidth, int destheight,
						int sourcex, int sourcey);

// The ErrorMessage parameter that is passed in must be a valid pointer to a
// char pointer, or zero. If it is non-zero then the pointer that it points to
// must be initialized to zero before the image loading function is called.
// When an error is detected by an image loading function and ErrorMessage is
// non-zero the image loading function can (if it wishes) assign a string constant
// to *ErrorMessage which the caller can optionally display.
bool LoadBitmapFile(const char* FileName, CSBitmap* DestBitmap, char** ErrorMessage);

bool LoadBMPFile(const char* FileName, CSBitmap* DestBitmap, char** ErrorMessage);
bool SaveBMPFile(const char* FileName, CSBitmap* DestBitmap);

bool LoadPCXFile(const char* FileName, CSBitmap* DestBitmap, char** ErrorMessage);

// Function to scale the bitmap, with or without filtering.
void ScaleBitmap(CSBitmap* SourceBitmap, CSBitmap* DestBitmap, bool Filtered);

// CompositeBitmap:
//		This function composites the top bitmap onto the bottom bitmap
// at the specified location. The x,y location refers to where the top
// left corner of the top bitmap should be placed.
//		If the top bitmap has an alpha channel
// then that is used to do an alpha composite.
//		If no top alpha channel is present
// then the top image replaces the dest image for whatever area it
// covers.
//		If the Dest image has an alpha channel it is unaffected (this
// is not a perfectly standard implementation, but adequate).
//
// This routine must be able to handle:
//		Top images that are 8-bit, 24-bit or 32-bit.
//		Bottom images that are 24-bit or 32-bit.
//		Any combination of bitmap sizes and composite coordinates. This
// includes top images that are larger than the bottom image, bottom
// images that are larger than the top image, and coordinates that
// may be outside of the destination bitmap. Note: just because the
// coordinates are outside of the destination bitmap does not mean there
// is no work to be done - if they are off the left or top edge then there
// may still be a valid intersection.
//
// GlobalOpacity can be ignored if you wish. If you want to implement this option
// just multiply all alpha values by GlobalOpacity. This allows the overlayed image
// to fade in and out.
void CompositeBitmap(CSBitmap* Bottom, CSBitmap* Top, int StartX, int StartY, uint8_t GlobalOpacity = 255);

enum EDitherMethod
{
	kDitherMethodRandom,
	kDitherMethodOrdered,
	kDitherMethodErrorDiffusion
};
void DitherBitmap(CSBitmap* destBitmap, EDitherMethod ditherMethod, int ditherAmount);
// Pass in the desired number of levels, between two and 256.
// Two is 'monochrome' (actually eight colours) and 256 is
// normal.
void QuantizeBitmap(CSBitmap* pBitmap, int Levels);

#endif

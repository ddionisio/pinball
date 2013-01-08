#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

// Copyright © 1999 Bruce Dawson.

#include "csbitmap.h"

const char* CompositingFunctionWrittenBy()
{
	// Fill in your name HERE!!!!! No Problem
	return "David Dionisio";
}

// See the header file for a full description of how this function
// behaves.
void CompositeBitmap(CSBitmap* Bottom, CSBitmap* Top, int StartX, int StartY, uint8_t GlobalOpacity /* = 255*/)
{
	// Insert your code here.


	//Check to see if the Top bitmap is not visible at all, if it's true, then just quit
	if((StartX >= GetWidth(Bottom))
		|| (StartY >= GetHeight(Bottom)))
		return;

	int indexX = StartX, indexY = StartY; //the starting index from the image bits
	int EndX = StartX + GetWidth(Top); //the ending index
	int EndY = StartY + GetHeight(Top);

	//adjust the starting image index if the location is off the edge
	if(StartX < 0)
		StartX = 0;
	if(StartY < 0)
		StartY = 0;

	//check to see again if the Top bitmap is not visible at all
	if((EndX < 0)
		|| (EndY < 0))
		return;
	
	//adjust the starting image index if the location is off the edge
	if(EndX > GetWidth(Bottom))
		EndX = GetWidth(Bottom);
	if(EndY > GetHeight(Bottom))
		EndY = GetHeight(Bottom);


	uint8_t *TopBit, *BottomBit;
	uint8_t alpha;

	//get this only if we have 8-bit Top
	RGBQUAD palette[255] = {0}; 
	if(GetChannels(Top) == 1)
		GetPalette(Top, 0, 256, palette);

	switch(GetChannels(Top))
	{
	case 4: //32
		for(int Ty = StartY; Ty < EndY; Ty++)
		{
			TopBit = GetPixelPtr(Top, StartX - indexX, Ty - indexY);
			BottomBit = GetPixelPtr(Bottom, StartX, Ty);

			for(int Tx = StartX; Tx < EndX; Tx++)
			{
				alpha = TopBit[ALPHA_BITMAP_OFFSET];
				alpha = (alpha * GlobalOpacity + 127) / 255;
				
				if(alpha == 255)
				{
					//use memcpy next time...might be faster
					BottomBit[RED_BITMAP_OFFSET] = TopBit[RED_BITMAP_OFFSET];
					BottomBit[GREEN_BITMAP_OFFSET] = TopBit[GREEN_BITMAP_OFFSET];
					BottomBit[BLUE_BITMAP_OFFSET] = TopBit[BLUE_BITMAP_OFFSET];
				}
				else if(alpha > 0)
				{
					BottomBit[RED_BITMAP_OFFSET] = BottomBit[RED_BITMAP_OFFSET] + ((TopBit[RED_BITMAP_OFFSET] - BottomBit[RED_BITMAP_OFFSET]) * alpha + 127) / 255;
					BottomBit[GREEN_BITMAP_OFFSET] = BottomBit[GREEN_BITMAP_OFFSET] + ((TopBit[GREEN_BITMAP_OFFSET] - BottomBit[GREEN_BITMAP_OFFSET]) * alpha + 127) / 255;
					BottomBit[BLUE_BITMAP_OFFSET] = BottomBit[BLUE_BITMAP_OFFSET] + ((TopBit[BLUE_BITMAP_OFFSET] - BottomBit[BLUE_BITMAP_OFFSET]) * alpha + 127) / 255;
				}
				TopBit += GetChannels(Top);
				BottomBit += GetChannels(Bottom);
			}
		}
		break;
	case 3: //24
		//since 24-bit don't have Alpha, just use the global opacity
		for(int Ty = StartY; Ty < EndY; Ty++)
		{
			TopBit = GetPixelPtr(Top, StartX - indexX, Ty - indexY);
			BottomBit = GetPixelPtr(Bottom, StartX, Ty);

			for(int Tx = StartX; Tx < EndX; Tx++)
			{	
				if(GlobalOpacity == 255)
				{
					//use memcpy next time...might be faster
					BottomBit[RED_BITMAP_OFFSET] = TopBit[RED_BITMAP_OFFSET];
					BottomBit[GREEN_BITMAP_OFFSET] = TopBit[GREEN_BITMAP_OFFSET];
					BottomBit[BLUE_BITMAP_OFFSET] = TopBit[BLUE_BITMAP_OFFSET];
				}
				else if(GlobalOpacity > 0)
				{
					BottomBit[RED_BITMAP_OFFSET] = BottomBit[RED_BITMAP_OFFSET] + ((TopBit[RED_BITMAP_OFFSET] - BottomBit[RED_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
					BottomBit[GREEN_BITMAP_OFFSET] = BottomBit[GREEN_BITMAP_OFFSET] + ((TopBit[GREEN_BITMAP_OFFSET] - BottomBit[GREEN_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
					BottomBit[BLUE_BITMAP_OFFSET] = BottomBit[BLUE_BITMAP_OFFSET] + ((TopBit[BLUE_BITMAP_OFFSET] - BottomBit[BLUE_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
				}
				TopBit += GetChannels(Top);
				BottomBit += GetChannels(Bottom);
			}
		}
		break;
	case 2: //16
		//arg
		break;
	case 1: //8
		for(int Ty = StartY; Ty < EndY; Ty++)
		{
			TopBit = GetPixelPtr(Top, StartX - indexX, Ty - indexY);
			BottomBit = GetPixelPtr(Bottom, StartX, Ty);

			for(int Tx = StartX; Tx < EndX; Tx++)
			{	
				if(GlobalOpacity == 255)
				{
					BottomBit[RED_BITMAP_OFFSET] = palette[*TopBit].rgbRed;
					BottomBit[GREEN_BITMAP_OFFSET] = palette[*TopBit].rgbGreen;
					BottomBit[BLUE_BITMAP_OFFSET] = palette[*TopBit].rgbBlue;
				}
				else if(GlobalOpacity > 0 && GlobalOpacity < 255)
				{
					BottomBit[RED_BITMAP_OFFSET] = BottomBit[RED_BITMAP_OFFSET] + ((palette[*TopBit].rgbRed - BottomBit[RED_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
					BottomBit[GREEN_BITMAP_OFFSET] = BottomBit[GREEN_BITMAP_OFFSET] + ((palette[*TopBit].rgbGreen - BottomBit[GREEN_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
					BottomBit[BLUE_BITMAP_OFFSET] = BottomBit[BLUE_BITMAP_OFFSET] + ((palette[*TopBit].rgbBlue - BottomBit[BLUE_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
				}

				TopBit += GetChannels(Top);
				BottomBit += GetChannels(Bottom);
			}
		}
		break;
	}
}
/*
void CompositeBitmap(CSBitmap* Bottom, CSBitmap* Top, int StartX, int StartY, uint8_t GlobalOpacity)
{
	// Insert your code here.


	//Check to see if the Top bitmap is not visible at all, if it's true, then just quit
	if((StartX >= GetWidth(Bottom))
		|| (StartY >= GetHeight(Bottom)))
		return;

	int indexX = 0, indexY = 0; //the starting index from the image bits
	int EndX = GetWidth(Top); //the ending index
	int EndY = GetHeight(Top);

	//check to see again if the Top bitmap is not visible at all
	if((StartX + EndX < 0)
		|| (StartY + EndY < 0))
		return;
	
	//adjust the starting image index if the location is off the edge
	if(EndX > GetWidth(Bottom))
		//EndX -= EndX - GetWidth(Bottom);
		EndX = GetWidth(Bottom);
	if(EndY > GetHeight(Bottom))
		//EndY -= EndY - GetHeight(Bottom);
		EndY = GetHeight(Bottom);

	//adjust the starting image index if the location is off the edge
	if(StartX < 0)
	{ indexX -= StartX; StartX = 0;  } 
	if(StartY < 0)
	{ indexY -= StartY; StartY = 0;  }

	uint8_t *TopBit, *BottomBit;
	uint8_t alpha;
	int By = StartY;

	//get this only if we have 8-bit Top
	RGBQUAD palette[255] = {0}; 
	if(GetChannels(Top) == 1)
		GetPalette(Top, 0, 256, palette);

	switch(GetChannels(Top))
	{
	case 4: //32
		for(int Ty = indexY; Ty < EndY; Ty++, By++)
		{
			TopBit = GetPixelPtr(Top, indexX, Ty);
			BottomBit = GetPixelPtr(Bottom, StartX, Ty - indexY);

			for(int Tx = indexX; Tx < EndX; Tx++)
			{
				alpha = TopBit[ALPHA_BITMAP_OFFSET];
				alpha = (alpha * GlobalOpacity + 127) / 255;
				
				if(alpha == 255)
				{
					//use memcpy next time...might be faster
					BottomBit[RED_BITMAP_OFFSET] = TopBit[RED_BITMAP_OFFSET];
					BottomBit[GREEN_BITMAP_OFFSET] = TopBit[GREEN_BITMAP_OFFSET];
					BottomBit[BLUE_BITMAP_OFFSET] = TopBit[BLUE_BITMAP_OFFSET];
				}
				else if(alpha > 0)
				{
					BottomBit[RED_BITMAP_OFFSET] = BottomBit[RED_BITMAP_OFFSET] + ((TopBit[RED_BITMAP_OFFSET] - BottomBit[RED_BITMAP_OFFSET]) * alpha + 127) / 255;
					BottomBit[GREEN_BITMAP_OFFSET] = BottomBit[GREEN_BITMAP_OFFSET] + ((TopBit[GREEN_BITMAP_OFFSET] - BottomBit[GREEN_BITMAP_OFFSET]) * alpha + 127) / 255;
					BottomBit[BLUE_BITMAP_OFFSET] = BottomBit[BLUE_BITMAP_OFFSET] + ((TopBit[BLUE_BITMAP_OFFSET] - BottomBit[BLUE_BITMAP_OFFSET]) * alpha + 127) / 255;
				}
				TopBit += GetChannels(Top);
				BottomBit += GetChannels(Bottom);
			}
		}
		break;
	case 3: //24
		//since 24-bit don't have Alpha, just use the global opacity
		for(int Ty = indexY; Ty < EndY; Ty++)
		{
			TopBit = GetPixelPtr(Top, indexX, Ty);
			BottomBit = GetPixelPtr(Bottom, StartX, Ty - indexY);

			for(int Tx = indexX; Tx < EndX; Tx++)
			{	
				if(GlobalOpacity == 255)
				{
					//use memcpy next time...might be faster
					BottomBit[RED_BITMAP_OFFSET] = TopBit[RED_BITMAP_OFFSET];
					BottomBit[GREEN_BITMAP_OFFSET] = TopBit[GREEN_BITMAP_OFFSET];
					BottomBit[BLUE_BITMAP_OFFSET] = TopBit[BLUE_BITMAP_OFFSET];
				}
				else if(GlobalOpacity > 0)
				{
					BottomBit[RED_BITMAP_OFFSET] = BottomBit[RED_BITMAP_OFFSET] + ((TopBit[RED_BITMAP_OFFSET] - BottomBit[RED_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
					BottomBit[GREEN_BITMAP_OFFSET] = BottomBit[GREEN_BITMAP_OFFSET] + ((TopBit[GREEN_BITMAP_OFFSET] - BottomBit[GREEN_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
					BottomBit[BLUE_BITMAP_OFFSET] = BottomBit[BLUE_BITMAP_OFFSET] + ((TopBit[BLUE_BITMAP_OFFSET] - BottomBit[BLUE_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
				}
				TopBit += GetChannels(Top);
				BottomBit += GetChannels(Bottom);
			}
		}
		break;
	case 2: //16
		//arg
		break;
	case 1: //8
		for(int Ty = indexY; Ty < EndY; Ty++)
		{
			TopBit = GetPixelPtr(Top, indexX, Ty);
			BottomBit = GetPixelPtr(Bottom, StartX, Ty - indexY);

			for(int Tx = indexX; Tx < EndX; Tx++)
			{	
				if(GlobalOpacity == 255)
				{
					BottomBit[RED_BITMAP_OFFSET] = palette[*TopBit].rgbRed;
					BottomBit[GREEN_BITMAP_OFFSET] = palette[*TopBit].rgbGreen;
					BottomBit[BLUE_BITMAP_OFFSET] = palette[*TopBit].rgbBlue;
				}
				else if(GlobalOpacity > 0 && GlobalOpacity < 255)
				{
					BottomBit[RED_BITMAP_OFFSET] = BottomBit[RED_BITMAP_OFFSET] + ((palette[*TopBit].rgbRed - BottomBit[RED_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
					BottomBit[GREEN_BITMAP_OFFSET] = BottomBit[GREEN_BITMAP_OFFSET] + ((palette[*TopBit].rgbGreen - BottomBit[GREEN_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
					BottomBit[BLUE_BITMAP_OFFSET] = BottomBit[BLUE_BITMAP_OFFSET] + ((palette[*TopBit].rgbBlue - BottomBit[BLUE_BITMAP_OFFSET]) * GlobalOpacity + 127) / 255;
				}

				TopBit += GetChannels(Top);
				BottomBit += GetChannels(Bottom);
			}
		}
		break;
	}
}*/
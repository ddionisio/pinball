#include "..\common.h"
// There's no point doing precompiled headers past this point.
// They only save you compile time if you always include the
// same set of header files up to the first #pragma hdrstop.
#pragma hdrstop

#include "csdefs.h"

// Copyright © 1999 Bruce Dawson.


// This function returns the file portion of the filename.  If there is
// no filename (ie; if PathName is an empty string, or if PathName appears
// to name a directory (ie; final character is a backslash)) then it returns
// a pointer to the null character at the end of the filename (ie; a null
// string).  If there are no backslashes then it is assumed that the entire
// name represents a filename, so the entire string is returned. This function
// will never return zero. You can remove the filename from any path by
// writing zero to the char pointed at by the return value.

char *GetFilePart(const char *PathName)
{
	assert(PathName);
	char* LastSlash = (char *)strrchr(PathName, '\\');
	if (LastSlash)
		return LastSlash + 1;
	return (char *)PathName;
}

// This function returns the file extension, pointing at the period, if there
// is one.  If there is no file extension, it points to the null character at
// the end of the filename (ie; a null string).

char *GetExtension(const char *PathName)
{
	assert(PathName);
	char* FilePart = GetFilePart(PathName);
	assert(FilePart);
	char* LastPeriod = strrchr(FilePart, '.');
	if (LastPeriod)
		return LastPeriod;
	return FilePart + strlen(FilePart);
}

bool GetFileName(HWND Window, char* FileName, size_t FileNameSize,
				const char* Filter, const char* Title, const char* Extension,
				DWORD Flags, bool OpenFileDialog)
{
	char	TempArray[MAX_PATH];
	char	InitialDir[MAX_PATH];

	OPENFILENAME OpenFile =
	{
		sizeof(OPENFILENAME),
		Window,
		0,
		Filter,
		0, 0, 0,	// CustomFilter, MaxCustFilter, nFilterIndex
		TempArray, sizeof(TempArray),
		0, 0,	// File title,
		InitialDir,
		Title,
		Flags,
		0,
		0,
		Extension,
		0, 0, 0	// CustData, hook, templatename
	};

	strcpy(TempArray, FileName);
	strcpy(InitialDir, FileName);
	GetFilePart(InitialDir)[0] = 0;	// Strip off file part.
	// Fix some quirkiness in drive names? I don't remember the point.
	char* DirEnd = InitialDir + strlen(InitialDir);
	if (DirEnd > InitialDir + 1 && DirEnd[-1] == '\\')
	{
		if (DirEnd[-2] != ':')
			DirEnd[-1] = 0;
	}

	BOOL Result;
	if (OpenFileDialog)
		Result = GetOpenFileName(&OpenFile);
	else
		Result = GetSaveFileName(&OpenFile);
	if (!Result && CommDlgExtendedError() == FNERR_INVALIDFILENAME)
	{
		TempArray[0] = 0;
		OpenFile.lpstrInitialDir = 0;
		if (OpenFileDialog)
			Result = GetOpenFileName(&OpenFile);
		else
			Result = GetSaveFileName(&OpenFile);
	}
	if (Result)
		lstrcpyn(FileName, TempArray, FileNameSize);
	return Result != FALSE;
}

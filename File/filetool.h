#ifndef _filetool_h
#define _filetool_h

#include "..\common.h"

typedef FILE *	 FILEHANDLE;		//Type cast of FILE* to a FILEHANDLE

/********************************************************************
*																	*
*							Flags   							    *
*																	*
********************************************************************/

#define		FILEREAD	0x0 // Read case
#define		FILEWRITE	0x1	// Write case

#define		FILEBINARY	0x0	// Binary case
#define		FILETEXT	0x2 // Text case

#define		FILESEEKSET 0xc //Beginning of file
#define		FILESEEKCUR 0xd //Current position of file
#define		FILESEEKEND 0xe //End of file

/*********************** Initialization/Deinitialization ***************/

// - Purpose:    Deinitializes a file manager
// - Returns:    True of initialization succeeded.  False if it failed.
PROTECTED BOOL FileInit ();

// - Purpose:    Deinitializes a file manager
// - Returns:    True of deinitialization succeeded.  False if it failed.
PROTECTED BOOL FileTerm ();

/*********************** File Opening/Closing **************************/

// - Purpose:    Opens a file
// - Returns:    Handle to the file if opened.  NULL if open failed.
PUBLIC FILEHANDLE FileOpenFile (char * path, FLAGS flags);

// - Purpose:    Closes a file
// - Returns:    Handle to the file if closed.  NULL if close failed.
PUBLIC BOOL FileCloseFile (FILEHANDLE file);

// - Purpose:    Uses a dialog window to pick a file for SAVE or LOAD
// - Returns:    Handle to the file if found.  NULL if not found
PUBLIC FILEHANDLE FileWindow(char * type, char * extention, FLAGS flag);

/*********************** File I/O **************************************/
PUBLIC BOOL FileMovePos(FILEHANDLE file, long offset, FLAGS flag);
// - Purpose:	 Moves the file pointer to a specific offset of the file
//				 for flag, use: FILESEEKSET, FILESEEKCUR, FILESEEKEND
// - Returns:	 true - success false - go insane!

/*********************** File I/O **************************************/

// - Purpose:    Reads a chunk of data from a file
// - Returns:    True if read was successful.  False if read failed.
PUBLIC BOOL FileRead (FILEHANDLE file, void * stream, int size);

// - Purpose:    Writes a chunk of data to a file
// - Returns:    True if write was successful.  False if write failed.
PUBLIC BOOL FileWrite (FILEHANDLE file, void * stream, int size);


/*********************** File Location *********************************/

// - Purpose:    Used to find a file based on the FILEHANDLE
// - Returns:    Index to the file.  -1 if not found
PUBLIC int FileFindFileHandle (FILEHANDLE file);

// - Purpose:    Used to find a file.  For a specific file.. not generic
// - Returns:    Handle to the file if found.  NULL if not found.
PUBLIC FILEHANDLE FileFindFile (char * path, FLAGS flags);

// - Purpose:    Finds the first file with the given extention from start
// - Returns:    The index of the file for use in FileGetFileNum. -1 if none
PUBLIC int FileFindAllFiles (char * extention, int start);

// - Purpose:    Access a file by it's index number in the fileList
// - Returns:    FILEHANDLE to the file at that index.  NULL if incorrect
PUBLIC FILEHANDLE FileGetFileNum (int fileNum);

#endif
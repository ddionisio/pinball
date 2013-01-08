#ifndef _i_filetool_h
#define _i_filetool_h

#include "filetool.h"
#include "..\List\list.h"

typedef struct _FILENODE {
	FILEHANDLE	 file;				//Contains a file's data
	char		 path[PATH_SIZE];	//Path of file
	FLAGS		 flags;				//Status flags for the file
}  FILENODE, * PFILENODE;

/*********************** File Opening/Closing **************************/

// - Purpose:    Initializes the OPENFILENAME variable for Dialog Open/Save
// - Returns:    OPENFILENAME that's been initialized
PRIVATE OPENFILENAME FileSetOPENFILENAME 
		(char* prompt, char* fileName, char* filter, FLAGS flags);

/*********************** File Location *********************************/

// - Purpose:    Used to find a file based on the FILEHANDLE
// - Returns:    Index to the file.  -1 if not found
PUBLIC int FileFindFileHandle (FILEHANDLE file);

/*********************** FILENODE Deconstruction ***********************/

// - Purpose:    Used to set a FILENODEs FILEHANDLE
// - Returns:    FILEHANDLE
PRIVATE void FileSetHandle (PFILENODE fileNode, FILEHANDLE file);

// - Purpose:    Used to set a FILENODEs path
// - Returns:    path 
PRIVATE void FileSetPath (PFILENODE fileNode, char* path);

// - Purpose:    Used to set a FILENODEs flags
// - Returns:    flags

PRIVATE void FileSetFlags (PFILENODE fileNode, FLAGS flags);
// - Purpose:    Used to get a FILENODEs FILEHANDLE
// - Returns:    FILEHANDLE

PRIVATE FILEHANDLE FileGetHandle (FILENODE fileNode);

// - Purpose:    Used to get a FILENODEs path
// - Returns:    path 
PRIVATE char* FileGetPath (FILENODE fileNode);

// - Purpose:    Used to get a FILENODEs flags
// - Returns:    flags
PRIVATE FLAGS FileGetFlags (FILENODE fileNode);

#endif

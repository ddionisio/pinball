#include "i_filetool.h"

PRIVATE LISTHANDLE fileList;			//Pointer to all open files

/*********************** Initialization/Deinitialization ***************/

// - Purpose:    Initializes a file manager
// - Returns:    True of initialization succeeded.  False if it failed.
PROTECTED BOOL FileInit ()
{
	fileList = ListInit(MAX_FILES, sizeof(FILENODE));
	if(fileList)
		return true;
	return false;
}

// - Purpose:    Deinitializes a file manager
// - Returns:    True of deinitialization succeeded.  False if it failed.
PROTECTED BOOL FileTerm ()
{
	ListDestroy(&fileList);
	if(fileList == NULL)
		return true;
	return false;
}

/*********************** File Opening/Closing **************************/

// - Purpose:    Opens a file
// - Returns:    Handle to the file if opened.  NULL if open failed.
PUBLIC FILEHANDLE FileOpenFile (char * path, FLAGS flags)
{
	if(FileFindFile(path, flags))
		return NULL;

	FILENODE newFile;

	switch(GETFLAGS(flags,FILEWRITE))
	{
	case FILEREAD:
		if (GETFLAGS(flags,FILETEXT))
			FileSetHandle(&newFile, fopen(path,"rt"));

		else
			FileSetHandle(&newFile, fopen(path,"rb"));

		break;

	case FILEWRITE:
		if (GETFLAGS(flags,FILETEXT))
			FileSetHandle(&newFile, fopen(path,"wt"));

		else
			FileSetHandle(&newFile, fopen(path,"wb"));

		break;
	}

	FileSetPath(&newFile, path);
	FileSetFlags(&newFile, flags);

	if(ListAppend(fileList, &newFile) != RETCODE_SUCCESS)
		return NULL;

	return FileGetHandle(newFile);
}

// - Purpose:    Closes a file
// - Returns:    True if the file was closed.  False if close failed.
PUBLIC BOOL FileCloseFile (FILEHANDLE file)
{
	int index = FileFindFileHandle(file);

	if(index < 0)
		return false;

	if(fclose(file) != 0)
		return false;
	
	if(ListDelete(fileList, index) != RETCODE_SUCCESS)
		return false;

	return true;
}

// - Purpose:    Uses a dialog window to pick a file for SAVE or LOAD
// - Returns:    Handle to the file if found.  NULL if not found
PUBLIC FILEHANDLE FileWindow(char * type, char * extention, FLAGS flag)
{
	OPENFILENAME fInfo;
	char		 fileName [PATH_SIZE];
	char		 filter   [PATH_SIZE] = "";
	char         prompt   [PATH_SIZE] = "";
	FLAGS		 parameters;

	if(flag == FILEREAD)
	{
		parameters = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
		strcat(prompt, "Open ");
	}
	else
	if(flag == FILEWRITE)
	{
		parameters = OFN_OVERWRITEPROMPT;
		strcat(prompt, "Save ");
	}
	else
		return NULL;
	
	if(type == NULL || extention == NULL)
	{
		strcat(prompt,"Any File");
	}
	else
	{
		strcat(prompt, type);
		strcat(prompt, " File");

		strcat(filter, type);
		strcat(filter, "  ");
		strcat(filter, extention);
		strcat(filter, "\0\0");
		filter[strlen(filter)-strlen(extention)-1] = '*';
		filter[strlen(filter)-strlen(extention)-2] = '\0';
	}
	
    fInfo = FileSetOPENFILENAME(prompt, fileName, filter, parameters);
	
	if(GetOpenFileName(&fInfo)==0)
		return false;

	if(fInfo.nFileExtension == NULL)
		strcat(fileName, extention);

	return FileOpenFile(fileName, flag);
}

// - Purpose:    Initializes the OPENFILENAME variable for Dialog Open/Save
// - Returns:    OPENFILENAME that's been initialized
PRIVATE OPENFILENAME FileSetOPENFILENAME 
		(char* prompt, char* fileName, char* filter, FLAGS flags)
{
	OPENFILENAME fInfo = {0};
	
	fInfo.lStructSize = sizeof(fInfo);
	fInfo.hwndOwner = NULL;
	fInfo.hInstance = 0;
	fInfo.lpstrFile = NULL;
	fInfo.nMaxFile = 0;
	fInfo.lpstrInitialDir = NULL;
	fInfo.lpstrFileTitle = fileName;
	fInfo.nMaxFileTitle = PATH_SIZE;
	fInfo.Flags = OFN_HIDEREADONLY | flags;
	fInfo.lpstrFilter = filter;
	fInfo.lpstrTitle = prompt;
	
	return fInfo;
}

/*********************** File I/O **************************************/

// - Purpose:    Reads a chunk of data from a file
// - Returns:    True if read was successful.  False if read failed.
PUBLIC BOOL FileRead (FILEHANDLE file, void * stream, int size)
{
	int count = fread(stream, sizeof(Byte), size, file);

	if(count == size)
		return true;
	return false;
}

// - Purpose:    Writes a chunk of data to a file
// - Returns:    True if write was successful.  False if write failed.
PUBLIC BOOL FileWrite (FILEHANDLE file, void * stream, int size)
{
	int count = fwrite(stream, size, sizeof(Byte), file);

	if(count == size)
		return true;
	return false;
}

/*********************** File I/O **************************************/

// - Purpose:	 Moves the file pointer to a specific offset of the file
//				 for flag, use: FILESEEKSET, FILESEEKCUR, FILESEEKEND
// - Returns:	 true - success false - go insane!
PUBLIC BOOL FileMovePos(FILEHANDLE file, long offset, FLAGS flag)
{
	switch(flag)
	{
	case FILESEEKSET:
		if(fseek(file, offset, SEEK_SET))
			return FALSE;
		break;
	case FILESEEKCUR:
		if(fseek(file, offset, SEEK_CUR))
			return FALSE;
		break;
	case FILESEEKEND:
		if(fseek(file, offset, SEEK_END))
			return FALSE;
		break;
	default:
		return FALSE;
	}

	return TRUE;
}


/*********************** File Location *********************************/

// - Purpose:    Used to find a file based on the FILEHANDLE
// - Returns:    Index to the file.  -1 if not found
PUBLIC int FileFindFileHandle (FILEHANDLE file)
{
	PFILENODE current;
	long	  max = ListGetNodeCount(fileList);

	for(int x = 0; x < max; x++)
	{
		current = (PFILENODE)ListExtract(fileList, x);
		if(FileGetHandle(*current) == file)
			return x;
	}

	return -1;
}

// - Purpose:    Used to find a file
// - Returns:    Handle to the file if found.  NULL if not found.
PUBLIC FILEHANDLE FileFindFile (char * path, FLAGS flags)
{
	PFILENODE current;
	long	  max = ListGetNodeCount(fileList);

	for(int x = 0; x < max; x++)
	{
		current = (PFILENODE)ListExtract(fileList, x);
		if(strcmp(FileGetPath(*current), path) == 0 && FileGetFlags(*current) == flags)
			return FileGetHandle(*current);
	}
	
	return NULL;
}

// - Purpose:    Finds the first file with the given extention from start
// - Returns:    The index of the file for use in FileGetFileNum. -1 if none
PUBLIC int FileFindAllFiles (char * extention, int start)
{
	PFILENODE current;
	long	  max = ListGetNodeCount(fileList);

	if(start < 0 || start > max)
		return -1;

	char extTemp [PATH_SIZE];
	int  idx;
	int  extLen = strlen(extention);
	int  patLen;

	for(int x = start; x < max; x++)
	{
		current = (PFILENODE)ListExtract(fileList, x);
		patLen = strlen(FileGetPath(*current));
		if(patLen > extLen)
		{
			for(idx = 0; idx < patLen; idx++)
				extTemp[idx] = FileGetPath(*current)[patLen-idx];

			extTemp[idx+1] = '\0';
	
			if(strcmp(extTemp, extention) == 0)
				return x;
		}
	}
	
	return -1;
}

// - Purpose:    Access a file by it's index number in the fileList
// - Returns:    FILEHANDLE to the file at that index.  NULL if incorrect
PUBLIC FILEHANDLE FileGetFileNum (int fileNum)
{
	if(fileNum < 0 || fileNum > ListGetNodeCount(fileList))
		return NULL;

	return FileGetHandle(*(PFILENODE)ListExtract(fileList, fileNum));
}

/*********************** FILENODE Deconstruction ***********************/

// - Purpose:    Used to set a FILENODEs FILEHANDLE
// - Returns:    FILEHANDLE
PRIVATE void FileSetHandle (PFILENODE fileNode, FILEHANDLE file)
{
	fileNode->file = file;
}

// - Purpose:    Used to set a FILENODEs path
// - Returns:    path 
PRIVATE void FileSetPath (PFILENODE fileNode, char* path)
{
	strcpy(fileNode->path, path);
}

// - Purpose:    Used to set a FILENODEs flags
// - Returns:    flags
PRIVATE void FileSetFlags (PFILENODE fileNode, FLAGS flags)
{
	fileNode->flags = flags;
}

// - Purpose:    Used to get a FILENODEs FILEHANDLE
// - Returns:    FILEHANDLE
PRIVATE FILEHANDLE FileGetHandle (FILENODE fileNode)
{
	return fileNode.file;
}

// - Purpose:    Used to get a FILENODEs path
// - Returns:    path 
PRIVATE char* FileGetPath (FILENODE fileNode)
{
	return fileNode.path;
}

// - Purpose:    Used to get a FILENODEs flags
// - Returns:    flags
PRIVATE FLAGS FileGetFlags (FILENODE fileNode)
{
	return fileNode.flags;
}
#ifndef MEMORY_H
#define MEMORY_H

/************************************************************************
*																		*
*							Includes									*
*																		*
************************************************************************/

#include "..\common.h"
#include "..\List\list.h"

/********************************************************************
*																	*
*							Return codes							*
*																	*
********************************************************************/

#define MEMCODE_MEMORYLEAK		0x2
#define MEMCODE_OUTOFMEMORY		0x3
#define MEMCODE_OVERFLOW		0x4

/********************************************************************
*																	*
*							Interface								*
*																	*
********************************************************************/

PUBLIC RETCODE MemAlloc (void ** memory, Dword numBytes);

// Purpose:    Used to allocate memory of a given size to a particular datum
// Input:      Pointer to context to attach memory to, and block size
// Return:     A code indicating the results of the allocation

PUBLIC RETCODE MemFree (void ** memory);

// Purpose:    Used to release memory of a particular datum
// Input:      Pointer to context to release from
// Return:     A code indicating the results of the release

/********************************************************************
*																	*
*							Utilities								*
*																	*
********************************************************************/

PROTECTED RETCODE MemInit (void);

// Purpose:    Initializes a memory manager
// Input:      No input
// Return:     A code indicating the results of the initialization    

PROTECTED RETCODE MemTerm (void);

// Purpose:    Deinitializes a memory manager
// Input:      Memory pointer to deinitialize through
// Return:     A code indicating the results of the termination

PROTECTED void * MemGrabBlock (Dword numBytes);

// Purpose:	   Used to grab a memory block for use by outside data
// Input:	   Byte size of block to grab
// Return:	   A pointer to a block of memory

PROTECTED RETCODE MemReleaseBlock (void * data, Dword numBytes);

// Purpose:    Used to release a memory block used by outside data
// Input:	   A data block, and byte size of block to release
// Return:	   A code indicating the results of the block release operation

#endif // MEMORY_H
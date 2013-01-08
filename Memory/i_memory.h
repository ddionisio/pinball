#ifndef I_MEMORY_H
#define I_MEMORY_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include <malloc.h>
#include <memory.h>

#include "..\common.h"
#include "memory.h"

#define MEMPOOL_SIZE	0x7F7F80

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define MALLOC(ptr,type,size) (ptr) = (type*) malloc (size), ZeroMemory((ptr),(size))
#define FREE(ptr)			  free (ptr), (ptr) = NULL

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

///////////////////////////////////////////////////////
// _MEMBLOCK: Container for memory block information //
///////////////////////////////////////////////////////

typedef struct _MEMBLOCK {
	struct _MEMBLOCK * Next;	// Pointer to next memory block
	Dword Size;					// Size of memory block in bytes
} MEMBLOCK, * PMEMBLOCK;

///////////////////////////////////////////////////////////
// _MEMHEAD: Encapsulation of memory control information //
///////////////////////////////////////////////////////////

typedef struct _MEMHEAD {
	PMEMBLOCK MemBlocks;		// Pointer to memory blocks in program
	Dword FreeBytes;			// Count of free memory bytes
	Dword NumAllocs;			// Number of dynamic allocations performed
	Byte Memory [BASE_AMOUNT];	// Memory bank
} MEMHEAD, * PMEMHEAD;

/********************************************************************
*																	*
*							Implementation							*
*																	*
********************************************************************/

PRIVATE RETCODE MemAllotBytes (Dword numBytes);

// Purpose:    Used to allot bytes of memory from the memory pool
// Input:      A number of bytes to allot
// Return:     A code indicating the results of the byte allotment

PRIVATE RETCODE MemCollectBytes (Dword numBytes);

// Purpose:    Used to collect alloted bytes of memory into the memory pool
// Input:      A number of bytes to collect
// Return:     A code indicating the results of the byte collection

#endif // I_MEMORY_H
#ifndef LIST_H
#define LIST_H

/********************************************************************
*																	*
*							Include									*
*																	*
********************************************************************/

#include "..\common.h"
#include "..\Memory\memory.h"

/********************************************************************
*																	*
*							Values									*
*																	*
********************************************************************/

#define LISTSORT_ASCEND		0x0	// Sort a list in ascending order
#define LISTSORT_DESCEND	0x1 // Sort a list in descending order

/********************************************************************
*																	*
*							Return code								*
*																	*
********************************************************************/

#define LISTCODE_NOOPERATION	0x2
#define LISTCODE_OUTOFBOUNDS	0x3
#define LISTCODE_LISTEMPTY		0x4
#define LISTCODE_LISTFULL		0x5

/********************************************************************
*																	*
*							Interface								*
*																	*
********************************************************************/

PUBLIC LISTHANDLE ListInit (short numNodes, long sizeofObject);

// Purpose:    Initializes a list pointer, and refers said pointer to a block of memory
// Input:      A maximal node count for the list, and a size of a single object to store in list
// Return:     A pointer to the initialized list

PUBLIC AMOUNT ListGetNodeCount (LISTHANDLE list);
	
// Purpose:    Access function; returns a list's node count
// Input:      Pointer to list to retrieve node count from
// Return:     Count of list nodes

PUBLIC AMOUNT ListGetMaxNodes (LISTHANDLE list);
	
// Purpose:    Access function; returns a list's maximal node count
// Input:      Pointer to list to retrieve node count from
// Return:     Maximal count of list nodes

PUBLIC RETCODE ListInsert (LISTHANDLE list, int index, void * data);

// Purpose:    Inserts a data item into a list at a given index
// Input:      Pointer to list to insert to, index of insertion, and address of data to insert
// Return:     A code indicating the results of the insertion

PUBLIC RETCODE ListAppend (LISTHANDLE list, void * data);

// Purpose:    Appends data to a list
// Input:      Pointer to list to append to, and address of data to append
// Return:     A code indicating the results of the appension

PUBLIC RETCODE ListMove (LISTHANDLE dstList, LISTHANDLE srcList, int index);

// Purpose:    Moves data from one list into another list
// Input:      Pointer to list to move data to, pointer to list to move data from, and index of data item to move
// Return:     A code indicating the results of the move operation

PUBLIC RETCODE ListCopy (LISTHANDLE dstList, LISTHANDLE srcList, int index);

// Purpose:    Copies data from one list into another list
// Input:      Pointer to list to copy data to, pointer to list to copy data from, and index of data item to copy
// Return:     A code indicating the results of the copy operation

PUBLIC RETCODE ListSwap (LISTHANDLE list, int indexOne, int indexTwo);

// Purpose:	   Swaps two data items in a list
// Input:	   Pointer to list to swap within, index of first data item, and index of second data item
// Return:	   A code indicating the results of the swap

PUBLIC void * ListGetPrev (LISTHANDLE list);
	
// Purpose:    Extracts data from previous node in list
// Input:      Pointer to list to extract from
// Return:     Address of extracted data

PUBLIC void * ListGetNext (LISTHANDLE list);

// Purpose:    Extracts data from subsequent node in list
// Input:      Pointer to list to extract from
// Return:     Address of extracted data

PUBLIC void * ListExtract (LISTHANDLE list, int index);

// Purpose:    Extracts data from a list at a given index
// Input:      Pointer to list to extract from, and index of extraction
// Return:     Address of extracted data

PUBLIC RETCODE ListDelete (LISTHANDLE list, int index);

// Purpose:    Deletes a data item from a list at a given index
// Input:      Pointer to list to delete from, and index of deletion
// Return:     A code indicating the results of the deletion

PUBLIC RETCODE ListEmptyList (LISTHANDLE list);

// Purpose:    Empties all data items from a list
// Input:      Pointer to list to empty
// Return:     A code indicating the results of the empty operation

PUBLIC RETCODE ListSort (LISTHANDLE list, COMPARE callback, long type);

// Purpose:    Sort the elements of a list in a user-defined manner
// Input:      Pointer to list to sort, a callback function to regulate sort process, and sort type
// Return:     A code indicating the results of the sort

PUBLIC INDEX ListSearch (LISTHANDLE list, EQUIVALENCE callback, void * item);

// Purpose:    Search for an item among the elements of a list in a user-defined manner
// Input:      Pointer to list to search, a callback function to regulate search process, and item to search for
// Return:     Index of first instance of item found, or SEARCH_NOTFOUND if item is not in list

PUBLIC RETCODE ListDestroy (LISTHANDLE * list);

// Purpose:    Deinitializes a list, and releases all memory owned by said list to the memory pool
// Input:      Address of pointer to list to destroy
// Return:     A code indicating the results of the destruction

PUBLIC void * ListGrabBuffer (LISTHANDLE list);

// Purpose:	   Used to grab the memory from a list
// Input:	   Pointer to list to grab memory from
// Return:	   Memory grabbed from list

#endif // LIST_H
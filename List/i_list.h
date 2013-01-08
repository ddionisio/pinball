#ifndef I_LIST_H
#define I_LIST_H

/********************************************************************
*																	*
*							Includes								*
*																	*
********************************************************************/

#include "..\common.h"
#include "list.h"

/********************************************************************
*																	*
*							Macros									*
*																	*
********************************************************************/

#define GRAB_ROOT(node,list) (node) = (list)->Head		// Used to grab a list's root node

#define WALK_BACK(walker)  (walker) = (walker)->Prev	// Used to walk back to previous node
#define WALK_AHEAD(walker) (walker) = (walker)->Next	// Used to walk ahead to subsequent node

/********************************************************************
*																	*
*							Types									*
*																	*
********************************************************************/

////////////////////////////////////////////////////
// _LISTNODE: Container for list node information //
////////////////////////////////////////////////////

typedef struct _LISTNODE {
	struct _LISTNODE * Prev;// Pointer back to previous list node
	struct _LISTNODE * Next;// Pointer ahead to subsequent list node
	Pbyte Data;				// Pointer to list memory
} LISTNODE, * PLISTNODE;

//////////////////////////////////////////////
// _LIST: Encapsulation of list information //
//////////////////////////////////////////////

typedef struct _LIST {
	PLISTNODE Current;	// Pointer to current list node
	PLISTNODE Head;		// Pointer to head of active nodes
	PLISTNODE Free;		// Pointer to head of free nodes
	Dword SizeofObject;	// Size of single object stored in list
	Word NumNodes;		// Maximal number of list nodes		
	Word Status;		// List status
	Word Index;			// Index of current list node
	Word NodeCount;		// Current list node count
} LIST, * PLIST;

/********************************************************************
*																	*
*							Implementation							*
*																	*
********************************************************************/

PRIVATE BOOL ListIsEmpty (PLIST list);

// Purpose:    Function used internally by list to gauge list status
// Input:      A this pointer to list to check status of
// Return:     A boolean indicating list status

PRIVATE BOOL ListIsFull (PLIST list);
	
// Purpose:    Function used internally by list to gauge list status
// Input:      A this pointer to list to check status of
// Return:     A boolean indicating list status

#endif // I_LIST_H
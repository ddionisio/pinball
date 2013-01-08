	/********************************************************************************
	*																				*
	*								Includes										*
	*																				*
	********************************************************************************/

	#include "i_list.h"
	#include "..\Memory\memory.h"

	/********************************************************************************
	*																				*	!
	*								ListInit										*	!
	*																				*	!
	********************************************************************************/

	// Purpose:    Initializes a list pointer, and refers said pointer to a block of memory
	// Input:      A maximal node count for the list, and a size of a single object to store in list
	// Return:     A pointer to the initialized list

	PUBLIC PLIST ListInit (short numNodes, long sizeofObject)
	{
		INDEX index, memIndex = 0;	// An index variable; a memory index
		AMOUNT temp;				// A variable used to store a consistent amount
		PLIST List = NULL;			// Pointer to a list to initialize
		PLISTNODE ListNodes;		// List nodes to bind into list
		Pbyte MemBytes;				// Memory bytes to bind into list

#ifdef _DEBUG	// Error checking
		if (numNodes == 0 || sizeofObject == 0)	// Ascertain that nodes are being alloted
		{
			MESSAGE_BOX("No operation: ListInit failed","Error");
			return NULL;
			// Return null list
		}
#endif	// _DEBUG

		List = (PLIST) MemGrabBlock (sizeof(LIST) + numNodes * (sizeof(LISTNODE) + sizeofObject));
		// Grab a memory block for list

		List->NumNodes	   = numNodes;		// Set NumNodes to numNodes
		List->SizeofObject = sizeofObject;	// Set SizeofObject to sizeofObject

		List->Index		   = 0;				// Set Index to 0
		List->NodeCount	   = 0;				// Set NodeCount to 0
		List->Status	   = 0;				// Set Status to 0

		ListNodes	= (PLISTNODE) &List [BASE_EXTENDED];
		MemBytes	= TYPE_CAST(Pbyte,ListNodes) + numNodes * sizeof(LISTNODE);
		// Point ListNodes beyond List, and MemBytes beyond ListNodes

		temp = numNodes - 1;	// Set temp to numNodes - 1

		for (index = 0; index < numNodes; ++index)	// Walk through all nodes of list
		{
			ListNodes [index].Prev = (index != 0)	? &ListNodes [index - 1] : // Bind node back to previous node
													  &ListNodes [temp];	   // Bind first node back to final node
			ListNodes [index].Next = (index < temp) ? &ListNodes [index + 1] : // Bind node ahead to subsequent node
													  &ListNodes [0];		   // Bind final node ahead to first node

			ListNodes [index].Data = &MemBytes [memIndex];	// Link node to byte string
			memIndex			  += sizeofObject;			// Increment memory index
		}

		List->Free	  = ListNodes;	// Bind ListNodes to Free field of list
		List->Head	  = NULL;		// Nullify list head
		List->Current = NULL;		// Nullify current node of list

		return List;	
		// Return initialized list
	}

	/********************************************************************************
	*																				*	!
	*								ListGetNodeCount								*	!
	*																				*	!
	********************************************************************************/	
	
	// Purpose:    Access function; returns a list's node count
	// Input:      Pointer to list to retrieve node count from
	// Return:     Count of list nodes

	PUBLIC AMOUNT ListGetNodeCount (LIST * list)
	{
		assert(list);	// Verify that list points to valid memory

		return list->NodeCount;
		// Return number of list nodes
	}

	/********************************************************************************
	*																				*	!
	*								ListGetMaxNodes									*	!
	*																				*	!
	********************************************************************************/	
	
	// Purpose:    Access function; returns a list's maximal node count
	// Input:      Pointer to list to retrieve node count from
	// Return:     Maximal count of list nodes

	PUBLIC AMOUNT ListGetMaxNodes (LIST * list)
	{
		assert(list);	// Verify that list points to valid memory

		return list->NumNodes;
		// Return maximal node count
	}

	/********************************************************************************
	*																				*	!
	*								ListInsert										*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Inserts a data item into a list at a given index
	// Input:      Pointer to list to insert to, index of insertion, and address of data to insert
	// Return:     A code indicating the results of the insertion

	PUBLIC RETCODE ListInsert (LIST * list, int index, void * data)
	{
		PLISTNODE newNode;	// New node to bind into list
		Pbyte DataBytes;	// Data byte string
		Pbyte MemBytes;		// Memory byte string

		assert(list), assert(data);	// Verify that list and data point to valid memory

		DataBytes = (Pbyte) data;	// Set data to DataBytes

		if (index == list->NodeCount)	// Check whether a node is being inserted at the end of the list
			return ListAppend (list, data);
			// Append the node to the list

#ifdef _DEBUG	// Error checking
		if (ListIsFull (list))	// Check whether list is full
		{
			MESSAGE_BOX("List full: ListInsert failed","Error");
			return LISTCODE_LISTFULL;
			// Return full list code
		}

		if (index < 0 || index > list->NodeCount)	// Ascertain that index is within list bounds
		{
			MESSAGE_BOX("Out of bounds: ListInsert failed","Error");
			return LISTCODE_OUTOFBOUNDS;
			// Return out of bounds code
		}
#endif // _DEBUG

		if (index < list->Index)	// Check whether index of insertion is less than list index
		{
			while (list->Index > index)	// Walk through list
			{
				--list->Index;				// Decrement current index
				WALK_BACK(list->Current);	// Walk to previous node
			}
		}

		else						// Check for other cases
		{
			while (list->Index < index)	// Walk through list
			{
				++list->Index;				// Increment current index
				WALK_AHEAD(list->Current);	// Walk to subsequent node
			}
		}

		newNode				= list->Free;			// Grab primary free node

		newNode->Prev->Next = newNode->Next;		// Bind newNode->Prev to newNode->Next
		newNode->Next->Prev = newNode->Prev;		// Bind newNode->Next to newNode->Prev

		list->Free			= newNode->Next;		// Bind list's free nodes ahead to newNode's next node

		MemBytes			= newNode->Data;		// Set newNode->Data to MemBytes

		newNode->Prev		= list->Current->Prev;	// Bind newNode to list->Current->Prev
		newNode->Next		= list->Current;		// Bind newNode to list->Current

		newNode->Prev->Next = newNode;				// Bind newNode->Prev to newNode
		newNode->Next->Prev = newNode;				// Bind newNode->Next to newNode

		++list->NodeCount;	// Increment node count

		memcpy (MemBytes, DataBytes, list->SizeofObject);
		// Copy DataBytes into MemBytes

		if (list->NodeCount == list->NumNodes)	// Check for full lists
			list->Free = NULL;	// Nullify list's free nodes
		
		list->Index   = 0;			// Set index to 0
		list->Current = list->Head; // Set current node to list head

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								ListAppend										*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Appends data to a list
	// Input:      Pointer to list to append to, and address of data to append
	// Return:     A code indicating the results of the appension

	PUBLIC RETCODE ListAppend (LIST * list, void * data)
	{
		PLISTNODE newNode;	// New node to bind into list
		Pbyte DataBytes;	// Data byte string
		Pbyte MemBytes;		// Memory byte string

		assert(list), assert(data);	// Verify that list and data point to valid memory

		DataBytes = (Pbyte) data;	// Set data to DataBytes

#ifdef _DEBUG	// Error checking
		if (ListIsFull (list))	// Check whether list is full
		{
			MESSAGE_BOX("List full: ListAppend failed","Error");
			return LISTCODE_LISTFULL;
			// Return full list code
		}
#endif // _DEBUG

		newNode				= list->Free;	// Grab primary free node

		newNode->Prev->Next = newNode->Next;// Bind newNode->Prev to newNode->Next
		newNode->Next->Prev = newNode->Prev;// Bind newNode->Next to newNode->Prev

		list->Free			= newNode->Next;// Bind list's free nodes ahead to newNode's next node

		MemBytes			= newNode->Data;// Set newNode->Data to MemBytes

		if (ListIsEmpty (list))	// Check whether list is empty
		{							
			newNode->Prev = newNode;// Bind newNode->Prev to newNode	
			newNode->Next = newNode;// Bind newNode->Next to newNode

			list->Head	  = newNode;// Set newNode to list's active nodes
		}

		else				// Check for other cases
		{
			newNode->Prev		= list->Head->Prev;	// Bind newNode to list's Head->Prev component
			newNode->Next		= list->Head;		// Bind newNode to list's active nodes

			newNode->Prev->Next	= newNode;			// Bind newNode->Prev to newNode
			newNode->Next->Prev = newNode;			// Bind newNode->Next to newNode												
		}

		++list->NodeCount;	// Increment node count

		memcpy (MemBytes, DataBytes, list->SizeofObject);
		// Copy DataBytes into MemBytes

		if (list->NodeCount == list->NumNodes)	// Check for full list
			list->Free = NULL;	// Nullify list's free nodes
		
		list->Index   = 0;			// Set index to 0
		list->Current = list->Head; // Set current node to list head

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								ListMove										*
	*																				*
	********************************************************************************/

	// Purpose:    Moves data from one list into another list
	// Input:      Pointer to list to move data to, pointer to list to move data from, and index of data item to move
	// Return:     A code indicating the results of the move operation

	PUBLIC RETCODE ListMove (LIST * dstList, LIST * srcList, int index)
	{				
		void * Data;// Context used to move data

		assert (dstList), assert (srcList);	// Verify that dstList and srcList point to valid memory

#ifdef _DEBUG	// Error checking
		if (ListIsEmpty (srcList) || ListIsFull (dstList))	// Check whether source is empty or destination is full
		{
			MESSAGE_BOX("Source empty or destination full: ListMove failed","Error");
			return RETCODE_FAILURE;
			// Return failure
		}
#endif // _DEBUG
											
		Data = ListExtract (srcList, index);
		// Extract data from source

#ifdef _DEBUG	// Error checking
		if (Data == NULL)	// Check whether valid data was extracted
		{
			MESSAGE_BOX("Unable to move data: ListMove failed","Error");
			return RETCODE_FAILURE;
			// Return failure
		}
#endif // _DEBUG

		ListAppend (dstList, Data);
		ListDelete (srcList, index);
		// Append context to destination, and delete context from source

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								ListCopy										*
	*																				*
	********************************************************************************/

	// Purpose:    Copies data from one list into another list
	// Input:      Pointer to list to copy data to, pointer to list to copy data from, and index of data item to copy
	// Return:     A code indicating the results of the copy operation

	PUBLIC RETCODE ListCopy (LIST * dstList, LIST * srcList, int index)
	{
		void * Data;// Context used

		assert (dstList), assert (srcList);	// Verify that dstList and srcList point to valid memory

		Data = ListExtract (srcList, index);
		// Extract data from source

#ifdef _DEBUG	// Error checking
		if (Data == NULL)	// Check whether valid data was extracted
		{
			MESSAGE_BOX("Unable to copy data: ListCopy failed","Error");
			return RETCODE_FAILURE;
			// Return failure
		}
#endif // _DEBUG

		ListAppend (dstList, Data);
		// Append context to destination

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*
	*								ListSwap										*
	*																				*
	********************************************************************************/

	// Purpose:	   Swaps two data items in a list
	// Input:	   Pointer to list to swap within, index of first data item, and index of second data item
	// Return:	   A code indicating the results of the swap

	PUBLIC RETCODE ListSwap (LIST * list, int indexOne, int indexTwo)
	{
		int index;					// Loop variable
		PLISTNODE nodeOne, nodeTwo;	// Placeholder nodes for swapping data
		Pbyte Temp;					// Temporary data storage			

		assert (list);	// Verify that list points to valid memory

#ifdef _DEBUG	// Error checking
		if (ListIsEmpty (list))	// Check whether list is empty
		{
			MESSAGE_BOX("List empty: ListSwap failed","Error");
			return LISTCODE_LISTEMPTY;
			// Return list empty code
		}

		if (indexOne < 0 || indexOne >= list->NodeCount)	// Ascertain that indexOne is within list bounds
		{
			MESSAGE_BOX("Index one out of bounds: ListSwap failed","Error");
			return LISTCODE_OUTOFBOUNDS;
			// Return out of bounds code
		}

		if (indexTwo < 0 || indexTwo >= list->NodeCount)	// Ascertain that indexTwo is within list bounds
		{
			MESSAGE_BOX("Index two out of bounds: ListSwap failed","Error");
			return LISTCODE_OUTOFBOUNDS;
			// Return out of bounds code
		}
#endif // _DEBUG

		if (indexOne == indexTwo)	// Check whether no swap will occur
			return RETCODE_SUCCESS;
			// Return success

		GRAB_ROOT(nodeOne,list), GRAB_ROOT(nodeTwo,list);
		// Grab list's root node

		for (index = 0; index < indexOne; ++index)	// Walk through active nodes
			WALK_AHEAD(nodeOne);// Walk to next list node						

		for (index = 0; index < indexTwo; ++index)	// Walk through active nodes
			WALK_AHEAD(nodeTwo);// Walk to next list node

		Temp = nodeOne->Data;	// Grab node one's data

		nodeOne->Data = nodeTwo->Data;	// Set nodeOne's data to nodeTwo's data
		nodeTwo->Data = Temp;			// Set nodeTwo's data to Temp

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								ListDelete										*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Deletes a data item from a list at a given index
	// Input:      Pointer to list to delete from, and index of deletion
	// Return:     A code indicating the results of the deletion

	PUBLIC RETCODE ListDelete (LIST * list, int index)
	{
		PLISTNODE oldNode;	// Old node to draw out of list

		assert(list);	// Verify that list points to valid memory

#ifdef _DEBUG	// Error checking
		if (ListIsEmpty (list))	// Check whether list is already empty
		{
			MESSAGE_BOX("List empty: ListDelete failed","Error");
			return LISTCODE_LISTEMPTY;
			// Return empty list code
		}

		if (index < 0 || index >= list->NodeCount)	// Ascertain that index is within list bounds
		{
			MESSAGE_BOX("Out of bounds: ListDelete failed","Error");
			return LISTCODE_OUTOFBOUNDS;
			// Return out of bounds code
		}
#endif // _DEBUG

		if (index < list->Index)	// Check whether index of insertion is less than list index
		{
			while (list->Index > index)	// Walk through list
			{
				--list->Index;				// Decrement current index
				WALK_BACK(list->Current);	// Walk to previous node
			}
		}

		else						// Check for other cases
		{
			while (list->Index < index)	// Walk through list
			{
				++list->Index;				// Increment current index
				WALK_AHEAD(list->Current);	// Walk to subsequent node
			}
		}

		oldNode				= list->Current;// Grab primary active node

		if (oldNode == list->Head)		// Check whether node to remove is head of list
			list->Head = oldNode->Next; // Set list head to oldNode->Next

		oldNode->Prev->Next = oldNode->Next;// Bind oldNode->Prev to oldNode->Next
		oldNode->Next->Prev = oldNode->Prev;// Bind oldNode->Next to oldNode->Prev

		if (ListIsFull (list))	// Check whether free list is empty
		{							
			oldNode->Prev = oldNode;// Bind oldNode->Prev to oldNode	
			oldNode->Next = oldNode;// Bind oldNode->Next to oldNode

			list->Free	  = oldNode;// Set oldNode to list's free nodes
		}

		else					// Check for other cases
		{
			oldNode->Prev		= list->Free->Prev; // Bind oldNode to list's Free->Prev component
			oldNode->Next		= list->Free;		// Bind oldNode to list's free nodes
			
			oldNode->Prev->Next	= oldNode;			// Bind oldNode->Prev to oldNode
			oldNode->Next->Prev = oldNode;			// Bind oldNode->Next to oldNode
		}

		--list->NodeCount;	// Decrement node count

		if (list->NodeCount == 0)	// Check for empty lists
			list->Head = NULL;	// Nullify list's active nodes

		list->Index	  = 0;			// Reset list index
		list->Current = list->Head;	// Reset current list node
		
		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								ListGetPrev										*	!
	*																				*	!
	********************************************************************************/	
	
	// Purpose:    Extracts data from previous node in list
	// Input:      Pointer to list to extract from
	// Return:     Address of extracted data

	PUBLIC void * ListGetPrev (LIST * list)
	{
		assert(list);	// Verify that list points to valid memory

#ifdef _DEBUG	// Error checking
		if (ListIsEmpty (list))	// Check whether list is empty
		{
			MESSAGE_BOX("List empty: ListGetPrev failed","Error");
			return NULL;
			// Return null pointer
		}
#endif // _DEBUG

		if (list->Index == 0)	// Check whether current node is node 0
			list->Index = list->NodeCount;
			// Set index to value beyond edge of list

		--list->Index;				// Decrement current index
		WALK_BACK(list->Current);	// Walk to previous node
		
		return list->Current->Data;
		// Return data at current node
	}

	/********************************************************************************
	*																				*	!
	*								ListGetNext										*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Extracts data from subsequent node in list
	// Input:      Pointer to list to extract from
	// Return:     Address of extracted data

	PUBLIC void * ListGetNext (LIST * list)
	{
		assert(list);	// Verify that list points to valid memory

#ifdef _DEBUG	// Error checking
		if (ListIsEmpty (list))	// Check whether list is empty
		{
			MESSAGE_BOX("List empty: ListGetNext failed","Error");
			return NULL;
			// Return null pointer
		}
#endif // _DEBUG

		++list->Index;				// Increment current index

		if (list->Index == list->NodeCount)	// Check whether current node is beyond edge of list
			list->Index = 0;
			// Set index to 0

		WALK_AHEAD(list->Current);	// Walk to next node

		return list->Current->Data;
		// Return data at current node
	}

	/********************************************************************************
	*																				*	!
	*								ListExtract										*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Extracts data from a list at a given index
	// Input:      Pointer to list to extract from, and index of extraction
	// Return:     Address of extracted data

	PUBLIC void * ListExtract (LIST * list, int index)
	{
		assert(list);	// Verify that list points to valid memory

#ifdef _DEBUG	// Error checking
		if (ListIsEmpty (list))	// Check whether list is empty
		{
			MESSAGE_BOX("List empty: ListExtract failed","Error");
			return NULL;
			// Return null pointer
		}

		if (index < 0 || index >= list->NodeCount)	// Ascertain that index is within list bounds
		{
		assert(!"Out of bounds: ListExtract failed");//,"Error");
			return NULL;
			// Return null pointer
		}
#endif // _DEBUG

		if (index < list->Index)	// Check whether index of extraction is less than list index
		{
			while (list->Index > index)	// Walk through list
			{
				--list->Index;				// Decrement current index
				WALK_BACK(list->Current);	// Walk to previous node
			}
		}

		else						// Check for other cases
		{
			while (list->Index < index)	// Walk through list
			{
				++list->Index;				// Increment current index
				WALK_AHEAD(list->Current);	// Walk to subsequent node
			}
		}

		return list->Current->Data;
		// Return data at current node
	}

	/********************************************************************************
	*																				*	!
	*								ListEmptyList									*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Empties all data items from a list
	// Input:      Pointer to list to empty
	// Return:     A code indicating the results of the empty operation

	PUBLIC RETCODE ListEmptyList (LIST * list)
	{										
		PLISTNODE lFreeNode, lActiveNode;	// Left free node; left active node
		PLISTNODE rFreeNode, rActiveNode;	// Right free node; right active node

		assert(list);	// Verify that list points to valid memory

		if (ListIsEmpty (list))	// Check whether list is already empty
			return RETCODE_SUCCESS;
			// Return success

		if (ListIsFull (list))	// Check whether list is full
		{
			list->Free = list->Head;// Set list's active nodes to list's free nodes
			list->Head = NULL;		// Nullify list's active nodes
		}

		else
		{
			lFreeNode		  = list->Free;			// Grab the primary free node
			rFreeNode		  = lFreeNode->Next;	// Grab the next free node

			rActiveNode		  = list->Head;			// Grab the primary active node
			lActiveNode		  = rActiveNode->Prev;	// Grab the previous active node

			lFreeNode->Next	  = rActiveNode;		// Bind left free node ahead to right active node
			rFreeNode->Prev	  = lActiveNode;		// Bind right free node back to left active node

			lActiveNode->Next = rFreeNode;			// Bind left active node ahead to right free node
			rActiveNode->Prev = lFreeNode;			// Bind right active node back to left free node
		}

		list->Index		  = 0;		// Set current index to 0
		list->NodeCount	  = 0;		// Set node count to 0

		list->Current	  = NULL;	// Set current node to NULL
		list->Head		  = NULL;	// Set active nodes to NULL

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								ListSort										*	!
	*																				*	!
	********************************************************************************/

	// Purpose:    Sort the elements of a list in a user-defined manner
	// Input:      Pointer to list to sort, a callback function to regulate sort process, and a sort type
	// Return:     A code indicating the results of the sort

	PUBLIC RETCODE ListSort (LIST * list, COMPARE callback, long type)
	{
		int index, temp;		// Loop variable; a variable used to store a consistent amount
		Pbyte Temp;				// Temporary data storage
		PLISTNODE Current;		// Pointer to current list node
		PLISTNODE Prev = NULL;	// Pointer to previous and next list nodes
		BOOL breakOff  = TRUE;	// Breaking condition

		assert(list), assert(callback);	// Verify that list and callback point to valid memory

#ifdef _DEBUG	// Error checking
		if (ListIsEmpty (list))	// Check whether list is empty
		{
			MESSAGE_BOX("List empty: ListSort failed","Error");
			return RETCODE_FAILURE;
			// Return failure
		}
#endif // _DEBUG

		temp = list->NodeCount - 1;	// Set temp to list->NodeCount - 1

		switch (type)
		{
		case LISTSORT_ASCEND:	// Sort values in ascending order
			while (breakOff)	// Loop while sort is incomplete
			{
				breakOff = FALSE;	// Set condition to allow sort to break

				GRAB_ROOT(Current,list);// Grab the list's root node

				for (index = 0; index < temp; ++index)	// Walk through active nodes
				{
					Prev = Current;		// Grab current node
					WALK_AHEAD(Current);// Walk to next list node

					switch (callback (Prev->Data, Current->Data))	// Compare previous node to current node
					{
					case COMPARE_GT:	// Prev's value > Current's value
						breakOff = TRUE;	// Set condition for another pass through sort

						Temp		  = Prev->Data;	// Grab previous node's data

						Prev->Data    = Current->Data;	// Set Current->Data to Prev->Data
						Current->Data = Temp;			// Set Temp to Current->Data

						break;	// Break out of switch

					case COMPARE_LT:	// Fall through
					case COMPARE_EQ:	// No operation
						break;	// Break out of switch
					}
				}
			}

			break;	// Break out of switch

		case LISTSORT_DESCEND:	// Sort values in descending order
			while (breakOff)	// Loop while sort is incomplete
			{
				breakOff = FALSE;	// Set condition to allow sort to break

				GRAB_ROOT(Current,list);// Grab the list's root node

				for (index = 0; index < temp; ++index)	// Walk through active nodes
				{
					Prev = Current;		// Grab current node
					WALK_AHEAD(Current);// Walk to next list node

					switch (callback (Prev->Data, Current->Data))	// Compare previous node to current node
					{
					case COMPARE_LT:	// Prev's value < Current's value
						breakOff = TRUE;	// Set condition for another pass through sort

						Temp		  = Prev->Data;	// Grab previous node's data

						Prev->Data    = Current->Data;	// Set Current->Data to Prev->Data
						Current->Data = Temp;			// Set Temp to Current->Data

						break;	// Break out of switch

					case COMPARE_GT:	// Fall through
					case COMPARE_EQ:	// No operation
						break;	// Break out of switch
					}
				}
			}

			break;	// Break out of switch

//#ifdef _DEBUG	// Error checking
		default:	// Other cases
			MESSAGE_BOX("Unknown sort type: No result","Message");
			break;	// Break out of switch
		}
//#endif	// _DEBUG

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								ListSearch										*	!
	*																				*	!
	********************************************************************************/

	// Purpose:    Search for an item among the elements of a list in a user-defined manner
	// Input:      Pointer to list to search, a callback function to regulate search process, and item to search for
	// Return:     Index of first instance of item found, or SEARCH_NOTFOUND if item is not in list

	PUBLIC INDEX ListSearch (LIST * list, EQUIVALENCE callback, void * item)
	{
		int index;			// Loop variable
		PLISTNODE Walker;	// List walker

		assert(list), assert(callback), assert(item);	// Verify that list, callback, and item point to valid memory

		if (ListIsEmpty (list))	// Check whether list is empty
			return SEARCH_NOTFOUND;
			// Return SEARCH_NOTFOUND

		GRAB_ROOT(Walker,list);	// Grab the list's root node

		for (index = 0; index < list->NodeCount; ++index)	// Walk through active nodes
		{
			if (callback (Walker->Data, item) == EQUIVALENCE_TRUE)	// Check whether item is found
				break;	// Break out of loop

			WALK_AHEAD(Walker);	// Walk to next node
		}

		return index < list->NodeCount ? index : SEARCH_NOTFOUND;
		// Return index of item found, or SEARCH_NOTFOUND if no result is obtained
	}

	/********************************************************************************
	*																				*	!
	*								ListDestroy										*	!
	*																				*	!
	********************************************************************************/

	// Purpose:    Deinitializes a list, and releases all memory owned by said list to the memory pool
	// Input:      Address of pointer to list to destroy
	// Return:     A code indicating the results of the destruction

	PUBLIC RETCODE ListDestroy (LIST ** list)
	{
		assert(list), assert(*list);// Verify that list and *list point to valid memory
									
		MemReleaseBlock (*list, sizeof(LIST) + (*list)->NumNodes * (sizeof(LISTNODE) + (*list)->SizeofObject));
		// Release memory block owned by list

		*list = NULL;	// Nullify list

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								ListGrabBuffer									*	!
	*																				*	!
	********************************************************************************/

	// Purpose:	   Used to grab the memory from a list
	// Input:	   Pointer to list to grab memory from
	// Return:	   Memory grabbed from list

	PUBLIC void * ListGrabBuffer (LIST * list)
	{
		assert(list);	// Verify that list points to valid memory

		return (void *) (TYPE_CAST(Pbyte,&list [BASE_EXTENDED]) + list->NumNodes * sizeof(LISTNODE));
		// Return pointer to array of list data
	}

	/********************************************************************************
	*																				*	!
	*								ListIsEmpty										*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Function used internally by list to gauge list status
	// Input:      A this pointer to list to check status of
	// Return:     A boolean indicating list status

	PRIVATE BOOL ListIsEmpty (LIST * list)
	{
		return list->Head == NULL;
		// Return a boolean indicating status of list's active nodes
	}

	/********************************************************************************
	*																				*	!
	*								ListIsFull										*	!
	*																				*	!
	********************************************************************************/	
	
	// Purpose:    Function used internally by list to gauge list status
	// Input:      A this pointer to list to check status of
	// Return:     A boolean indicating list status

	PRIVATE BOOL ListIsFull (LIST * list)
	{
		return list->Free == NULL;
		// Return a boolean indicating status of list's free nodes
	}

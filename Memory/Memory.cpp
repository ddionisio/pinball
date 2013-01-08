	/********************************************************************
	*																	*
	*							Includes								*
	*																	*
	********************************************************************/

	#include "i_memory.h"
	#include "..\List\list.h"

	/********************************************************************
	*																	*
	*							Internals								*
	*																	*
	********************************************************************/

	PRIVATE PMEMHEAD MemHead;	// Memory head; linkage restricted to memory library

	/********************************************************************************
	*																				*	!
	*								MemAlloc										*	!
	*																				*	!
	********************************************************************************/	
	
	// Purpose:    Used to allocate memory of a given size to a particular datum
	// Input:      Pointer to context to attach memory to, and block size
	// Return:     A memory datum indicating private memory information

	PUBLIC RETCODE MemAlloc (void ** memory, Dword numBytes)
	{
		assert(memory);	// Verify that memory points to valid memory

		MALLOC(*memory,Byte,numBytes);
		// Allocate requested amount of memory

		if (*memory == NULL)	// Ascertain that malloc succeeded
		{
			MESSAGE_BOX("Unable to allocate memory: MemAlloc failed","Error");
			return MEMCODE_OUTOFMEMORY;
			// Return out of memory code
		}

		++MemHead->NumAllocs;	// Increment count of active allocations

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemFree											*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Used to release memory of a particular datum
	// Input:      Pointer to context to release from
	// Return:     A code indicating the results of the release

	PUBLIC RETCODE MemFree (void ** memory)
	{
		assert(memory), assert(*memory);	// Verify that memory points to valid memory

		--MemHead->NumAllocs;	// Decrement count of active allocations

		FREE(*memory);
		// Deallocate memory

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemInit											*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Initializes a memory manager
	// Input:      No input
	// Return:     A code indicating the results of the initialization 

	PROTECTED RETCODE MemInit (void)
	{
		MALLOC(MemHead,MEMHEAD,sizeof(MEMHEAD) + MEMPOOL_SIZE);//GetMemPoolSize ());
		// Allocate memory for memory

		if (!MemHead)	// Ascertain that malloc succeeded
		{
			MESSAGE_BOX("Unable to allocate memory: MemInit failed","Error");
			return MEMCODE_OUTOFMEMORY;
			// Return out of memory code
		}

		MemHead->MemBlocks = (PMEMBLOCK) MemHead->Memory;	// Point MemBlocks field at Memory field
		MemHead->NumAllocs = 0;								// Set number of allocations to 0
	 
		MemHead->FreeBytes = MEMPOOL_SIZE;//GetMemPoolSize ();	// Set free bytes to pool size amount

		MemHead->MemBlocks->Next = NULL;				// Nullify Next field of MemBlocks
		MemHead->MemBlocks->Size = MEMPOOL_SIZE; //GetMemPoolSize ();	// Set Size field of MemBlocks to pool size amount

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemTerm											*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Deinitializes a memory manager
	// Input:      No input
	// Return:     A code indicating the results of the termination

	PROTECTED RETCODE MemTerm (void)
	{
		if (MemHead->NumAllocs != 0)	// Ascertain that allocations have been freed
		{
			char buf [MAXCHARBUFF];

			sprintf (buf, "Memory leak, %d unfreed allocations: MemTerm failed", MemHead->NumAllocs);
			assert(0);

			MESSAGE_BOX(buf,"Error");
			
			return MEMCODE_MEMORYLEAK;
			// Return memory leak code
		}

		FREE(MemHead);
		// Deallocate memory

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemGrabBlock									*	!
	*																				*	!
	********************************************************************************/

	// Purpose:	   Used to grab a memory block for use by outside data
	// Input:	   Byte size of block to grab
	// Return:	   A pointer to a block of memory

	PROTECTED void * MemGrabBlock (Dword numBytes)
	{
		PMEMBLOCK MemBlock  = MemHead->MemBlocks;	// Current memory block
		PMEMBLOCK PrevBlock	= NULL;					// Previous memory block
		PMEMBLOCK DataBlock = NULL;					// Data block
		Dword Size;									// Size container

		while (MemBlock)	// Check that MemBlock is not yet null
		{
			PrevBlock = MemBlock;		// Set current block as previous block

			if (MemBlock->Size >= numBytes)	// Check whether block is of at least requested capacity
				break;	// Break out of loop

			MemBlock  = MemBlock->Next;	// Grab next block in sequence
		}

		MemAllotBytes (numBytes);
		// Allot memory for the memory block

		DataBlock = MemBlock;					// Set MemBlock to DataBlock
		Size	  = MemBlock->Size - numBytes;	// Figure size difference between block size and requested bytes

		if (MemBlock == MemHead->MemBlocks)	// Check whether memory is being requested from main block
		{																			
			MemHead->MemBlocks = (PMEMBLOCK) (TYPE_CAST(Pbyte,MemBlock) + numBytes);// Point main block beyond requested block	
			MemBlock		   = MemHead->MemBlocks;								// Grab main memory block
		}

		else
		{
			MemBlock		= (PMEMBLOCK) (TYPE_CAST(Pbyte,DataBlock) + numBytes);	// Point MemBlock to new location
			PrevBlock->Next = MemBlock;												// Point previous block ahead beyond data block
		}

		if (Size != 0)	// Check whether difference is nonzero
		{
			MemBlock->Size	= Size;				// Set new memory block size
			MemBlock->Next	= DataBlock->Next;	// Point memory block ahead to next block
		}

		ZeroMemory(DataBlock,numBytes);
		// Set all bytes in data block to 0

		return (void *) DataBlock;
		// Return pointer to data block
	}

	/********************************************************************************
	*																				*	!
	*								MemReleaseBlock									*	!
	*																				*	!
	********************************************************************************/

	// Purpose:    Used to release a memory block used by outside data
	// Input:	   A data block, and byte size of block to release
	// Return:	   A code indicating the results of the block release operation

	PROTECTED RETCODE MemReleaseBlock (void * data, Dword numBytes)
	{
		PMEMBLOCK MemBlock  = MemHead->MemBlocks;	// Current memory block
		PMEMBLOCK DataBlock = (PMEMBLOCK) data;		// Block of data to release
		PMEMBLOCK PrevBlock = NULL;					// Previous block

		DataBlock->Size = numBytes;	// Set size of block to release into memory

		if (MemHead->MemBlocks < DataBlock)	// Check whether block is beyond main memory block
		{
			while (MemBlock < DataBlock)	// Search through memory blocks via pointer arithmetic
			{
				PrevBlock = MemBlock;		// Set previous block
				MemBlock  = MemBlock->Next;	// Set current block
			}
		}

		else								// Check for other cases
		{
			MemBlock		   = MemHead->MemBlocks;// Grab main memory block
			MemHead->MemBlocks = DataBlock;			// Point main memory block to data block
		}

		MemCollectBytes (numBytes);
		// Collect memory from the memory block

		if ((PMEMBLOCK) (TYPE_CAST(Pbyte,DataBlock) + DataBlock->Size) == MemBlock)	// Check whether data block may join with upper memory block
		{																																		
			DataBlock->Size += MemBlock->Size;	// Increment data block size by size of upper block
			DataBlock->Next  = MemBlock->Next;	// Point Next field of data block beyond upper block
		}

		else
		{
			DataBlock->Next = MemBlock;	// Bind data block ahad to memory block
		}

		if (PrevBlock)	// Check whether a previous block exists
		{
			if ((PMEMBLOCK) (TYPE_CAST(Pbyte,PrevBlock) + PrevBlock->Size) == DataBlock) // Check whether lower memory block may join with data block
			{
				PrevBlock->Size += DataBlock->Size;	// Increment lower block size by size of data block
				PrevBlock->Next  = DataBlock->Next;	// Point Next field of lower block beyond data block
			}

			else
				PrevBlock->Next = DataBlock; // Bind previous block to data block
		}

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemAllotBytes									*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Used to allot bytes of memory from the memory pool
	// Input:      A number of bytes to allot
	// Return:     A code indicating the results of the byte allotment

	PRIVATE RETCODE MemAllotBytes (Dword numBytes)
	{
#ifdef _DEBUG	// Error checking
		if (MemHead->FreeBytes < numBytes)	// Check whether allotment may be processed
		{
			MESSAGE_BOX("Out of memory: MemAllotBytes failed","Error");
			return MEMCODE_OUTOFMEMORY;
			// Return out of memory code
		}
#endif	// _DEBUG

		MemHead->FreeBytes -= numBytes;	// Decrement count of free bytes

		return RETCODE_SUCCESS;
		// Return success
	}

	/********************************************************************************
	*																				*	!
	*								MemCollectBytes									*	!
	*																				*	!
	********************************************************************************/	

	// Purpose:    Used to collect alloted bytes of memory into the memory pool
	// Input:      A number of bytes to collect
	// Return:     A code indicating the results of the byte collection

	PRIVATE RETCODE MemCollectBytes (Dword numBytes)
	{
#ifdef _DEBUG	// Error checking
		if (MemHead->FreeBytes + numBytes > MEMPOOL_SIZE)//GetMemPoolSize ())	// Check whether collection may be processed
		{
			MESSAGE_BOX("Overflow: MemCollectBytes failed","Error");
			return MEMCODE_OVERFLOW;
			// Return overflow code
		}
#endif	// _DEBUG

		MemHead->FreeBytes += numBytes;	// Increment count of free bytes

		return RETCODE_SUCCESS;
		// Return success
	}

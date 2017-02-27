Our malloc implementation works by using a metadata struct that contains the size of the block of memory following the metadata, a pointer to the next metadata in the sequence, and a free indicator field that denotes the status of the memory block.  
the process in sudocode is as follows: 
process for malloc:
	if block is first to be malloced:
		mem requested > mem available ? err : allocate mem and return pointer to first block after metadata:
	else:
		get first meta data, check meta1 size against meta2 address and find how much empty space there is.
			if the second block and its metadata fits in the empty space, place it there, update next pointers of previous meta data
			else proceede to next meta data.
			if metadata.next = null compare size of that meta data with address of last block in array.  
			if there is room for requested space, return pointer to block, else err
The process for free is very simple, you simply take the pointer to the metadata as a paramater and set the free paramater so that it indicated the following memory block is now free, then when mallocing, if malloc finds two adjecent metadatas, it simply updates the first one to point to the next metadata in the sequence.  This means that it now only requires a single pass through the array.
This implementation is efficient in the way it passes throught the array however is not very space efficient if small blocks of memory are being allocated.      
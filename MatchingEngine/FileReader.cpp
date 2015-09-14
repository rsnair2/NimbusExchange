/**
 *	FileReader
 *	
 *	Reads blocks of data from the filesystem resulting in more performant reads. 
 *
 *	See Readme for more information. 
 *
 *	author: Rajiv Nair (rsnair.com)
 *	date: September, 2015
 *	license: see License for more information
 **/
#include "FileReader.h"

void FileReader::run() {
	// single threaded (td has not yet been spawned)
	// all shared variables can be accessed without a lock

	// proactively reserve space for caching blocks in memory
	for(int i = 0; i < configMaxBlocksInQueue; i++) {
		FileBlock * block = new FileBlock;
		block->buffer = new char[configBlockSize];
		block->numberOfBytes = 0;
		freeMemoryBlocksList.push(block);
	}

	state = FileReader::Reading;
	td = thread(&FileReader::runBlockReader, this);
}

FileReader::FileBlock * FileReader::readBlock() {
	while(true) {
		fileBlocksQueueMutex.lock();

		// nothing to read yet
		if(fileBlocksQueue.size() == 0) {
			stateMutex.lock();

			// all blocks have been read and pushed onto the queue
			// since the queue is now empty, we are done. the file has been read
			if(state == FileReader::Done) {
				stateMutex.unlock();
				fileBlocksQueueMutex.unlock();
				return NULL;
			}

			// we are waiting on td to put some blocks into the queue
			// release locks and try again
			stateMutex.unlock();
			fileBlocksQueueMutex.unlock();
		}
		else
			// done, we got something to read, read it!
			break;
	}

	FileBlock * block = fileBlocksQueue.front();
	fileBlocksQueue.pop();
	fileBlocksQueueMutex.unlock();

	return block;
}

void FileReader::releaseBlock(FileReader::FileBlock * block) {
	freeMemoryBlocksListMutex.lock();
	freeMemoryBlocksList.push(block);
	freeMemoryBlocksListMutex.unlock();
}

FileReader::FileReader(string filename) {
	// all shared variables can be accessed without a lock
	// as td hasn't been spawned yet
	configBlockSize = defaultBlockSize;
	configMaxBlocksInQueue = defaultMaxBlocksInQueue;

	state = FileReader::Idle;

	filename = filename;
	infile.open(filename);

	if(!infile.is_open()) {
		// error condition here
	}
}	

FileReader::~FileReader() {
	if(td.joinable())
		td.join();
	infile.close();

	// td is no longer active after td.join
	while(freeMemoryBlocksList.size() > 0) {
		FileBlock * block = freeMemoryBlocksList.front();
		char * buffer = block->buffer;
		free(buffer);
		free(block);
		freeMemoryBlocksList.pop();
	}
}

void FileReader::runBlockReader() {
	while(infile) {
		// attempt to get a free memory block
		while(true) {
			freeMemoryBlocksListMutex.lock();
			if(freeMemoryBlocksList.size() == 0)
				freeMemoryBlocksListMutex.unlock();
			else 
				break;
		}

		// take the free block and remove it from our free list
		FileBlock * block = freeMemoryBlocksList.front();
		freeMemoryBlocksList.pop();
		freeMemoryBlocksListMutex.unlock();

		// read input
		infile.read(block->buffer, configBlockSize);
		block->numberOfBytes = infile.gcount();

		// buffer input
		fileBlocksQueueMutex.lock();
		fileBlocksQueue.push(block);
		fileBlocksQueueMutex.unlock();
	}

	// this only happens when everything is on the queue!
	stateMutex.lock();
	state = FileReader::Done;
	stateMutex.unlock();
}
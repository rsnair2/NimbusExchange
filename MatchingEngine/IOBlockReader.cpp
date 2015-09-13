/**
 *	IOBlockReader
 *	
 *	Reads blocks of data from the filesystem resulting in more performant reads. 
 *
 *	See Readme for more information. 
 *
 *	author: Rajiv Nair (rsnair.com)
 *	date: September, 2015
 *	license: see License for more information
 **/
#include "IOBlockReader.h"


/* Public interface functions */

/**
 *	description: begins buffering a file on a seperate thread. a maximum buffer size 
 * 	of configBlockSize * configMaxBlocksInQueue is maintained at any given time. if 
 *	the internal buffer is full, the file reading is stopped till blocks are released
 * 	back into the pool. 
 **/
void IOBlockReader::run() {
	stateMutex.lock();
	state = IOBlockReader::Reading;
	stateMutex.unlock();

	td = thread(&IOBlockReader::_run, this);
}

/**
 *	description: returns a block, where a block contains a pointer to the buffered input
 * 	data and the number of bytes that are useful. the data is returned in sequential access.
 *	important: every block must be released back to the reader. 
 *	also, NULL is returned when all the buffered data has been read. 
 **/
IOBlockReader::Block * IOBlockReader::readBlock() {
	while(true) {
		bufferedInputQueueMutex.lock();

		// done reading and buffer has been emptied
		if(bufferedInputQueue.size() == 0) {
			stateMutex.lock();
			if(state == IOBlockReader::Done) {
				stateMutex.unlock();
				bufferedInputQueueMutex.unlock();
				return NULL;
			}
			stateMutex.unlock();
			bufferedInputQueueMutex.unlock();
		}
		else if(bufferedInputQueue.size() == 0) {
			bufferedInputQueueMutex.unlock();
		}
		else
			break;
	}

	Block * block = bufferedInputQueue.front();
	bufferedInputQueue.pop();
	bufferedInputQueueMutex.unlock();

	return block;
}

/**
 *	description: returns a block back to the pool. the reader has a limited number of blocks and
 *	will not be able to cache the file if all its blocks are being used by the user. 
 **/
void IOBlockReader::releaseBlock(IOBlockReader::Block * block) {
	freeBlocksListMutex.lock();
	freeBlocksList.push(block);
	freeBlocksListMutex.unlock();
}

/* Private functions */

IOBlockReader::IOBlockReader(string filename) {
	configBlockSize = defaultBlockSize;
	configMaxBlocksInQueue = defaultMaxBlocksInQueue;

	stateMutex.lock();
	state = IOBlockReader::Idle;
	stateMutex.unlock();

	filename = filename;
	infile.open(filename);

	if(!infile.is_open()) {
		// error condition here
	}

	for(int i = 0; i < configMaxBlocksInQueue; i++) {
		Block * block = new Block;
		block->buffer = new char[configBlockSize];
		block->numberOfBytes = 0;

		// no need to lock here as the i/o operation has not yet begun
		freeBlocksList.push(block);
	}
}	

IOBlockReader::~IOBlockReader() {
	td.join();
	infile.close();

	while(freeBlocksList.size() > 0) {
		Block * block = freeBlocksList.front();
		char * buffer = block->buffer;
		free(buffer);
		free(block);
		freeBlocksList.pop();
	}
}

void IOBlockReader::_run() {
	while(infile) {
		while(true) {
			freeBlocksListMutex.lock();
			if(freeBlocksList.size() == 0)
				freeBlocksListMutex.unlock();
			else 
				break;
		}

		// get a free block and remove it from our free list
		Block * block = freeBlocksList.front();
		freeBlocksList.pop();
		freeBlocksListMutex.unlock();

		// read input
		infile.read(block->buffer, configBlockSize);
		block->numberOfBytes = infile.gcount();

		// buffer input
		bufferedInputQueueMutex.lock();
		bufferedInputQueue.push(block);
		bufferedInputQueueMutex.unlock();
	}

	// this only happens when everything is on the queue!
	stateMutex.lock();
	state = IOBlockReader::Done;
	stateMutex.unlock();
}
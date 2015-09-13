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
#ifndef IO_BLOCK_READER__
#define IO_BLOCK_READER__

#include <thread>
#include <iostream>
#include <fstream>
#include <string>
#include <queue>

using namespace std; // yes, I am being lazy for now!

class IOBlockReader {
public:
	// configurations
	unsigned int configBlockSize;
	unsigned int configMaxBlocksInQueue;

	string filename;

	// constructor
	IOBlockReader(string filename);

	// destructor
	~IOBlockReader();

    struct Block {
		char * buffer;
		unsigned int numberOfBytes;
	};

	// main public interfaces
	void run();
 	Block * readBlock();
	void releaseBlock(Block *);

	enum State {Reading, Done, Idle};
	State state;

private:
	void _run();

	ifstream infile;
	thread td;

	queue<Block *> bufferedInputQueue;
	mutex bufferedInputQueueMutex;

	queue<Block *> freeBlocksList;
	mutex freeBlocksListMutex;

	// default configurations
	static const unsigned int defaultBlockSize = 4096;
	static const unsigned int defaultMaxBlocksInQueue = 10;
};

#endif

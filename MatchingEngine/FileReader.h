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
#ifndef __FILE_READER__
#define __FILE_READER__

#include <thread>
#include <fstream>
#include <string>
#include <queue>

using namespace std;

class FileReader {
public:
	// user specific configurations
	unsigned int configBlockSize;	// the size of file block for reads
	unsigned int configMaxBlocksInQueue;	// maximum number of file blocks to buffer

	string filename;

	// constructor
	FileReader(string filename);

	// destructor
	~FileReader();

    struct FileBlock {
		char * buffer;
		unsigned int numberOfBytes;
	};

	// main public interfaces
	void run();
 	FileBlock * readBlock();
	void releaseBlock(FileBlock *);

private:
	void runBlockReader();

	ifstream infile;

	thread td;

	// file blocks are buffered in the fileBlocksQueue
	queue<FileBlock *> fileBlocksQueue;
	mutex fileBlocksQueueMutex;

	// we proactively reserve some memory for buffering file 
	// blocks. this is done to avoid costs associated with malloc
	queue<FileBlock *> freeMemoryBlocksList;
	mutex freeMemoryBlocksListMutex;

	// default configurations
	// default cache size for buffer = 4000 kB
	static const unsigned int defaultBlockSize = 4096;
	static const unsigned int defaultMaxBlocksInQueue = 1000;

	// state variables
	enum State {Reading, Done, Idle};
	State state;
	mutex stateMutex;
};

#endif

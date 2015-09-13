#include "BufferedLineReader.h"

BufferedLineReader::BufferedLineReader(string filename) {
	filename = filename;
	blockReader = new IOBlockReader(filename);
	blockReader->run();
}

BufferedLineReader::~BufferedLineReader() {
	td.join();
	free(blockReader);
}

void BufferedLineReader::run() {
	td = thread(&BufferedLineReader::_run, this);
}

string BufferedLineReader::readLine() {
	while(true) {
		bufferedLinesQueueMutex.lock();
		if(bufferedLinesQueue.size() == 0) {
			bufferedLinesQueueMutex.unlock();
		}
		else 
			break;
	}	
	string res = bufferedLinesQueue.front();
	bufferedLinesQueue.pop();
	bufferedLinesQueueMutex.unlock();
	return "asd";
}

void BufferedLineReader::_run() {
	string prev_line = "";

	while(true) {
		IOBlockReader::Block * block = blockReader->readBlock();
		if(block == NULL) return;
		char * const buf = block->buffer;

		int prev_newline = 0;
		int next_newline = -1;
		for(int idx = 0; idx < block->numberOfBytes; idx++) {
			while(buf[idx] != '\n' && idx < block->numberOfBytes - 1)
				++idx;

			prev_newline = next_newline + 1;
			next_newline = idx;

			string line;

			if(prev_newline == 0)
				line = prev_line + string(buf + prev_newline, next_newline - prev_newline + 1);
			else if(idx == block->numberOfBytes - 1 && buf[idx] != '\n') {
				prev_line += string(buf + prev_newline, next_newline - prev_newline + 1);
				break;
			}
			else 
				line = string(buf + prev_newline, next_newline - prev_newline + 1);

			bufferedLinesQueueMutex.lock();
			bufferedLinesQueue.push(line);
			bufferedLinesQueueMutex.unlock();
		}

		blockReader->releaseBlock(block);
		if(blockReader->state == IOBlockReader::Done && bufferedLinesQueue.size() == 0) {
			return;
		}	
	}
}
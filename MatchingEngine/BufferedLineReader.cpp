#include "BufferedLineReader.h"
#include <string.h>

BufferedLineReader::BufferedLineReader(string filename) {
	filename = filename;
	blockReader = new IOBlockReader(filename);
	stateMutex.lock();
	state = BufferedLineReader::Idle;
	stateMutex.unlock();
	linesBuffered = 0;
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
			stateMutex.lock();
			if(state == BufferedLineReader::Done) {
				stateMutex.unlock();
				bufferedLinesQueueMutex.unlock();
				return "";
			}
			stateMutex.unlock();
			bufferedLinesQueueMutex.unlock();
		}
		else 
			break;
	}	
	string res = bufferedLinesQueue.front();
	bufferedLinesQueue.pop();
	--linesBuffered;
	bufferedLinesQueueMutex.unlock();
	return res;
}

const char * find_substr(const char * start, const char * end, char delim) {
	const char * curr = start;
	while(curr <= end) {
		if(start[0] == delim) return start;
		++curr;
	}
	return NULL;
}

void BufferedLineReader::_run() {
	string prev_line = "";

	stateMutex.lock();
	state = BufferedLineReader::Reading;
	stateMutex.unlock();

	while(true) {
		IOBlockReader::Block * block = blockReader->readBlock();

		if(block == NULL) {	
			if(prev_line != "") {
				bufferedLinesQueueMutex.lock();
				bufferedLinesQueue.push(prev_line);
				++linesBuffered;
				bufferedLinesQueueMutex.unlock();
			};

			stateMutex.lock();
			state = BufferedLineReader::Done;
			stateMutex.unlock();
			return;
		}
		
		const char * buf = block->buffer;
		const char * end = buf + block->numberOfBytes;
		string prev = "";

		while(buf != end) {
			const char * next = find_substr(buf, end, '\n');

			if(next == NULL) {
				prev_line += string(buf, (int)(end - buf));
				break;
			}
			string line = string(buf, (int)(next - buf));
			bufferedLinesQueueMutex.lock();
			bufferedLinesQueue.push(line);
			++linesBuffered;
			bufferedLinesQueueMutex.unlock();
			buf = next;
		}

		blockReader->releaseBlock(block);
	}
}
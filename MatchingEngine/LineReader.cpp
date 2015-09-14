#include "LineReader.h"
#include <string.h>

LineReader::LineReader(string filename) {
	filename = filename;
	blockReader = new FileReader(filename);
	state = LineReader::Idle;
	blockReader->run();
}

LineReader::~LineReader() {
	td.join();
	free(blockReader);
}

void LineReader::run() {
	td = thread(&LineReader::runLineReader, this);
}

string LineReader::readLine() {
	while(true) {
		linesQueueMutex.lock();
		if(linesQueue.size() == 0) {
			stateMutex.lock();
			if(state == LineReader::Done) {
				stateMutex.unlock();
				linesQueueMutex.unlock();
				return "";
			}
			stateMutex.unlock();
			linesQueueMutex.unlock();
		}
		else 
			break;
	}	
	string res = linesQueue.front();
	linesQueue.pop();
	linesQueueMutex.unlock();
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

void LineReader::runLineReader() {
	string prev_line = "";

	stateMutex.lock();
	state = LineReader::Reading;
	stateMutex.unlock();

	while(true) {
		FileReader::FileBlock * block = blockReader->readBlock();

		if(block == NULL) {
			if(prev_line != "") {
				linesQueueMutex.lock();
				linesQueue.push(prev_line);
				linesQueueMutex.unlock();
			};

			stateMutex.lock();
			state = LineReader::Done;
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
			linesQueueMutex.lock();
			linesQueue.push(line);
			linesQueueMutex.unlock();
			buf = next;
		}

		blockReader->releaseBlock(block);
	}
}
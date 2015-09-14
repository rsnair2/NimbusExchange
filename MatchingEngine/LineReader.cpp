#include "LineReader.h"

template <typename T>
LineReader<T>::LineReader(string filename) {
	filename = filename;
	delimiter = defaultDelimiter;
	blockReader = new FileReader(filename);
	state = LineReader::Idle;
	blockReader->run();
}

template <typename T>
LineReader<T>::~LineReader() {
	if(td.joinable())
		td.join();
	free(blockReader);
}

template <typename T>
void LineReader<T>::run() {
	state = LineReader::Reading;
	td = thread(&LineReader::runLineReader, this);
}

template <typename T>
T * LineReader<T>::readLine() {
	while(true) {
		linesQueueMutex.lock();
		if(linesQueue.size() <= 0) {
			stateMutex.lock();
			if(state == LineReader::Done) {
				stateMutex.unlock();
				linesQueueMutex.unlock();
				return NULL;
			}
			stateMutex.unlock();
			linesQueueMutex.unlock();
		}
		else 
			break;
	}	

	T * res = linesQueue.front();
	linesQueue.pop();
	linesQueueMutex.unlock();
	return res;
}

template <typename T>
const char * LineReader<T>::find_substr(const char * start, const char * end, char delim) {
	const char * curr = start;
	while(curr <= end) {
		if(curr[0] == delim) return curr;
		++curr;
	}
	return NULL;
}

template <typename T>
void LineReader<T>::runLineReader() {
	string prev_line = "";

	stateMutex.lock();
	state = LineReader::Reading;
	stateMutex.unlock();

	while(true) {
		FileReader::FileBlock * block = blockReader->readBlock();

		if(block == NULL) {	
			if(prev_line != "") {
				T * newObj = new T(prev_line);
				prev_line = "";
				linesQueueMutex.lock();
				linesQueue.push(newObj);
				linesQueueMutex.unlock();
			};

			stateMutex.lock();
			state = LineReader::Done;
			stateMutex.unlock();

			return;
		}

		const char * buf = block->buffer;
		const char * end = buf + block->numberOfBytes - 1;

		while(buf <= end) {
			const char * next = find_substr(buf, end, delimiter);

			if(next == NULL) {
				prev_line += string(buf, (int)(end - buf + 1));
				break;
			}

			string line = prev_line + string(buf, (int)(next - buf + 1));
			prev_line = "";
			T * newObj = new T(line);

			linesQueueMutex.lock();
			linesQueue.push(newObj);
			linesQueueMutex.unlock();
			buf = next + 1;
		}

		blockReader->releaseBlock(block);
	}
}
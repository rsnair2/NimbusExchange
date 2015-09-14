#ifndef __LINE_READER__
#define __LINE_READER__

#include "FileReader.h"
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>

using namespace std;

template <typename T>
class LineReader {
public:
	LineReader(string filename);
	~LineReader();

	void run();
	T * readLine();

	string filename;
	char delimiter;

	void setDelimiter(char delim) {
		delimiter = delim;
	}

private:
	queue<T *> linesQueue;
	mutex linesQueueMutex;

	ifstream infile;

	enum State {Reading, Done, Idle};
	State state;
	mutex stateMutex;

	FileReader * blockReader;
	thread td;

	void runLineReader();
	const char * find_substr(const char * start, const char * end, char delim);

	const char defaultDelimiter = '\n';
};

#endif

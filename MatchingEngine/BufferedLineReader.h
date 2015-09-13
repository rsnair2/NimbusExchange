#include "IOBlockReader.h"
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>

using namespace std;

class BufferedLineReader {
public:
	BufferedLineReader(string filename);
	~BufferedLineReader();

	void run();
	string readLine();

	string filename;

	enum State {Reading, Done, Idle};
	State state;

private:
	queue<string> bufferedLinesQueue;
	mutex bufferedLinesQueueMutex;

	int linesBuffered;

	ifstream infile;

	mutex stateMutex;

	IOBlockReader * blockReader;
	thread td;

	void _run();
};
#include "FileReader.h"
#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <fstream>
#include <vector>

using namespace std;

class LineReader {
public:
	LineReader(string filename);
	~LineReader();

	void run();
	string readLine();

	string filename;

	enum State {Reading, Done, Idle};
	State state;

private:
	queue<string> linesQueue;
	mutex linesQueueMutex;

	ifstream infile;

	mutex stateMutex;

	FileReader * blockReader;

	thread td;

	void runLineReader();
};
#include <iostream>
#include <string>
#include <queue>
#include <fstream>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include <iomanip>
#include <queue>         
#include <vector>
#include <algorithm>   
#include <map> 
#include "MarketInstruction.h"
#include "MarketInstructionIterator.h"
#include "OrderBook.h"
#include <iomanip>      // std::setprecision
#include "OrderFiller.h"
#include <mutex>

using namespace std;

#define MAX_NUMBER_OF_THREADS 1

// outputfile
string outputfile_name = "/Users/rsnair2/Studio/MatchingEngine/data/output.txt";
ofstream outfile;
ifstream correct_out;
ifstream output_test;

map<string, OrderBook> assetNameToOrderBookMap;
map<string, mutex> assetNameToOrderBookMutex;

map< string, queue<MarketInstruction *> > orderQueue;
mutex orderQueueMutex;

// used for round robin scheduling
vector<string> assetsInQueue;
mutex assetsInQueueMutex;

enum State {Reading, Done};
State state;
mutex stateMutex;

vector<thread> threadsActive;

mutex printMutex;

int logFill(string assetName, unsigned long long buyer, unsigned long long seller, 
	unsigned int quantity, double price) {

	outfile << setprecision(10);
	outfile << "FILL " << assetName << " " << buyer << " " << seller << " ";
	outfile << quantity << " " << price << endl;

	ostringstream s;
	s << setprecision(10);
	s << price;
	string output_line = "FILL " + assetName + " " + to_string(buyer) + " " + to_string(seller) + " " + to_string(quantity) + " " + s.str() + "\r";
	string correct_line;
	getline(correct_out, correct_line);

	printMutex.lock();
	cout << output_line << endl;
	printMutex.unlock();

	if(correct_line.substr(0, correct_line.size()) != output_line) {
		printMutex.lock();
		cout << output_line << " " << correct_line << endl;
		printMutex.unlock();
		exit(EXIT_FAILURE);
	}
	
	return 0;
}

OrderFiller of(logFill);

void handleFill(string assetName, unsigned long long buyer, unsigned long long seller, 
		unsigned int quantity, double price, unsigned int latestTimeStamp, unsigned int otherTimeStamp) {
	of.fillOrder(assetName, buyer, seller, quantity, price, latestTimeStamp, otherTimeStamp);
}

bool checkIfDone() {
	stateMutex.lock();
	assetsInQueueMutex.lock();
	if(state == Done && assetsInQueue.size() == 0) {
		assetsInQueueMutex.unlock();
		stateMutex.unlock();
		return true;
	}
	assetsInQueueMutex.unlock();
	stateMutex.unlock();
	return false;
}

void handle_market_instruction(int thread_index) {
	while(true) {
		if(checkIfDone()) {return;}

		assetsInQueueMutex.lock();
		if(thread_index >= assetsInQueue.size()) {
			assetsInQueueMutex.unlock();
			continue;
		}
		string assetName = assetsInQueue[thread_index];
		assetsInQueueMutex.unlock();

		assetNameToOrderBookMutex[assetName].lock();
		assetsInQueueMutex.lock();
		orderQueueMutex.lock();
		MarketInstruction * mi = orderQueue[assetName].front();
		orderQueue[assetName].pop();

		if(orderQueue[assetName].size() == 0) {
			vector<string>::iterator it;
			it = find(assetsInQueue.begin(), assetsInQueue.end(), assetName);
			if(it != assetsInQueue.end())
				assetsInQueue.erase(it);
		}

		orderQueueMutex.unlock();
		assetsInQueueMutex.unlock();

		if(assetNameToOrderBookMap.find(assetName) == assetNameToOrderBookMap.end())
			assetNameToOrderBookMap[assetName] = OrderBook(handleFill);
		assetNameToOrderBookMap[assetName].add_order(mi);
		of.completedOrderWithTimestamp(mi->timestamp);
		assetNameToOrderBookMutex[assetName].unlock();
	}
}

void addInstructionToQueue(MarketInstruction * mi) {
	vector<string>::iterator it;
	assetsInQueueMutex.lock();
	it = find(assetsInQueue.begin(), assetsInQueue.end(), mi->targetAssetName);

	if(it == assetsInQueue.end()) {
		assetsInQueue.push_back(mi->targetAssetName);
	}

	orderQueueMutex.lock();
	orderQueue[mi->targetAssetName].push(mi);
	orderQueueMutex.unlock();
	assetsInQueueMutex.unlock();
}

/**
 *	fetches instructions and queues them
 **/
void fetchInstructions(const char * filename) {
	MarketInstructionIterator mii(filename);
	MarketInstruction * mi;

	while((mi = mii.getNextInstruction())) {
		addInstructionToQueue(mi);
	}

	stateMutex.lock();
	state = Done;
	stateMutex.unlock();
}

int main(int argc, char * argv[]) {
	outfile.open(outputfile_name);
	correct_out.open("/Users/rsnair2/Studio/MatchingEngine/data/fills.txt");

	state = Reading;

	of.run();
	thread td = thread(fetchInstructions, argv[1]);

	for(int i = 0; i < MAX_NUMBER_OF_THREADS; i++) {
		threadsActive.push_back(thread(handle_market_instruction, i));
	}

	td.join();
	for(int i = 0; i < MAX_NUMBER_OF_THREADS; i++) {
		threadsActive[i].join();
	}
}
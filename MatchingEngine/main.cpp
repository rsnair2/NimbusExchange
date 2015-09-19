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

// outputfile
string outputfile_name = "/Users/rsnair2/Studio/MatchingEngine/data/output.txt";
ofstream outfile;
ifstream correct_out;
ifstream output_test;

map<string, OrderBook> assetNameToOrderBookMap;
map<string, mutex> assetNameToOrderBookMutex;

map< string, vector<MarketInstruction *> > orderQueue;
mutex orderQueueMutex;

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
	cout << output_line << endl;

	if(correct_line.substr(0, correct_line.size() - 1) != output_line) {
		cout << output_line << " " << correct_line << endl;
		exit(EXIT_FAILURE);
	}
	
	return 0;
}

OrderFiller of(logFill);

void handleFill(string assetName, unsigned long long buyer, unsigned long long seller, 
		unsigned int quantity, double price, unsigned int latestTimeStamp, unsigned int otherTimeStamp) {
	of.fillOrder(assetName, buyer, seller, quantity, price, latestTimeStamp, otherTimeStamp);
}

void handle_market_instruction(MarketInstruction * mi) {
	string assetTarget = mi->targetAssetName;
	if(assetNameToOrderBookMap.find(assetTarget) == assetNameToOrderBookMap.end())
		assetNameToOrderBookMap[assetTarget] = OrderBook(handleFill);

	assetNameToOrderBookMutex[assetTarget].lock();
	assetNameToOrderBookMap[assetTarget].add_order(mi);
	assetNameToOrderBookMutex[assetTarget].unlock();
}

// void fetchInstructions(const char * filename) {
// 	MarketInstructionIterator mii(filename);
// 	MarketInstruction * mi;
// 	while((mi = mii.getNextInstruction())) {
// 		string assetTarget = mi->targetAssetName;
// 		orderQueueMutex.lock();
// 		orderQueue[assetTarget].push(mi);
// 		orderQueueMutex.unlock();
// 	}
// }

int main(int argc, char * argv[]) {
	output_test.open(outputfile_name);
	correct_out.open("/Users/rsnair2/Studio/MatchingEngine/data/fills.txt");

	of.run();

	MarketInstructionIterator mii(argv[1]);
	MarketInstruction * mi;

	// while((mi = mii.getNextInstruction())) {
	// 	// cout << *mi << endl;
	// 	of.completedOrderWithTimestamp(mi->timestamp);
	// 	// cout << "OUT" << endl;
	// 	handle_market_instruction(mi);
	// 	// cout << "OUT2" << endl;
	// }

	string line0;
	string line1;
	while(correct_out) {
		getline(correct_out, line0);
		getline(output_test, line1);

		line1 = line1 + '\r';


		if(line0 != line1) {
			cout << line0 << endl << line0.size() << endl;
			cout << line1 << endl << line1.size() << endl;

			for(int i = 0; i < line0.size(); i++) {
				cout << int(line0[i]) << " " << int(line1[i]) << endl;
			}

			return 0;
		}
	}
}
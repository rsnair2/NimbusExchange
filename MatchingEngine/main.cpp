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

using namespace std;

// outputfile
string outputfile_name = "/Users/rsnair2/Desktop/output.txt";
ofstream outfile;

map<string, OrderBook> assetNameToOrderBookMap;

OrderFiller * of = new OrderFiller();
of->run();

void handle_market_instruction(MarketInstruction * mi) {
	string assetTarget = mi->targetAssetName;
	assetNameToOrderBookMap[assetTarget].add_order(mi);
}

int main(int argc, char * argv[]) {
	outfile.open(outputfile_name);
	// correct_out.open("/Users/rsnair2/Studio/MatchingEngine/data/small_fills.txt");
	MarketInstructionIterator mii(argv[1]);
	MarketInstruction * mi;
	while((mi = mii.getNextInstruction())) {
		// cout << *mi << endl;
		handle_market_instruction(mi);
	}
}
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
#include "BinaryHeap.h"
#include <iomanip>      // std::setprecision

using namespace std;

// outputfile
string outputfile_name = "/Users/rsnair2/Desktop/output.txt";
ofstream outfile;
ifstream correct_out;

// highest price has higher priority
bool compareBuyOrders(MarketInstruction * left, MarketInstruction * right) {
	if(left->price == right->price) 
		return right->timestamp > left->timestamp;

	return left->price > right->price;
}

// lower price has higher priority
bool compareSellOrders(MarketInstruction * left, MarketInstruction * right) {
	if(left->price == right->price) 
		return right->timestamp > left->timestamp;

	return left->price < right->price;
}

class OrderBook {
public:
	BinaryHeap<MarketInstruction *> * buyOrders;
	BinaryHeap<MarketInstruction *> * sellOrders;

	OrderBook() {
		buyOrders = (new BinaryHeap<MarketInstruction *>(compareBuyOrders));
		sellOrders = (new BinaryHeap<MarketInstruction *>(compareSellOrders));
	}

	void fill(unsigned int quantity, double price, int buyer, int seller, 
		string assetName) {

		outfile << setprecision(10);
		outfile << "FILL " << assetName << " " << buyer << " " << seller << " ";
		outfile << quantity << " " << price << endl;

		ostringstream s;
		s << setprecision(10);
		s << price;
		string output_line = "FILL " + assetName + " " + to_string(buyer) + " " + to_string(seller) + " " + to_string(quantity) + " " + s.str();
		string correct_line;
		getline(correct_out, correct_line);

		if(correct_line != output_line) {
			cout << output_line << " " << correct_line << endl;
			exit(EXIT_FAILURE);
		}
	}

	void add_order(MarketInstruction * mi) {
 		if(mi->type == MarketInstruction::Order) execute_new_order(mi);
		else if(mi->type == MarketInstruction::Cancel) execute_new_cancel_instruction(mi);
		else if(mi->type == MarketInstruction::Replace) execute_new_replace_instruction(mi);
	}

	void execute_new_buy_order(MarketInstruction * mi) {
		MarketInstruction * mostImportantSellOrder = NULL;

		while(sellOrders->size() > 0) {
			if(mi->quantity == 0) break;

			mostImportantSellOrder = sellOrders->rootElem();

			if(mi->price >= mostImportantSellOrder->price) {
				unsigned int quantity = min(mi->quantity, mostImportantSellOrder->quantity);

				mi->quantity -= quantity;
				mostImportantSellOrder->quantity -= quantity;

				double price = mostImportantSellOrder->price;

				fill(quantity, price, mi->id, mostImportantSellOrder->id, mi->targetAssetName);

				if(mostImportantSellOrder->quantity == 0) {
					sellOrders->deleteRootElem();
				}
			}
			else {
				break;
			}
		}

		if(mi->quantity > 0) {
			rest_order(mi);
		}
	}

	void execute_new_sell_order(MarketInstruction * mi) {
		MarketInstruction * mostImportantBuyOrder = NULL;

		while(buyOrders->size() > 0) {
			if(mi->quantity == 0) break;

			mostImportantBuyOrder = buyOrders->rootElem();

			if(mi->price <= mostImportantBuyOrder->price) {

				unsigned int quantity = min(mi->quantity, mostImportantBuyOrder->quantity);

				mi->quantity -= quantity;
				mostImportantBuyOrder->quantity -= quantity;

				double price = mostImportantBuyOrder->price;

				fill(quantity, price, mostImportantBuyOrder->id, mi->id, mi->targetAssetName);

				if(mostImportantBuyOrder->quantity == 0) {
					buyOrders->deleteRootElem();
				}
			}
			else 
				break;
		}

		if(mi->quantity > 0) {
			rest_order(mi);
		}
	}

	void execute_new_order(MarketInstruction * mi) {
		if(mi->type != MarketInstruction::Order) return;

		if(mi->typeOfOrder == MarketInstruction::Buy) {
			execute_new_buy_order(mi);
		}
		else {
			execute_new_sell_order(mi);
		}
	}

	void rest_order(MarketInstruction * mi) {
		if(mi->type == MarketInstruction::Order) {
			if(mi->typeOfOrder == MarketInstruction::Sell) {
				sellOrders->insert(mi, mi->id);
			}
			else if(mi->typeOfOrder == MarketInstruction::Buy) {
				buyOrders->insert(mi, mi->id);
			}
		}
	}

	void execute_new_cancel_instruction(MarketInstruction * mi) {
		if(mi->typeOfOrder == MarketInstruction::Buy) {
			buyOrders->deleteElemWithId(mi->id);
		}
		else {
			sellOrders->deleteElemWithId(mi->id);
		}
	}

	void execute_new_replace_instruction(MarketInstruction * mi) {
		BinaryHeap<MarketInstruction *> * restingOrders = mi->typeOfOrder == MarketInstruction::Buy ? buyOrders : sellOrders;
		MarketInstruction * old = (*restingOrders)[mi->id];
		if(old == NULL) return;
		old->price = mi->new_price;
		old->quantity = mi->new_quantity;

		if(mi->quantity < mi->new_quantity || mi->price != mi->new_price)
			old->timestamp = mi->timestamp;
		else {
			return;
		}
		
		execute_new_cancel_instruction(mi);
		add_order(old);
	}

	void print() {
		cout << "BUY" << endl;
		vector<MarketInstruction *> buyOrdersCopy;
		vector<MarketInstruction *> sellOrdersCopy;

		while(buyOrders->size() > 0) {
			cout << *(buyOrders->rootElem());
			buyOrdersCopy.push_back(buyOrders->rootElem());
			buyOrders->deleteRootElem();
		}
		for(int i = 0; i < buyOrdersCopy.size(); i++) {
			buyOrders->insert(buyOrdersCopy[i], buyOrdersCopy[i]->id);
		}
		cout << endl << endl;
		cout << "SELL" << endl;
		while(sellOrders->size() > 0) {
			cout << *(sellOrders->rootElem());
			sellOrdersCopy.push_back(sellOrders->rootElem());
			sellOrders->deleteRootElem();
		}
		for(int i = 0; i < sellOrdersCopy.size(); i++) {
			sellOrders->insert(sellOrdersCopy[i], sellOrdersCopy[i]->id);
		}
		cout << endl;
	}
};

map<string, OrderBook> assetNameToOrderBookMap;

void handle_market_instruction(MarketInstruction * mi) {
	string assetTarget = mi->targetAssetName;
	assetNameToOrderBookMap[assetTarget].add_order(mi);
}

int main(int argc, char * argv[]) {
	outfile.open(outputfile_name);
	correct_out.open("/Users/rsnair2/Studio/MatchingEngine/data/fills.txt");
	MarketInstructionIterator mii(argv[1]);
	MarketInstruction * mi;
	while((mi = mii.getNextInstruction())) {
		// cout << *mi << endl;
		handle_market_instruction(mi);
	}
}
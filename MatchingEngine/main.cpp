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

// for multithreaded purposes only
#include <pthread.h>

using namespace std;

enum TypeOfInstruction {Order, Cancel, Replace};
enum TypeOfOrder {Buy, Sell};


class MarketInstruction {
public: 
	TypeOfInstruction type;
	unsigned int id;
	unsigned int timestamp;
	string targetAssetName;
	TypeOfOrder typeOfOrder;
	unsigned int quantity;
	double price;
	unsigned int new_quantity; // only valid in the context of a replace instruction
	double new_price;	// valid only for replace instruction

	void print() {
		cout << setprecision(10);
		cout << type << " " << id << " " << timestamp << " " << targetAssetName << " " << typeOfOrder;
		cout << " " << quantity << " " << price << " ";
		cout << endl;
	} 
};

// global input instruction queue
queue<MarketInstruction *> instruction_queue;

// global clock
unsigned int timeTracker = 0;

// outputfile
string outputfile_name = "/home/rsnair2/output.txt";
ofstream outfile;

MarketInstruction * processInputLine(string & line) {
	vector<string> elems;
	string delimiter = ",";
	int delim_index = line.find(delimiter);
	string token = line.substr(0, delim_index);
	MarketInstruction * mi = new MarketInstruction();

	while(delim_index != -1) {
		elems.push_back(token);
		line = line.substr(delim_index + 1, line.size());
		delim_index = line.find(delimiter);
		token = line.substr(0, delim_index);
	}	
	elems.push_back(token);

	for(int i = 0; i < elems.size(); i++) {
		if(elems[0].compare("ORDER") == 0) {
			mi->type = Order;
		}
		else if(elems[0].compare("CANCEL") == 0) {
			mi->type = Cancel;
		}
		else {
			mi->type = Replace;
		}

		istringstream(elems[1]) >> mi->id;
		mi->targetAssetName = elems[2];

		if(elems[3].compare("B") == 0) {
			mi->typeOfOrder = Buy;
		}
		else {
			mi->typeOfOrder = Sell;
		}

		istringstream(elems[4]) >> mi->quantity;
		istringstream(elems[5]) >> mi->price;

		if(mi->type == Replace) {
			mi->new_quantity = mi->quantity;
			mi->new_price = mi->price;
			istringstream(elems[6]) >> mi->quantity;
			istringstream(elems[7]) >> mi->price;
		}
	}

	mi->timestamp = timeTracker++;
	return mi;
}

void * readInputFile(void * t) {
	string filename = *static_cast<std::string*>(t);
	ifstream infile(filename.c_str());

	string line;
	while(getline(infile, line)) {
		MarketInstruction * mi = processInputLine(line);
		instruction_queue.push(mi);
	}

	return NULL;
}

// highest priority is last elem
bool compareBuyOrders(MarketInstruction * left, MarketInstruction * right) {
	if(left->price == right->price) 
		return right->timestamp < left->timestamp;

	return left->price < right->price;
}

bool compareSellOrders(MarketInstruction * left, MarketInstruction * right) {
	if(left->price == right->price) 
		return right->timestamp < left->timestamp;

	return left->price > right->price;
}

class OrderBook {
public:
	vector<MarketInstruction *> buyOrders;
	vector<MarketInstruction *> sellOrders;

	OrderBook() {
	}

	void fill(unsigned int quantity, double price, unsigned int buyer, unsigned int seller, 
		string assetName) {
		outfile << setprecision(10);
		outfile << "FILL " << assetName << " " << buyer << " " << seller << " ";
		outfile << quantity << " " << price << endl;
	}

	void add_order(MarketInstruction * mi) {
		if(mi->type == Order) execute_new_order(mi);
		else if(mi->type == Cancel) execute_new_cancel_instruction(mi);
		else if(mi->type == Replace) execute_new_replace_instruction(mi);
	}

	void execute_new_buy_order(MarketInstruction * mi) {
		unsigned int index = sellOrders.size() -1;
		while(true) {
			if(index == -1) break;
			if(mi->quantity == 0) break;

			MarketInstruction * restingSellOrderWithHighestPriority = sellOrders[index];

			if(mi->price >= restingSellOrderWithHighestPriority->price) {
				unsigned int quantity = min(mi->quantity, restingSellOrderWithHighestPriority->quantity);

				// execute!!!
				mi->quantity -= quantity;
				restingSellOrderWithHighestPriority->quantity -= quantity;

				double price = restingSellOrderWithHighestPriority->price;

				fill(quantity, price, mi->id, restingSellOrderWithHighestPriority->id, mi->targetAssetName);

				if(restingSellOrderWithHighestPriority->quantity == 0) {
					sellOrders.pop_back();
					index = sellOrders.size() - 1;
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
		unsigned int index = buyOrders.size() -1;
		while(true) {
			if(index == -1) break;
			if(mi->quantity == 0) break;

			MarketInstruction * restingBuyOrderWithHighestPriority = buyOrders[index];

			if(mi->price <= restingBuyOrderWithHighestPriority->price) {
				unsigned int quantity = min(mi->quantity, restingBuyOrderWithHighestPriority->quantity);

				// execute!!!
				mi->quantity -= quantity;
				restingBuyOrderWithHighestPriority->quantity -= quantity;

				double price = restingBuyOrderWithHighestPriority->price;
				fill(quantity, price, restingBuyOrderWithHighestPriority->id, mi->id, mi->targetAssetName);

				if(restingBuyOrderWithHighestPriority->quantity == 0) {
					buyOrders.pop_back();
					index = buyOrders.size() - 1;
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

	// handles buy/sell orders only
	void execute_new_order(MarketInstruction * mi) {
		if(mi->type != Order) return;

		if(mi->typeOfOrder == Buy) {
			execute_new_buy_order(mi);
		}
		else {
			execute_new_sell_order(mi);
		}
	}

	void rest_order(MarketInstruction * mi) {
		if(mi->type == Order) {
			if(mi->typeOfOrder == Sell) {
				sellOrders.push_back(mi);
			}
			else if(mi->typeOfOrder == Buy) {
				buyOrders.push_back(mi);
			}
		}

		// sort the lists
		if(mi->typeOfOrder == Sell) {
			sort(sellOrders.begin(), sellOrders.end(), compareSellOrders);
		}
		else if(mi->typeOfOrder == Buy) {
			sort(buyOrders.begin(), buyOrders.end(), compareBuyOrders);
		}
	}

	void execute_new_cancel_instruction(MarketInstruction * mi) {
		vector<MarketInstruction *> & restingOrders = mi->typeOfOrder == Buy ? buyOrders : sellOrders;

		for(unsigned int i = 0; i < restingOrders.size(); i++) {
			MarketInstruction * restingOrder = restingOrders[i];
			if(restingOrder->id == mi->id) {
				restingOrders.erase(restingOrders.begin() + i);
			}
		}
	}

	void execute_new_replace_instruction(MarketInstruction * mi) {
		vector<MarketInstruction *> & restingOrders = mi->typeOfOrder == Buy ? buyOrders : sellOrders;

		if(mi->new_quantity == 0) execute_new_cancel_instruction(mi);

		for(unsigned int i = 0; i < restingOrders.size(); i++) {
			MarketInstruction * restingOrder = restingOrders[i];
			if(restingOrder->id == mi->id) {
				if(mi->price == mi->new_price && mi->new_quantity < mi->quantity) {
					if(restingOrder->quantity < mi->new_quantity) return;
					restingOrder->quantity = mi->new_quantity;
					break;
				}
				else if(mi->price != mi->new_price || mi->new_quantity > mi->quantity) {
					mi->type = Order;
					mi->quantity = (restingOrder->quantity) + (mi->new_quantity - mi->quantity);
					mi->price = mi->new_price;
					execute_new_cancel_instruction(mi);
					execute_new_order(mi);
					break;
				}
			}
		}
	}

	void print() {
		for(vector<MarketInstruction *>::iterator it = buyOrders.begin(); it != buyOrders.end(); ++it) {
			(*it)->print();
		}
		cout << "~~~" << endl;
		for(vector<MarketInstruction *>::iterator it = sellOrders.begin(); it != sellOrders.end(); ++it) {
			(*it)->print();
		}	
		cout << "---" << endl;
	}
};

map<string, OrderBook> assetNameToOrderBookMap;

// for debugging purposes
void print_instruction_queue() {
	while(instruction_queue.size() > 0) {
		instruction_queue.front()->print();
		instruction_queue.pop();
	}
}

void handle_market_instruction(MarketInstruction * mi) {
	string assetTarget = mi->targetAssetName;
	assetNameToOrderBookMap[assetTarget].add_order(mi);
}

void handle_market_instruction_queue() {
	while(instruction_queue.size() > 0) {
		handle_market_instruction(instruction_queue.front());
		instruction_queue.pop();
	}
}

int main(int argc, char *argv[])
{
	string filename = string("../../data/small_orders.txt");
	
	/*
	// multithreaded solution
	// // start thread to read input here
	// pthread_t input_thread;
	// void * input_thread_payload;
 	//   	void * status;

   	// input_thread_payload = static_cast<void*>(&filename);
	// pthread_create(&input_thread, NULL, readInputFile, input_thread_payload);
	// pthread_join(input_thread, &status);
	*/

   	// single threaded solution
	void * input_thread_payload;
   	input_thread_payload = static_cast<void*>(&filename);
	readInputFile(input_thread_payload);
	outfile.open(outputfile_name.c_str());
	handle_market_instruction_queue();
}
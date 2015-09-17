#include "OrderFiller.h"

#include <stdlib.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
#include <fstream>

ifstream correct_out;
using namespace std;

//void (*fill_order_handler)(string, int, int, int, double)
OrderFiller::OrderFiller() {
	// this->fill_order_handler = fill_order_handler;
	state = OrderFiller::Working;
	minCompletedOrder = -1;
}

OrderFiller::~OrderFiller() {
	stateMutex.lock();
	state = OrderFiller::Done;
	stateMutex.unlock();

}

void OrderFiller::completedOrderWithTimestamp(unsigned int timestamp) {
	completedOrdersQueueMutex.lock();
	completedOrdersQueue.push(timestamp);
	completedOrdersQueueMutex.unlock();
}

void OrderFiller::fillOrder(string assetName, unsigned int buyer, 
	unsigned int seller, int quantity, double price, unsigned int latestTimeStamp) {
	FilledOrder fo(assetName, buyer, seller, quantity, price, latestTimeStamp);
	filledOrdersQueueMutex.lock();
	filledOrdersQueue.push(fo);
	filledOrdersQueueMutex.unlock();
}

void OrderFiller::run() {
	td = thread(&OrderFiller::runOrderFiller, this);
}

void OrderFiller::runOrderFiller() {
	cout << "HERE" << endl;
	correct_out.open("/Users/rsnair2/Studio/MatchingEngine/data/small_fills.txt");
	// while(true) {
	// 	completedOrdersQueueMutex.lock();
	// 	while(completedOrdersQueue.size() > 0) {
	// 		completedOrders.push(completedOrdersQueue.front());
	// 		completedOrdersQueue.pop();
	// 	}
	// 	completedOrdersQueueMutex.unlock();

	// 	filledOrdersQueueMutex.lock();
	// 	while(filledOrdersQueue.size() > 0) {
	// 		filledOrders.push(filledOrdersQueue.front());
	// 		filledOrdersQueue.pop();
	// 	}
	// 	filledOrdersQueueMutex.unlock();

	// 	while(completedOrders.size() && completedOrders.top() == minCompletedOrder + 1) {
	// 		minCompletedOrder += 1;
	// 		completedOrders.pop();
	// 	}

	// 	while(filledOrders.size() && filledOrders.top().latestTimeStamp <= minCompletedOrder) {
	// 		FilledOrder fo = filledOrders.top();
	// 		filledOrders.pop();

	// 		double price = fo.price;
	// 		string assetName = fo.assetName;
	// 		unsigned int buyer = fo.buyer;
	// 		unsigned int seller = fo.seller;
	// 		int quantity = fo.quantity;
	// 		price = fo.price;

	// 		// outfile << setprecision(10);
	// 		// outfile << "FILL " << assetName << " " << buyer << " " << seller << " ";
	// 		// outfile << quantity << " " << price << endl;

	// 		ostringstream s;
	// 		s << setprecision(10);
	// 		s << price;
	// 		string output_line = "FILL " + assetName + " " + to_string(buyer) + " " + to_string(seller) + " " + to_string(quantity) + " " + s.str();
			// string correct_line;

			// getline(correct_out, correct_line);

			// 			cout << correct_out.is_open() << endl;


			// if(correct_line != output_line) {
			// 	cout << ": " << output_line << " :" << correct_line << endl;
			// 	exit(EXIT_FAILURE);
			// }
		// }

		// stateMutex.lock();
		// if(state == OrderFiller::Done && filledOrders.size() == 0) {
		// 	stateMutex.unlock();
		// 	break;
		// }
		// stateMutex.unlock();
	// }
}
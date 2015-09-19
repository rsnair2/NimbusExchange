#ifndef __ORDER_FILLER__
#define __ORDER_FILLER__

#include "MarketInstruction.h"
#include <thread>
#include <mutex>
#include <queue>
#include <vector>
#include "FilledOrder.h"

using namespace std;

class OrderFiller {
public:
	OrderFiller(int (*fill_order_handler)(string, unsigned long long, unsigned long long, unsigned int, double));
	~OrderFiller();
	void completedOrderWithTimestamp(unsigned int timestamp);
	void fillOrder(string assetName, unsigned long long buyer, 
		unsigned long long seller, int quantity, double price, unsigned int latestTimeStamp, unsigned int otherTimeStamp);
	void run();
	void flush();

	int (*fill_order_handler)(string, unsigned long long, unsigned long long, unsigned int, double);

	enum State {Done, Working};
	State state;
	mutex stateMutex;

	int minCompletedOrder;

	priority_queue<FilledOrder> filledOrders;
	priority_queue< int, vector<int>, greater<int> > completedOrders;

	queue<int> completedOrdersQueue;
	mutex completedOrdersQueueMutex;

	queue<FilledOrder> filledOrdersQueue;
	mutex filledOrdersQueueMutex;

	thread td;

	void runOrderFiller();
};

#endif
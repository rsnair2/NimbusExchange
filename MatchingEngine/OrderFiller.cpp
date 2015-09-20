#include "OrderFiller.h"

using namespace std;

OrderFiller::OrderFiller(int (*fill_order_handler)(string, unsigned long long, unsigned long long, unsigned int, double)) {
	this->fill_order_handler = fill_order_handler;
	state = OrderFiller::Working;
	minCompletedOrder = -1;
}

OrderFiller::~OrderFiller() {
	stateMutex.lock();
	state = OrderFiller::Done;
	stateMutex.unlock();
	td.join();
}

void OrderFiller::completedOrderWithTimestamp(unsigned int timestamp) {
	completedOrdersQueueMutex.lock();
	completedOrdersQueue.push(timestamp);
	completedOrdersQueueMutex.unlock();
}

void OrderFiller::fillOrder(string assetName, unsigned long long buyer, 
	unsigned long long seller, int quantity, double price, unsigned int latestTimeStamp, unsigned int otherTimeStamp) {
	FilledOrder fo(assetName, buyer, seller, quantity, price, latestTimeStamp, otherTimeStamp);
	filledOrdersQueueMutex.lock();
	filledOrdersQueue.push(fo);
	filledOrdersQueueMutex.unlock();
}

void OrderFiller::run() {
	td = thread(&OrderFiller::runOrderFiller, this);
}

void OrderFiller::flush() {
	while(filledOrders.size() > 0) {
		FilledOrder fo = filledOrders.top();
		if(fo.latestTimeStamp >= minCompletedOrder)
			return;
		fill_order_handler(fo.assetName, fo.buyer, fo.seller, fo.quantity, fo.price);
		filledOrders.pop();
	}
}

void OrderFiller::runOrderFiller() {
	while(true) {
		completedOrdersQueueMutex.lock();
		while(completedOrdersQueue.size() > 0) {
			completedOrders.push(completedOrdersQueue.front());
			completedOrdersQueue.pop();
		}
		completedOrdersQueueMutex.unlock();

		filledOrdersQueueMutex.lock();
		while(filledOrdersQueue.size() > 0) {
			filledOrders.push(filledOrdersQueue.front());
			filledOrdersQueue.pop();
		}
		filledOrdersQueueMutex.unlock();

		while(completedOrders.size() > 0 && completedOrders.top() == minCompletedOrder + 1) {
			minCompletedOrder += 1;
			completedOrders.pop();
		}

		flush();

		stateMutex.lock();
		filledOrdersQueueMutex.lock();
		if(state == OrderFiller::Done && filledOrdersQueue.size() == 0) {
			filledOrdersQueueMutex.unlock();
			stateMutex.unlock();
			flush();
			break;
		}
		filledOrdersQueueMutex.unlock();
		stateMutex.unlock();
	}
}
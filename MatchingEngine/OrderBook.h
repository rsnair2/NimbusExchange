#ifndef __ORDER_BOOK__
#define __ORDER_BOOK__

#include "MarketInstruction.h"
#include "BinaryHeap.h"

class OrderBook {
public:
	void add_order(MarketInstruction * mi);
	OrderBook(void (*fillOrderHandler)(string, unsigned long long, unsigned long long, unsigned int, double, unsigned int, unsigned int));
	OrderBook() {};

private:
	unsigned int localTime;

	BinaryHeap<MarketInstruction *> * buyOrders;
	BinaryHeap<MarketInstruction *> * sellOrders;

	static bool compareSellOrders(MarketInstruction * left, MarketInstruction * right);
	static bool compareBuyOrders(MarketInstruction * left, MarketInstruction * right);

	void execute_new_buy_order(MarketInstruction * mi);
	void execute_new_sell_order(MarketInstruction * mi);
	void execute_new_order(MarketInstruction * mi);
	void rest_order(MarketInstruction * mi);
	void execute_new_cancel_instruction(MarketInstruction * mi);
	void execute_new_replace_instruction(MarketInstruction * mi);
	void fill_order(MarketInstruction * one, MarketInstruction * two);

	void (*fillOrderHandler)(string, unsigned long long, unsigned long long, unsigned int, double, unsigned int, unsigned int);
};

#endif
// #include "MarketInstruction.h"
// #include <thread>
// #include <mutex>
// #include <queue>

// class Market {
// public:
// 	Market((void)(*fill_order_handler)(string, int, int, int, double));
// 	void add_order();


// private:
// 	map<string, OrderBook> assetNameToOrderBookMap;
// 	map<string, mutex> orderBookMutexes;

// 	map< string, queue<MarketInstruction *> > orderQueue;
// 	mutex orderQueueMutex;

// 	(void)(*fill_order_handler)(string, int, int, int, double));
// }
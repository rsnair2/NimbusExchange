#include "MarketInstruction.h"
#include <thread>
#include <mutex>
#include <queue>
#include <vector>

using namespace std;

// void (*fill_order_handler)(string, int, int, int, double)

class OrderFiller {
public:
	OrderFiller();
	~OrderFiller();
	void completedOrderWithTimestamp(unsigned int timestamp);
	void fillOrder(string assetName, unsigned int buyer, 
		unsigned int seller, int quantity, double price, unsigned int latestTimeStamp);
	void run();

private:
	// void (*fill_order_handler)(string, int, int, int, double);

	enum State {Done, Working};
	State state;
	mutex stateMutex;

	int minCompletedOrder;

	class FilledOrder {
	public:

		string assetName;
		unsigned int buyer;
		unsigned int seller;
		int quantity;
		double price;
		unsigned int latestTimeStamp;

		FilledOrder(string assetName, unsigned int buyer, unsigned int seller, int quantity, double price, unsigned int latestTimeStamp) {
			this->assetName = assetName;
			this->buyer = buyer;
			this->seller = seller;
			this->quantity = quantity;
			this->price = price;
			this->latestTimeStamp = latestTimeStamp;
		}

		// this is BAD!!! REMEMBER TO FIX THIS
		bool operator<(const FilledOrder & two) const {
			return this->latestTimeStamp > two.latestTimeStamp;
		}
	};

	priority_queue<FilledOrder> filledOrders;
	priority_queue< int, vector<int>, greater<int> > completedOrders;

	queue<int> completedOrdersQueue;
	mutex completedOrdersQueueMutex;

	queue<FilledOrder> filledOrdersQueue;
	mutex filledOrdersQueueMutex;

	thread td;

	void runOrderFiller();
};
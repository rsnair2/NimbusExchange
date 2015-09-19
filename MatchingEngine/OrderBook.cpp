#include "OrderBook.h"

// higher price has higher priority
bool OrderBook::compareBuyOrders(MarketInstruction * left, MarketInstruction * right) {
	if(left->price == right->price) 
		return right->timestamp > left->timestamp;

	return left->price > right->price;
}

// lower price has higher priority
bool OrderBook::compareSellOrders(MarketInstruction * left, MarketInstruction * right) {
	if(left->price == right->price) 
		return right->timestamp > left->timestamp;

	return left->price < right->price;
}

void OrderBook::execute_new_buy_order(MarketInstruction * mi) {
	MarketInstruction * mostImportantSellOrder = NULL;

	while(sellOrders->size() > 0) {
		if(mi->quantity == 0) break;

		mostImportantSellOrder = sellOrders->rootElem();

		if(mi->price >= mostImportantSellOrder->price) {
			fill_order(mi, mostImportantSellOrder);

			if(mostImportantSellOrder->quantity == 0) {
				sellOrders->deleteRootElem();
			}
		}
		else
			break;
	}

	if(mi->quantity > 0) {
		rest_order(mi);
	}
}

void OrderBook::execute_new_sell_order(MarketInstruction * mi) {
	MarketInstruction * mostImportantBuyOrder = NULL;

	while(buyOrders->size() > 0) {
		if(mi->quantity == 0) break;

		mostImportantBuyOrder = buyOrders->rootElem();

		if(mi->price <= mostImportantBuyOrder->price) {
			fill_order(mi, mostImportantBuyOrder);

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

void OrderBook::execute_new_order(MarketInstruction * mi) {
	if(mi->type != MarketInstruction::Order) return;

	if(mi->typeOfOrder == MarketInstruction::Buy) {
		execute_new_buy_order(mi);
	}
	else {
		execute_new_sell_order(mi);
	}
}

void OrderBook::rest_order(MarketInstruction * mi) {
	if(mi->type == MarketInstruction::Order) {
		if(mi->typeOfOrder == MarketInstruction::Sell) {
			sellOrders->insert(mi, mi->id);
		}
		else if(mi->typeOfOrder == MarketInstruction::Buy) {
			buyOrders->insert(mi, mi->id);
		}
	}
}

void OrderBook::execute_new_cancel_instruction(MarketInstruction * mi) {
	if(mi->typeOfOrder == MarketInstruction::Buy) {
		buyOrders->deleteElemWithId(mi->id);
	}
	else {
		sellOrders->deleteElemWithId(mi->id);
	}
}

void OrderBook::execute_new_replace_instruction(MarketInstruction * mi) {
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

void OrderBook::fill_order(MarketInstruction * one, MarketInstruction * two) {

	unsigned int quantity = min(one->quantity, two->quantity);
	one->quantity -= quantity;
	two->quantity -= quantity;
	
	double price = one->timestamp < two->timestamp ? one->price : two->price;	// take the older one's timestamp

	unsigned long long buyer = one->typeOfOrder == MarketInstruction::Buy ? one->id : two->id;
	unsigned long long seller = one->typeOfOrder == MarketInstruction::Sell ? one->id : two->id;

	string assetName = one->targetAssetName;

	unsigned int latestTimeStamp = one->timestamp > two->timestamp ? one->timestamp : two->timestamp;
	unsigned int otherTimeStamp = localTime++;

	fillOrderHandler(assetName, buyer, seller, quantity, price, latestTimeStamp, otherTimeStamp);
}

OrderBook::OrderBook(void (*fillOrderHandler)(string, unsigned long long, unsigned long long, unsigned int, double, unsigned int, unsigned int)) {
	buyOrders = (new BinaryHeap<MarketInstruction *>(compareBuyOrders));
	sellOrders = (new BinaryHeap<MarketInstruction *>(compareSellOrders));
	this->fillOrderHandler = fillOrderHandler;
	localTime = 0;
}

void OrderBook::add_order(MarketInstruction * mi) {
	if(mi->type == MarketInstruction::Order) execute_new_order(mi);
	else if(mi->type == MarketInstruction::Cancel) execute_new_cancel_instruction(mi);
	else if(mi->type == MarketInstruction::Replace) execute_new_replace_instruction(mi);
}
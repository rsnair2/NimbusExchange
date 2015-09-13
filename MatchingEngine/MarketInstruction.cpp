#include "MarketInstruction.h"
using std::endl;

std::ostream & operator<<(std::ostream & os, const MarketInstruction & mi) {
	os << mi.timestamp << " ";
	os << mi.id << " ";
	os << mi.targetAssetName << " ";

	switch(mi.type) {
		case(MarketInstruction::Order): os << "Order "; break;
		case(MarketInstruction::Replace): os << "Replace "; break; 
		case(MarketInstruction::Cancel): os << "Cancel "; break;
	}

	switch(mi.typeOfOrder) {
		case(MarketInstruction::Buy): os << "Buy" << " "; break;
		case(MarketInstruction::Sell): os << "Sell" << " "; break;
	}

	os << mi.price << " ";
	os << mi.quantity << " ";

	return os << endl;
}
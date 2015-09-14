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

MarketInstruction::MarketInstruction(string & line) {
	vector<string> elems;
	string delimiter = ",";
	int delim_index = line.find(delimiter);
	string token = line.substr(0, delim_index);

	// get a list of tokens
	while(delim_index != -1) {
		elems.push_back(token);
		line = line.substr(delim_index + 1, line.size());
		delim_index = line.find(delimiter);
		token = line.substr(0, delim_index);
	}	
	elems.push_back(token);

	// initialize the fields based on line input
	for(int i = 0; i < elems.size(); i++) {
		if(elems[0].compare("ORDER") == 0) {
			this->type = MarketInstruction::Order;
		}
		else if(elems[0].compare("CANCEL") == 0) {
			this->type = MarketInstruction::Cancel;
		}
		else {
			this->type = MarketInstruction::Replace;
		}

		istringstream(elems[1]) >> this->id;
		this->targetAssetName = elems[2];

		if(elems[3].compare("B") == 0) {
			this->typeOfOrder = MarketInstruction::Buy;
		}
		else {
			this->typeOfOrder = MarketInstruction::Sell;
		}

		istringstream(elems[4]) >> this->quantity;
		istringstream(elems[5]) >> this->price;

		if(this->type == MarketInstruction::Replace) {
			this->new_quantity = this->quantity;
			this->new_price = this->price;
			istringstream(elems[6]) >> this->quantity;
			istringstream(elems[7]) >> this->price;
		}
	}
}
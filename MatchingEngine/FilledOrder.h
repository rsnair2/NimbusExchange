#ifndef __FILLED_ORDER__
#define __FILLED_ORDER__

class FilledOrder {
public:

	string assetName;
	unsigned long long buyer;
	unsigned long long seller;
	int quantity;
	double price;
	unsigned int latestTimeStamp;
	unsigned int otherTimeStamp;

	FilledOrder(string assetName, unsigned long long buyer, 
		unsigned long long seller, 
		int quantity, 
		double price, 
		unsigned int latestTimeStamp, unsigned int otherTimeStamp) {
		this->assetName = assetName;
		this->buyer = buyer;
		this->seller = seller;
		this->quantity = quantity;
		this->price = price;
		this->latestTimeStamp = latestTimeStamp;
		this->otherTimeStamp = otherTimeStamp;
	}

	FilledOrder() {};

	// this is BAD!!! REMEMBER TO FIX THIS
	bool operator<(const FilledOrder & two) const {
		if(this->latestTimeStamp == two.latestTimeStamp)
			return this->otherTimeStamp > two.otherTimeStamp; 
		return this->latestTimeStamp > two.latestTimeStamp;
	}
};

#endif
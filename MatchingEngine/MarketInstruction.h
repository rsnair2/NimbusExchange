#include <iostream>
#include <string>

using std::string;

class MarketInstruction {
public: 
	enum TypeOfInstruction {Order, Cancel, Replace};
	enum TypeOfOrder {Buy, Sell};

	TypeOfInstruction type;
	unsigned int id;	
	unsigned int timestamp; 
	string targetAssetName; // a.k.a SYMBOL
	TypeOfOrder typeOfOrder; 
	unsigned int quantity;
	double price;
	unsigned int new_quantity; 
	double new_price;	// new_price and new_quantity are valid only when type == Replace

	friend std::ostream & operator<<(std::ostream & os, const MarketInstruction & mi);
};
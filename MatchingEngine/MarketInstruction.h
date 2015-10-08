#ifndef __MARKET_INSTRUCTION__
#define __MARKET_INSTRUCTION__

#include <iostream>
#include <string>
#include <vector>
#include <sstream> 

using namespace std;

class MarketInstruction {
public: 
	enum TypeOfInstruction {Order, Cancel, Replace};
	enum TypeOfOrder {Buy, Sell};

	TypeOfInstruction type;
	unsigned long long id;	
	unsigned int timestamp; 
	string targetAssetName; // a.k.a SYMBOL
	TypeOfOrder typeOfOrder; 
	unsigned int quantity;
	double price;
	unsigned int new_quantity; 
	double new_price;	// new_price and new_quantity are valid only when type == Replace

	friend std::ostream & operator<<(std::ostream & os, const MarketInstruction & mi);

	// legacy function
	void print() { cout << this;};

	// constructor
	MarketInstruction(string & line);
};

#endif
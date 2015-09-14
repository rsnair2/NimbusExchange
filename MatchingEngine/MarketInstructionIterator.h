#ifndef __MARKET_INSTRUCTION_ITERATOR__
#define __MARKET_INSTRUCTION_ITERATOR__

#include "LineReader.h"
#include "MarketInstruction.h"

class MarketInstructionIterator {
public:
	MarketInstructionIterator(string filename);
	~MarketInstructionIterator();

	MarketInstruction * getNextInstruction();

private:
	LineReader<MarketInstruction> * lineReader;
	unsigned int timestamp;
};

#endif
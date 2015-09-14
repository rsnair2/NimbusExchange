#include "MarketInstructionIterator.h"

MarketInstructionIterator::MarketInstructionIterator(string filename) {
	timestamp = 0;
	lineReader = new LineReader<MarketInstruction>(filename);
	lineReader->run();
}

MarketInstructionIterator::~MarketInstructionIterator() {
	free(lineReader);
}

MarketInstruction * MarketInstructionIterator::getNextInstruction() {
	MarketInstruction * mi = lineReader->readLine();
	if(mi)
		mi->timestamp = timestamp++;
	return mi;
}
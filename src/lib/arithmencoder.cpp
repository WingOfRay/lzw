/**
 * @file arithmencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "arithmencoder.h"

ArithmeticEncoder::ArithmeticEncoder() {
	reset();
}

void ArithmeticEncoder::reset() {
	intervalLow = 0;
	intervalHigh = MAX_INTERVAL;
	counter = 0;

	bitPosInLastByte = 0;
	bitsWritten = 0;
}

void ArithmeticEncoder::writeBit(bool bit) {
	// inc written bits counter
	bitsWritten++;

	// if we are at first bit add new byte
	if (bitPosInLastByte == 0)
		buffer.push_back(0);

	// set or clear desired bit in last buffer byte
	if (bit)
		buffer.back() |= 1 << bitPosInLastByte;
	else
		buffer.back() &= ~(1 << bitPosInLastByte);

	// inc bitPos if we overflow number of bits in byte
	bitPosInLastByte++;
	if (bitPosInLastByte >= CHAR_BIT)
		bitPosInLastByte = 0;			// zero bitPos
}

void ArithmeticEncoder::encode(unsigned symbol, DataModel* dataModel) {
	// on adaptive data model we increase symbol frequency
	auto adaptiveModel = dynamic_cast<AdaptiveDataModel*>(dataModel);
	if (adaptiveModel != nullptr)
		adaptiveModel->incSymbolFreq(symbol);

	// compute helper value
	int s = (intervalHigh - intervalLow + 1) / dataModel->getCumulativeFreq(dataModel->size() - 1);

	// interval lower bound is computed as low + s * cumFreq(i-1) and cumFreq(-1) == 0 so
	// if symbol == 0 then interval lower bound is not modified 
	if (symbol != 0) {
		intervalLow += s * dataModel->getCumulativeFreq(symbol - 1);
	}

	// interval upper bound is computed as low + s * cumFreq(i) - 1
	intervalHigh = intervalLow + s * dataModel->getCumulativeFreq(symbol) - 1;

	// enlarge interval and send info about it to output
	// loop ends when interval is large enough
	for (;;) {
		// interval is in lower half of possible range
		if (intervalHigh < HALF_INTERVAL) {
			// enlarge interval
			intervalLow = 2 * intervalLow;
			intervalHigh = 2 * intervalHigh + 1;

			// send zero to output and number of ones as specified in counter variable
			writeBit(false);
			for (size_t i = 0; i < counter; ++i)
				writeBit(true);

			counter = 0;				// reset counter

		// interval is in upper half of possible range
		} else if (intervalLow >= HALF_INTERVAL) {
			// enlarge interval
			intervalLow = 2 * (intervalLow - HALF_INTERVAL);
			intervalHigh = 2 * (intervalHigh - HALF_INTERVAL) + 1;

			// send one to output and number of zeros as specified in counter variable
			writeBit(true);
			for (size_t i = 0; i < counter; ++i)
				writeBit(false);

			counter = 0;				// reset counter

		// interval is in middle of possible range
		} else if (intervalLow >= QUATER_INTERVAL && intervalHigh < THREE_QUATERS_INTERVAL) {
			// enlarge interval
			intervalLow = 2 * (intervalLow - QUATER_INTERVAL);
			intervalHigh = 2 * (intervalHigh - QUATER_INTERVAL) + 1;

			// increase counter
			counter++;

		// none of these cases so break loop
		} else
			break;
	}
}

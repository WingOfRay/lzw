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
	intervalHigh = IntervalTraitsType::MAX;
	counter = 0;

	bitPosInLastByte = 0;
	bufferBits = 0;
	buffer.clear();
}

void ArithmeticEncoder::writeBit(bool bit) {
	// inc written bits counter
	bufferBits++;

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
		if (intervalHigh < IntervalTraitsType::HALF) {
			// enlarge interval
			intervalLow = 2 * intervalLow;
			intervalHigh = 2 * intervalHigh + 1;

			// encode first case as zero
			writeBit(false);
			// handle third case, we use relation that (C3)^k C1 = C1 (C2)^k
			for (size_t i = 0; i < counter; ++i)
				writeBit(true);

			counter = 0;				// reset counter

		// interval is in upper half of possible range
		} else if (intervalLow >= IntervalTraitsType::HALF) {
			// enlarge interval
			intervalLow = 2 * (intervalLow - IntervalTraitsType::HALF);
			intervalHigh = 2 * (intervalHigh - IntervalTraitsType::HALF) + 1;

			// encode second case as one
			writeBit(true);
			// handle third case, we use relation that (C3)^k C2 = C2 (C1)^k
			for (size_t i = 0; i < counter; ++i)
				writeBit(false);

			counter = 0;				// reset counter

		// interval is in middle of possible range
		} else if (intervalLow >= IntervalTraitsType::QUARTER && intervalHigh < IntervalTraitsType::THREE_QUARTERS) {
			// enlarge interval
			intervalLow = 2 * (intervalLow - IntervalTraitsType::QUARTER);
			intervalHigh = 2 * (intervalHigh - IntervalTraitsType::QUARTER) + 1;

			// this case can't be encoded directly but we can prove that
			// (C3)^k C1 = C1 (C2)^k and (C3)^k C2 = C2 (C1)^k so we count
			// third cases in row and than handle them in first and second case using
			// above formulas

			// increase counter
			counter++;

		// none of these cases so break loop
		} else
			break;
	}
}

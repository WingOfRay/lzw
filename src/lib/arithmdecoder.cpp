/**
 * @file arithmdecoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "arithmdecoder.h"

ArithmeticDecoder::ArithmeticDecoder(const char* data, size_t numBits) {
	reset(data, numBits);
}

void ArithmeticDecoder::reset(const char* data, size_t numBits) {
	this->data = data;
	this->numBits = numBits;

	intervalLow = 0;
	intervalHigh = IntervalTraitsType::MAX;

	// construct data end iterator
	dataEnd = BitIterator(data + numBits / CHAR_BIT, numBits % CHAR_BIT);
	dataIter = BitIterator(data, 0);

	// read first IntervalTraitsType::BITS from data to value
	value = 0;
	for (size_t i = 0; i < IntervalTraitsType::BITS; ++i)
		readBit();
}

void ArithmeticDecoder::readBit() {
	bool bit = false;			// on data end we append zero bit
	if (dataIter != dataEnd)
		bit = *dataIter++;

	value <<= 1;
	if (bit)
		value += 1;
}

unsigned ArithmeticDecoder::decode(DataModel* dataModel) {
	uint64_t range = intervalHigh - intervalLow + 1;
	auto scale = dataModel->getCumulativeFreq(dataModel->size() - 1);
	auto cumulativeFreq = ((value - intervalLow + 1) * scale - 1) / range;

	unsigned symbol = 0;
	// find i where cumuliveFreqs[i-1] < cumulativeFreq < cumulativeFreqs[i]
	for (size_t i = 0; i < dataModel->size(); ++i) {
		auto lowerBound = i != 0 ? dataModel->getCumulativeFreq(i - 1) : 0U;
		auto upperBound = dataModel->getCumulativeFreq(i);
		if (lowerBound <= cumulativeFreq && cumulativeFreq < upperBound) {
			symbol = i;
			break;
		}
	}

	// compute new interval bounds

	// interval upper bound is computed as low + s * cumFreq(i) - 1
	intervalHigh = intervalLow + (range * dataModel->getCumulativeFreq(symbol)) / scale - 1;

	// interval lower bound is computed as low + s * cumFreq(i-1) and cumFreq(-1) == 0 so
	// if symbol == 0 then interval lower bound is not modified 
	if (symbol != 0) {
		intervalLow += (range * dataModel->getCumulativeFreq(symbol - 1)) / scale;
	}

	// enlarge interval and get bits from data
	// loop ends when interval is large enough
	for (;;) {
		// interval is in lower half of possible range
		if (intervalHigh < IntervalTraitsType::HALF) {
			;	// do nothing
		// interval is in upper half of possible range
		} else if (intervalLow >= IntervalTraitsType::HALF) {
			intervalLow -= IntervalTraitsType::HALF;
			intervalHigh -= IntervalTraitsType::HALF;
			value -= IntervalTraitsType::HALF;
		// interval is in middle of possible range
		} else if (intervalLow >= IntervalTraitsType::QUARTER && intervalHigh <= IntervalTraitsType::THREE_QUARTERS) {
			intervalLow -= IntervalTraitsType::QUARTER;
			intervalHigh -= IntervalTraitsType::QUARTER;
			value -= IntervalTraitsType::QUARTER;
		// none of these cases so break loop
		} else
			break;

		intervalLow <<= 1;
		intervalHigh = (intervalHigh << 1) + 1;
		readBit();
	}

	//auto scale = dataModel->getCumulativeFreq(dataModel->size() - 1);
	//auto range = (intervalHigh - intervalLow + 1) / scale;
	//auto cumulativeFreq = (uint64_t(value - intervalLow + 1) * scale - 1) / uint64_t(intervalHigh - intervalLow + 1);

	//unsigned symbol = 0;
	//// find i where cumuliveFreqs[i-1] < cumulativeFreq < cumulativeFreqs[i]
	//for (size_t i = 0; i < dataModel->size(); ++i) {
	//	auto lowerBound = i != 0 ? dataModel->getCumulativeFreq(i - 1) : 0U;
	//	auto upperBound = dataModel->getCumulativeFreq(i);
	//	if (lowerBound <= cumulativeFreq && cumulativeFreq < upperBound) {
	//		symbol = i;
	//		break;
	//	}
	//}

	//intervalHigh = intervalLow + (range * dataModel->getCumulativeFreq(symbol)) - 1;
	//if (symbol != 0)
	//	intervalLow = intervalLow + range * dataModel->getCumulativeFreq(symbol - 1);

	//for (;;) {
	//	bool lowMsb = getBit(intervalLow, IntervalTraitsType::BITS);
	//	bool highMsb = getBit(intervalHigh, IntervalTraitsType::BITS);
	//	// compare high and low msb
	//	if (lowMsb == highMsb) {
	//		intervalLow <<= 1;
	//		intervalHigh = (intervalHigh << 1) | 1;
	//		value <<= 1;
	//		readBit();
	//	} else {
	//		bool secondLowMsb = getBit(intervalLow, IntervalTraitsType::BITS - 1);
	//		bool secondHighMsb = getBit(intervalHigh, IntervalTraitsType::BITS - 1);
	//		if (secondLowMsb && !secondHighMsb) {
	//			value ^= IntervalTraitsType::QUARTER;
	//			intervalHigh |= IntervalTraitsType::QUARTER;
	//			intervalLow &= IntervalTraitsType::QUARTER - 1;

	//			intervalLow <<= 1;
	//			intervalHigh = (intervalHigh << 1) | 1;
	//			value <<= 1;
	//			readBit();
	//		} else
	//			break;
	//	}
	//}


	// on adaptive data model we increase symbol frequency
	auto adaptiveModel = dynamic_cast<AdaptiveDataModel*>(dataModel);
	if (adaptiveModel != nullptr)
		adaptiveModel->incSymbolFreq(symbol);

	return symbol;
}
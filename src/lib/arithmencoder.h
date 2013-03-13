/**
 * @file arithmencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef ARITHMENCODER_H
#define ARITHMENCODER_H

#include "arithmcodec.h"

class ArithmeticEncoder : public ArithmeticCodec
{
public:
	ArithmeticEncoder();

	void reset();

	void encode(unsigned symbol, DataModel* dataModel);

	std::size_t writtenBits() const {
		return bitsWritten;
	}

	const char* data() const {
		return buffer.data();
	}
private:
	void writeBit(bool bit);

	std::vector<char> buffer;
	unsigned char bitPosInLastByte;
	std::size_t bitsWritten;

	std::uint32_t intervalLow;
	std::uint32_t intervalHigh;
	std::size_t counter;
};

#endif // !ARITHMENCODER_H

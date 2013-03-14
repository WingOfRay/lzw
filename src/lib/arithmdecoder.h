/**
 * @file arithmdecoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef ARITHMDECODER_H
#define ARITHMDECODER_H

#include "arithmcodec.h"

class ArithmeticDecoder
{
public:
	ArithmeticDecoder(const char* data, size_t numBits);

	void reset(const char* data, size_t numBits);

	unsigned decode(DataModel* dataModel);
};

#endif // !ARITHMDECODER_H

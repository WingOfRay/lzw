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
private:
	typedef IntervalTraits<sizeof(uint32_t)> IntervalTraitsType;

	class BitIterator
	{
	public:
		BitIterator() : p(nullptr), offset(0) { }
		BitIterator(const char* p, uint8_t offset) : p(p), offset(offset) { }
		BitIterator(const BitIterator& other) : p(other.p), offset(other.offset) { }

		bool operator*() const {
			return !!(uint8_t(*p) & (1 << offset));
		}

		BitIterator& operator++() {
			bumpUp();
			return *this;
		}

		BitIterator operator++(int) {
			BitIterator tmp = *this;
			bumpUp();
			return tmp;
		}

		bool operator==(const BitIterator& rhs) const {
			return (p == rhs.p && offset == rhs.offset);
		}

		bool operator<(const BitIterator& rhs) const {
			return p < rhs.p || (p == rhs.p && offset < rhs.offset);
		}

		bool operator!=(const BitIterator& rhs) const {
			return !(*this == rhs);
		}

		bool operator>(const BitIterator& rhs) const {
			return rhs < *this;
		}

		bool operator<=(const BitIterator& rhs) const {
			return !(rhs < *this);
		}

		bool operator>=(const BitIterator& rhs) const {
			return !(*this < rhs);
		}
	private:
		void bumpUp() {
			if (offset++ == CHAR_BIT - 1) {
				offset = 0;
				++p;
			}
		}

		const char* p;
		uint8_t offset;
	};

	void readBit();

	uint64_t intervalLow;			/// lower interval bound
	uint64_t intervalHigh;			/// upper interval bound
	uint64_t value;

	const char* data;
	size_t numBits;

	BitIterator dataEnd;					/// end iterator to data (points behind data)
	BitIterator dataIter;					/// data iterator to current bit
};

#endif // !ARITHMDECODER_H

/**
 * @file arithmencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef ARITHMENCODER_H
#define ARITHMENCODER_H

#include "arithmcodec.h"

/**
 * Encoder for arithmetic coding.
 * 32bit integer implementation.
 * Encoder uses internal dynamic array buffer, where it stores encoded
 * symbols and allows it to handle theoretically infinite input sequences
 * @see http://phoenix.inf.upol.cz/esf/ucebni/komprese.pdf
 */
class ArithmeticEncoder
{
public:
	/// Ctor
	ArithmeticEncoder();

	/**
	 * Resets encoder so you can start encoding new sequence.
	 * Encoder will store new encoding symbols to beginning of data buffer
	 * and writtenBits counter will be reset
	 */
	void reset();

	/**
	 * Finishes encoding, writing last necessary bits.
	 */
	void close();

	/**
	 * Encodes given symbol with data model.
	 * Result will be stored to data buffer accessible with {@link data} method,
	 * also {@link writtenBits} counter will be increased by number of bits
	 * required to store encoded symbol.
	 * @param symbol symbol from dataModel to encode
	 * @param dataModel data model with frequencies
	 */
	void encode(unsigned symbol, DataModel* dataModel);

	/**
	 * Returns number of bits in data buffer
	 * @return number of bits written to data buffer
	 */
	std::size_t bitsWritten() const {
		return bufferBits;
	}

	/**
	 * Returns data buffer.
	 * @return pointer to first byte of data buffer
	 */
	const char* data() const {
		return buffer.data();
	}
private:
	typedef IntervalTraits<sizeof(uint32_t)> IntervalTraitsType;
	typedef IntervalTraitsType::ValueType IntervalValueType;

	void encodeIntervalChange(bool flag);

	/**
	 * Append bit to data buffer.
	 * @param bit value to append
	 */
	void writeBit(bool bit);

	std::vector<char> buffer;			/// data buffer
	unsigned char bitPosInLastByte;		/// bit position in last data buffer byte
	std::size_t bufferBits;				/// number of bits in data buffer

	uint64_t intervalLow;			/// lower interval bound
	uint64_t intervalHigh;			/// upper interval bound
	/// counter that counts how many times in row was interval enlarged from
	/// middle possible range
	std::size_t counter;
};

#endif // !ARITHMENCODER_H

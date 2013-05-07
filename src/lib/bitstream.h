/**
 * @file bitstream.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <iostream>
#include <cstdint>
#include <stdexcept>
#include <cassert>

/**
 * Reader for individual bits from stl streams.
 */
class BitStreamReader
{
public:
	/**
	 * Constructs new reader from stream.
	 * @param stream pointer to stl istream, its caller responsibility 
	 *        that object is not destroyed while this instance is alive
	 */
	explicit BitStreamReader(std::istream* stream) {
		reset(stream);
	}

	/**
	 * Resets reader to work on new stream.
	 * @param stream new istream to read from
	 */
	void reset(std::istream* stream) {
		byte = 0;
		mask = 0;
		this->stream = stream;
	}

	/**
	 * Read single bit from stream.
	 * @return true if read bit set false otherwise
	 * @throws std::runtime_error when unable to read from stream
	 */
	bool readBit() {
		if (mask == 0) {
			if (!stream->read(reinterpret_cast<char*>(&byte), 1))
				throw std::runtime_error("Unable to read from stream!");
			mask = 0x80;
		}

		bool bit = !!(byte & mask);
		mask >>= 1;
		return bit;
	}

	/**
	 * Reads n bits from stream.
	 * @param n number of bits that will be read to result.
	 * @return bits read. Bits are stored here starting from LSB
	 * @throws std::runtime_error when unable to read from stream
	 */
	size_t readBits(size_t n) {
		assert(n <= sizeof(size_t) * CHAR_BIT);

		size_t result = 0;
		for (size_t i = 0; i < n; ++i) {
			if (readBit())
				result |= 1 << i;
		}
		return result;
	}
private:
	std::istream* stream;

	uint8_t byte;		/// buffer for current byte
	uint8_t mask;
};

/**
 * Writer for individual bits to stl streams.
 */
class BitStreamWriter
{
public:
	/**
	 * Constructs new writer for stream.
	 * @param stream pointer to stl ostream, its caller responsibility 
	 *        that object is not destroyed while this instance is alive
	 */
	explicit BitStreamWriter(std::ostream* stream) {
		reset(stream);
	}

	~BitStreamWriter() {
		flush();
	}

	/**
	 * Flushes internal writing buffer.
	 */
	void flush() {
		// when we have something in buffer, write whole byte to stream
		if (mask != 0x80) {
			stream->put(byte);
			byte = 0;
			mask = 0x80;
		}
	}

	/**
	 * Resets reader to work on new stream.
	 * @param stream new ostream to write to
	 */
	void reset(std::ostream* stream) {
		byte = 0;
		mask = 0x80;
		this->stream = stream;
	}

	/**
	 * Writes single bit to stream.
	 * @param bit true if new bit should be set, false otherwise
	 * @throws std::runtime_error when failed to write to stream
	 */
	void writeBit(bool bit) {
		// set bit if we should
		if (bit)
			byte |= mask;

		mask >>= 1;
		if (mask == 0) {
			if (!stream->put(byte))
				throw std::runtime_error("Unable to put byte into stream!");

			byte = 0;
			mask = 0x80;
		}
	}

	/**
	 * Write n bits to stream.
	 * @param bits buffer with bits to write
	 * @param n number of bits that will be put to stream, starting from LSB.
	 * @throws std::runtime_error when failed to write to stream
	 */
	void writeBits(size_t bits, size_t n) {
		for (size_t i = 0; i < n; ++i) {
			writeBit((bits & (1 << i)) != 0);
		}
	}
private:
	std::ostream* stream;

	uint8_t byte;
	uint8_t mask;
};

#endif // !BITSTREAM_H

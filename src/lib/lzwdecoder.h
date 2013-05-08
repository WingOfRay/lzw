/**
 * @file lzwdecoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef LZW_DECODER_H
#define LZW_DECODER_H

#include "lzwcommon.h"
#include "bitstream.h"

#include <memory>
#include <iostream>
#include <map>

#ifdef _MSC_VER
// disable inheriting via dominance warning
#pragma warning(disable : 4250)
#endif // _MSC_VER

/**
 * Interface for reading LZW codes.
 */
class LzwCodeReader : public virtual LzwCodeIOBase
{
public:
	virtual ~LzwCodeReader() { }

	/**
	 * Reads next code.
	 * @retval code read code
	 * @return true when read successful otherwise false
	 */
	virtual bool readNextCode(size_t& code) = 0;
};

/**
 * Simple LZW codes reader.
 * Codes are expected to be in text representation divided by white space.
 */
class SimpleCodeReader : public LzwSimpleCoding, public LzwCodeReader
{
public:
	explicit SimpleCodeReader(std::istream* stream) : LzwSimpleCoding(0, (1L << 30L) - 1L), stream(stream) { }

	virtual bool readNextCode(size_t& code) {
		*stream >> code;
		return !!(*stream);
	};
private:
	std::istream* stream;
};

/**
 * LZW codes reader.
 * Variable codes length starting from 9 bits.
 */
class VariableCodeReader : public LzwVariableCoding, public LzwCodeReader
{
public:
	explicit VariableCodeReader(const BitStreamReader& reader) : reader(reader) { }
	explicit VariableCodeReader(std::istream* stream) : reader(stream) { }

	virtual bool readNextCode(size_t& code);
private:
	BitStreamReader reader;
};

/**
 * Decoder for LZW algorithm.
 * @see http://marknelson.us/1989/10/01/lzw-data-compression/
 */
class LzwDecoder
{
public:
	explicit LzwDecoder(std::shared_ptr<LzwCodeReader> reader) : codeReader(std::move(reader)) {
		init();
	}

	void decode(std::ostream& out);
private:
	void init();

	std::shared_ptr<LzwCodeReader> codeReader;
	std::map<size_t, std::string> dictionary;
};

#endif // !LZW_DECODER_H

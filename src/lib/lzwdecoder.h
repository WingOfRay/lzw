/**
 * @file lzwdecoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef LZW_DECODER_H
#define LZW_DECODER_H

#include <memory>
#include <iostream>
#include <map>

/**
 * Interface for reading LZW codes.
 */
class LzwCodeReader
{
public:
	virtual ~LzwCodeReader() { }

	/**
	 * Obtains next code for storing in LZW dictionary.
	 * @return next code
	 */
	virtual size_t obtainNextCode() = 0;

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
class SimpleCodeReader : public LzwCodeReader
{
public:
	explicit SimpleCodeReader(std::istream* stream) : stream(stream), nextCode(0) { }

	virtual size_t obtainNextCode() {
		return nextCode++;
	};

	virtual bool readNextCode(size_t& code) {
		*stream >> code;
		return !!(*stream);
	};
private:
	std::istream* stream;
	size_t nextCode;
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

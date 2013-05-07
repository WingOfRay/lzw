/**
 * @file lzwencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef LZW_ENCODER_H
#define LZW_ENCODER_H

#include "bitstream.h"

#include <cstdint>
#include <memory>
#include <map>
#include <string>
#include <stdexcept>

class LzwCodeWriter
{

public:
	virtual ~LzwCodeWriter() {}

	/**
	 * Flushes writer.
	 * If implementation uses some buffer it flushes it.
	 */
	virtual void flush() = 0;

	/**
	 * Obtains next code for storing in LZW dictionary.
	 * @return next code
	 */
	virtual size_t obtainNextCode() = 0;

	/**
	 * Writes code to some output stream.
	 * How and where this method writes is implementation defined
	 * @param code LZW code obtained from obtainNexCode
	 */
	virtual void writeCode(size_t code) = 0;
};

class SimpleCodeWriter : public LzwCodeWriter
{
public:
	SimpleCodeWriter(std::ostream* stream) : stream(stream), nextCode(0) { }

	virtual void flush() {
		stream->flush();
	}

	virtual size_t obtainNextCode() {
		return nextCode++;
	}

	virtual void writeCode(size_t code) {
		*stream << code << std::endl;
		if (!stream)
			throw std::runtime_error("SimpleCodeWriter::writeCode unable to write code to stream");
	}

private:
	std::ostream* stream;
	size_t nextCode;
};

class LzwEncoder
{
public:
	explicit LzwEncoder(std::shared_ptr<LzwCodeWriter> codeWriter);

	~LzwEncoder() {
		flush();
	}

	/**
	 * Flushes output bit stream.
	 */
	void flush();

	/**
	 * Resets encoder to write to new output stream
	 * @param bsw writer to new output stream
	 */
	void reset(std::shared_ptr<LzwCodeWriter> codeWriter);

	/**
	 * Encodes byte to output stream
	 * @param byte
	 */
	void encode(int byte);
private:
	void init();

	std::shared_ptr<LzwCodeWriter> codeWriter;

	std::map<std::string, size_t> dictionary;
	std::string encodedStr;
};

#endif // !LZW_ENCODER_H

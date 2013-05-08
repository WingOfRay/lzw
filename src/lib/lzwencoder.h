/**
 * @file lzwencoder.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef LZW_ENCODER_H
#define LZW_ENCODER_H

#include "lzwcommon.h"
#include "bitstream.h"

#include <cstdint>
#include <memory>
#include <map>
#include <string>
#include <stdexcept>

#ifdef _MSC_VER
// disable inheriting via dominance warning
#pragma warning(disable : 4250)
#endif // _MSC_VER

/**
 * Interface for writing LZW codes.
 */
class LzwCodeWriter : public virtual LzwCodeIOBase
{
public:
	virtual ~LzwCodeWriter() {}

	/**
	 * Flushes writer.
	 * If implementation uses some buffer it flushes it.
	 */
	virtual void flush() = 0;

	/**
	 * Writes code to some output stream.
	 * How and where this method writes is implementation defined
	 * @param code LZW code obtained from obtainNexCode
	 */
	virtual void writeCode(size_t code) = 0;
};

/**
 * Simple LZW code writer. 
 * Writes codes as text per line to given stream
 */
class SimpleCodeWriter : public LzwSimpleCoding, public LzwCodeWriter
{
public:
	SimpleCodeWriter(std::ostream* stream) : LzwSimpleCoding(0, (1L << 30L) - 1L), stream(stream) { }

	virtual void flush() {
		stream->flush();
	}

	virtual void writeCode(size_t code) {
		*stream << code << std::endl;
		if (!stream)
			throw std::runtime_error("SimpleCodeWriter::writeCode unable to write code to stream");
	}

private:
	std::ostream* stream;
};

/**
 * LZW codes writer.
 * Variable codes length starting from 9 bits.
 */
class VariableCodeWriter : public LzwVariableCoding, public LzwCodeWriter
{
public:
	explicit VariableCodeWriter(std::ostream* stream) : writer(stream) { }
	explicit VariableCodeWriter(const BitStreamWriter& writer) : writer(writer) { }

	virtual void flush();

	virtual void writeCode(size_t code);
private:
	static size_t codeBitLength(size_t code);

	BitStreamWriter writer;
};

/**
 * Encoder for LZW algorithm.
 * Its parametrized with LzwCodeWriter which specifies
 * how are codes writen.
 * @see http://marknelson.us/1989/10/01/lzw-data-compression/
 */
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

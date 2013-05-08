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
class ICodeWriter : public virtual ILzwIOBase
{
public:
	virtual ~ICodeWriter() {}

	/**
	 * Flushes writer.
	 * If implementation uses some buffer it flushes it.
	 */
	virtual void flush() = 0;

	/**
	 * Write code to some output stream.
	 * How and where this method writes is implementation defined
	 * @param code LZW code obtained from obtainNexCode
	 */
	virtual void writeCode(code_type code) = 0;

	/**
	 * Write code that indicates dictionary reset.
	 */
	virtual void writeDictReset() = 0;
};

/**
 * Simple LZW code writer. 
 * Writes codes as text per line to given stream
 */
class SimpleCodeWriter : public LzwSimpleCoding, public ICodeWriter
{
public:
	SimpleCodeWriter(std::ostream* stream) : LzwSimpleCoding(1, (1L << 30L) - 1L), stream(stream) { }

	virtual void flush() {
		stream->flush();
	}

	virtual void writeCode(code_type code) {
		*stream << code << std::endl;
		if (!stream)
			throw std::runtime_error("SimpleCodeWriter::writeCode unable to write code to stream");
	}

	virtual void writeDictReset() {
		writeCode(0);
	}
private:
	std::ostream* stream;
};

/**
 * LZW codes writer.
 * Variable codes length starting from 9 bits up to 16 bits.
 */
class VariableCodeWriter : public LzwVariableCoding, public ICodeWriter
{
public:
	explicit VariableCodeWriter(std::ostream* stream) : writer(stream) { }
	explicit VariableCodeWriter(const BitStreamWriter& writer) : writer(writer) { }

	virtual void flush();

	virtual void writeCode(code_type code);

	virtual void writeDictReset();
private:
	static code_type codeBitLength(code_type code);

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
	explicit LzwEncoder(std::shared_ptr<ICodeWriter> codeWriter);

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
	void reset(std::shared_ptr<ICodeWriter> codeWriter);

	/**
	 * Encodes byte to output stream
	 * @param byte
	 */
	void encode(int byte);

	/**
	 * Erases dictionary used while encoding.
	 */
	void eraseDictionary();
private:
	void initDictionary();

	std::shared_ptr<ICodeWriter> codeWriter;

	typedef std::map<std::string, ICodeWriter::code_type> dictionary_type;
	dictionary_type dictionary;
	dictionary_type::iterator encodedIt;

	//std::string encodedStr;
};

#endif // !LZW_ENCODER_H

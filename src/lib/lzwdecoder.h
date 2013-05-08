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
#include <unordered_map>

#ifdef _MSC_VER
// disable inheriting via dominance warning
#pragma warning(disable : 4250)
#endif // _MSC_VER

/**
 * Interface for reading LZW codes.
 */
class ICodeReader : public virtual ILzwIOBase
{
public:
	virtual ~ICodeReader() { }

	/**
	 * Reads next code.
	 * @retval code read code
	 * @return true when read successful otherwise false
	 */
	virtual bool readNextCode(code_type& code) = 0;

	virtual code_type dictResetCode() const = 0;
};

/**
 * Simple LZW codes reader.
 * Codes are expected to be in text representation divided by white space.
 */
class SimpleCodeReader : public LzwSimpleCoding, public ICodeReader
{
public:
	explicit SimpleCodeReader(std::istream* stream) : LzwSimpleCoding(1, (1L << 30L) - 1L), stream(stream) { }

	virtual bool readNextCode(code_type& code) {
		*stream >> code;
		return !!(*stream);
	};

	virtual code_type dictResetCode() const {
		return 0;
	}
private:
	std::istream* stream;
};

/**
 * LZW codes reader.
 * Variable codes length starting from 9 bits up to 16 bits.
 */
class VariableCodeReader : public LzwVariableCoding, public ICodeReader
{
public:
	explicit VariableCodeReader(const BitStreamReader& reader) : reader(reader) { }
	explicit VariableCodeReader(std::istream* stream) : reader(stream) { }

	virtual bool readNextCode(code_type& code);

	virtual code_type dictResetCode() const {
		return CODE_DICT_RESET;
	}
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
	explicit LzwDecoder(std::shared_ptr<ICodeReader> reader) : codeReader(std::move(reader)) {
		initDictionary();
	}

	void decode(std::ostream& out);
private:
	void initDictionary();

	void firstRun(size_t& code, std::string& codeStr, char& c);

	std::shared_ptr<ICodeReader> codeReader;
	// LZW codes might be sparse so hash table will be more efficient than tree
	std::unordered_map<ICodeReader::code_type, std::string> dictionary;
};

#endif // !LZW_DECODER_H

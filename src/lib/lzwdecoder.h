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

class LzwCodeReader
{
public:
	virtual ~LzwCodeReader() { }

	virtual size_t obtainNextCode() = 0;

	virtual bool readNextCode(size_t& code) = 0;
};

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

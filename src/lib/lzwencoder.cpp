/**
 * @file lzwencoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "lzwencoder.h"

#include <limits>
#include <ios>
#include <stdexcept>

void VariableCodeWriter::flush() {
	writer.flush();
}

void VariableCodeWriter::writeCode(size_t code) {
	size_t codeLen = codeBitLength(code);
	if (codeLen > curBitLen) {
		if (codeLen == curBitLen + 1) {
			// write mark(all zeros) that we are changing code len
			writer.writeBits(0, curBitLen);
			curBitLen = codeLen;
		} else
			throw std::runtime_error("VariableCodeWriter::writeCode: code bitlen is larger than current bit len by more than 1");
	}

	writer.writeBits(code, curBitLen);
}

size_t VariableCodeWriter::codeBitLength(size_t code) {
	size_t res = 0;
	while (code >>= 1)
		res++;
	return res + 1;
}

LzwEncoder::LzwEncoder(std::shared_ptr<LzwCodeWriter> codeWriter) : codeWriter(std::move(codeWriter)) {
	init();
}

void LzwEncoder::flush() {
	if (!encodedStr.empty())
		codeWriter->writeCode(dictionary.at(encodedStr));

	encodedStr.clear();

	codeWriter->flush();
}

void LzwEncoder::reset(std::shared_ptr<LzwCodeWriter> codeWriter) {
	flush();
	this->codeWriter = std::move(codeWriter);

	init();
}

void LzwEncoder::encode(int byte) {
	if (byte == std::char_traits<char>::eof()) {
		codeWriter->writeCode(dictionary.at(encodedStr));
		return;
	}

	auto concatenated = encodedStr + std::string(1, byte);
	auto it = dictionary.find(concatenated);
	// concatenated in dictionary
	if (it != dictionary.end()) {
		encodedStr = concatenated;
	// concatenated isn't in dictionary
	} else {
		codeWriter->writeCode(dictionary.at(encodedStr));
		if (codeWriter->generator()->haveNext())
			dictionary[concatenated] = codeWriter->generator()->next();
		encodedStr = std::string(1, byte);
	}
}

void LzwEncoder::init() {
	// init dictionary with entry for each byte
	dictionary.clear();
	for (int b = 0; b <= std::numeric_limits<uint8_t>::max(); b++) {
		auto key = std::string(1, b);
		dictionary[key] = codeWriter->generator()->next();
	}
}
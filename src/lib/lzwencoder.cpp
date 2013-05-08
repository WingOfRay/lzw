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

void VariableCodeWriter::writeCode(code_type code) {
	code_type codeLen = codeBitLength(code);
	if (codeLen > curBitLen) {
		if (codeLen == curBitLen + 1) {
			// write mark that we are changing code len
			writer.writeBits(CODE_MARK, curBitLen);
			curBitLen = codeLen;
		} else
			throw std::runtime_error("VariableCodeWriter::writeCode: code bitlen is larger than current bit len by more than 1");
	}

	writer.writeBits(code, curBitLen);
}

void VariableCodeWriter::writeDictReset() {
	writeCode(CODE_DICT_RESET);
}

VariableCodeWriter::code_type VariableCodeWriter::codeBitLength(code_type code) {
	size_t res = 0;
	while (code >>= 1)
		res++;
	return res + 1;
}

void ArithmeticCodeWriter::writeCode(code_type code) {
	encoder->encode(code, &dataModel);
}

void ArithmeticCodeWriter::writeDictReset() {
	writeCode(CODE_DICT_RESET);
	dataModel.reset();
}

LzwEncoder::LzwEncoder(std::shared_ptr<ICodeWriter> codeWriter) : codeWriter(std::move(codeWriter)), encodedIt(dictionary.end()) {
	initDictionary();
}

void LzwEncoder::flush() {
	if (encodedIt != dictionary.end())
		codeWriter->writeCode(encodedIt->second);
	encodedIt = dictionary.end();

	codeWriter->flush();
}

void LzwEncoder::reset(std::shared_ptr<ICodeWriter> codeWriter) {
	flush();
	this->codeWriter = std::move(codeWriter);

	initDictionary();
}

void LzwEncoder::encode(int byte) {
	if (byte == std::char_traits<char>::eof()) {
		codeWriter->writeCode(encodedIt->second);
		return;
	}

	auto concatenated = (encodedIt == dictionary.end() ? "" : encodedIt->first) + std::string(1, byte);
	auto concatenatedIt = dictionary.find(concatenated);
	// concatenated in dictionary
	if (concatenatedIt != dictionary.end()) {
		encodedIt = concatenatedIt;
	// concatenated isn't in dictionary
	} else {
		codeWriter->writeCode(encodedIt->second);
		if (codeWriter->generator()->haveNext())
			dictionary[concatenated] = codeWriter->generator()->next();

		encodedIt = dictionary.find(std::string(1, byte));
	}
}

void LzwEncoder::initDictionary() {
	// init dictionary with entry for each byte
	dictionary.clear();
	for (int b = 0; b <= std::numeric_limits<uint8_t>::max(); b++) {
		auto key = std::string(1, b);
		dictionary[key] = codeWriter->generator()->next();
	}
}

void LzwEncoder::eraseDictionary() {
	if (encodedIt != dictionary.end())
		codeWriter->writeCode(encodedIt->second);
	encodedIt = dictionary.end();

	codeWriter->generator()->reset();
	initDictionary();

	codeWriter->writeDictReset();
}
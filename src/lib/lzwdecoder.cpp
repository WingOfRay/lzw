/**
 * @file lzwdecoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "lzwdecoder.h"

#include <string>
#include <stdexcept>

bool VariableCodeReader::readNextCode(size_t& code) {
	try {
		code = reader.readBits(curBitLen);
	} catch (std::exception&) {
		return false;
	}

	// if we read mark indicating code length change
	if (code == CODE_MARK) {
		curBitLen++;
		return readNextCode(code);
	}

	return true;
}

void LzwDecoder::decode(std::ostream& out) {
	size_t oldCode;
	// read first code and output it
	codeReader->readNextCode(oldCode);
	auto codeStr = dictionary.at(oldCode);
	out << codeStr;

	// first code corresponds to one byte
	if (codeStr.size() != 1)
		throw std::runtime_error("LzwDecoder::decode: first code doesn't correspond to one byte only!!!");
	char c = codeStr[0];
	
	size_t newCode;
	while (codeReader->readNextCode(newCode)) {
		auto it = dictionary.find(newCode);
		// newCode in dictionary
		if (it != dictionary.end()) {
			codeStr = dictionary.at(newCode);
		// newCode NOT in dictionary
		} else {
			codeStr = dictionary.at(oldCode);
			codeStr += c;
		}

		out << codeStr;
		c = codeStr[0];
		dictionary[codeReader->obtainNextCode()] = dictionary.at(oldCode) + c;
		oldCode = newCode;
	}
}

void LzwDecoder::init() {
	// init dictionary with entry for each byte
	dictionary.clear();
	for (int b = 0; b <= std::numeric_limits<uint8_t>::max(); b++) {
		dictionary[codeReader->obtainNextCode()] = std::string(1, b);
	}
}
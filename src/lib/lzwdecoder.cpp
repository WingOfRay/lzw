/**
 * @file lzwdecoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "lzwdecoder.h"

#include <string>
#include <stdexcept>
#include <limits>

bool VariableCodeReader::readNextCode(code_type& code) {
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
	std::string codeStr;
	char c;
	firstRun(oldCode, codeStr, c);
	out << codeStr;

	size_t newCode;
	while (codeReader->readNextCode(newCode)) {
		// when codeReader read dict reset code we have to rebuild dictionary
		if (newCode == codeReader->dictResetCode()) {
			codeReader->generator()->reset();
			initDictionary();
			// we need to handle oldCode cos current oldCode is not valid now
			firstRun(oldCode, codeStr, c);
			out << codeStr;
			continue;
		}

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
		if (codeReader->generator()->haveNext())
			dictionary[codeReader->generator()->next()] = dictionary.at(oldCode) + c;
		oldCode = newCode;
	}
}

void LzwDecoder::firstRun(size_t& code, std::string& codeStr, char& c) {
	// read first code
	codeReader->readNextCode(code);
	codeStr = dictionary.at(code);

	// first code corresponds to one byte
	if (codeStr.size() != 1)
		throw std::runtime_error("LzwDecoder::decode: first code doesn't correspond to one byte only!!!");
	c = codeStr[0];
}

void LzwDecoder::initDictionary() {
	// init dictionary with entry for each byte
	dictionary.clear();
	for (int b = 0; b <= std::numeric_limits<uint8_t>::max(); b++) {
		dictionary[codeReader->generator()->next()] = std::string(1, b);
	}
}
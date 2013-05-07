/**
 * @file lzwencoder.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "lzwencoder.h"

#include <limits>
#include <ios>

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
		dictionary[concatenated] = codeWriter->obtainNextCode();
		codeWriter->writeCode(dictionary.at(encodedStr));
		encodedStr = std::string(1, byte);
	}
}

void LzwEncoder::init() {
	// init dictionary with entry for each byte
	dictionary.clear();
	for (int b = 0; b <= std::numeric_limits<uint8_t>::max(); b++) {
		auto key = std::string(1, b);
		dictionary[key] = codeWriter->obtainNextCode();
	}
}
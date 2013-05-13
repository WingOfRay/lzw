/**
 * @file main.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "utils.h"
#include "lzwdecoder.h"
#include "lzwencoder.h"

#include <iostream>
#include <fstream>

void printUsage() {
	std::cout << "lzw [-a] INPUT OUTPUT\n"
		<< "lzw -d INPUT OUTPUT\n";
}

template <class CodeWriter>
void compressData(std::istream& in, std::ostream& out) {
	auto initSize = out.tellp();

	LzwEncoder encoder(std::make_shared<CodeWriter>(&out));

	std::streamoff bytesWritten = 0;
	size_t bytesRead = 0;
	double oldRatio = 1.0;
	int ratioIncreaseCounter = 0;
	int c;
	while ((c = in.get()) != std::char_traits<char>::eof()) {
		encoder.encode(c);

		/*bytesWritten = out.tellp() - initSize;
		bytesRead++;

		// erase encoder dictionary if compression ration has increased
		// 10times in row. This could happen if encoder dictionary is full
		double compressRatio = bytesWritten / (double)bytesRead;
		if (compressRatio > oldRatio) {
			if (ratioIncreaseCounter++ > 10)
				encoder.eraseDictionary();
		} else
			ratioIncreaseCounter = 0;

		oldRatio = compressRatio;*/
	}
}

void compressVariableLength(std::istream& in, std::ostream& out) {
	const char* header = "LZW\x00";
	out.write(header, 4);

	compressData<VariableCodeWriter>(in, out);
}

void compressWithArithmeticCoding(std::istream& in, std::ostream& out) {
	const char* header = "LZW\x01";
	out.write(header, 4);

	compressData<ArithmeticCodeWriter>(in, out);
}

template <class CodeReader>
void decompressData(std::istream& in, std::ostream& out) {
	LzwDecoder decoder(std::make_shared<CodeReader>(&in));
	decoder.decode(out);
}

void decompress(std::istream& in, std::ostream& out) {
	char header[5] = {0};
	in.read(header, 4);
	if (header[0] != 'L' || header[1] != 'Z' || header[2] != 'W')
		throw std::runtime_error("Bad input header magic string.");

	if (header[3] == '\x00')
		decompressData<VariableCodeReader>(in, out);
	else if (header[3] == '\x01')
		decompressData<ArithmeticCodeReader>(in, out);
	else
		throw std::runtime_error("Invalid header value.");
}

int main(int argc, char* argv[]) {
	std::string input, output;
	OptionsMap options = create_map<OptionsMap::key_type, OptionsMap::mapped_type>
		("d", Option())("a", Option());
	try {
		auto lefovers = parseCmdline(argc, argv, options);
		if (lefovers.size() != 2)
			throw std::runtime_error("Missing leftover args");
		input = lefovers[0];
		output = lefovers[1];
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		printUsage();
		return 2;
	}

	std::ifstream ifile(input.c_str(), std::ios_base::binary);
	if (!ifile) {
		std::cerr << "Error: Unable to open input file: " << input << std::endl;
		return 1;
	}

	std::ofstream ofile(output.c_str(), std::ios_base::binary);
	if (!ofile) {
		std::cerr << "Error: Unable to open output file: " << output << std::endl;
		return 1;
	}

	if (options["d"].isPresent) {
		decompress(ifile, ofile);
	} else {
		if (options["a"].isPresent) {
			compressWithArithmeticCoding(ifile, ofile);
		} else {
			compressVariableLength(ifile, ofile);
		}
	}

	return 0;
}
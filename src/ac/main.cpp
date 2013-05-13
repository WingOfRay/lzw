/**
 * @file main.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "utils.h"
#include "arithmcodec.h"
#include "arithmdecoder.h"
#include "arithmencoder.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include <stdexcept>

const unsigned int NUM_SYMBOLS = std::numeric_limits<unsigned char>::max() + 2;		// 0..255 + 1 for ending symbol

void printUsage() {
	std::cout << "ac [-s] INPUT OUTPUT\n"
		<< "ac -d INPUT OUTPUT\n\n"
		<< "    -s    Use static instead of adaptive data model\n"
		<< "    -d    Decompression instead compression\n";
}

void compressAdaptive(std::istream& in, std::ostream& out) {
	const char* header = "AC\x00";
	out.write(header, 3);

	ArithmeticEncoder encoder(std::make_shared<BitStreamWriter>(&out));
	AdaptiveDataModel dataModel(NUM_SYMBOLS);
	for (;;) {
		int c = in.get();
		if (c == std::char_traits<char>::eof()) {
			encoder.encode(NUM_SYMBOLS - 1, &dataModel);
			break;
		}

		encoder.encode(c, &dataModel);
	}
}

void compressStaticly(std::istream& in, std::ostream& out) {
	const char* header = "AC\x01";
	out.write(header, 3);

	std::vector<unsigned> freqs(NUM_SYMBOLS);
	std::vector<char> data;

	// read in and count frequencies
	char c;
	while (in.get(c)) {
		freqs[c]++;
		data.push_back(c);
	}
	freqs.back() = 1;		// last symbol is ending symbol

	// store freqs to out cos decoder needs to have them
	for (auto freq : freqs) {
		out.write(reinterpret_cast<const char*>(&freq), sizeof(freq));
	}

	ArithmeticEncoder encoder(std::make_shared<BitStreamWriter>(&out));
	StaticDataModel dataModel(freqs);
	for (auto c : data) {
		encoder.encode(c, &dataModel);
	}
	encoder.encode(NUM_SYMBOLS - 1, &dataModel);	// encode last symbol
}

void decompressAdaptive(std::istream& in, std::ostream& out) {
	AdaptiveDataModel dataModel(NUM_SYMBOLS);
	ArithmeticDecoder decoder(std::make_shared<BitStreamReader>(&in));
	for (;;) {
		auto symbol = decoder.decode(&dataModel);

		if (symbol >= NUM_SYMBOLS)
			throw std::runtime_error("Read bad symbol value. Symbols are expected to be 1 byte long.");

		// on ending symbol exit loop
		if (symbol == NUM_SYMBOLS - 1)
			break;

		out << static_cast<char>(symbol);
	}
}

void decompressStaticly(std::istream& in, std::ostream& out) {
	// read frequencies that we need to for static data model
	std::vector<unsigned> freqs(NUM_SYMBOLS);
	for (size_t i = 0; i < freqs.size(); ++i) {
		in.read(reinterpret_cast<char*>(&freqs[i]), sizeof(unsigned));
	}

	StaticDataModel dataModel(freqs);
	ArithmeticDecoder decoder(std::make_shared<BitStreamReader>(&in));
	for (;;) {
		auto symbol = decoder.decode(&dataModel);

		if (symbol >= NUM_SYMBOLS)
			throw std::runtime_error("Read bad symbol value. Symbols are expected to be 1 byte long.");

		// on ending symbol exit loop
		if (symbol == NUM_SYMBOLS - 1)
			break;

		out << static_cast<char>(symbol);
	}
}

void decompress(std::istream& in, std::ostream& out) {
	char header[4] = {0};
	in.read(header, 3);
	if (header[0] != 'A' || header[1] != 'C')
		throw std::runtime_error("Bad input header magic string.");

	if (header[2] == '\x00')
		decompressAdaptive(in, out);
	else if (header[2] == '\x01')
		decompressStaticly(in, out);
	else
		throw std::runtime_error("Invalid header value.");
}

int main(int argc, char* argv[]) {
	std::string input, output;
	OptionsMap options = create_map<OptionsMap::key_type, OptionsMap::mapped_type>
		("d", Option())("s", Option());
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

	try {
		if (options["d"].isPresent) {
			decompress(ifile, ofile);
		} else {
			if (options["s"].isPresent)
				compressStaticly(ifile, ofile);
			else
				compressAdaptive(ifile, ofile);
		}
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}

	return 0;
}
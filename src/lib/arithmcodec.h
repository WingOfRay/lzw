/**
 * @file arithmcodec.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef ARITHMCODEC_H
#define ARITHMCODEC_H

#include <vector>
#include <cassert>
#include <iostream>

/**
 * Static data model.
 * Symbol frequencies are set only once.
 */
class StaticDataModel
{
public:
	StaticDataModel() {}
	explicit StaticDataModel(const std::vector<unsigned>& freqs) {
		computeCumulativeFreqs(freqs);
	}

	/**
	 * Set symbol frequencies.
	 */
	void setFrequencies(const std::vector<unsigned>& freqs) {
		computeCumulativeFreqs(freqs);
	}

	/**
	 * Get cumulative frequency which is g_n = f_1 + f_2 + ... + f_n.
	 * Cumulative frequencies are precomputed so complexity of this class is constant
	 * @param symbol symbol position in frequencies table
	 * @return cumulative frequency for symbol
	 */
	unsigned getCumulativeFreq(unsigned symbol) const {
		assert(symbol < cumulativeFreqs.size());

		return cumulativeFreqs[symbol];
	}

	/**
	 * Gets number of symbols in data model
	 * @return number of symbols in data model
	 */
	std::size_t size() const {
		return cumulativeFreqs.size();
	}
private:
	void computeCumulativeFreqs(const std::vector<unsigned>& freqs);

	std::vector<unsigned> cumulativeFreqs;
};

/**
 * Adaptive data model.
 * Symbol frequencies are built online.
 */
class AdaptiveDataModel
{
public:
	/**
	 * Creates new data model
	 * Sets all symbol frequencies to 1.
	 * @param numSymbols number of symbols in frequency table, created by this call
	 */
	explicit AdaptiveDataModel(std::size_t numSymbols);

	/**
	 * Get cumulative frequency which is g_n = f_1 + f_2 + ... + f_n.
	 * Cumulative frequencies are precomputed so complexity of this class is constant
	 * @param symbol symbol position in frequencies table
	 * @return cumulative frequency for symbol
	 */
	unsigned getCumulativeFreq(unsigned symbol) const;

	/**
	 * Resets data model to initial state, which is all frequencies to 1.
	 */
	void reset();

	/**
	 * Add frequency to symbol.
	 * @param symbol symbol position in frequencies table
	 * @param freq frequency to add to current symbol frequency, operation +=
	 */
	void addSymbolFreq(unsigned symbol, unsigned freq = 1);

	/**
	 * Gets number of symbols in data model
	 * @return number of symbols in data model
	 */
	std::size_t size() const;
};

class ArithmeticCodec
{
public:
	ArithmeticCodec(std::istream* in, std::ostream* out) : in(in), out(out) {}

	std::istream& input() {
		assert(in != nullptr);
		return *in;
	}

	std::ostream& output() {
		assert(out != nullptr);
		return *out;
	}
private:
	std::istream* in;
	std::ostream* out;
};

#endif // !ARITHMCODEC_H

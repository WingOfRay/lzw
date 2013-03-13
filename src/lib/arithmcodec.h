/**
 * @file arithmcodec.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef ARITHMCODEC_H
#define ARITHMCODEC_H

#include <vector>
#include <iostream>
#include <algorithm>
#include <cassert>
#include <cstdint>

class DataModel
{
public:
	~DataModel() {}

	virtual unsigned getCumulativeFreq(unsigned symbol) const = 0;
	virtual std::size_t size() const = 0;
};

/**
 * Static data model.
 * Symbol frequencies are set only once.
 */
class StaticDataModel : public DataModel
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
	virtual unsigned getCumulativeFreq(unsigned symbol) const {
		assert(symbol < cumulativeFreqs.size());

		return cumulativeFreqs[symbol];
	}

	/**
	 * Gets number of symbols in data model
	 * @return number of symbols in data model
	 */
	virtual std::size_t size() const {
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
class AdaptiveDataModel : public DataModel
{
public:
	/**
	 * Creates new data model
	 * Sets all symbol frequencies to 1.
	 * @param numSymbols number of symbols in frequency table, created by this call
	 */
	explicit AdaptiveDataModel(std::size_t numSymbols) : cumulativeFreqs(numSymbols) {
		reset();
	}

	/**
	 * Get cumulative frequency which is g_n = f_1 + f_2 + ... + f_n.
	 * Cumulative frequencies are precomputed so complexity of this class is constant
	 * @param symbol symbol position in frequencies table
	 * @return cumulative frequency for symbol
	 */
	virtual unsigned getCumulativeFreq(unsigned symbol) const {
		assert(symbol < cumulativeFreqs.size());

		return cumulativeFreqs[symbol];
	}

	/**
	 * Gets number of symbols in data model
	 * @return number of symbols in data model
	 */
	virtual std::size_t size() const {
		return cumulativeFreqs.size();
	}

	/**
	 * Resets data model to initial state, which is all frequencies to 1.
	 */
	void reset() {
		// generate cumulative frequencies, that's sequence with every number increased by 1.
		size_t counter = 1;
		std::generate(cumulativeFreqs.begin(), cumulativeFreqs.end(), [&counter] () { return counter++; });
	}

	/**
	 * Add frequency to symbol.
	 * @param symbol symbol position in frequencies table
	 * @param freq frequency to add to current symbol frequency, operation +=
	 */
	void incSymbolFreq(unsigned symbol, unsigned freq = 1) {
		for (auto i = symbol; i < cumulativeFreqs.size(); ++i) {
			cumulativeFreqs[i] += freq;
		}
	}
private:
	std::vector<unsigned> cumulativeFreqs;
};

template <size_t N>
struct Pow2
{
	enum : size_t { value = 2U * Pow2<N - 1U>::value };
};

template <>
struct Pow2<0>
{
	enum : size_t { value = 1U };
};

class ArithmeticCodec
{
protected:
	static const size_t INTERVAL_BITS = 31;
	static const uint32_t MAX_INTERVAL = Pow2<INTERVAL_BITS>::value - 1U;
	static const uint32_t QUATER_INTERVAL = (MAX_INTERVAL + 1U) / 4U;
	static const uint32_t HALF_INTERVAL = 2*QUATER_INTERVAL;
	static const uint32_t THREE_QUATERS_INTERVAL = 3*QUATER_INTERVAL;
};

#endif // !ARITHMCODEC_H

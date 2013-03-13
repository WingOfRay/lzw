/**
 * @file arithmcodec.cpp
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#include "arithmcodec.h"

void StaticDataModel::computeCumulativeFreqs(const std::vector<unsigned>& freqs) {
	cumulativeFreqs.resize(freqs.size());

	// TODO: handle overflow

	for (int i = 0; i < (int)cumulativeFreqs.size(); ++i) {
		for (int j = i; j >= 0; --j) {
			cumulativeFreqs[i] += freqs[j];
		}
	}
}

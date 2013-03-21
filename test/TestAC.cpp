#include <gtest/gtest.h>

#include <memory>
#include <limits>

#include "arithmencoder.h"
#include "arithmdecoder.h"

class ArithmeticCodingTest : public ::testing::Test
{
protected:
	virtual void SetUp() {
	}
};

TEST_F(ArithmeticCodingTest, StaticModel) {
	std::string testStr = "ahojky mam nove kalhoty a nic to neznamena tohle je jen testovaci retezec";
	std::vector<unsigned> freq(128);
	for (auto c : testStr) {
		freq[c]++;
	}
	std::unique_ptr<DataModel> dataModel(new StaticDataModel(freq));

	ArithmeticEncoder encoder;
	for (auto c : testStr) {
		encoder.encode((unsigned)c, dataModel.get());
	}
	encoder.close();

	ArithmeticDecoder decoder(encoder.data(), encoder.bitsWritten());
	for (auto c : testStr) {
		EXPECT_EQ(c, decoder.decode(dataModel.get()));
	}

}
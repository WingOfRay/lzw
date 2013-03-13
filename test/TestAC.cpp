#include <gtest/gtest.h>

#include <memory>
#include <limits>

#include "arithmencoder.h"

class ArithmeticCodingTest : public ::testing::Test
{
protected:
	virtual void SetUp() {
		testStr = "ahojky";
		std::vector<unsigned> freq(128);
		for (auto c : testStr) {
			freq[c]++;
		}

		dataModel.reset(new StaticDataModel(freq));
	}

	std::string testStr;
	std::unique_ptr<DataModel> dataModel;
};

TEST_F(ArithmeticCodingTest, Encoder) {
	ArithmeticEncoder encoder;

	for (auto c : testStr) {
		encoder.encode((unsigned)c, dataModel.get());
	}

	ASSERT_EQ(14, encoder.bitsWritten());

	EXPECT_EQ(0x58, encoder.data()[0]);
	EXPECT_EQ(0x3D, encoder.data()[1]);
}
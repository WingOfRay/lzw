#include <gtest/gtest.h>

#include "lzwdecoder.h"
#include "lzwencoder.h"

#include <sstream>
#include <cstdlib>

class TestLzw : public ::testing::Test
{
protected:
	void SetUp() {
		simpleTestStr.assign("Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua.");
	}

	std::string simpleTestStr;

	static void SetUpTestCase() {
		for (int i = 0; i < 100000; ++i)
			longTestStr += std::string(1, '0' + rand() % 10);
	}

	static std::string longTestStr;

	template <class Reader, class Writer>
	std::string lzwTest(const std::string& str) {
		std::ostringstream oss;
		LzwEncoder encoder(std::make_shared<Writer>(&oss));
		for (auto c : str) {
			encoder.encode(c);
		}
		encoder.flush();

		std::istringstream iss(oss.str());
		LzwDecoder decoder(std::make_shared<Reader>(&iss));
		std::ostringstream result;
		decoder.decode(result);

		return result.str();
	}
};

std::string TestLzw::longTestStr = "";

TEST_F(TestLzw, Simple) {
	auto resultStr = lzwTest<SimpleCodeReader, SimpleCodeWriter>(simpleTestStr);
	EXPECT_EQ(simpleTestStr, resultStr);
}

TEST_F(TestLzw, Variable) {
	auto resultStr = lzwTest<VariableCodeReader, VariableCodeWriter>(simpleTestStr);
	EXPECT_EQ(simpleTestStr, resultStr);
}

TEST_F(TestLzw, VariableDictReset) {
	std::ostringstream oss;
	LzwEncoder encoder(std::make_shared<VariableCodeWriter>(&oss));
	for (size_t i = 0; i < simpleTestStr.size(); i++) {
		if (i == simpleTestStr.size() / 2)
			encoder.eraseDictionary();

		encoder.encode(simpleTestStr[i]);
	}

	encoder.flush();

	std::istringstream iss(oss.str());
	LzwDecoder decoder(std::make_shared<VariableCodeReader>(&iss));
	std::ostringstream result;
	decoder.decode(result);

	auto resultStr = result.str();
	EXPECT_EQ(simpleTestStr, resultStr);
}

TEST_F(TestLzw, ArithmeticSimple) {
	auto resultStr = lzwTest<ArithmeticCodeReader, ArithmeticCodeWriter>(simpleTestStr);
	EXPECT_EQ(simpleTestStr, resultStr);
}

TEST_F(TestLzw, VariableLong) {
	auto resultStr = lzwTest<VariableCodeReader, VariableCodeWriter>(longTestStr);
	EXPECT_EQ(longTestStr, resultStr);
}

TEST_F(TestLzw, ArithmeticLong) {
	auto resultStr = lzwTest<ArithmeticCodeReader, ArithmeticCodeWriter>(longTestStr);
	EXPECT_EQ(longTestStr, resultStr);
}
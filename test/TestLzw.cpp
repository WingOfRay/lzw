#include <gtest/gtest.h>

#include "lzwdecoder.h"
#include "lzwencoder.h"

#include <sstream>

class TestLzw : public ::testing::Test
{
protected:
	void SetUp() {
		simpleTestStr.assign("Lorem ipsum dolor sit amet, consectetur adipisici elit, sed eiusmod tempor incidunt ut labore et dolore magna aliqua.");
	}

	std::string simpleTestStr;
};

TEST_F(TestLzw, Simple) {
	std::ostringstream oss;
	LzwEncoder encoder(std::make_shared<SimpleCodeWriter>(&oss));
	for (auto c : simpleTestStr) {
		encoder.encode(c);
	}
	encoder.flush();

	std::istringstream iss(oss.str());
	LzwDecoder decoder(std::make_shared<SimpleCodeReader>(&iss));
	std::ostringstream result;
	decoder.decode(result);

	auto resultStr = result.str();
	EXPECT_EQ(simpleTestStr, resultStr);
}

TEST_F(TestLzw, Variable) {
	std::ostringstream oss;
	LzwEncoder encoder(std::make_shared<VariableCodeWriter>(&oss));
	for (auto c : simpleTestStr) {
		encoder.encode(c);
	}
	encoder.flush();

	std::istringstream iss(oss.str());
	LzwDecoder decoder(std::make_shared<VariableCodeReader>(&iss));
	std::ostringstream result;
	decoder.decode(result);

	auto resultStr = result.str();
	EXPECT_EQ(simpleTestStr, resultStr);
}
/**
 * @file lzwcommon.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef LZW_COMMON_H
#define LZW_COMMON_H

#include <cstdlib>

class LzwCodeGenerator
{
public:
	typedef size_t code_type;

	virtual ~LzwCodeGenerator() { }

	virtual code_type next() = 0;

	virtual bool haveNext() = 0;
};

class SimpleCodeGenerator : public LzwCodeGenerator
{
public:
	SimpleCodeGenerator(code_type init, code_type max) : nextCode(init), maxCode(max) { }

	virtual code_type next() {
		if (nextCode >= maxCode)
			return nextCode;
		return nextCode++;
	}

	virtual bool haveNext() {
		return nextCode < maxCode;
	}
private:
	code_type nextCode;
	code_type maxCode;
};

class LzwCodeIOBase
{
public:
	virtual ~LzwCodeIOBase() { }

	virtual LzwCodeGenerator* generator() = 0;
};

class LzwSimpleCoding : public virtual LzwCodeIOBase
{
public:
	virtual LzwCodeGenerator* generator() {
		return &codeGen;
	}
protected:
	LzwSimpleCoding(LzwCodeGenerator::code_type init, LzwCodeGenerator::code_type max) : codeGen(init, max) { }

	SimpleCodeGenerator codeGen;
};

/**
 * Some common variables and constants for VariableCodeReader and VariableCodeWriter
 */
class LzwVariableCoding : public LzwSimpleCoding
{
protected:
	static const size_t CODE_MARK = 0;		// code indicating that code length has changed +1

	static const int INIT_NEXT_CODE = 1;	// we have 1 predefined code
	static const int INIT_CODE_LEN = 9;		// cos code 0 is mark we need 9bits for representing byte
	static const int MAX_CODE_LEN = 16;		// maximum code length

	LzwVariableCoding() : LzwSimpleCoding(INIT_NEXT_CODE, (1 << MAX_CODE_LEN) - 1), curBitLen(INIT_CODE_LEN) { }

	size_t curBitLen;
};

#endif // !LZW_COMMON_H

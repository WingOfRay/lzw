/**
 * @file lzwcommon.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef LZW_COMMON_H
#define LZW_COMMON_H

#include <cstdlib>

/**
 * Generator for LZW codes.
 */
class ICodeGenerator
{
public:
	typedef size_t code_type;

	virtual ~ICodeGenerator() { }

	/// Gets next LZW code. if haveNext() == false then result is undefined.
	virtual code_type next() = 0;

	/// True when next() will return valid value, false otherwise
	virtual bool haveNext() = 0;

	/// Resets generator
	virtual void reset() = 0;
};

/**
 * Simple code generator that generates codes in sequence from init to max.
 */
class SimpleCodeGenerator : public ICodeGenerator
{
public:
	SimpleCodeGenerator(code_type init, code_type max) : initial(init), nextCode(init), maxCode(max) { }

	virtual code_type next() {
		if (nextCode >= maxCode)
			return nextCode;
		return nextCode++;
	}

	virtual bool haveNext() {
		return nextCode < maxCode;
	}

	virtual void reset() {
		nextCode = initial;
	}
private:
	code_type initial;
	code_type nextCode;
	code_type maxCode;
};

/// Base class for readers and writers
class ILzwIOBase
{
public:
	typedef ICodeGenerator::code_type code_type;

	virtual ~ILzwIOBase() { }

	/// Get LZW codes generator
	virtual ICodeGenerator* generator() = 0;
};

/**
 * Base class of SimpleCodeReader and VariableCodeReader.
 */
class LzwSimpleCoding : public virtual ILzwIOBase
{
public:
	virtual ICodeGenerator* generator() {
		return &codeGen;
	}
protected:
	LzwSimpleCoding(ICodeGenerator::code_type init, ICodeGenerator::code_type max) : codeGen(init, max) { }

	SimpleCodeGenerator codeGen;
};

/**
 * Base class for VariableCodeReader and VariableCodeWriter.
 */
class LzwVariableCoding : public LzwSimpleCoding
{
protected:
	static const size_t CODE_MARK = 0;		// code indicating that code length has changed +1
	static const size_t CODE_DICT_RESET = 1;// code indicating that dictionary has been reseted

	static const int INIT_NEXT_CODE = 2;	// we have 2 predefined code
	static const int INIT_CODE_LEN = 9;		// cos code 0 is mark we need 9bits for representing byte
	static const int MAX_CODE_LEN = 16;		// maximum code length

	LzwVariableCoding() : LzwSimpleCoding(INIT_NEXT_CODE, (1 << MAX_CODE_LEN) - 1), curBitLen(INIT_CODE_LEN) { }

	size_t curBitLen;
};

#endif // !LZW_COMMON_H

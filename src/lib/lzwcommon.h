/**
 * @file lzwcommon.h
 *
 * @author Jan Dusek <xdusek17@stud.fit.vutbr.cz>
 * @date 2013
 */

#ifndef LZW_COMMON_H
#define LZW_COMMON_H

/**
 * Some common variables and constants for VariableCodeReader and VariableCodeWriter
 */
class LzwVariableCoding
{
protected:
	static const size_t CODE_MARK = 0;		// code indicating that code length has changed +1

	static const int INIT_NEXT_CODE = 1;	// we have 1 predefined code
	static const int INIT_CODE_LEN = 9;		// cos code 0 is mark we need 9bits for representing byte

	LzwVariableCoding() : curBitLen(INIT_CODE_LEN), nextCode(INIT_NEXT_CODE) { }

	size_t curBitLen, nextCode;
};

#endif // !LZW_COMMON_H

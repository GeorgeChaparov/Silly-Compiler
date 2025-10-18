#pragma once

#include <string>
#include "SymbolTable.h"

using string = std::string;

class Lex
{
public:
	static void Init(string document);
private:

	enum CharType
	{
		Unsure = 0, Digit, Letter, Separator, Operator, Punctuation, FlowControl, Compareson, Keyword
	};

	static int GetNextSymbol();

	static CharType GetCharType(char ch);

	static size_t FindIntegerChar(char ch);
	static bool FindOperatorChar(char ch, string& buff);

	static bool IsDigit(unsigned int ascii);
	static bool IsDigit(char ch);

	static bool IsInSet(char ch, auto set, size_t setLength);
	static bool IsInSet(string word, auto set, size_t setLength);

	static void GetNextLine();
	static int GetEndWordPosition();
};
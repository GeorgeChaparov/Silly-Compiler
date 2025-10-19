#pragma once

#include <string>
#include "SymbolTable.h"

using string = std::string;

class Lex
{
public:
	static void Init(string document);
	static int GetNextSymbol();
	static void Build();
	static int GetCurrentLineCount();
private:

	enum CharType
	{
		Unsure = 0, Digit, Letter, Separator, Operator, Punctuation, FlowControl, Compareson, Keyword, FileEnd, NewLine
	};


	static string m_Document;
	static size_t m_CurrentPosition;
	static int m_CurrentLine;

	static CharType GetCharType(char ch);

	static size_t AddUnknownToken(char ch);

	static size_t FindIntegerChar(char ch);

	static bool IsDigit(unsigned int ascii);
	static bool IsDigit(char ch);
	static bool IsSpecialPunctuationSymbol(char ch, bool includeDash = false);

	static bool IsInSet(char ch, auto set, size_t setLength);

	static void GetNextLine();
	static int GetEndWordPosition();
};
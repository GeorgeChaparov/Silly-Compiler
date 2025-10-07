#pragma once

#include <string>
#include "SymbolTable.h"

using string = std::string;

class Lex
{
public:
	static void BuildSymbolTable(string document);
private:

	enum CharType
	{
		Digit, Letter, Separatior, Operator, Punctuation, CharacterLiteral
	};

	static void FindSymbol(bool (*callback)(char, string&), SymbolCode guessedCode);

	static CharType GetCharType(char ch);
	static bool AddWordIfKeyword(string word);
	static bool IsInSet(char ch, auto set, size_t setLength);
	static bool IsInSet(string word, auto set, size_t setLength);

	static bool FindIntegerChar(char ch, string& buff);
	static bool FindIdentifierChar(char ch, string& buff);
	static bool FindOperatorChar(char ch, string& buff);
	static bool FindPunctuationChar(char ch, string& buff);
	static bool FindCharacterLiteralChar(char ch, string& buff);

	static void GetNextLine();
	static int GetEndWordPosition();
};
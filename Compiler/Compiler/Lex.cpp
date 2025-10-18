#include "Lex.h"
#include "Consts.h"
#include "Utils.h"
#include "SymbolTable.h"

string document = "";
size_t currentPosition = 0;

int Lex::GetNextSymbol()
{
	try
	{
		string buff = "";
		size_t tableIndex = -1;

		char ch = document[currentPosition];
		switch (GetCharType(ch))
		{
		case Separator:
			break;

		case Digit:
			tableIndex = FindIntegerChar(ch);
			break;

		case Letter:
			tableIndex = SymbolTable::AddItem(ch, SymbolCode::Identifier);
			break;

		case Punctuation:
			tableIndex = SymbolTable::AddItem(ch, SymbolCode::Punctuation);
			break;

		case Keyword:
			tableIndex = SymbolTable::AddItem(ch + document[++currentPosition], SymbolCode::Keyword);
			break;

		case Unsure:

			break;
		default:
			break;
		}

		return tableIndex;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
	}
}

void Lex::Init(string _document)
{
	document = _document;
	currentPosition = 0;
}

Lex::CharType Lex::GetCharType(char ch)
{
	if (IsInSet(ch, SEPARATORS, SEPARATORS_SET_LENGTH))
	{
		return CharType::Separator;
	}

	unsigned int ascii = static_cast<int>(ch);

	if ((ascii >= UPPER_CASE_LETTERS_MIN_CODE && ascii <= UPPER_CASE_LETTERS_MAX_CODE) ||
		(ascii >= LOWER_CASE_LETTERS_MIN_CODE && ascii <= LOWER_CASE_LETTERS_MAX_CODE))
	{
		return CharType::Letter;
	}
	else if (IsDigit(ascii))
	{
		return CharType::Digit;
	}
	else if (ch == '-')
	{
		return CharType::Keyword;
	}
	else if (IsInSet(ch, PUNCTUATION, PUNCTUATION_SET_LENGTH))
	{
		return CharType::Punctuation;
	}
	else if (ch == ';' || ch == ':')
	{
		return CharType::Unsure;
	}

	//Error: unknown character.
	throw std::runtime_error(string("Unknown character: ") + ch + document.substr(currentPosition + 1, GetEndWordPosition() - currentPosition));
}

bool Lex::IsInSet(char ch, auto set, size_t setLength)
{
	for (size_t i = 0; i < setLength; i++)
	{
		char current = set[i];
		if (ch == current)
		{
			return true;
		}
	}

	return false;
}

bool Lex::IsInSet(string word, auto set, size_t setLength)
{
	for (size_t i = 0; i < setLength; i++)
	{
		string current = std::string() + set[i];
		if (word._Equal(current))
		{
			return true;
		}
	}

	return false;
}

size_t Lex::FindIntegerChar(char ch)
{
	string buff = "";
	size_t index = currentPosition;
	do
	{
		buff.push_back(ch);

		ch = document[++index];
	} while (IsDigit(ch));


	currentPosition = --index;

	return SymbolTable::AddItem(buff, SymbolCode::IntegerLiteral);
}

/*bool Lex::FindOperatorChar(char ch, string& buff)
{
	bool foundAnotherSymbol = false;

	switch (GetCharType(ch))
	{
	case Separator:
		break;
	case Digit:
		buff.push_back(ch);
		break;
	case Operator:
		FindSymbol(FindOperatorChar, SymbolCode::Operator);
		foundAnotherSymbol = true;
		break;
		//Error: Forbidden character.
		throw std::runtime_error(string("Forbidden character: ") + buff + '|' + ch + '|' + document.substr(currentPosition + 1, GetEndWordPosition() - currentPosition));
		break;
	}
	return foundAnotherSymbol;
}*/

void Lex::GetNextLine()
{
	while (document[currentPosition] != '\n')
	{
		++currentPosition;
	}
}

int Lex::GetEndWordPosition()
{
	int pos = currentPosition + 1;

	while (!IsInSet(document[pos], SEPARATORS, SEPARATORS_SET_LENGTH))
	{
		++pos;
	}

	return pos;
}

bool Lex::IsDigit(unsigned int ascii)
{
	return ascii >= DIGITS_MIN_CODE && ascii <= DIGITS_MAX_CODE;
}

bool Lex::IsDigit(char ch)
{
	unsigned int ascii = static_cast<int>(ch);

	return ascii >= DIGITS_MIN_CODE && ascii <= DIGITS_MAX_CODE;
}
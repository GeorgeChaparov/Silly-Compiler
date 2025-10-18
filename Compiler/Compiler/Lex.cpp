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

		case Operator:
			FindSymbol(FindOperatorChar, SymbolCode::Operator);
			SymbolTable::AddItem(buff, SymbolCode::Operator);
			break;

		case Digit:
			FindSymbol(FindIntegerChar, SymbolCode::IntegerLiteral);
			break;

		case Letter:
			tableIndex = SymbolTable::AddItem(ch, SymbolCode::Identifier);
			break;

		case Punctuation:
			FindSymbol(FindPunctuationChar, SymbolCode::Punctuation);
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

void Lex::FindSymbol(bool (*callback)(char, string&), string& buff, SymbolCode guessedCode)
{
	buff.push_back(document[currentPosition]);

	size_t nextPosition = currentPosition + 1;
	char ch = document[nextPosition];

	// In case is the last char or the word is only one char.
	if (ch == '\0' || ( IsInSet(ch, SEPARATORS, SEPARATORS_SET_LENGTH)))
	{
		return;
	}

	++currentPosition;

	// While the current char is not a separator.
	do
	{
		// If the callback return true, that means that we have found another token that is concatenated to that one (int a=... , a+b..., exp;...)
		if (callback(ch, buff))
		{
			break;
		}

		// Checking if we are at the end of the stream. 
		ch = document[++currentPosition];
		if (ch == '\0')
		{
			break;
		}
	} while (!IsInSet(ch, SEPARATORS, SEPARATORS_SET_LENGTH));

	// Checking if the found token is a keyword and if so we are adding it to the symbol table.
	if (AddWordIfKeyword(buff))
	{
		return;
	}
	// If its not a keyword, we are checking if its a comment.
	else if (buff == "//")
	{
		return;
	}

	// If we are at this point, we have guessed correctly and we can add the token.
	SymbolTable::AddItem(buff, guessedCode);
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
	else if (ascii >= DIGITS_MIN_CODE && ascii <= DIGITS_MAX_CODE)
	{
		return CharType::Digit;
	}
	else if (IsInSet(std::string() + ch, OPERATORS, OPERATORS_SET_LENGTH))
	{
		return CharType::Operator;
	}
	else if (IsInSet(ch, PUNCTUATION, PUNCTUATION_SET_LENGTH))
	{
		return CharType::Punctuation;
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

bool Lex::AddWordIfKeyword(string word)
{
	if (IsInSet(word, KEYWORDS, KEYWORDS_SET_LENGTH))
	{
		SymbolTable::AddItem(word, SymbolCode::Keyword);
		return true;
	}

	return false;
}

bool Lex::FindIntegerChar(char ch, string& buff)
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
	case Punctuation:
		FindSymbol(FindPunctuationChar, SymbolCode::Punctuation);
		foundAnotherSymbol = true;
		break;
	default:
		//Error: Forbidden character.
		throw std::runtime_error(string("Forbidden character: ") + buff + '|' + ch + '|' + document.substr(currentPosition + 1, GetEndWordPosition() - currentPosition));
		break;
	}
	return foundAnotherSymbol;
}
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
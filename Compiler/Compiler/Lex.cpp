#include "Lex.h"
#include "Consts.h"
#include "Utils.h"
#include "SymbolTable.h"

string document = "";
size_t currentPosition = 0;

void Lex::BuildSymbolTable(string _document)
{
	try
	{
		document = _document;
		currentPosition = 0;

		// Goes through every character and checks for different types of tokens.
		for (currentPosition = 0; currentPosition < document.length(); currentPosition++)
		{
			char ch = document[currentPosition];

			switch (GetCharType(ch))
			{
			case Separatior:
				break;

			case Operator:
				FindSymbol(FindOperatorChar, SymbolCode::Operator);
				break;

			case Digit:
				FindSymbol(FindIntegerChar, SymbolCode::IntegerLiteral);
				break;

			case Letter:
				FindSymbol(FindIdentifierChar, SymbolCode::Identifier);
				break;

			case Punctuation:
				FindSymbol(FindPunctuationChar, SymbolCode::Punctuation);
				break;

			case CharacterLiteral:
				FindSymbol(FindCharacterLiteralChar, SymbolCode::CharacterLiteral);
				break;
			default:
				break;
			}
		}
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
	}
}

void Lex::FindSymbol(bool (*callback)(char, string&), SymbolCode guessedCode)
{
	string buff = "";
	buff.push_back(document[currentPosition]);

	size_t nextPosition = currentPosition + 1;
	char ch = document[nextPosition];

	// In case is the last char or the word is only one char.
	if (ch == '\0' || (guessedCode != SymbolCode::CharacterLiteral && IsInSet(ch, SEPARATIORS, SEPARATIORS_SET_LENGTH)))
	{
		SymbolTable::AddItem(buff, guessedCode);

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
	} while (!IsInSet(ch, SEPARATIORS, SEPARATIORS_SET_LENGTH));

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
	if (IsInSet(ch, SEPARATIORS, SEPARATIORS_SET_LENGTH))
	{
		return CharType::Separatior;
	}

	unsigned int ascii = static_cast<int>(ch);

	if ((ascii >= UPPER_CASE_LETTERS_MIN_CODE && ascii <= UPPER_CASE_LETTERS_MAX_CODE) ||
		(ascii >= LOWER_CASE_LETTERS_MIN_CODE && ascii <= LOWER_CASE_LETTERS_MAX_CODE) ||
		IsInSet(ch, SPECIAL_IDENTIFIER_CHARS, SPECIAL_IDENTIFIER_SET_LENGTH))
	{
		return CharType::Letter;
	}
	else if (ascii >= DIGITS_MIN_CODE && ascii <= DIGITS_MAX_CODE)
	{
		return CharType::Digit;
	}
	else if (IsInSet(std::string() + ch, OPERATORS, OPERATORS_SET_LENGTH) || IsInSet(ch, START_OPERATOR_SYMBOL, START_OPERATOR_SYMBOL_SET_LENGTH))
	{
		return CharType::Operator;
	}
	else if (IsInSet(ch, PUNCTUATION, PUNCTUATION_SET_LENGTH))
	{
		return CharType::Punctuation;
	}
	// that the ascii code for '.
	else if (ascii == 39)
	{
		return CharType::CharacterLiteral;
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
	case Separatior:
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

bool Lex::FindIdentifierChar(char ch, string& buff)
{
	bool foundAnotherSymbol = false;

	switch (GetCharType(ch))
	{
	case Separatior:
		break;
	case Digit:
		buff.push_back(ch);
		break;
	case Letter:
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

bool Lex::FindOperatorChar(char ch, string& buff)
{
	bool foundAnotherSymbol = false;

	switch (GetCharType(ch))
	{
	case Separatior:
		break;
	case Operator:
		if (buff.length() > 1)
		{
			if (ch == buff[buff.length() - 1] && ch != buff[buff.length() - 2])
			{
				buff.push_back(ch);
			}
			else
			{
				//Error: Unknown token, can be ===, +++ or any other threple operator.
				throw std::runtime_error("Unknown token: " + buff + ch + document.substr(currentPosition + 1, GetEndWordPosition() - currentPosition));
			}
		}
		else
		{
			if (ch == buff[buff.length() - 1])
			{
				buff.push_back(ch);

				if (buff == "//")
				{
					GetNextLine();
					foundAnotherSymbol = true;
				}

				else if (!IsInSet(buff, OPERATORS, OPERATORS_SET_LENGTH))
				{
					//Error: Unknown token
					throw std::runtime_error("Unknown token: " + buff + ch + document.substr(currentPosition + 1, GetEndWordPosition() - currentPosition));
				}
			}
			else if (IsInSet(buff, OPERATORS, OPERATORS_SET_LENGTH) || IsInSet(buff, START_OPERATOR_SYMBOL, START_OPERATOR_SYMBOL_SET_LENGTH))
			{
				buff.push_back(ch);
			}
			else
			{
				//Error: Unknown token
				throw std::runtime_error("Unknown token: " + buff + ch + document.substr(currentPosition + 1, GetEndWordPosition() - currentPosition));
			}
		}

		break;
	case Digit:
		FindSymbol(FindIntegerChar, SymbolCode::IntegerLiteral);
		foundAnotherSymbol = true;
		break;
	case Letter:
		FindSymbol(FindIdentifierChar, SymbolCode::Identifier);
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

bool Lex::FindPunctuationChar(char ch, string& buff)
{
	bool foundAnotherSymbol = false;

	switch (GetCharType(ch))
	{
	case Separatior:
		break;

	case Punctuation:
		FindSymbol(FindPunctuationChar, SymbolCode::Punctuation);
		foundAnotherSymbol = true;
		break;
	case Digit:
		FindSymbol(FindIntegerChar, SymbolCode::IntegerLiteral);
		foundAnotherSymbol = true;
		break;
	case Letter:
		FindSymbol(FindIdentifierChar, SymbolCode::Identifier);
		foundAnotherSymbol = true;
		break;
	default:
		//Error: Forbidden character.
		throw std::runtime_error(string("Forbidden character: ") + buff + '|' + ch + '|' + document.substr(currentPosition, GetEndWordPosition() - currentPosition));
		break;
	}
	return foundAnotherSymbol;
}

bool Lex::FindCharacterLiteralChar(char ch, string& buff)
{
	bool foundAnotherSymbol = false;

	int buffLength = buff.length();
	if (buffLength >= 3)
	{
		switch (GetCharType(ch))
		{
		case Separatior:
			break;
		case Digit:
			FindSymbol(FindIntegerChar, SymbolCode::IntegerLiteral);
			break;
		case Letter:
			FindSymbol(FindIdentifierChar, SymbolCode::Identifier);
			break;
		case Punctuation:
			FindSymbol(FindPunctuationChar, SymbolCode::Punctuation);
			break;
		case Operator:
			FindSymbol(FindOperatorChar, SymbolCode::Operator);
			break;

		default:
			break;
		}
		foundAnotherSymbol = true;
	}
	else if (buffLength == 1)
	{
		if (static_cast<int>(ch) == 39)
		{
			throw std::runtime_error("empty char literal is not valid");
		}
		else
		{
			buff.push_back(ch);
		}

	}
	else if (static_cast<int>(ch) == 39)
	{
		buff = buff[1];
		foundAnotherSymbol = true;
	}
	else
	{
		throw std::runtime_error("There is an ' for the beginning of a char literal, but there isn't one to close it or there is more then one character between them.");
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

	while (!IsInSet(document[pos], SEPARATIORS, SEPARATIORS_SET_LENGTH))
	{
		++pos;
	}

	return pos;
}
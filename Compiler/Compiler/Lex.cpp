#include "Lex.h"
#include "Consts.h"
#include "Utils.h"
#include "SymbolTable.h"
#include "GrammarTrie.h"

string Lex::m_Document = "";
size_t Lex::m_CurrentPosition = 0;

void Lex::Init(string _document)
{
	m_Document = _document;
	m_CurrentPosition = 0;

	// Building the Trie.
	GrammarTrie::Insert("::::", SymbolCode::Operator);	//	+
	GrammarTrie::Insert(";;;;", SymbolCode::Operator);	//	-
	GrammarTrie::Insert("::;;", SymbolCode::Operator);	//	*
	GrammarTrie::Insert(";;::", SymbolCode::Operator);	//	/
	GrammarTrie::Insert("::", SymbolCode::Operator);	//	=

	GrammarTrie::Insert(":;", SymbolCode::Operator);	//	>
	GrammarTrie::Insert(":::", SymbolCode::Operator);	//	==

	GrammarTrie::Insert(":-", SymbolCode::Keyword);		//	cout
	GrammarTrie::Insert("-:", SymbolCode::Keyword);		//	cin

	GrammarTrie::Insert(";-", SymbolCode::Keyword); 	//	continue
	GrammarTrie::Insert("-;", SymbolCode::Keyword);		//	break
	GrammarTrie::Insert(":", SymbolCode::Keyword); 		//	if / while
	GrammarTrie::Insert(";;", SymbolCode::Keyword);		//	else-if / else
}

void Lex::Build()
{
	try
	{
		while (GetNextSymbol() != -1);
	}
	catch (const std::exception&)
	{
		return;
	}
	
}

int Lex::GetNextSymbol()
{
	try
	{
		string buff = "";
		size_t tableIndex = -2;

		char ch = m_Document[m_CurrentPosition];
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
			tableIndex = SymbolTable::AddItem(string() + ch + m_Document[++m_CurrentPosition], SymbolCode::Keyword);
			break;

		case FileEnd:
			tableIndex = -1;
			break;

		case Unsure:
			tableIndex = AddUnknownToken(ch);
			break;
		default:
			break;
		}

		++m_CurrentPosition;

		return tableIndex;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
		throw error;
	}
}

Lex::CharType Lex::GetCharType(char ch)
{
	if (IsInSet(ch, SEPARATORS, SEPARATORS_SET_LENGTH))
	{
		return CharType::Separator;
	}

	if (ch == '\0')
	{
		return CharType::FileEnd;
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
	else if (IsSpecialPunctuationSymbol(ch))
	{
		return CharType::Unsure;
	}

	// Error: unknown character.
	throw std::runtime_error(string("Unknown character: ") + ch);
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

size_t Lex::FindIntegerChar(char ch)
{
	string buff = "";
	size_t index = m_CurrentPosition;
	do
	{
		buff.push_back(ch);

		ch = m_Document[++index];
	} while (IsDigit(ch));


	m_CurrentPosition = --index;

	return SymbolTable::AddItem(buff, SymbolCode::IntegerLiteral);
}

size_t Lex::AddUnknownToken(char ch)
{
	string buff = "";

	buff.push_back(ch);

	
	size_t index = m_CurrentPosition;

	size_t i = 1;

	bool includeDashInSearch = true;
	while (i < 4)
	{
		char nextCh = m_Document[++index];
		if (!IsSpecialPunctuationSymbol(nextCh, includeDashInSearch))
		{
			break;
		}

		buff.push_back(nextCh);
		++i;
	}

	if (i != 4)
	{
		--index;
	}

	m_CurrentPosition = index;

	try
	{
		TrieNode* node = GrammarTrie::Find(buff);

		return SymbolTable::AddItem(node->tokenValue, node->tokenType);
	}
	catch (const std::exception& error)
	{
		throw std::runtime_error(string("Unknown token: ") + buff + m_Document.substr(m_CurrentPosition + 1, m_CurrentPosition + 1 + i));
	}
}

void Lex::GetNextLine()
{
	while (m_Document[m_CurrentPosition] != '\n')
	{
		++m_CurrentPosition;
	}
}

int Lex::GetEndWordPosition()
{
	int pos = m_CurrentPosition + 1;
	char ch = m_Document[pos];

	while (!IsInSet(ch, SEPARATORS, SEPARATORS_SET_LENGTH) && ch != '\0')
	{
		ch = m_Document[++pos];
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

bool Lex::IsSpecialPunctuationSymbol(char ch, bool includeDash)
{
	bool isSpecial = false;

	if (ch == ';' || ch == ':')
	{
		isSpecial = true;
	}

	if (includeDash && ch == '-')
	{
		isSpecial = true;
	}

	return isSpecial;
}
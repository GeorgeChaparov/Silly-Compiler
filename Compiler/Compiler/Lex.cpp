#include "Lex.h"
#include "Consts.h"
#include "Utils.h"
#include "SymbolTable.h"
#include "GrammarTrie.h"
#include "Logger.h"
#include "Globals.h"

size_t Lex::m_CurrentPosition = g_CurrentPosition;
unsigned int Lex::m_LineSymbolCount = g_LineSymbolCount;
unsigned int Lex::m_CurrentLine = g_CurrentLine;

void Lex::Init(string _document)
{
	g_Document = _document;
	g_DocumentLength = _document.length();

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
	GrammarTrie::Insert(";;", SymbolCode::Keyword);		//	else-if 
	GrammarTrie::Insert(";;-", SymbolCode::Keyword);	//  else
	GrammarTrie::Insert(";-;", SymbolCode::Keyword);	//  &&
	GrammarTrie::Insert(";-:", SymbolCode::Keyword);	//  ||
}

void Lex::Build()
{
	while (GetNextSymbol(false) != END_OF_FILE_CODE);
}

int Lex::CheckNextSymbol(bool _includeNewLine)
{
	 return GetNextSymbol(_includeNewLine, false);
}

int Lex::GetNextSymbol(bool _includeNewLine, bool _advance)
{
	try
	{
		size_t tableIndex = UNEXPECTED_LEXICAL_ERROR_CODE;

		m_CurrentPosition = g_CurrentPosition;
		m_LineSymbolCount = g_LineSymbolCount;
		m_CurrentLine = g_CurrentLine;

		while (tableIndex == UNEXPECTED_LEXICAL_ERROR_CODE)
		{
			char ch = g_Document[m_CurrentPosition];
			switch (GetCharType(ch))
			{
			case Separator:
				break;

			case NewLine:
				++m_CurrentLine;
				m_LineSymbolCount = 1;

				if (_includeNewLine)
				{
					tableIndex = SymbolTable::AddItem(ch, SymbolCode::Keyword);
				}
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
				if (m_CurrentPosition != g_DocumentLength)
				{
					tableIndex = SymbolTable::AddItem(string() + ch + g_Document[++m_CurrentPosition], SymbolCode::Keyword);
				}
				else
				{
					tableIndex = SymbolTable::AddItem(string() + ch, SymbolCode::Keyword);
				}
				
				break;

			case FileEnd:
				tableIndex = END_OF_FILE_CODE;
				break;

			case Unsure:
				tableIndex = AddUnknownToken(ch);
				break;
			default:
				break;
			}

			++m_LineSymbolCount;

			if (m_CurrentPosition != g_DocumentLength)
			{
				++m_CurrentPosition;
			}
		}

		if (_advance)
		{
			g_CurrentPosition = m_CurrentPosition;
			g_LineSymbolCount = m_LineSymbolCount;
			g_CurrentLine = m_CurrentLine;
		}

		return tableIndex;
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Lexical, false);
	}

	return UNEXPECTED_LEXICAL_ERROR_CODE;
}

Lex::CharType Lex::GetCharType(char _ch)
{
	if (IsInSet(_ch, SEPARATORS, SEPARATORS_SET_LENGTH))
	{
		return CharType::Separator;
	}

	if (_ch == '\n')
	{
		return CharType::NewLine;
	}

	if (_ch == '\0')
	{
		return CharType::FileEnd;
	}

	unsigned int ascii = static_cast<int>(_ch);

	if ((ascii >= UPPER_CASE_LETTERS_MIN_ASCII_CODE && ascii <= UPPER_CASE_LETTERS_MAX_ASCII_CODE) ||
		(ascii >= LOWER_CASE_LETTERS_MIN_ASCII_CODE && ascii <= LOWER_CASE_LETTERS_MAX_ASCII_CODE))
	{
		return CharType::Letter;
	}
	else if (IsDigit(ascii))
	{
		return CharType::Digit;
	}
	else if (_ch == '-')
	{
		return CharType::Keyword;
	}
	else if (IsInSet(_ch, PUNCTUATION, PUNCTUATION_SET_LENGTH))
	{
		return CharType::Punctuation;
	}
	else if (IsSpecialPunctuationSymbol(_ch))
	{
		return CharType::Unsure;
	}

	// Error: unknown character.
	throw std::runtime_error(string("Unknown character: ") + _ch);
}

bool Lex::IsInSet(char _ch, auto _set, size_t _setLength)
{
	for (size_t i = 0; i < _setLength; i++)
	{
		char current = _set[i];
		if (_ch == current)
		{
			return true;
		}
	}

	return false;
}

size_t Lex::FindIntegerChar(char _ch)
{
	string buff = "";
	size_t index = m_CurrentPosition;
	do
	{
		buff.push_back(_ch);

		_ch = g_Document[++index];
	} while (IsDigit(_ch));


	m_CurrentPosition = --index;

	return SymbolTable::AddItem(buff, SymbolCode::IntegerLiteral);
}

size_t Lex::AddUnknownToken(char _ch)
{
	string buff = "";
	buff.push_back(_ch);
	
	size_t index = m_CurrentPosition;
	size_t i = 1;

	while (i < 4)
	{
		char nextCh = g_Document[++index];
		if (!IsSpecialPunctuationSymbol(nextCh, true))
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
		std::shared_ptr<TrieNode> node = GrammarTrie::Find(buff);

		return SymbolTable::AddItem(node->tokenValue, node->tokenType);
	}
	catch (const std::exception& error)
	{
		throw std::runtime_error(string("Unknown token: ") + buff + g_Document.substr(m_CurrentPosition + 1, m_CurrentPosition + 1 + i));
	}
}

void Lex::GetNextLine()
{
	while (g_Document[m_CurrentPosition] != '\n')
	{
		++m_CurrentPosition;
	}
}

int Lex::GetEndWordPosition()
{
	int nextPos = m_CurrentPosition + 1;
	char ch = g_Document[nextPos];

	while (!IsInSet(ch, SEPARATORS, SEPARATORS_SET_LENGTH) && ch != '\0')
	{
		ch = g_Document[++nextPos];
	}

	return nextPos;
}

bool Lex::IsDigit(unsigned int _ascii)
{
	return _ascii >= DIGITS_MIN_ASCII_CODE && _ascii <= DIGITS_MAX_ASCII_CODE;
}

bool Lex::IsDigit(char _ch)
{
	unsigned int ascii = static_cast<int>(_ch);

	return ascii >= DIGITS_MIN_ASCII_CODE && ascii <= DIGITS_MAX_ASCII_CODE;
}

bool Lex::IsSpecialPunctuationSymbol(char _ch, bool _includeDash)
{
	bool isSpecial = false;

	if (_ch == ';' || _ch == ':')
	{
		isSpecial = true;
	}

	if (_includeDash && _ch == '-')
	{
		isSpecial = true;
	}

	return isSpecial;
}
#include <exception>
#include <iostream>
#include "Parser.h"

string* Parser::m_Token = Parser::GetNextToken();

string* Parser::GetNextToken()
{
	try
	{
		size_t index = Lex::GetNextSymbol();
		
		if (index < 0)
		{
			throw std::runtime_error("Unexpected error");
		}

		SymbolTableItem* item = SymbolTable::GetElementAt(index);

		return &item->symbol;
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
		return;
	}
}

void Parser::Pars()
{
	Start();
}

void Parser::Start()
{
	Block();
}

void Parser::Block()
{
	Stms();
}

void Parser::Stms()
{
	Stm();

	while(*m_Token == "\n")
	{
		m_Token = GetNextToken();
		Stms();
	}
}

void Parser::Stm()
{

}

#include <exception>
#include <iostream>
#include "Parser.h"

SymbolTableItem* Parser::m_Token = new SymbolTableItem{ "", SymbolCode::Unknown};
int Parser::m_ExprLevel = 0;

SymbolTableItem* Parser::GetNextToken()
{
	try
	{
		size_t index = Lex::GetNextSymbol();
		
		if (index < 0)
		{
			throw std::runtime_error("Unexpected error");
		}

		return SymbolTable::GetElementAt(index);
	}
	catch (const std::exception& error)
	{
		std::cout << error.what() << std::endl;
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

	while(m_Token->symbol == "\n")
	{
		m_Token = GetNextToken();
		Stms();
	}
}

void Parser::Stm()
{
	// Ident
	if (m_Token->code == SymbolCode::Identifier)
	{
		//Ident ::
		m_Token = GetNextToken();
		if (m_Token->symbol != "::")
		{
			/*		ERROR		*/
		}

		m_Token = GetNextToken();

		// Ident :: -:
		if (m_Token->symbol == "-:")
		{
			m_Token = GetNextToken();
		}
		// if	-> Ident :: -: expr
		// else -> Ident :: expr

		Expr();
	}
	// -: | :-
	else if (m_Token->symbol == "-:" || m_Token->symbol == ":-")
	{
		m_Token = GetNextToken();

		//  -: expr | :- expr
		Expr();
	}
	else if (m_Token->symbol == "-;" || m_Token->symbol == ";-")
	{
		// This is valid.
	}
	else if (m_Token->symbol == ":")
	{
		m_Token = GetNextToken();

		if (m_Token->symbol != "(")
		{
			/*		ERROR		*/
		}

		m_Token = GetNextToken();
		Expr();

		m_Token = GetNextToken();
		if (m_Token->symbol != ")")
		{
			/*		ERROR		*/
		}

		m_Token = GetNextToken();
		if (m_Token->symbol != "(")
		{
			/*		ERROR		*/
		}

		++m_ExprLevel;
		m_Token = GetNextToken();
		Block();

		m_Token = GetNextToken();
		if (m_Token->symbol != ")")
		{
			/*		ERROR		*/
		}

		--m_ExprLevel;

		// Its an "While", we do not need to do anything more. Else it will be an "If"
		if (m_ExprLevel % 2 != 0)
		{
			return;
		}

		m_Token = GetNextToken();
		while (m_Token->symbol == ";;")
		{
			m_Token = GetNextToken();
			ElseIf();	
		}

		m_Token = GetNextToken();
		if (m_Token->symbol == ";;-")
		{
			m_Token = GetNextToken();
			Block();
		}
	}
	else
	{
		/*		ERROR		*/		
	}
}

void Parser::ElseIf()
{
	if (m_Token->symbol != "(")
	{
		/*		ERROR		*/
	}

	m_Token = GetNextToken();
	Expr();

	m_Token = GetNextToken();
	if (m_Token->symbol != ")")
	{
		/*		ERROR		*/
	}

	m_Token = GetNextToken();
	if (m_Token->symbol != "(")
	{
		/*		ERROR		*/
	}

	++m_ExprLevel;
	m_Token = GetNextToken();
	Block();

	m_Token = GetNextToken();
	if (m_Token->symbol != ")")
	{
		/*		ERROR		*/
	}

	--m_ExprLevel;
}

void Parser::Expr()
{
	Equality();
}

void Parser::Equality()
{
	Comparison();

	m_Token = GetNextToken();
	if (m_Token->symbol == ":::")
	{
		m_Token = GetNextToken();
		Comparison();
	}
}

void Parser::Comparison()
{
	Term();

	m_Token = GetNextToken();
	if (m_Token->symbol == ":;")
	{
		m_Token = GetNextToken();
		Comparison();
	}
}

void Parser::Term()
{
	Factor();
}

void Parser::Factor()
{
	Primary();
}

void Parser::Primary()
{

}
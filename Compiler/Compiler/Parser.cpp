#include <exception>
#include <iostream>
#include "Parser.h"
#include "Logger.h"
#include "Globals.h"
#include "Consts.h"

int Parser::m_ExprLevel = 0;

SymbolTableItem* Parser::GetNextToken(bool includeNewLine = false)
{
	try
	{
		size_t index = Lex::GetNextSymbol(includeNewLine);
		
		if (index == SYMBOL_TABLE_SIZE + 2)
		{
			throw std::logic_error("Unexpected error");
		}
		else if (index == SYMBOL_TABLE_SIZE + 1)
		{
			return new SymbolTableItem{ "12422233456568678 34564365234 87923323 5825685673 345436 MyRandomString ThatWill Never BeTHE SAme as Any StrInG ThAT theendUser Will Ever Write!@!@#$!@)#@%*@()9234", SymbolCode::Unknown };
		}

		return SymbolTable::GetElementAt(index);
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Semantic, false);
	}
}

SymbolTableItem* Parser::CheckNextToken()
{
	try
	{
		size_t index = Lex::CheckNextSymbol();

		if (index == SYMBOL_TABLE_SIZE + 2)
		{
			throw std::logic_error("Unexpected error");
		}
		else if (index == SYMBOL_TABLE_SIZE + 1)
		{
			return new SymbolTableItem{ "MyRandomString ThatWill Never BeTHE SAme as Any StrInG ThAT theendUser Will Ever Write!@!@#$!@)#@%*@()9234", SymbolCode::Unknown };
		}

		return SymbolTable::GetElementAt(index);
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Semantic, false);
	}
}

void Parser::Pars()
{
	try
	{
		Start();

		std::cout << "Parser completed" << std::endl;
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Semantic);
	}
}

void Parser::Start()
{
	g_CurrentToken = GetNextToken();
	Block();
}

void Parser::Block()
{
	Stms();
}

void Parser::Stms()
{
	Stm();


	SymbolTableItem* nextToken = CheckNextToken();
	if(nextToken->symbol != ")" && g_CurrentToken->symbol == ")")
	{
		if (nextToken->symbol != ";;" && nextToken->symbol != ";;-")
		{
			g_CurrentToken = GetNextToken(true);
			while (g_CurrentToken->symbol == "\n")
			{
				g_CurrentToken = GetNextToken();
				Stms();
			}
		}
	}
	else if (nextToken->symbol != ")" && g_CurrentToken->symbol != ")")
	{
		g_CurrentToken = GetNextToken(true);
		while (g_CurrentToken->symbol == "\n")
		{
			g_CurrentToken = GetNextToken();
			Stms();
		}
	}
	
}

void Parser::Stm()
{
	// Ident
	if (g_CurrentToken->code == SymbolCode::Identifier)
	{
		//Ident ::
		g_CurrentToken = GetNextToken();
		if (g_CurrentToken->symbol != "::")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected ::");
		}

		g_CurrentToken = GetNextToken();

		// Ident :: expr
		if (g_CurrentToken->symbol != "-:")
		{
			Expr();
		}
		// else -> Ident :: -:
	}
	// -:
	else if (g_CurrentToken->symbol == "-:")
	{
		// This is valid.
	}
	// :-
	else if (g_CurrentToken->symbol == ":-")
	{
		g_CurrentToken = GetNextToken();

		// :- expr
		Expr();
	}
	else if (g_CurrentToken->symbol == "-;" || g_CurrentToken->symbol == ";-")
	{
		// This is valid.
	}
	else if (g_CurrentToken->symbol == ":")
	{
		g_CurrentToken = GetNextToken();

		if (g_CurrentToken->symbol != "(")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected (");
		}

		g_CurrentToken = GetNextToken();
		Expr();

		if (g_CurrentToken->symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}

		g_CurrentToken = GetNextToken();
		if (g_CurrentToken->symbol != "(")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected (");
		}

		++m_ExprLevel;

		if (CheckNextToken()->symbol != ")")
		{
			g_CurrentToken = GetNextToken();
			Block();
		}

		g_CurrentToken = GetNextToken();
		if (g_CurrentToken->symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}

		--m_ExprLevel;

		// Its an "While", we do not need to do anything more. Else it will be an "If"
		if (m_ExprLevel % 2 == 0 )
		{
			if (CheckNextToken()->symbol == ";;")
			{
				/*		ERROR		*/
				throw std::runtime_error("This is an 'While' not an 'if'!");
			}
			else 
			{
				return;
			}
		}

		g_CurrentToken = GetNextToken();
		while (g_CurrentToken->symbol == ";;")
		{
			g_CurrentToken = GetNextToken();
			ElseIf();	
			g_CurrentToken = GetNextToken();
		}

		if (g_CurrentToken->symbol == ";;-")
		{
			g_CurrentToken = GetNextToken();
			if (g_CurrentToken->symbol != "(")
			{
				/*		ERROR		*/
				throw std::runtime_error("Expected (");
			}

			++m_ExprLevel;
			g_CurrentToken = GetNextToken();
			Block();

			g_CurrentToken = GetNextToken();
			if (g_CurrentToken->symbol != ")")
			{
				/*		ERROR		*/
				throw std::runtime_error("Expected )");
			}

			--m_ExprLevel;
		}
	}
	else
	{
		/*		ERROR		*/		
		throw std::runtime_error("Expected Identifier or -: or :- or -; or ;- or :");
	}
}

void Parser::ElseIf()
{
	if (g_CurrentToken->symbol != "(")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected (");
	}

	g_CurrentToken = GetNextToken();
	Expr();

	if (g_CurrentToken->symbol != ")")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected )");
	}

	g_CurrentToken = GetNextToken();
	if (g_CurrentToken->symbol != "(")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected (");
	}

	++m_ExprLevel;
	g_CurrentToken = GetNextToken();
	Block();

	if (g_CurrentToken->symbol != ")")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected )");
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

	if (g_CurrentToken->symbol == ":::")
	{
		g_CurrentToken = GetNextToken();
		Comparison();
	}
}

void Parser::Comparison()
{
	Term();

	if (g_CurrentToken->symbol == ":;")
	{
		g_CurrentToken = GetNextToken();
		Comparison();
	}
}

void Parser::Term()
{
	Factor();

	while (g_CurrentToken->symbol == ";;;;" || g_CurrentToken->symbol == "::::")
	{
		g_CurrentToken = GetNextToken();
		Term();
	}
}

void Parser::Factor()
{
	Primary();

	g_CurrentToken = GetNextToken();
	while (g_CurrentToken->symbol == "::;;" || g_CurrentToken->symbol == ";;::")
	{
		g_CurrentToken = GetNextToken();
		Factor();
	}
}

void Parser::Primary()
{
	if (g_CurrentToken->code == SymbolCode::Identifier)
	{

	}
	else if (g_CurrentToken->code == SymbolCode::IntegerLiteral)
	{

	}
	else if (g_CurrentToken->symbol == "(")
	{
		g_CurrentToken = GetNextToken();
		Expr();
		
		if (g_CurrentToken->symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}
	}
	else
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected Identifier or Constant Literal or (");
	}
}

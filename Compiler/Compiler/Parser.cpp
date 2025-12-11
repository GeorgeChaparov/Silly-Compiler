#include <exception>
#include <iostream>
#include "Parser.h"
#include "Logger.h"
#include "Globals.h"
#include "Consts.h"

int Parser::m_ExprLevel = 0;

// This string is used to create an SymbolTableItem object with purpose to not match any string that the user may write. This object is created when we have hit the end of the file.
static const string M_RANDOM_STRING = "12422233456568678 34564365234 87923323 5825685673 345436 MyRandomString ThatWill Never BeTHE SAme as Any StrInG ThAT theendUser Will Ever Write!@!@#$!@)#@%*@()9234";

size_t Parser::m_TableIndex = -1;
int Parser::m_CustomVarIndex = 1;
std::vector<Quad*>* Parser::m_QuadTable = new std::vector<Quad *>();

size_t m_DefaultArgumentPosValue = SYMBOL_TABLE_SIZE + 100;

size_t Parser::GenCustomVar()
{
	return SymbolTable::AddItem("!" + std::to_string(m_CustomVarIndex++), SymbolCode::Identifier);
}

void Parser::Log()
{
	std::cout << std::endl;
	std::cout << "Quad table:" << std::endl;
	int index = 0;

	for (auto& quad : *m_QuadTable)
	{
		if (quad->operation == "JMP")
		{
			std::cout << index << " -> " << quad->operation << " at " << quad->arg1 << std::endl;
		}
		else if (quad->operation == "BRZ")
		{
			std::cout << index << " -> " << quad->operation << " at " << quad->arg1 << " if " << SymbolTable::GetElementAt(quad->arg2)->symbol << std::endl;
		}
		else if (quad->arg2)
		{
			std::cout << index << " -> " << SymbolTable::GetElementAt(quad->result)->symbol << " = " << SymbolTable::GetElementAt(quad->arg1)->symbol << " " << quad->operation << " " << SymbolTable::GetElementAt(quad->arg2)->symbol << std::endl;
		}
		else if (quad->operation == "END")
		{
			std::cout << index << " -> " << quad->operation << std::endl;
		}
		else if (quad->operation == "IN" || quad->operation == "OUT")
		{
			std::cout << index << " -> " << quad->operation << " " << SymbolTable::GetElementAt(quad->result)->symbol << std::endl;
		}
		else
		{
			std::cout << index << " -> " << SymbolTable::GetElementAt(quad->result)->symbol << " " << quad->operation << " " << SymbolTable::GetElementAt(quad->arg1)->symbol << std::endl;
		}

		++index;
	}
}

void Parser::ReturnWithOneToken()
{
	if (g_CurrentToken->symbol == "\n")
	{
		g_LineSymbolCount -= 1;
	}

	if (g_CurrentPosition != 0)
	{
		g_CurrentPosition -= 1;
	}
}

SymbolTableItem* Parser::GetNextToken(bool _includeNewLine = false)
{
	try
	{
		m_TableIndex = Lex::GetNextSymbol(_includeNewLine);
		
		if (m_TableIndex == UNEXPECTED_LEXICAL_ERROR_CODE)
		{
			throw std::logic_error("Unexpected error");
		}
		else if (m_TableIndex == END_OF_FILE_CODE)
		{
			return new SymbolTableItem{ M_RANDOM_STRING, SymbolCode::EndFile };
		}

		return SymbolTable::GetElementAt(m_TableIndex);
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Semantic, false);
	}
}

SymbolTableItem* Parser::CheckNextToken(bool _includeNewLine)
{
	try
	{
		size_t index = Lex::CheckNextSymbol(_includeNewLine);

		if (index == UNEXPECTED_LEXICAL_ERROR_CODE)
		{
			throw std::logic_error("Unexpected error");
		}
		else if (index == END_OF_FILE_CODE)
		{
			return new SymbolTableItem{ M_RANDOM_STRING, SymbolCode::EndFile };
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

		Quad* endQuad = new Quad();
		endQuad->operation = "END";
		m_QuadTable->push_back(endQuad);

		std::cout << "Parser completed" << std::endl;

		Log();
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

string Parser::Stms()
{
	Stm();

	SymbolTableItem* nextToken = CheckNextToken();

	if (nextToken->code == SymbolCode::EndFile || g_CurrentToken->code == SymbolCode::EndFile)
	{
		return "";
	}

	if (g_CurrentToken->symbol == ")" )
	{
		return "";
	}

	Stms();
	
	return "";
}

string Parser::Stm()
{
	// Ident
	if (g_CurrentToken->code == SymbolCode::Identifier)
	{
		size_t arg1, result = m_TableIndex;

		
		//Ident ::
		g_CurrentToken = GetNextToken();
		if (g_CurrentToken->symbol != "::")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected ::");
		}

		Quad* quad = new Quad();

		g_CurrentToken = GetNextToken();
		// Ident :: expr
		if (g_CurrentToken->symbol != "-:")
		{
			arg1 = Expr();

			quad->operation = "::";
			quad->arg1 = arg1;
		}
		// Ident :: -:
		else
		{
			quad->operation = "IN";
		}

		quad->result = result;
		m_QuadTable->push_back(quad);
	}
	// -:
	else if (g_CurrentToken->symbol == "-:")
	{
		// This is valid.
	}
	// :-
	else if (g_CurrentToken->symbol == ":-")
	{
		Quad* quad = new Quad();

		g_CurrentToken = GetNextToken();

		// :- expr
		quad->result = Expr();
		quad->operation = "OUT";
	}
	else if (g_CurrentToken->symbol == "-;" || g_CurrentToken->symbol == ";-")
	{
		// This is valid.
		Quad* quad = new Quad();
	}
	else if (g_CurrentToken->symbol == ":")
	{
		size_t condStartPos;
		std::vector<Quad*>* elses = new std::vector<Quad*>();
		Quad* jmpDownQuad = new Quad();

		g_CurrentToken = GetNextToken();

		if (g_CurrentToken->symbol != "(")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected (");
		}

		condStartPos = m_QuadTable->size();

		g_CurrentToken = GetNextToken();

		jmpDownQuad->operation = "BRZ";
		jmpDownQuad->arg2 = Expr();
		m_QuadTable->push_back(jmpDownQuad);

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
		if (g_CurrentToken->symbol != ")")
		{
			Block();
		}
		
		if (g_CurrentToken->symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}

		--m_ExprLevel;

		// Its an "While", we do not need to do anything more. Else it will be an "If"
		if (m_ExprLevel % 2 == 0 )
		{
			g_CurrentToken = GetNextToken();

			if (g_CurrentToken->symbol == ";;")
			{
				/*		ERROR		*/
				throw std::runtime_error("This is an 'While' not an 'if'!");
			}
			
			Quad* jmpUpQuad = new Quad();
			jmpUpQuad->operation = "JMP";
			jmpUpQuad->arg1 = condStartPos;
			m_QuadTable->push_back(jmpUpQuad);

			jmpDownQuad->arg1 = m_QuadTable->size();

			return "";
		}

		Quad* jmpAfterIfQuad = new Quad();
		jmpAfterIfQuad->operation = "JMP";
		

		SymbolTableItem* nextToken = CheckNextToken();

		g_CurrentToken = GetNextToken();
		if (g_CurrentToken->symbol == ";;")
		{
			m_QuadTable->push_back(jmpAfterIfQuad);
			
			jmpDownQuad->arg1 = m_QuadTable->size();
			do 
			{
				g_CurrentToken = GetNextToken();
				Quad* elseQuad = ElseIf();

				Quad* jmpAfterElseQuad = new Quad();
				jmpAfterElseQuad->operation = "JMP";
				elses->push_back(jmpAfterElseQuad);
				m_QuadTable->push_back(jmpAfterElseQuad);

				elseQuad->arg1 = m_QuadTable->size();
				g_CurrentToken = GetNextToken();
			}
			while (g_CurrentToken->symbol == ";;");

			if (g_CurrentToken->symbol == ";;-")
			{
				g_CurrentToken = GetNextToken();
				Else();
				g_CurrentToken = GetNextToken();
			}

			for (auto& _else : *elses)
			{
				_else->arg1 = m_QuadTable->size();
			}

			jmpAfterIfQuad->arg1 = m_QuadTable->size();

		}
		else if (g_CurrentToken->symbol == ";;-")
		{
			m_QuadTable->push_back(jmpAfterIfQuad);

			g_CurrentToken = GetNextToken();
			jmpDownQuad->arg1 = m_QuadTable->size();

			Else();

			g_CurrentToken = GetNextToken();
			
			jmpAfterIfQuad->arg1 = m_QuadTable->size();
		}
		else
		{
			jmpDownQuad->arg1 = m_QuadTable->size();
		}
	}
	else
	{
		/*		ERROR		*/		
		throw std::runtime_error("Expected Identifier or -: or :- or -; or ;- or :");
	}

	return "";
}

Quad* Parser::ElseIf()
{
	size_t endPos, condResult;
	Quad* jmpDownQuad = new Quad();

	if (g_CurrentToken->symbol != "(")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected (");
	}

	g_CurrentToken = GetNextToken();
	condResult = Expr();

	jmpDownQuad->operation = "BRZ";
	jmpDownQuad->arg2 = condResult;
	m_QuadTable->push_back(jmpDownQuad);

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

	return jmpDownQuad;
}

string Parser::Else()
{
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

	return "";
}

size_t Parser::Expr()
{
	return Equality();
}

size_t Parser::Equality()
{
	size_t arg1, arg2, result;

	arg1 = Comparison();

	if (g_CurrentToken->symbol == ":::")
	{
		string operation = g_CurrentToken->symbol;
		
		g_CurrentToken = GetNextToken();
		arg2 = Comparison();

		result = GenCustomVar();
		Quad* quad = new Quad{ operation, arg1, arg2, result };
		m_QuadTable->push_back(quad);
		arg1 = result;
	}

	return arg1;
}

size_t Parser::Comparison()
{
	size_t arg1, arg2, result;

	arg1 = Term();

	if (g_CurrentToken->symbol == ":;")
	{
		string operation = g_CurrentToken->symbol;

		g_CurrentToken = GetNextToken();
		arg2 = Comparison();
		result = GenCustomVar();
		m_QuadTable->push_back(new Quad{ operation, arg1, arg2, result });
		arg1 = result;
	}

	return arg1;
}

size_t Parser::Term()
{
	size_t arg1, arg2, result;

	arg1 = Factor();

	while (g_CurrentToken->symbol == ";;;;" || g_CurrentToken->symbol == "::::")
	{
		string operation = g_CurrentToken->symbol;

		g_CurrentToken = GetNextToken();
		arg2 = Term();
		result = GenCustomVar();
		m_QuadTable->push_back(new Quad{ operation, arg1, arg2, result });
		arg1 = result;
	}

	return arg1;
}

size_t Parser::Factor()
{
	size_t arg1, arg2, result;

	arg1 = Primary();

	g_CurrentToken = GetNextToken();
	while (g_CurrentToken->symbol == "::;;" || g_CurrentToken->symbol == ";;::")
	{
		string operation = g_CurrentToken->symbol;
		g_CurrentToken = GetNextToken();
		arg2 = Factor();
		result = GenCustomVar();
		m_QuadTable->push_back(new Quad{ operation, arg1, arg2, result });
		arg1 = result;
	}

	return arg1;
}

size_t Parser::Primary()
{
	size_t arg;

	if (g_CurrentToken->code == SymbolCode::Identifier)
	{
		arg = m_TableIndex;
	}
	else if (g_CurrentToken->code == SymbolCode::IntegerLiteral)
	{
		arg = m_TableIndex;
	}
	else if (g_CurrentToken->symbol == "(")
	{
		g_CurrentToken = GetNextToken();
		arg = Expr();
		
		if (g_CurrentToken->symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}
		
		if (CheckNextToken(true)->symbol == "\n")
		{
			ReturnWithOneToken();
		}
	}
	else
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected Identifier or Constant Literal or (");
	}

	return arg;
}

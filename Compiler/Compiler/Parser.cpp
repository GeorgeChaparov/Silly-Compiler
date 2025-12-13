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
			if (quad->result)
			{
				std::cout << index << " -> " << quad->operation << " " << SymbolTable::GetElementAt(quad->result)->symbol << std::endl;
			}
			else
			{
				std::cout << index << " -> " << quad->operation << std::endl;
			}
			
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

std::vector<Quad*>* Parser::Block()
{
	return Stms();
}

std::vector<Quad*>* Parser::Stms()
{
	std::vector<Quad*>* returnQuads = new std::vector<Quad*>(); // Used to store all of the "continue" and "break" quads so that they can be passed to the "while" they are meant for.

	// We return if the current character is ")" because that means we are exiting a scope (the body of ether "if" or "while").
	if (g_CurrentToken->symbol == ")")
	{
		return returnQuads;
	}

	Quad* quad = Stm();
	if (quad->operation != "")
	{
		// Adding the quad only if there is something meaningful in it.
		returnQuads->push_back(quad);
	}
	

	SymbolTableItem* nextToken = CheckNextToken();

	if (nextToken->code == SymbolCode::EndFile || g_CurrentToken->code == SymbolCode::EndFile)
	{
		return returnQuads;
	}

	std::vector<Quad*>* Quads = Stms();
	returnQuads->insert(returnQuads->end(), Quads->begin(), Quads->end());

	return returnQuads;
}

Quad* Parser::Stm()
{
	Quad* returnQuad = new Quad();

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

			g_CurrentToken = GetNextToken();
		}

		quad->result = result;
		m_QuadTable->push_back(quad);
	}
	// -:
	else if (g_CurrentToken->symbol == "-:")
	{
		// This is valid. Its used to "eat" an character.

		Quad* quad = new Quad();
		quad->operation = "IN";
		m_QuadTable->push_back(quad);

		g_CurrentToken = GetNextToken();
	}
	// :-
	else if (g_CurrentToken->symbol == ":-")
	{
		Quad* quad = new Quad();

		g_CurrentToken = GetNextToken();
		
		SymbolTableItem* nextToken = CheckNextToken(true);
		// :- expr
		if (nextToken->symbol != "\n")
		{
			quad->result = Expr();
		}
		// Else is just writing empty new line 
		
		quad->operation = "OUT";
		m_QuadTable->push_back(quad);
	}
	else if (g_CurrentToken->symbol == ":")
	{
		size_t condStartPos; // Marks the position in the quad table of the first command that generates the condition.
		
		Quad* jmpDownQuad = new Quad(); // Used to jump to the first "if-else", or to the "else" if there is no "if-else", or to skip the "if", or to skip the "while".

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

		// This check is so that we skip empty "ifs" or "whiles";
		if (g_CurrentToken->symbol != ")")
		{
			// As the syntax of the "if" and the "while" are the same, we decide which is which based on the scope level .
			// (level 0 - its an "while", level 1 - its an "if", level 2 - its an "while"....).
			if (m_ExprLevel % 2 != 0)
			{
				// Its "while".
				// There might be "continue" or "break". That's why we are storing the return value from "Block()".
				std::vector<Quad*>* quads = Block();

				for (auto& quad : *quads)
				{
					// We are adding to "arg1", because if the command is "break", the default value of "arg1" is 1, and the default value of "continue" is 0. 
					// That's so "break" can skip the "JMP" command that will return it to the condition of the "while".
					quad->arg1 += m_QuadTable->size();
				}
			}
			else
			{
				// Its "if".
				Block();
			}
		}
		// We don't have anything in the body, just returning.
		else
		{
			jmpDownQuad->arg1 = m_QuadTable->size();
			g_CurrentToken = GetNextToken();
			--m_ExprLevel;

			return returnQuad;
		}
		
		if (g_CurrentToken->symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}

		--m_ExprLevel;

		// If it's true, it's an "while"
		if (m_ExprLevel % 2 == 0)
		{
			g_CurrentToken = GetNextToken();

			if (g_CurrentToken->symbol == ";;")
			{
				/*		ERROR		*/
				throw std::runtime_error("This is an 'While' not an 'if'!");
			}
			
			// Creating the jump command that will point to the beginning of the condition of the "while"
			Quad* jmpUpQuad = new Quad();
			jmpUpQuad->operation = "JMP";
			jmpUpQuad->arg1 = condStartPos;
			m_QuadTable->push_back(jmpUpQuad);

			jmpDownQuad->arg1 = m_QuadTable->size();

			// Returning because everything else below is for "ifs".
			return returnQuad;
		}

		// This quad is used to "JMP" after the whole "if"
		// If we have if:
		// 1 -  (a == 2) 
		// 2 -  {
		// 3 -  
		// 4 -  } 
		// 5 -  else if (b == 2)
		// 6 -  {
		// 7 -  
		// 8 -  }
		// 9 -  else
		// 10 - {
		// 11 - 
		// 12 - }
		// 13 - 
		//
		// This quad will point to 13 and it will be at the end of each block
		Quad* jmpAfterIfQuad = new Quad();
		jmpAfterIfQuad->operation = "JMP";
		

		SymbolTableItem* nextToken = CheckNextToken();

		g_CurrentToken = GetNextToken();
		if (g_CurrentToken->symbol == ";;")
		{
			m_QuadTable->push_back(jmpAfterIfQuad);
			
			// This is used to store all the "if-else-es"'s "JMP" quads that point to the end of the "if"
			std::vector<Quad*>* elses = new std::vector<Quad*>();

			jmpDownQuad->arg1 = m_QuadTable->size();
			do 
			{
				g_CurrentToken = GetNextToken();
				Quad* elseQuad = ElseIf();

				// This is used for the same as "jmpAfterIfQuad" but it's for every "if-else" except the first one.
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
	else if (g_CurrentToken->symbol == ")")
	{
		return returnQuad;
	}
	else
	{
		if (m_ExprLevel % 2 != 0)
		{
			if (g_CurrentToken->symbol == ";-")
			{
				// This is valid.
				returnQuad->operation = "JMP";
				m_QuadTable->push_back(returnQuad);

				g_CurrentToken = GetNextToken();
				return returnQuad;
			}
			else if (g_CurrentToken->symbol == "-;")
			{
				// This is valid.
				returnQuad->operation = "JMP";
				returnQuad->arg1 = 1;
				m_QuadTable->push_back(returnQuad);

				g_CurrentToken = GetNextToken();
				return returnQuad;
			}
		}

		/*		ERROR		*/		
		throw std::runtime_error("Expected Identifier or -: or :- or : . It can also be -; or ;- if its in an loop.");
	}

	return returnQuad;
}

Quad* Parser::ElseIf()
{
	Quad* jmpDownQuad = new Quad();

	if (g_CurrentToken->symbol != "(")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected (");
	}

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

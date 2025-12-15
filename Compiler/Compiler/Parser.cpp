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

size_t m_DefaultArgumentPosValue = SYMBOL_TABLE_SIZE + 100;

size_t Parser::GenCustomVar()
{
	return SymbolTable::AddItem("!" + std::to_string(m_CustomVarIndex++), SymbolCode::Identifier);
}

void Parser::LogQuad(int _index)
{
	Quad quad = *g_QuadTable.at(_index);

	if (quad.operation == "JMP")
	{
		std::cout << _index << " -> " << quad.operation << " at " << quad.arg1 << std::endl;
	}
	else if (quad.operation == "BRZ")
	{
		std::cout << _index << " -> " << quad.operation << " at " << quad.arg1 << " if " << SymbolTable::GetElementAt(quad.arg2)->symbol << std::endl;
	}
	else if (quad.arg2)
	{
		std::cout << _index << " -> " << SymbolTable::GetElementAt(quad.result)->symbol << " = " << SymbolTable::GetElementAt(quad.arg1)->symbol << " " << quad.operation << " " << SymbolTable::GetElementAt(quad.arg2)->symbol << std::endl;
	}
	else if (quad.operation == "END")
	{
		std::cout << _index << " -> " << quad.operation << std::endl;
	}
	else if (quad.operation == "IN" || quad.operation == "OUT")
	{
		if (quad.result)
		{
			std::cout << _index << " -> " << quad.operation << " " << SymbolTable::GetElementAt(quad.result)->symbol << std::endl;
		}
		else
		{
			std::cout << _index << " -> " << quad.operation << std::endl;
		}

	}
	else if (quad.operation == "SUP" || quad.operation == "SDW")
	{
		std::cout << _index << " -> " << quad.operation << std::endl;
	}
	else
	{
		std::cout << _index << " -> " << SymbolTable::GetElementAt(quad.result)->symbol << " " << quad.operation << " " << SymbolTable::GetElementAt(quad.arg1)->symbol << std::endl;
	}
}

void Parser::Log()
{
	std::cout << std::endl;
	std::cout << "Quad table:" << std::endl;

	for (int i = 0; i < g_QuadTable.size(); i++)
	{
		LogQuad(i);
	}
}

void Parser::ReturnWithOneToken()
{
	if (g_CurrentToken.symbol == "\n")
	{
		g_LineSymbolCount -= 1;
	}

	if (g_CurrentPosition != 0)
	{
		g_CurrentPosition -= 1;
	}
}

SymbolTableItem Parser::GetNextToken(bool _includeNewLine = false)
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
			return SymbolTableItem{ M_RANDOM_STRING, SymbolCode::EndFile };
		}

		return *SymbolTable::GetElementAt(m_TableIndex);
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Semantic, false);
	}
}

SymbolTableItem Parser::CheckNextToken(bool _includeNewLine)
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
			return SymbolTableItem{ M_RANDOM_STRING, SymbolCode::EndFile };
		}

		return *SymbolTable::GetElementAt(index);
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

		std::shared_ptr<Quad> endQuad = std::make_shared<Quad>();
		endQuad->operation = "END";
		g_QuadTable.push_back(endQuad);
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Semantic);
	}

	try
	{
		SemanticValidation();

		std::cout << "Parser completed" << std::endl;

		Log();
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what());
	}
}

void Parser::Start()
{
	g_CurrentToken = GetNextToken();
	Block();
}

std::vector<std::shared_ptr<Quad>> Parser::Block()
{
	return Stms();
}

std::vector<std::shared_ptr<Quad>> Parser::Stms()
{
	std::vector<std::shared_ptr<Quad>> returnQuads = std::vector<std::shared_ptr<Quad>>(); // Used to store all of the "continue" and "break" quads so that they can be passed to the "while" they are meant for.

	// We return if the current character is ")" because that means we are exiting a scope (the body of ether "if" or "while").
	if (g_CurrentToken.symbol == ")")
	{
		return returnQuads;
	}

	std::shared_ptr<Quad> quad = Stm();
	if (quad->operation != "")
	{
		// Adding the quad only if there is something meaningful in it.
		returnQuads.push_back(quad);
	}
	

	SymbolTableItem nextToken = CheckNextToken();

	if (nextToken.code == SymbolCode::EndFile || g_CurrentToken.code == SymbolCode::EndFile)
	{
		return returnQuads;
	}

	std::vector<std::shared_ptr<Quad>> Quads = Stms();
	returnQuads.insert(returnQuads.end(), Quads.begin(), Quads.end());

	return returnQuads;
}

std::shared_ptr<Quad> Parser::Stm()
{
	std::shared_ptr<Quad> returnQuad = std::make_shared<Quad>();

	// Ident
	if (g_CurrentToken.code == SymbolCode::Identifier)
	{
		size_t arg1, result = m_TableIndex;

		
		//Ident ::
		g_CurrentToken = GetNextToken();
		if (g_CurrentToken.symbol != "::")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected ::");
		}

		std::shared_ptr<Quad> quad = std::make_shared<Quad>();

		g_CurrentToken = GetNextToken();
		// Ident :: expr
		if (g_CurrentToken.symbol != "-:")
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
		g_QuadTable.push_back(quad);
	}
	// -:
	else if (g_CurrentToken.symbol == "-:")
	{
		// This is valid. Its used to "eat" an character.

		std::shared_ptr<Quad> quad = std::make_shared<Quad>();
		quad->operation = "IN";
		g_QuadTable.push_back(quad);

		g_CurrentToken = GetNextToken();
	}
	// :-
	else if (g_CurrentToken.symbol == ":-")
	{
		std::shared_ptr<Quad> quad = std::make_shared<Quad>();
		
		SymbolTableItem nextToken = CheckNextToken(true);
		// :- expr
		if (nextToken.symbol != "\n")
		{
			g_CurrentToken = GetNextToken();
			quad->result = Expr();
		}
		// Else is just writing empty new line 
		
		quad->operation = "OUT";
		g_QuadTable.push_back(quad);
	}
	else if (g_CurrentToken.symbol == ":")
	{
		size_t condStartPos; // Marks the position in the quad table of the first command that generates the condition.
		
		std::shared_ptr<Quad> jmpDownQuad = std::make_shared<Quad>(); // Used to jump to the first "if-else", or to the "else" if there is no "if-else", or to skip the "if", or to skip the "while".

		g_CurrentToken = GetNextToken();
		if (g_CurrentToken.symbol != "(")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected (");
		}

		condStartPos = g_QuadTable.size();

		g_CurrentToken = GetNextToken();

		jmpDownQuad->operation = "BRZ";
		jmpDownQuad->arg2 = Expr();
		g_QuadTable.push_back(jmpDownQuad);

		if (g_CurrentToken.symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}

		g_CurrentToken = GetNextToken();
		if (g_CurrentToken.symbol != "(")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected (");
		}

		++m_ExprLevel;
		// This quad is used to keep track of the scope in validation and debugging.
		std::shared_ptr<Quad> scopeUp = std::make_shared<Quad>();
		scopeUp->operation = "SUP";
		g_QuadTable.push_back(scopeUp);

		g_CurrentToken = GetNextToken();

		// This check is so that we skip empty "ifs" or "whiles";
		if (g_CurrentToken.symbol != ")")
		{
			// As the syntax of the "if" and the "while" are the same, we decide which is which based on the scope level .
			// (level 0 - its an "while", level 1 - its an "if", level 2 - its an "while"....).
			if (m_ExprLevel % 2 != 0)
			{
				// Its "while".
				// There might be "continue" or "break". That's why we are storing the return value from "Block()".
				std::vector<std::shared_ptr<Quad>> quads = Block();


				--m_ExprLevel;
				// This quad is used to keep track of the scope in validation and debugging.
				std::shared_ptr<Quad> scopeDown = std::make_shared<Quad>();
				scopeDown->operation = "SDW";
				g_QuadTable.push_back(scopeDown);

				for (auto& quad : quads)
				{
					// We are adding to "arg1", because if the command is "break", the default value of "arg1" is 1, and the default value of "continue" is 0. 
					// That's so "break" can skip the "JMP" command that will return it to the condition of the "while".
					quad->arg1 += g_QuadTable.size();
				}
			}
			else
			{
				// Its "if".
				Block();


				--m_ExprLevel;
				// This quad is used to keep track of the scope in validation and debugging.
				std::shared_ptr<Quad> scopeDown = std::make_shared<Quad>();
				scopeDown->operation = "SDW";
				g_QuadTable.push_back(scopeDown);
			}
		}
		// We don't have anything in the body, just returning.
		else
		{
			jmpDownQuad->arg1 = g_QuadTable.size();
			g_CurrentToken = GetNextToken();

			--m_ExprLevel;
			// This quad is used to keep track of the scope in validation and debugging.
			std::shared_ptr<Quad> scopeDown = std::make_shared<Quad>();
			scopeDown->operation = "SDW";
			g_QuadTable.push_back(scopeDown);

			return returnQuad;
		}
		
		if (g_CurrentToken.symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}

		// If it's true, it's an "while"
		if (m_ExprLevel % 2 == 0)
		{
			g_CurrentToken = GetNextToken();

			if (g_CurrentToken.symbol == ";;")
			{
				/*		ERROR		*/
				throw std::runtime_error("This is an 'While' not an 'if'!");
			}
			
			// Creating the jump command that will point to the beginning of the condition of the "while"
			std::shared_ptr<Quad> jmpUpQuad = std::make_shared<Quad>();
			jmpUpQuad->operation = "JMP";
			jmpUpQuad->arg1 = condStartPos;
			g_QuadTable.push_back(jmpUpQuad);

			jmpDownQuad->arg1 = g_QuadTable.size();

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
		std::shared_ptr<Quad> jmpAfterIfQuad = std::make_shared<Quad>();
		jmpAfterIfQuad->operation = "JMP";
		

		SymbolTableItem nextToken = CheckNextToken();

		g_CurrentToken = GetNextToken();
		if (g_CurrentToken.symbol == ";;")
		{
			g_QuadTable.push_back(jmpAfterIfQuad);
			
			// This is used to store all the "if-else-es"'s "JMP" quads that point to the end of the "if"
			std::vector<std::shared_ptr<Quad>>* elses = new std::vector<std::shared_ptr<Quad>>();

			jmpDownQuad->arg1 = g_QuadTable.size();
			do 
			{
				g_CurrentToken = GetNextToken();
				std::shared_ptr<Quad> elseQuad = ElseIf();

				// This is used for the same as "jmpAfterIfQuad" but it's for every "if-else" except the first one.
				std::shared_ptr<Quad> jmpAfterElseQuad = std::make_shared<Quad>();
				jmpAfterElseQuad->operation = "JMP";
				elses->push_back(jmpAfterElseQuad);
				g_QuadTable.push_back(jmpAfterElseQuad);

				elseQuad->arg1 = g_QuadTable.size();
				g_CurrentToken = GetNextToken();
			}
			while (g_CurrentToken.symbol == ";;");

			if (g_CurrentToken.symbol == ";;-")
			{
				g_CurrentToken = GetNextToken();
				Else();
				g_CurrentToken = GetNextToken();
			}

			for (auto& _else : *elses)
			{
				_else->arg1 = g_QuadTable.size();
			}

			jmpAfterIfQuad->arg1 = g_QuadTable.size();

		}
		else if (g_CurrentToken.symbol == ";;-")
		{
			g_QuadTable.push_back(jmpAfterIfQuad);

			g_CurrentToken = GetNextToken();
			jmpDownQuad->arg1 = g_QuadTable.size();

			Else();

			g_CurrentToken = GetNextToken();
			
			jmpAfterIfQuad->arg1 = g_QuadTable.size();
		}
		else
		{
			jmpDownQuad->arg1 = g_QuadTable.size();
		}
	}
	else if (g_CurrentToken.symbol == ")")
	{
		return returnQuad;
	}
	else
	{
		if (m_ExprLevel % 2 != 0)
		{
			if (g_CurrentToken.symbol == ";-")
			{
				// This is valid.
				returnQuad->operation = "JMP";
				g_QuadTable.push_back(returnQuad);

				g_CurrentToken = GetNextToken();
				return returnQuad;
			}
			else if (g_CurrentToken.symbol == "-;")
			{
				// This is valid.
				returnQuad->operation = "JMP";
				returnQuad->arg1 = 1;
				g_QuadTable.push_back(returnQuad);

				g_CurrentToken = GetNextToken();
				return returnQuad;
			}
		}

		/*		ERROR		*/		
		throw std::runtime_error("Expected Identifier or -: or :- or : . It can also be -; or ;- if its in an loop.");
	}

	return returnQuad;
}

std::shared_ptr<Quad> Parser::ElseIf()
{
	std::shared_ptr<Quad> jmpDownQuad = std::make_shared<Quad>();

	if (g_CurrentToken.symbol != "(")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected (");
	}

	g_CurrentToken = GetNextToken();

	jmpDownQuad->operation = "BRZ";
	jmpDownQuad->arg2 = Expr();
	g_QuadTable.push_back(jmpDownQuad);

	if (g_CurrentToken.symbol != ")")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected )");
	}

	g_CurrentToken = GetNextToken();
	if (g_CurrentToken.symbol != "(")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected (");
	}

	++m_ExprLevel;
	// This quad is used to keep track of the scope in validation and debugging.
	std::shared_ptr<Quad> scopeUp = std::make_shared<Quad>();
	scopeUp->operation = "SUP";
	g_QuadTable.push_back(scopeUp);

	g_CurrentToken = GetNextToken();
	Block();

	if (g_CurrentToken.symbol != ")")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected )");
	}

	--m_ExprLevel;
	// This quad is used to keep track of the scope in validation and debugging.
	std::shared_ptr<Quad> scopeDown = std::make_shared<Quad>();
	scopeDown->operation = "SDW";
	g_QuadTable.push_back(scopeDown);

	return jmpDownQuad;
}

string Parser::Else()
{
	if (g_CurrentToken.symbol != "(")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected (");
	}

	++m_ExprLevel;
	// This quad is used to keep track of the scope in validation and debugging.
	std::shared_ptr<Quad> scopeUp = std::make_shared<Quad>();
	scopeUp->operation = "SUP";
	g_QuadTable.push_back(scopeUp);

	g_CurrentToken = GetNextToken();
	Block();

	if (g_CurrentToken.symbol != ")")
	{
		/*		ERROR		*/
		throw std::runtime_error("Expected )");
	}

	--m_ExprLevel;
	// This quad is used to keep track of the scope in validation and debugging.
	std::shared_ptr<Quad> scopeDown = std::make_shared<Quad>();
	scopeDown->operation = "SDW";
	g_QuadTable.push_back(scopeDown);

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

	if (g_CurrentToken.symbol == ":::")
	{
		string operation = g_CurrentToken.symbol;
		
		g_CurrentToken = GetNextToken();
		arg2 = Comparison();

		result = GenCustomVar();
		std::shared_ptr<Quad> quad = std::make_shared<Quad>(Quad{ operation, arg1, arg2, result });
		g_QuadTable.push_back(quad);
		arg1 = result;
	}

	return arg1;
}

size_t Parser::Comparison()
{
	size_t arg1, arg2, result;

	arg1 = Term();

	if (g_CurrentToken.symbol == ":;")
	{
		string operation = g_CurrentToken.symbol;

		g_CurrentToken = GetNextToken();
		arg2 = Comparison();
		result = GenCustomVar();
		g_QuadTable.push_back(std::make_shared<Quad>(Quad{ operation, arg1, arg2, result }));
		arg1 = result;
	}

	return arg1;
}

size_t Parser::Term()
{
	size_t arg1, arg2, result;

	arg1 = Factor();

	while (g_CurrentToken.symbol == ";;;;" || g_CurrentToken.symbol == "::::")
	{
		string operation = g_CurrentToken.symbol;

		g_CurrentToken = GetNextToken();
		arg2 = Term();
		result = GenCustomVar();
		g_QuadTable.push_back(std::make_shared<Quad>(Quad{ operation, arg1, arg2, result }));
		arg1 = result;
	}

	return arg1;
}

size_t Parser::Factor()
{
	size_t arg1, arg2, result;

	arg1 = Primary();

	g_CurrentToken = GetNextToken();
	while (g_CurrentToken.symbol == "::;;" || g_CurrentToken.symbol == ";;::")
	{
		string operation = g_CurrentToken.symbol;
		g_CurrentToken = GetNextToken();
		arg2 = Factor();
		result = GenCustomVar();
		g_QuadTable.push_back(std::make_shared<Quad>(Quad{ operation, arg1, arg2, result }));
		arg1 = result;
	}

	return arg1;
}

size_t Parser::Primary()
{
	size_t arg;

	if (g_CurrentToken.code == SymbolCode::Identifier)
	{
		arg = m_TableIndex;
	}
	else if (g_CurrentToken.code == SymbolCode::IntegerLiteral)
	{
		arg = m_TableIndex;
	}
	else if (g_CurrentToken.symbol == "(")
	{
		g_CurrentToken = GetNextToken();
		arg = Expr();
		
		if (g_CurrentToken.symbol != ")")
		{
			/*		ERROR		*/
			throw std::runtime_error("Expected )");
		}
		
		if (CheckNextToken(true).symbol == "\n")
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


void Parser::SemanticValidation()
{
	// Contains all defined variables. Each vector in the second dimension is a different scope.
	std::vector<std::vector<string>*>* definedVariable = new std::vector<std::vector<string>*>();
	definedVariable->push_back(new std::vector<string>());

	// Checks if the given variable is defined in the 2D vector (definedVariable).
	auto isDefined = [definedVariable](string x) 
	{
		for (auto& scope : *definedVariable)
		{
			for (auto& variable : *scope)
			{
				if (variable == x)
				{
					return true;
				}
			}
		}

		return false;
	};

	int currentQuadIndex = 0;
	while (currentQuadIndex < g_QuadTable.size())
	{
		std::shared_ptr<Quad> currentQuad = g_QuadTable.at(currentQuadIndex);
		string quadStrResult;
		string quadStrArg1;
		string quadStrArg2;
		string quadOperation = currentQuad->operation;

		std::vector<string>* currentScope = definedVariable->back();

		if (quadOperation == "::")
		{
			quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
			quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

			if (!IsInteger(quadStrArg1) && !isDefined(quadStrArg1))
			{
				/*		ERROR		*/
				throw std::runtime_error("Undefined variable: " + quadStrArg1);
			}

			if (!isDefined(quadStrResult))
			{
				currentScope->push_back(quadStrResult);
			}
		}
		else if (quadOperation == "::::" ||
				 quadOperation == "::;;" ||
				 quadOperation == ";;::" ||
				 quadOperation == ";;;;" ||
				 quadOperation == ":::" ||
				 quadOperation == ":;" )
		{
			quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
			quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;
			quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

			if (!IsInteger(quadStrArg1) && !isDefined(quadStrArg1))
			{
				/*		ERROR		*/
				throw std::runtime_error("Undefined variable: " + quadStrArg1);
			}

			if (!IsInteger(quadStrArg2) && !isDefined(quadStrArg2))
			{
				/*		ERROR		*/
				throw std::runtime_error("Undefined variable: " + quadStrArg2);
			}

			if (!isDefined(quadStrResult))
			{
				currentScope->push_back(quadStrResult);
			}
		}
		else if (quadOperation == "OUT")
		{
			if (!currentQuad->result)
			{
				++currentQuadIndex;
				continue;
			}

			quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

			if (!IsInteger(quadStrResult) && !isDefined(quadStrResult))
			{
				/*		ERROR		*/
				throw std::runtime_error("Undefined variable: " + quadStrResult);
			}
		}
		else if (quadOperation == "IN")
		{
			if (!currentQuad->result)
			{
				++currentQuadIndex;
				continue;
			}

			quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

			if (!isDefined(quadStrResult))
			{
				currentScope->push_back(quadStrResult);
			}
		}
		else if (quadOperation == "BRZ")
		{
			quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;

			if (!IsInteger(quadStrArg2) && !isDefined(quadStrArg2))
			{
				/*		ERROR		*/
				throw std::runtime_error("Undefined variable: " + quadStrArg2);
			}
		}
		else if (quadOperation == "SUP")
		{
			definedVariable->push_back(new std::vector<string>());
		}
		else if (quadOperation == "SDW")
		{
			definedVariable->pop_back();
		}

		++currentQuadIndex;
	}
}
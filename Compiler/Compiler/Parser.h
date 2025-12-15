#pragma once

#include <string>
#include <vector>
#include "Lex.h"

using string = std::string;

struct Quad
{
	string operation;
	size_t arg1;
	size_t arg2;
	size_t result;
};

class Parser
{
public:
	static void Pars();
	static void LogQuad(int _index);
	
private:
	static int m_ExprLevel;
	static int m_CustomVarIndex;

	static size_t m_TableIndex;

	static SymbolTableItem GetNextToken(bool includeNewLine);
	static void ReturnWithOneToken();
	static SymbolTableItem CheckNextToken(bool includeNewLine = false);

	static size_t GenCustomVar();
	static void Log();

	static void Start();
	static std::vector<std::shared_ptr<Quad>> Block();
	static std::vector<std::shared_ptr<Quad>> Stms();
	static std::shared_ptr<Quad> Stm();
	static std::shared_ptr<Quad> ElseIf();
	static string Else();
	static size_t Expr();
	static size_t Equality();
	static size_t Comparison();
	static size_t Term();
	static size_t Factor();
	static size_t Primary();

	static void SemanticValidation();
};


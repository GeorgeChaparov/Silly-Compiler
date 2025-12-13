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
	
private:
	static int m_ExprLevel;
	static int m_CustomVarIndex;

	static size_t m_TableIndex;

	static SymbolTableItem* GetNextToken(bool includeNewLine);
	static void ReturnWithOneToken();
	static SymbolTableItem* CheckNextToken(bool includeNewLine = false);

	static size_t GenCustomVar();
	static void Log();

	static void Start();
	static std::vector<Quad*>* Block();
	static std::vector<Quad*>* Stms();
	static Quad* Stm();
	static Quad* ElseIf();
	static string Else();
	static size_t Expr();
	static size_t Equality();
	static size_t Comparison();
	static size_t Term();
	static size_t Factor();
	static size_t Primary();

	static void SemanticValidation();
};


#pragma once

#include <string>
#include "Lex.h"

using string = std::string;

class Parser
{
public:
	static void Pars();
	
private:
	static int m_ExprLevel;

	static SymbolTableItem* GetNextToken(bool includeNewLine);
	static SymbolTableItem* CheckNextToken();

	static void Start();
	static void Block();
	static void Stms();
	static void Stm();
	static void ElseIf();
	static void Expr();
	static void Equality();
	static void Comparison();
	static void Term();
	static void Factor();
	static void Primary();
};


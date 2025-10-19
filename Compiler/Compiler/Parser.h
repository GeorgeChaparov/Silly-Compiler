#pragma once

#include <string>
#include "Lex.h"

using string = std::string;

class Parser
{
public:
	static void Pars();
	
private:
	static string* m_Token;

	static string* GetNextToken();

	static void Start();
	static void Block();
	static void Stms();
	static void Stm();
};


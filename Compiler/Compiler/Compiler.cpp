#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <execution>
#include "Lex.h"
#include "Parser.h"
#include "Logger.h"
#include "SymbolTable.h"


// g_VariableName -> variable in Global.h
// m_VariableName -> private member variable
// M_VARIABLE_NAME -> member constant
// VARIABLE_NAME -> constant in Consts.h
// varableName -> scoped variable
// _variableName -> function or method parameter

using namespace std;
int main() {

	try
	{
		std::ifstream file("D:\\GitHub\\New folder\\Silly-Compiler\\Compiler\\Compiler\\Source.txt");
		if (!file) {
			std::cerr << "Failed to open file\n";
			return 1;
		}

		std::ostringstream buffer;
		buffer << file.rdbuf();

		string fileStream = buffer.str();

		SymbolTable::Init();
		Lex::Init(fileStream);
		Parser::Pars();

		//Lex::Build();

		cout << endl << endl << "File:" << endl << fileStream << endl << endl;

		SymbolTable::Log();
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Unknown, false);
	}
	
	return 0;
}
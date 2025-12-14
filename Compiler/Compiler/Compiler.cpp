#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <execution>
#include "Lex.h"
#include "Parser.h"
#include "Logger.h"
#include "SymbolTable.h"
#include "VirtualMachine.h"


// g_VariableName -> variable in Global.h
// m_VariableName -> private member variable
// M_VARIABLE_NAME -> member constant
// VARIABLE_NAME -> constant in Utils\Consts\Consts.h
// variableName -> scoped variable
// _variableName -> function or method parameter

int main() {

	try
	{
		std::ifstream file("D:\\GitHub\\New folder\\Silly-Compiler\\Compiler\\Compiler\\Source - Copy.txt");
		if (!file) {
			std::cerr << "Failed to open file\n";
			return 1;
		}

		std::ostringstream buffer;
		buffer << file.rdbuf();

		std::string fileStream = buffer.str();

		SymbolTable::Init();
		Lex::Init(fileStream);
		Parser::Pars();

		std::cout << std::endl << std::endl << "File:" << std::endl << fileStream << std::endl << std::endl;

		SymbolTable::Log();

		std::cout << std::endl << std::endl << std::endl << std::endl << "Result" << std::endl << std::endl;

		VirtualMachine::Run();
	}
	catch (const std::exception& error)
	{
		Logger::Log(error.what(), ErrorType::Unknown, false);
	}
	
	return 0;
}
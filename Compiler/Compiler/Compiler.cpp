#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <execution>
#include "Lex.h"
#include "Parser.h"
#include "Logger.h"
#include "SymbolTable.h"

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
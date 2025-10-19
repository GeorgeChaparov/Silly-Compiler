#include "SymbolTable.h"
#include <string>
#include <iostream>
#include "Lex.h"
#include <fstream>
#include <sstream>

using namespace std;
int main() {

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
	Lex::Build();

	cout << endl << endl << "File:" << endl << fileStream << endl << endl;

	SymbolTable::Log();
	return 0;
}
#pragma once

#include <string>
#include <iostream>

using string = std::string;

enum SymbolCode
{
	Unknown = -1,
	Keyword = 1,
	Identifier,
	Operator,
	Punctuation,
	IntegerLiteral,
};

struct SymbolTableItem
{
	string symbol;
	SymbolCode code;

	void Log()
	{
		std::cout << "Symbol: '" << symbol << "', Code: " << code << std::endl;
	}
};

class SymbolTable
{
public:
	static void Init();

	static size_t AddItem(SymbolTableItem& item);
	static size_t AddItem(string name, SymbolCode code);
	static size_t AddItem(char name, SymbolCode code);

	static size_t GetPosition(SymbolTableItem& item);

	static bool Has(SymbolTableItem& item, size_t& position);

	static void Log();

	static SymbolTableItem* GetElementAt(size_t index);

private:
	static unsigned int GetHash(SymbolTableItem& item);
	static SymbolTableItem** m_HashTable;
};
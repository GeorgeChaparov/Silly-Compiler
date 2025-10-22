#include "SymbolTable.h"
#include <iostream>
#include "Utils.h"
#include <string>
#include "Consts.h"

using string = std::string;

SymbolTableItem** SymbolTable::m_HashTable = nullptr;

void SymbolTable::Init()
{
	m_HashTable = new SymbolTableItem * [SYMBOL_TABLE_SIZE]();
}

size_t SymbolTable::AddItem(SymbolTableItem& item)
{
	size_t position = 0;

	if (!Has(item, position))
	{
		while (m_HashTable[position] != nullptr)
		{
			position = (position + 1) % SYMBOL_TABLE_SIZE;
		}

		m_HashTable[position] = &item;
	}

	return position;
}

size_t SymbolTable::AddItem(string name, SymbolCode code)
{
	SymbolTableItem* item = new SymbolTableItem{ name, code };

	return AddItem(*item);
}

size_t SymbolTable::AddItem(char name, SymbolCode code)
{
	SymbolTableItem* item = new SymbolTableItem{string() + name, code };

	return AddItem(*item);
}

unsigned int SymbolTable::GetHash(SymbolTableItem& item) {

	string value = item.symbol;

	size_t position = 0;

	for (int i = 0; i < value.length(); i++)
	{
		char c = value.at(i);

		unsigned int ascii = static_cast<int>(c);

		position += (i + 1) * ascii;
	}

	return position % SYMBOL_TABLE_SIZE;
}

bool SymbolTable::Has(SymbolTableItem& item, size_t& position)
{
	size_t pos = GetHash(item);
	size_t start = pos;

	while (m_HashTable[pos] != nullptr) {

		if (m_HashTable[pos]->symbol == item.symbol && m_HashTable[pos]->code == item.code) {
			position = pos;
			return true;
		}
		pos = (pos + 1) % SYMBOL_TABLE_SIZE;

		if (pos == start) break;
	}

	position = start;
	return false;
}

size_t SymbolTable::GetPosition(SymbolTableItem& item)
{
	size_t position = 0;
	Has(item, position);

	return position;
}

void SymbolTable::Log()
{
	for (size_t i = 0; i < SYMBOL_TABLE_SIZE; i++)
	{
		SymbolTableItem* item = m_HashTable[i];

		if (item != nullptr)
		{
			item->Log();
		}
	}
}

SymbolTableItem* SymbolTable::GetElementAt(size_t index)
{
	return m_HashTable[index];
}
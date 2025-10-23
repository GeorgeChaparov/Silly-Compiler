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

size_t SymbolTable::AddItem(SymbolTableItem& _item)
{
	size_t position = 0;

	if (!Has(_item, position))
	{
		while (m_HashTable[position] != nullptr)
		{
			position = (position + 1) % SYMBOL_TABLE_SIZE;
		}

		m_HashTable[position] = &_item;
	}

	return position;
}

size_t SymbolTable::AddItem(string _name, SymbolCode _code)
{
	SymbolTableItem* item = new SymbolTableItem{ _name, _code };

	return AddItem(*item);
}

size_t SymbolTable::AddItem(char _name, SymbolCode _code)
{
	SymbolTableItem* item = new SymbolTableItem{string() + _name, _code };

	return AddItem(*item);
}

unsigned int SymbolTable::GetHash(SymbolTableItem& _item) {

	string value = _item.symbol;

	size_t position = 0;

	for (int i = 0; i < value.length(); i++)
	{
		char c = value.at(i);

		unsigned int ascii = static_cast<int>(c);

		position += (i + 1) * ascii;
	}

	return position % SYMBOL_TABLE_SIZE;
}

bool SymbolTable::Has(SymbolTableItem& _item, size_t& _position)
{
	size_t pos = GetHash(_item);
	size_t start = pos;

	while (m_HashTable[pos] != nullptr) {

		if (m_HashTable[pos]->symbol == _item.symbol && m_HashTable[pos]->code == _item.code) {
			_position = pos;
			return true;
		}
		pos = (pos + 1) % SYMBOL_TABLE_SIZE;

		if (pos == start) break;
	}

	_position = start;
	return false;
}

size_t SymbolTable::GetPosition(SymbolTableItem& _item)
{
	size_t position = 0;
	Has(_item, position);

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

SymbolTableItem* SymbolTable::GetElementAt(size_t _index)
{
	return m_HashTable[_index];
}
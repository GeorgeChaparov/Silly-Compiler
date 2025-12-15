#pragma once

#include <string>
#include "SymbolTable.h"

using string = std::string;

struct TrieNode 
{
	static const int CHILDREN_COUNT = 3; // ':', ';', '-'
	char symbol[CHILDREN_COUNT] = { ':', ';', '-' };
	std::shared_ptr<TrieNode> children[CHILDREN_COUNT] = { nullptr, nullptr, nullptr };

	bool isEnd = false;
	SymbolCode tokenType = SymbolCode::Unknown;  // "Operator"
	string tokenValue = ""; // "::::"
};

class GrammarTrie
{
public:
	static void Insert(string token, SymbolCode type);
	static std::shared_ptr<TrieNode> Find(string token);

private:
	static std::shared_ptr<TrieNode> m_Root;

	static int GetIndex(char c);
};


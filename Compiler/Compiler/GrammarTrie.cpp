#include "GrammarTrie.h"

TrieNode* GrammarTrie::m_Root = new TrieNode();

void GrammarTrie::Insert(string token, SymbolCode type) {

	TrieNode* node = m_Root;

	for (char c : token) {
		int index = GetIndex(c);
		if (!node->children[index])
		{
			node->children[index] = new TrieNode();
		}
			
		node = node->children[index];
	}
	node->isEnd = true;
	node->tokenType = type;
	node->tokenValue = token;
}

TrieNode* GrammarTrie::Find(string token)
{
	TrieNode* node = m_Root;

	for (char c : token) {
		int index = GetIndex(c);
		if (!node->children[index])
		{
			throw std::runtime_error("Unknown symbol: " + c);
		}

		node = node->children[index];
	}

	if (!node->isEnd)
	{
		throw std::runtime_error("");
	}

	return node;
}

int GrammarTrie::GetIndex(char c) {
	switch (c) {
	case ':': return 0;
	case ';': return 1;
	case '-': return 2;
	default: return -1;
	}
}
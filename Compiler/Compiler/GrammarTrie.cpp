#include "GrammarTrie.h"

std::shared_ptr<TrieNode> GrammarTrie::m_Root = std::make_shared<TrieNode>();

void GrammarTrie::Insert(string _token, SymbolCode _type) {

	std::shared_ptr<TrieNode> node = m_Root;

	for (char c : _token) {
		int index = GetIndex(c);
		if (!node->children[index])
		{
			node->children[index] = std::make_shared<TrieNode>();
		}
			
		node = node->children[index];
	}
	node->isEnd = true;
	node->tokenType = _type;
	node->tokenValue = _token;
}

std::shared_ptr<TrieNode> GrammarTrie::Find(string _token)
{
	std::shared_ptr<TrieNode> node = m_Root;

	for (char ch : _token) {
		int index = GetIndex(ch);
		if (!node->children[index])
		{
			throw std::runtime_error("Unknown symbol: " + ch);
		}

		node = node->children[index];
	}

	if (!node->isEnd)
	{
		throw std::runtime_error("");
	}

	return node;
}

int GrammarTrie::GetIndex(char _ch) {
	switch (_ch) 
	{
	case ':': 
		return 0;

	case ';': 
		return 1;

	case '-': 
		return 2;

	default: 
		return -1;
	}
}
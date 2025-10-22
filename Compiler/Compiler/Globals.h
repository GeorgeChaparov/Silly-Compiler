#pragma once
#include "SymbolTable.h"
#include <string>

extern size_t g_CurrentPosition;		// Shows the index of the current character in the document.
extern unsigned int g_CurrentLine;		// Shows the position of the current line.
extern unsigned int g_LineSymbolCount;	// Shows the position of the current character on the current line.
extern SymbolTableItem* g_CurrentToken;	// Contains the current token.
extern std::string g_Document;			// Contains the document that is being parsed.
extern int g_DocumentLength;	// The length of the document that is being parsed.
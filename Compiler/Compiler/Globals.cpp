#include "Globals.h"

size_t g_CurrentPosition = 0;
unsigned int g_CurrentLine = 1;
unsigned int g_LineSymbolCount = 1;
SymbolTableItem* g_CurrentToken = new SymbolTableItem{ "", SymbolCode::Unknown };
std::string g_Document = "";
int g_DocumentLength = g_Document.length();
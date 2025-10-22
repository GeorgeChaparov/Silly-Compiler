#include "Logger.h"
#include "Globals.h"

void Logger::Log(string message, ErrorType type, bool includeCurrentToken)
{
	if (includeCurrentToken)
	{
		std::cout << "Error: type " << static_cast<int>(type) << ", " << message << ", got " << g_CurrentToken->symbol
			<< " at " << g_CurrentLine << " / " << g_LineSymbolCount - g_CurrentToken->symbol.length() << std::endl;
	}
	else
	{
		std::cout << "Error: type " << static_cast<int>(type) << ", " << message << " at " 
			<< g_CurrentLine << " / " << g_LineSymbolCount - g_CurrentToken->symbol.length() << std::endl;
	}
	
}
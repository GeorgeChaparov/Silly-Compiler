#include "Logger.h"
#include "Globals.h"

void Logger::Log(string _message, ErrorType _type, bool _includeCurrentToken)
{
	if (_includeCurrentToken)
	{
		std::cout << "Error: type " << static_cast<int>(_type) << ", " << _message << ", got " << g_CurrentToken->symbol
			<< " at " << g_CurrentLine << " / " << g_LineSymbolCount - g_CurrentToken->symbol.length() << std::endl;
	}
	else
	{
		std::cout << "Error: type " << static_cast<int>(_type) << ", " << _message << " at " 
			<< g_CurrentLine << " / " << g_LineSymbolCount - g_CurrentToken->symbol.length() << std::endl;
	}
}

void Logger::Log(string _message)
{
	std::cout << "Error: " << _message << std::endl;
}

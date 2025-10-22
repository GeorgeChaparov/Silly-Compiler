#pragma once
#include <string>
#include <iostream>

using string = std::string;

enum class ErrorType
{
	Unknown, Lexical, Semantic
};

class Logger
{
public:
	static void Log(string message, ErrorType type, bool includeCurrentToken = true);
};


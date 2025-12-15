#pragma once
#include <iostream>
#include <string>
#include "RuntimeGlobals.h"
#include "VirtualMachine.h"

using string = std::string;

class Debugger
{
public:
	static void Run();

private:
	static void ShowVariables(bool _currentScopeOnly);
};


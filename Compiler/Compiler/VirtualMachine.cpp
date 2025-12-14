#include "VirtualMachine.h"

void VirtualMachine::Step()
{
	// Checks if the given variable is defined in the 2D vector (definedVariable).
	auto getValueIfDefined = [](string name)
		{
			for (std::unordered_map<string, int>& scope : g_Variables)
			{
				 if (scope.contains(name))
				 {
					 return scope.at(name);
				 }
			}

			return -1;
		};

	auto isDefined = [](string x)
		{
			for (std::unordered_map<string, int>& scope : g_Variables)
			{
				if (scope.contains(x))
				{
					return true;
				}
			}

			return false;
		};

	auto addOrSetVar = [](string name, int value)
		{
			int index = -1;
			for (std::unordered_map<string, int>& scope : g_Variables)
			{
				if (scope.contains(name))
				{
					scope.at(name) = value;
					return;
				}
				++index;
			}

			g_Variables.at(index).insert({ name , value });
		};

	Quad* currentQuad = g_QuadTable->at(g_QuadIndex);
	string quadStrResult;
	string quadStrArg1;
	string quadStrArg2;
	string quadOperation = currentQuad->operation;

	auto& currentScope = g_Variables.back();

	int arg1Val = -1;
	int arg2Val = -1;
	int resultVal = -1;

	if (quadOperation == "::")
	{
		quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		arg1Val = IsInteger(quadStrArg1) ? ConvertToInt(quadStrArg1) : getValueIfDefined(quadStrArg1);

		addOrSetVar(quadStrResult, arg1Val);
	}
	else if (quadOperation == "::::")
	{
		quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
		quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		arg1Val = IsInteger(quadStrArg1) ? ConvertToInt(quadStrArg1) : getValueIfDefined(quadStrArg1);
		arg2Val = IsInteger(quadStrArg2) ? ConvertToInt(quadStrArg2) : getValueIfDefined(quadStrArg2);

		int value = arg1Val + arg2Val;
		addOrSetVar(quadStrResult, value);
	}
	else if (quadOperation == "::;;")
	{
		quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
		quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		arg1Val = IsInteger(quadStrArg1) ? ConvertToInt(quadStrArg1) : getValueIfDefined(quadStrArg1);
		arg2Val = IsInteger(quadStrArg2) ? ConvertToInt(quadStrArg2) : getValueIfDefined(quadStrArg2);

		int value = arg1Val * arg2Val;
		addOrSetVar(quadStrResult, value);
	}
	else if (quadOperation == ";;;;")
	{
		quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
		quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		arg1Val = IsInteger(quadStrArg1) ? ConvertToInt(quadStrArg1) : getValueIfDefined(quadStrArg1);
		arg2Val = IsInteger(quadStrArg2) ? ConvertToInt(quadStrArg2) : getValueIfDefined(quadStrArg2);

		int value = arg1Val - arg2Val;
		addOrSetVar(quadStrResult, value);
	}
	else if (quadOperation == ";;::")
	{
		quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
		quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		arg1Val = IsInteger(quadStrArg1) ? ConvertToInt(quadStrArg1) : getValueIfDefined(quadStrArg1);
		arg2Val = IsInteger(quadStrArg2) ? ConvertToInt(quadStrArg2) : getValueIfDefined(quadStrArg2);

		int value = arg1Val / arg2Val;
		addOrSetVar(quadStrResult, value);
	}
	else if (quadOperation == ":::")
	{
		quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
		quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		arg1Val = IsInteger(quadStrArg1) ? ConvertToInt(quadStrArg1) : getValueIfDefined(quadStrArg1);
		arg2Val = IsInteger(quadStrArg2) ? ConvertToInt(quadStrArg2) : getValueIfDefined(quadStrArg2);

		int value = arg1Val == arg2Val;
		addOrSetVar(quadStrResult, value);
	}
	else if (quadOperation == ":;")
	{
		quadStrArg1 = SymbolTable::GetElementAt(currentQuad->arg1)->symbol;
		quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		arg1Val = IsInteger(quadStrArg1) ? ConvertToInt(quadStrArg1) : getValueIfDefined(quadStrArg1);
		arg2Val = IsInteger(quadStrArg2) ? ConvertToInt(quadStrArg2) : getValueIfDefined(quadStrArg2);

		int value = arg1Val > arg2Val;
		addOrSetVar(quadStrResult, value);
	}
	else if (quadOperation == "OUT")
	{
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		if (!currentQuad->result)
		{
			std::cout << std::endl;
		}
		else if (!IsInteger(quadStrResult))
		{
			resultVal = getValueIfDefined(quadStrResult);
			std::cout << resultVal << std::endl;
		}
	}
	else if (quadOperation == "IN")
	{
		quadStrResult = SymbolTable::GetElementAt(currentQuad->result)->symbol;

		if (!currentQuad->result)
		{
			std::cin;
		}
		else
		{
			std::cin >> resultVal;
			addOrSetVar(quadStrResult, resultVal);
		}
	}
	else if (quadOperation == "BRZ")
	{
		quadStrArg2 = SymbolTable::GetElementAt(currentQuad->arg2)->symbol;


		arg2Val = IsInteger(quadStrArg2) ? ConvertToInt(quadStrArg2) : getValueIfDefined(quadStrArg2);

		if (!arg2Val)
		{
			g_QuadIndex = currentQuad->arg1;
			return;
		}
	}
	else if (quadOperation == "JMP")
	{
		g_QuadIndex = currentQuad->arg1;
		return;
	}
	else if (quadOperation == "SUP")
	{
		g_Variables.emplace_back(std::unordered_map<string, int>());
	}
	else if (quadOperation == "SDW")
	{
		g_Variables.pop_back();
	}

	++g_QuadIndex;
}


void VirtualMachine::Run()
{
	while (g_QuadIndex < g_QuadTable->size())
	{
		Step();
	}
}
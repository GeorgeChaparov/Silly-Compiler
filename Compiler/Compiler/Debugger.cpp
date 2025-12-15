#include "Debugger.h"

void Debugger::Run()
{
	string onlyCurrentScope;
	string shouldContinue = "y";

	std::cout << "Show only current scope? (y/n): ";
	std::cin >> onlyCurrentScope;

	while (g_QuadIndex < g_QuadTable.size() && shouldContinue == "y")
	{
		
		VirtualMachine::Step();
		ShowVariables(onlyCurrentScope == "y" ? true : false);
		std::cout << "Continue? (y/n): ";
		std::cin >> shouldContinue;

		std::cout << std::endl;
		std::cout << std::endl;
	}
}

void Debugger::ShowVariables(bool _currentScopeOnly)
{
	std::cout << std::endl;
	std::cout << std::endl;

	if (_currentScopeOnly)
	{
		auto scope = g_Variables.back();
		for (auto variable : scope)
		{
			if (variable.first[0] == '!')
			{
				continue;
			}

			std::cout << "Variable: " << variable.first << " = " << variable.second << std::endl;

			if (g_QuadIndex != 0)
			{
				std::cout << "\t";
				Parser::LogQuad(g_QuadIndex - 1);
			}

			std::cout << "--->\t";
			Parser::LogQuad(g_QuadIndex);

			if (g_QuadIndex != g_QuadTable.size() - 1)
			{
				std::cout << "\t";
				Parser::LogQuad(g_QuadIndex + 1);
			}
		}
	}
	else
	{
		for (auto scope : g_Variables)
		{
			for (auto variable : scope)
			{
				if (variable.first[0] == '!')
				{
					continue;
				}

				std::cout << "Variable: " << variable.first << " = " << variable.second << std::endl << std::endl;


				if (g_QuadIndex != 0)
				{
					std::cout << "\t";
					Parser::LogQuad(g_QuadIndex - 1);
				}

				std::cout << "--->\t";
				Parser::LogQuad(g_QuadIndex);

				if (g_QuadIndex != g_QuadTable.size() - 1)
				{
					std::cout << "\t";
					Parser::LogQuad(g_QuadIndex + 1);
				}
			}
		}
	}

	std::cout << std::endl;
}
#include "RuntimeGlobals.h"

size_t g_QuadIndex = 0;
std::vector<std::unordered_map<std::string, int>> g_Variables = std::vector<std::unordered_map<std::string, int>>{ std::unordered_map<std::string, int>() }; // Contains all defined variables. Each map is a different scope.
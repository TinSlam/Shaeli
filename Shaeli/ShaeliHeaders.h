#include "SymbolTable.h"

namespace Shaeli{
	std::string shaeliOutputDirectory = "Output";

	FILE* output;
	FILE* headersFile;
	FILE* functionHeadersFiles;
	
	char* mainFunctionsContent;
	
	std::map<std::string, bool> referencedFiles;
	
	std::stack<std::string> classes;
	std::stack<std::string> functions;
	std::stack<std::string> structs;
	
	SymbolTable symbolTable;
	Scope* currentScope = nullptr;
	
	std::list<Variable*> staticVariables;
	
	std::map<std::string, std::list<std::string>> structInitializationMap;
}
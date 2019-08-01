#include "Classes.h"

#include "SymbolTable.h"

namespace Shaeli{
	extern std::stack<std::string> classes;
	extern std::stack<std::string> structs;

	extern SymbolTable symbolTable;
	
	extern Scope* currentScope;
	
	extern FILE* headersFile;

	void enteredClass(std::string className){
		classes.push(className);
		currentScope = symbolTable.addScope(currentScope);
		std::string temp = "";
		temp += "\nstruct " + className + "{\n";
		structs.push(temp);
	}
	
	void classEnded(){
		std::string temp = structs.top();
		structs.pop();
		temp += "};\n";
		fprintf(headersFile, "%s", temp.c_str());
		currentScope = currentScope->parent;
		classes.pop();
	}
}
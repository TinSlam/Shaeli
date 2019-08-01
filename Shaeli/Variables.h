#pragma once

#include "SymbolTable.h"

namespace Shaeli{
	void addStaticVariable(Variable* variable);
	void variableDeclaration(std::string varType, std::string name, bool isStatic);
	void statementVariableDeclared(std::string varType, std::string name);
	void statementVarDec(std::string varType, std::string name);
	void statementVarDecAssigned(std::string expression, std::string varType, std::string varName);
	void varDecDone(bool isStatic);
	void varDecAssigned(std::string expression, bool isStatic);
	void structObjInitialization(std::string className, std::string name);
}
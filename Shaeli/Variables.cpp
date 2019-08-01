#include "Variables.h"

namespace Shaeli{
	extern std::map<std::string, std::list<std::string>> structInitializationMap;
	
	void addStaticVariable(Variable* variable){
		staticVariables.push_back(variable);
	}
	
	void variableDeclaration(std::string varType, std::string name, bool isStatic){
		std::string varName = "";
		if(isStatic){
			varName += "_" + classes.top() + "_" + name;
			Variable* variable = new Variable(varName, varType, isStatic, false);
			addStaticVariable(variable);
			fprintf(headersFile, "%s %s", varType.c_str(), varName.c_str());
		}else{
			varName += name;
			std::string temp = structs.top();
			structs.pop();
			temp += varType + " " + name;
			structs.push(temp);
			structInitializationMap[classes.top()].push_back(name);
			symbolTable.addVariable(currentScope, varName, varType, isStatic);
		}
	}
	
	void statementVariableDeclared(std::string varType, std::string name){
		symbolTable.addVariable(currentScope, name, varType, false, true);
		if(varType == "int"){
			fprintf(output, "%s %s;\n", varType.c_str(), name.c_str());
		}else{
			fprintf(output, "struct %s %s;\n", varType.c_str(), name.c_str());
			structObjInitialization(varType, name);
		}
	}
	
	void statementVarDec(std::string varType, std::string name){
		symbolTable.addVariable(currentScope, name, varType, false, true);
		if(varType == "int"){
			fprintf(output, "%s %s", varType.c_str(), name.c_str());
		}else{
			fprintf(output, "struct %s %s;\n", varType.c_str(), name.c_str());
			structObjInitialization(varType, name);
		}
	}
	
	void statementVarDecAssigned(std::string expression, std::string varType, std::string varName){
		if(varType == "int"){
			fprintf(output, " = %s;\n", expression.c_str());			
		}else{
			fprintf(output, "%s = %s;\n", varName.c_str(), expression.c_str());			
		}
	}
	
	void varDecDone(bool isStatic){
		if(isStatic){
			std::string temp = structs.top();
			structs.pop();
			temp += ";\n";
			structs.push(temp);
		}else{
			fprintf(output, ";\n");
		}
	}
	
	void varDecAssigned(std::string expression, bool isStatic){
		if(isStatic){
			fprintf(headersFile, " = %s;\n", expression.c_str());		
		}else{
			structInitializationMap[classes.top()].push_back(expression);
			std::string temp = structs.top();
			structs.pop();
			temp += ";\n";
			structs.push(temp);
		}
	}
	
	void structObjInitialization(std::string className, std::string name){
		std::list<std::string> classInitializations = structInitializationMap[className];
		int counter = 0;
		for(auto it = classInitializations.begin(); it != classInitializations.end(); it++){
			if(counter % 2 == 0){
				fprintf(output, "%s.%s", name.c_str(), (*it).c_str());
			}else{
				fprintf(output, " = %s;\n", (*it).c_str());
			}
			counter++;
		}
	}
}
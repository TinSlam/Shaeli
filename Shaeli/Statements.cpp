#include "Statements.h"

#include "Symbols.h"
#include "SymbolTable.h"

namespace Shaeli{
	extern SymbolTable symbolTable;
	
	extern std::list<Variable*> staticVariables;
	
	extern FILE* output;

	std::string forSymbol = "";
	
	void forStatementFound(std::string variable){
		forSymbol = variable;
		fprintf(output, "for(int %s", variable.c_str());
	}
	
	void forStatementStartFound(std::string expression){
		fprintf(output, " = %s; ", expression.c_str());
	}
	
	void forExpressionFound(std::string expression){
		fprintf(output, "%s <= %s; ", forSymbol.c_str(), expression.c_str());
	}
	
	void forStepsFound(std::string expression){
		fprintf(output, "%s += %s)", forSymbol.c_str(), expression.c_str());
	}
	
	void whileStatementFound(std::string expression){
		fprintf(output, "while(%s)", expression.c_str());
	}
	
	void emptyStatement(){
		fprintf(output, ";\n");
	}
	
	void ifStatementFound(std::string expression){
		fprintf(output, "if(%s)", expression.c_str());
	}
	
	void elseIfStatementFound(std::string expression){
		fprintf(output, "else if(%s)", expression.c_str());
	}
	
	void elseStatementFound(){
		fprintf(output, "else ");
	}
	
	void returnStatement(std::string expression){
		fprintf(output, "return %s;\n", expression.c_str());
	}
	
	void breakStatement(){
		fprintf(output, "break;\n");
	}
	
	void continueStatement(){
		fprintf(output, "continue;\n");
	}
	
	void print(std::string data){
		std::queue<std::string> variables;
		for(int i = 0; i < data.size(); i++){
			int old = i;
			if(data[i] == '$'){
				if(data[i + 1] == '{'){
					std::string var = "";
					bool ended = false;
					for(i += 2; i < data.size(); i++){
						if(data[i] == '}'){
							ended = true;
							break;
						}else{
							var += data[i];
						}
					}
					if(!ended) i = old;
					else{
						variables.push(getVariablesFullName(var));
						std::string type = "%s";
						Variable* variable = lookupStaticVariable(var);
						if(variable == nullptr){
							variable = symbolTable.lookupVariable(currentScope, var);
						}
						if(variable != nullptr){
							if(variable->type == "int"){
								type = "%d";
							}else{
								type = "%s";
							}							
						}
						data = data.substr(0, old) + type + data.substr(i + 1, data.size());
						i = old;
					}
				}
			}
		}
		data += "\"";
		if(variables.size() != 0){
			data += ", ";
			data += variables.front();
			variables.pop();
		}
		while(variables.size() != 0){
			data += ", " + variables.front();
			variables.pop();
		}
		fprintf(output, "printf(\"%s);\n", data.c_str());
	}
	
	void assignment(std::string right, std::string left){
		fprintf(output, "%s\n", (right + " = " + left + ";").c_str());
	}
	
	void addStatement(std::string statement){
		fprintf(output, "%s;\n", statement.c_str());
	}
}
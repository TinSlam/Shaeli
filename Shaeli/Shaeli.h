#include <map>
#include <stack>
#include <queue>
#include <list>

#include "SymbolTable.h"

extern void parseFile(std::string fileName);

std::string shaeliOutputDirectory = "Output";

FILE* output;
FILE* headersFile;
FILE* functionHeadersFiles;
char* mainFunctionsContent;

namespace Shaeli{
	std::map<std::string, bool> referencedFiles;
	std::stack<std::string> classes;
	std::stack<std::string> functions;
	std::stack<std::string> structs;
	SymbolTable symbolTable;
	Scope* currentScope = nullptr;
	std::list<Variable*> staticVariables;
	std::map<std::string, std::list<std::string>> structInitializationMap;
	
	bool staticVarNameMatches(Variable* var, std::string name){
		for(int i = 1; i < var->lexeme.size(); i++){
			if(var->lexeme[i] == '_'){
				if(var->lexeme.substr(i + 1, var->lexeme.size()).compare(name) == 0){
					return true;
				}else{
					return false;
				}
			}
		}
		return false;
	}
	
	Variable* lookupStaticVariable(std::string simpleName){
		for(auto it = staticVariables.begin(); it != staticVariables.end(); it++){
			if(staticVarNameMatches(*it, simpleName))
				return *it;
		}
		return nullptr;
	}
	
	std::string getVariablesFullName(std::string name){
		Variable* var = symbolTable.lookupVariable(currentScope, name);
		if(var == nullptr){
			var = lookupStaticVariable(name);
			if(var == nullptr)
				return nullptr;
			else
				return var->lexeme;
		}else{
			if(var->isTemp){
				return var->lexeme;
			}else{
				return "_Object->" + var->lexeme;
			}
		}
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
		data += "\", ";
		if(variables.size() != 0){
			data += variables.front();
			variables.pop();
		}
		while(variables.size() != 0){
			data += ", " + variables.front();
			variables.pop();
		}
		fprintf(output, "printf(\"%s);\n", data.c_str());
	}
	
	void functionCall(std::string name){
		std::string className = classes.top();
		std::string temp = "_" + className + "_" + name + "(";
		functions.push(temp);
	}
	
	void functionCallClass(std::string name, std::string instanceName){
		std::string temp;
		Variable* var = symbolTable.lookupVariable(currentScope, instanceName);
		if(var == nullptr){
			temp = "_" + instanceName + "_" + name + "(";
		}else{
			temp = "_" + var->type + "_" + name + "(&" + instanceName;
		}
		functions.push(temp);
	}
	
	void assignment(std::string right, std::string left){
		fprintf(output, "%s\n", (right + " = " + left + ";").c_str());
	}
	
	std::string binaryOperation(std::string operation, std::string second, std::string first){
		return first + " " + operation + " " + second;
	}
	
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
	
	void referenceFile(std::string fileName){
		if(referencedFiles[fileName]) return;
		parseFile(fileName);
		referencedFiles[fileName] = true;
	}
	
	void generatePrecode(){
		fopen_s(&headersFile, (shaeliOutputDirectory + "/ShaeliReferences.h").c_str(), "w");
		fopen_s(&functionHeadersFiles, (shaeliOutputDirectory + "/FunctionHeaders.h").c_str(), "w");
		fprintf(output, "#include <stdio.h>\n#include \"ShaeliReferences.h\"\n\n");
	}
	
	void generateMainFunction(){
		fprintf(output, "int main(){\n");
	}

	void wrapUpFile(){
		fprintf(headersFile, "#include \"FunctionHeaders.h\"");
		fclose(headersFile);
		fclose(functionHeadersFiles);
		fprintf(output, "\t_main();\n\treturn 0;\n}\n");
	}
	
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
	
	void statementVariableDeclared(std::string varType, std::string name){
		symbolTable.addVariable(currentScope, name, varType, false);
		if(varType == "int" ||
			varType == "string"){
			fprintf(output, "%s %s;\n", varType.c_str(), name.c_str());
		}else if(varType == "string"){
			
		}else{
			fprintf(output, "struct %s %s;\n", varType.c_str(), name.c_str());
			structObjInitialization(varType, name);
		}
	}
	
	void functionDeclaration(std::string returnType, std::string name, bool isStatic){
		std::string funcName = "";
		if(isStatic){
			if(!name.compare("main")){
				funcName = "_main";
			}else{
				funcName = funcName + "_" + classes.top() + "_" + name;
			}
			fprintf(output, "%s %s(", returnType.c_str(), funcName.c_str());				
			fprintf(functionHeadersFiles, "%s %s(", returnType.c_str(), funcName.c_str());				
		}else{
			if(!name.compare("main")){
				funcName = "_main";
			}else{
				funcName = funcName + "_" + classes.top() + "_" + name;
			}
			fprintf(output, "%s %s(", returnType.c_str(), funcName.c_str());	
			fprintf(functionHeadersFiles, "%s %s(", returnType.c_str(), funcName.c_str());	
		}
	}
	
	void addFormalArgument(std::string type, std::string name, bool first, bool isStatic){
		symbolTable.addVariable(currentScope, name, type, false, true);
		if(first){
			if(isStatic){
				fprintf(output, "%s %s", type.c_str(), name.c_str());
				fprintf(functionHeadersFiles, "%s %s", type.c_str(), name.c_str());
			}else{
				fprintf(output, "struct %s* _Object, %s %s", classes.top().c_str(), type.c_str(), name.c_str());
				fprintf(functionHeadersFiles, "struct %s* _Object, %s %s", classes.top().c_str(), type.c_str(), name.c_str());
			}
		}else{
			fprintf(output, ", %s %s", type.c_str(), name.c_str());
			fprintf(functionHeadersFiles, ", %s %s", type.c_str(), name.c_str());
		}
	}
	
	void addActualArgument(std::string name, bool first){
		std::string temp = functions.top();
		functions.pop();
		if(first){
			temp += name;
		}else{
			temp += ", " + name;
		}
		functions.push(temp);
	}
	
	void formalArgumentsEnd(){
		fprintf(output, ")");
		fprintf(functionHeadersFiles, ");\n");
	}
	
	void emptyFunction(bool isStatic){
		if(!isStatic){
			fprintf(output, "struct %s* _Object", classes.top().c_str());
			fprintf(functionHeadersFiles, "struct %s* _Object", classes.top().c_str());
		}
	}
	
	std::string functionConstructed(){
		std::string function = functions.top();
		functions.pop();
		function += ")";
		return function;
	}
	
	void addStatement(std::string statement){
		fprintf(output, "%s;\n", statement.c_str());
	}
	
	void closeParentheses(){
		fprintf(output, ")");
	}
	
	void openCurlyBracket(){
		fprintf(output, "{\n");
	}
	
	void closeCurlyBracket(){
		fprintf(output, "}\n\n");
	}
	
	void addSemiColon(){
		fprintf(output, ";\n");
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
	
	void setOutputFile(FILE* outputFile){
		output = outputFile;
	}
}
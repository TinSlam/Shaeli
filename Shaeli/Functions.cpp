#include "Functions.h"

namespace Shaeli{
	extern std::stack<std::string> functions;
	
	extern FILE* output;
	extern FILE* functionHeadersFiles;
	
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
		if(type != "int"){
			type = "struct " + type;
		}
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
}
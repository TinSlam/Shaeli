#include "Symbols.h"

namespace Shaeli{
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
}
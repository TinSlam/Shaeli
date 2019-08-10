#include "Expressions.h"

namespace Shaeli{
	std::string binaryOperation(std::string operation, std::string first, std::string second){
		if(operation == "^"){
			return "pow(" + first + ", " + second + ")";
		}else{
			return first + " " + operation + " " + second;
		}
	}
	
	std::string relationalOperation(std::string operation, std::string first, std::string second){
		return first + " " + operation + " " + second;
	}
	
	std::string comparisonOperation(std::string operation, std::string first, std::string second){
		return first + " " + operation + " " + second;
	}
	
	std::string bitwiseOperation(std::string operation, std::string first, std::string second){
		return first + " " + operation + " " + second;
	}
	
	std::string unaryOperation(std::string operation, std::string expression){
		return operation + " " + expression;
	}
}
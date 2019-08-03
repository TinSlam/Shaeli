#include "Expressions.h"

namespace Shaeli{
	std::string binaryOperation(std::string operation, std::string first, std::string second){
		if(operation == "^"){
			return "pow(" + first + ", " + second + ")";
		}else{
			return first + " " + operation + " " + second;
		}
	}
}
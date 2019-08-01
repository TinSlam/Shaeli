#include "Expressions.h"

namespace Shaeli{
	std::string binaryOperation(std::string operation, std::string second, std::string first){
		return first + " " + operation + " " + second;
	}
}
#pragma once

namespace Shaeli{
	std::string binaryOperation(std::string operation, std::string second, std::string first);
	std::string relationalOperation(std::string operation, std::string second, std::string first);
	std::string comparisonOperation(std::string operation, std::string second, std::string first);
	std::string bitwiseOperation(std::string operation, std::string second, std::string first);
	std::string unaryOperation(std::string operation, std::string expression);
}
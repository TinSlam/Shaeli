#pragma once

namespace Shaeli{
	bool staticVarNameMatches(Variable* var, std::string name);
	Variable* lookupStaticVariable(std::string simpleName);
	std::string getVariablesFullName(std::string name);
}

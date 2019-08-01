#pragma once

namespace Shaeli{
	void functionCall(std::string name);
	void functionCallClass(std::string name, std::string instanceName);
	void functionDeclaration(std::string returnType, std::string name, bool isStatic);
	void addFormalArgument(std::string type, std::string name, bool first, bool isStatic);
	void addActualArgument(std::string name, bool first);
	void formalArgumentsEnd();
	void emptyFunction(bool isStatic);
	std::string functionConstructed();
}
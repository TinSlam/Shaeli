#pragma once

namespace Shaeli{
	void closeParentheses();
	void openCurlyBracket();
	void closeCurlyBracket();
	void addSemiColon();
	void setOutputFile(FILE* outputFile);
	void referenceFile(std::string fileName);
	void generatePrecode();
	void generateMainFunction();
	void wrapUpFile();
}
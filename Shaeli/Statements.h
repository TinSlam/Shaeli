#pragma once

namespace Shaeli{
	void print(std::string data);
	void assignment(std::string right, std::string left);
	void addStatement(std::string statement);
	void returnStatement(std::string expression);
	void breakStatement();
	void continueStatement();
	void ifStatementFound(std::string expression);
	void elseIfStatementFound(std::string expression);
	void elseStatementFound();
	void emptyStatement();
	void whileStatementFound(std::string expression);
	void forStatementFound(std::string variable);
	void forStatementStartFound(std::string expression);
	void forExpressionFound(std::string expression);
	void forStepsFound(std::string expression);
}
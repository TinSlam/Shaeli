#pragma once

namespace Shaeli{
	class Variable{
		public :
			std::string lexeme;
			std::string type;
			bool isStatic;
			bool isTemp;
			
			Variable(std::string lexeme, std::string type, bool isStatic, bool isTemp);
	};

	class Scope{
		private :
			std::list<Variable*> variables;

		public :
			~Scope();
			Scope* parent = nullptr;
			void addVariable(Variable* variable);
			Variable* findVariable(std::string lexeme);
	};

	class SymbolTable{
		private :
			std::list<Scope*> scopes;

		public :
			~SymbolTable();
		
			Scope* addScope(Scope* parent);
			void addVariable(Scope* scope, std::string lexeme, std::string type, bool isStatic);
			void addVariable(Scope* scope, std::string lexeme, std::string type, bool isStatic, bool isTemp);
			Variable* lookupVariable(Scope* scope, std::string varName);
	};
}
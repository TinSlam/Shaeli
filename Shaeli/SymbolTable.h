#include <list>
#include <string>

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

Scope* SymbolTable::addScope(Scope* parent){
	Scope* scope = new Scope();
	scope->parent = parent;
	scopes.push_back(scope);
	return scope;
}

void SymbolTable::addVariable(Scope* scope, std::string lexeme, std::string type, bool isStatic){
	addVariable(scope, lexeme, type, isStatic, false);
}

void SymbolTable::addVariable(Scope* scope, std::string lexeme, std::string type, bool isStatic, bool isTemp){
	scope->addVariable(new Variable(lexeme, type, isStatic, isTemp));
}

Variable* SymbolTable::lookupVariable(Scope* scope, std::string varName){
	Variable* variable = nullptr;
	while(variable == nullptr && scope != nullptr){
		variable = scope->findVariable(varName);
		scope = scope->parent;
	}
	return variable;
}

Variable* Scope::findVariable(std::string lexeme){
	for(auto it = variables.begin(); it != variables.end(); it++){
		if((*it)->lexeme == lexeme) return *it;
	}
	return nullptr;
}

Variable::Variable(std::string lexeme, std::string type, bool isStatic, bool isTemp){
	this->lexeme = lexeme;
	this->type = type;
	this->isStatic = isStatic;
	this->isTemp = isTemp;
}

void Scope::addVariable(Variable* variable){
	variables.push_front(variable);
}

SymbolTable::~SymbolTable(){
	for(auto it = scopes.begin(); it != scopes.end(); it++){
		delete(*it);
	}
}

Scope::~Scope(){
	for(auto it = variables.begin(); it != variables.end(); it++){
		delete(*it);
	}
}
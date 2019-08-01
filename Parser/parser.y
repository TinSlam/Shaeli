%{

#include <string>
#include <iostream>
#include <stack>

#include "Shaeli/Shaeli.h"

extern std::string currentLexeme;
extern std::string lastLexeme;
extern int yylex();
extern FILE* yyin;

void yyerror(char const* error);
void setStatic();
void resetStatic();

FILE* outputFile;

std::string inputDirectory = "Input";
std::string outputDirectory = "Output";
std::string inputFile = "sample.shl";
std::string outputFileName = "output.c";

bool isStatic = false;
std::string varType = "";

std::stack<std::string> stack;
void push(std::string entry);
std::string pop();

%}

%start program

%token INTEGER
%token STRING
%token ID

%token CLASS
%token REFERENCE
%token STATIC
%token PRINT

%token INT
%token VOID

%token OP_ASSIGNMENT
%left OP_ADDITION

%token CUR_BRACE_OPEN
%token CUR_BRACE_CLOSE
%token PAR_OPEN
%token PAR_CLOSE

%token DOT
%token SEMI_COLON
%token COMMA

%%

program : macros classes

macros : macros macro
	|
	
macro : reference

reference : REFERENCE STRING { Shaeli::referenceFile(currentLexeme.c_str()); }

classes : classes class
	|

class : CLASS ID { Shaeli::enteredClass(currentLexeme); } CUR_BRACE_OPEN symbols_decls CUR_BRACE_CLOSE { Shaeli::classEnded(); }

symbols_decls : symbols_decls symbol_decl
	|

symbol_decl : var_dec
	| func_dec

var_dec : func_var_decl var_dec_m SEMI_COLON { Shaeli::varDecDone(isStatic); resetStatic(); }
	| func_var_decl var_dec_m OP_ASSIGNMENT expression { Shaeli::varDecAssigned(pop(), isStatic); resetStatic(); } SEMI_COLON

var_dec_m : { Shaeli::variableDeclaration(varType, currentLexeme, isStatic); } 

func_var_decl : declaration
	| STATIC { setStatic(); } declaration

func_dec : func_var_decl func_dec_m PAR_OPEN formal_arguments PAR_CLOSE block

func_dec_m : { Shaeli::functionDeclaration(varType, currentLexeme, isStatic); }

formal_arguments : formal_arguments_list { Shaeli::formalArgumentsEnd(); resetStatic(); }
	| { Shaeli::emptyFunction(isStatic); Shaeli::formalArgumentsEnd(); resetStatic(); }

formal_arguments_list : formal_arguments_list COMMA declaration { Shaeli::addFormalArgument(varType, currentLexeme, false, isStatic); }
	| declaration { Shaeli::addFormalArgument(varType, currentLexeme, true, isStatic); }

declaration : return_type ID

return_type : INT { varType = "int"; }
	| VOID { varType = "void"; }

block : CUR_BRACE_OPEN { Shaeli::openCurlyBracket(); } statements_list CUR_BRACE_CLOSE { Shaeli::closeCurlyBracket(); } 

statements_list : statements_list statement
	|
	
statement : assignment
	| print
	| expression SEMI_COLON { Shaeli::addStatement(pop()); }
	| statement_var_dec
	
statement_var_dec : return_type ID SEMI_COLON { Shaeli::statementVariableDeclared(varType, currentLexeme); }
	| return_type ID OP_ASSIGNMENT expression SEMI_COLON
	| ID { varType = lastLexeme; } ID SEMI_COLON { Shaeli::statementVariableDeclared(varType, currentLexeme); }
	
assignment : ID { push(Shaeli::getVariablesFullName(currentLexeme)); } OP_ASSIGNMENT expression SEMI_COLON { Shaeli::assignment(pop(), pop()); }
	
print : PRINT PAR_OPEN STRING PAR_CLOSE SEMI_COLON { Shaeli::print(currentLexeme); }
	
function_call : ID { Shaeli::functionCall(currentLexeme); } PAR_OPEN actual_arguments PAR_CLOSE
	| ID { push(currentLexeme); } DOT ID {Shaeli::functionCallClass(currentLexeme, pop());} PAR_OPEN actual_arguments PAR_CLOSE
	
actual_arguments : actual_arguments_list
	|
	
actual_arguments_list : actual_arguments_list COMMA expression { Shaeli::addActualArgument(pop(), false); }
	| expression { Shaeli::addActualArgument(pop(), true); }
	
expression : INTEGER { push(currentLexeme); }
	| STRING { std::string temp = "\"" + currentLexeme + "\""; push(temp); }
	| ID { push(Shaeli::getVariablesFullName(currentLexeme)); }
	| expression OP_ADDITION expression { push(Shaeli::binaryOperation("+", pop(), pop())); }
	| PAR_OPEN expression PAR_CLOSE
	| function_call { push(Shaeli::functionConstructed()); }
	
%%

int main(){
	fopen_s(&yyin, (inputDirectory + "/" + inputFile).c_str(), "r");
	fopen_s(&outputFile, (outputDirectory + "/" + outputFileName).c_str(), "w");
	
	Shaeli::setOutputFile(outputFile);
	Shaeli::generatePrecode();
	
	yyparse();
	
	Shaeli::generateMainFunction();
	Shaeli::wrapUpFile();
	
	fclose(outputFile);
	
	printf("C file successfully generated.\n");
	
	return 0;
}

void push(std::string entry){
	stack.push(entry);
}

std::string pop(){
	std::string returnedValue = stack.top();
	stack.pop();
	return returnedValue;
}

void setStatic(){
	isStatic = true;
}

void resetStatic(){
	isStatic = false;
}

void yyerror(const char* error){
	printf("Error : %s\n", error);
}
%{
#include "Shaeli/PrecompiledHeaders/PrecompiledHeaders.h"

#include "Shaeli/Shaeli.h"

extern std::string currentLexeme;
extern std::string lastLexeme;
extern int yylex();
extern int yylineno;
extern FILE* yyin;

void yyerror(char const* error);
void setStatic();
void resetStatic();

FILE* outputFile;

std::string inputDirectory = "Input";
std::string outputDirectory = "Output";
std::string inputFile = "Sample.shl";
std::string outputFileName = "Output.c";

bool isStatic = false;
std::string varType = "";
std::string currentSymbol = "";

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
%token RETURN
%token BREAK
%token CONTINUE
%token PRINT

%token IF
%token WHILE
%token FOR
%token IN
%token TO
%token STEPS

%token INT
%token VOID

%token OP_ASSIGNMENT

%left OR
%left AND
%left NOT

%left GE GT NE EQ LE LT

%left OP_ADDITION OP_SUBTRACTION
%left OP_MULTIPLICATION OP_DIVISION
%left OP_MODULO OP_POWER SHIFT_LEFT SHIFT_RIGHT

%left BITWISE_AND BITWISE_OR
%left BITWISE_NOT

%token CUR_BRACE_OPEN
%token CUR_BRACE_CLOSE
%token PAR_OPEN
%token PAR_CLOSE
	
%token DOT
%token SEMI_COLON
%token COMMA

%left ELFSEIF_PREC
%left ELSEIF
%left ELSE

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

formal_arguments_list : formal_arguments_list COMMA declaration { varType = lastLexeme; Shaeli::addFormalArgument(varType, currentLexeme, false, isStatic); }
	| declaration { varType = lastLexeme; Shaeli::addFormalArgument(varType, currentLexeme, true, isStatic); }

declaration : return_type ID

return_type : INT { varType = "int"; }
	| VOID { varType = "void"; }
	| ID { varType = lastLexeme; }

block : CUR_BRACE_OPEN { Shaeli::openCurlyBracket(); } statements_list CUR_BRACE_CLOSE { Shaeli::closeCurlyBracket(); } 
	| statement

statements_list : statements_list statement
	|
	
statement : assignment
	| print
	| expression SEMI_COLON { Shaeli::addStatement(pop()); }
	| statement_var_dec
	| if
	| for
	| while
	| return
	| break
	| continue
	| SEMI_COLON { Shaeli::emptyStatement(); }
	
return : RETURN expression SEMI_COLON { Shaeli::returnStatement(pop()); }

break : BREAK SEMI_COLON { Shaeli::breakStatement(); }

continue : CONTINUE SEMI_COLON { Shaeli::continueStatement(); }

while : WHILE PAR_OPEN expression while_m PAR_CLOSE block

while_m : { Shaeli::whileStatementFound(pop()); }

if : IF PAR_OPEN expression if_m PAR_CLOSE block elseifs %prec ELFSEIF_PREC
	| IF PAR_OPEN expression if_m PAR_CLOSE block elseifs ELSE else_m block

if_m : { Shaeli::ifStatementFound(pop()); }
	
elseifs : elseifs ELSEIF PAR_OPEN expression elseif_m PAR_CLOSE block
	|
	
elseif_m : { Shaeli::elseIfStatementFound(pop()); }

else_m : { Shaeli:: elseStatementFound(); }
	
for : FOR PAR_OPEN ID { Shaeli::forStatementFound(currentLexeme); } IN expression { Shaeli::forStatementStartFound(pop()); } TO expression { Shaeli::forExpressionFound(pop()); } STEPS expression { Shaeli::forStepsFound(pop()); } PAR_CLOSE block
	
statement_var_dec : return_type ID SEMI_COLON { Shaeli::statementVariableDeclared(varType, currentLexeme); }
	| return_type ID { currentSymbol = currentLexeme; Shaeli::statementVarDec(varType, currentLexeme); } OP_ASSIGNMENT expression SEMI_COLON { Shaeli::statementVarDecAssigned(pop(), varType, currentSymbol); }
	
assignment : lvalue OP_ASSIGNMENT expression SEMI_COLON { Shaeli::assignment(pop(), pop()); }
	
print : PRINT PAR_OPEN STRING PAR_CLOSE SEMI_COLON { Shaeli::print(currentLexeme); }
	
function_call : ID { Shaeli::functionCall(currentLexeme); } PAR_OPEN actual_arguments PAR_CLOSE
	| ID DOT ID {Shaeli::functionCallClass(currentLexeme, lastLexeme);} PAR_OPEN actual_arguments PAR_CLOSE
	
actual_arguments : actual_arguments_list
	|
	
actual_arguments_list : actual_arguments_list COMMA expression { Shaeli::addActualArgument(pop(), false); }
	| expression { Shaeli::addActualArgument(pop(), true); }
	
expression : INTEGER { push(currentLexeme); }
	| STRING { std::string temp = "\"" + currentLexeme + "\""; push(temp); }
	| lvalue
	| binary_operation
	| relational_operation
	| comparison_operation
	| bitwise_operation
	| unary_operation
	| PAR_OPEN expression PAR_CLOSE { push("(" + pop() + ")"); }
	| function_call { push(Shaeli::functionConstructed()); }
	
binary_operation : expression OP_ADDITION expression { push(Shaeli::binaryOperation("+", pop(), pop())); }
	| expression OP_SUBTRACTION expression { push(Shaeli::binaryOperation("-", pop(), pop())); }
	| expression OP_MULTIPLICATION expression { push(Shaeli::binaryOperation("*", pop(), pop())); }
	| expression OP_DIVISION expression { push(Shaeli::binaryOperation("/", pop(), pop())); }
	| expression OP_MODULO expression { push(Shaeli::binaryOperation("%", pop(), pop())); }
	| expression OP_POWER expression{ push(Shaeli::binaryOperation("^", pop(), pop())); }
	| expression SHIFT_LEFT expression{ push(Shaeli::binaryOperation("<<", pop(), pop())); }
	| expression SHIFT_RIGHT expression{ push(Shaeli::binaryOperation(">>", pop(), pop())); }
	
comparison_operation : expression GE expression { push(Shaeli::comparisonOperation(">=", pop(), pop())); }
	| expression GT expression { push(Shaeli::comparisonOperation(">", pop(), pop())); }
	| expression LE expression { push(Shaeli::comparisonOperation("<=", pop(), pop())); }
	| expression LT expression { push(Shaeli::comparisonOperation("<", pop(), pop())); }
	| expression EQ expression { push(Shaeli::comparisonOperation("==", pop(), pop())); }
	| expression NE expression { push(Shaeli::comparisonOperation("!=", pop(), pop())); }
	
relational_operation : expression AND expression { push(Shaeli::relationalOperation("&&", pop(), pop())); }
	| expression OR expression { push(Shaeli::relationalOperation("||", pop(), pop())); }

bitwise_operation : expression BITWISE_AND expression { push(Shaeli::bitwiseOperation("&", pop(), pop())); }
	| expression BITWISE_OR expression { push(Shaeli::bitwiseOperation("|", pop(), pop())); }
	
unary_operation : expression BITWISE_NOT expression { push(Shaeli::unaryOperation("~", pop())); }
	| expression NOT expression { push(Shaeli::unaryOperation("!", pop())); }

lvalue : ID { push(Shaeli::getVariablesFullName(currentLexeme)); }
	| ID DOT ID { push(lastLexeme + "." + currentLexeme); }
	
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
	printf("Error on line %d : %s\n", yylineno, error);
}
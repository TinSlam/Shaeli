%{

#include "Parser/parser.tab.h"
#include <string>
#include <stack>

std::string currentLexeme;
std::string lastLexeme;
std::stack<YY_BUFFER_STATE> openBuffers;

%}

%%

[ \t\n]	;

class	{
	return CLASS;
}

reference	{
	return REFERENCE;
}

static {
	return STATIC;
}

int	{
	return INT;
}

void {
	return VOID;
}

print	{
	return PRINT;
}

"="	{
	return OP_ASSIGNMENT;
}

"+"	{
	return OP_ADDITION;
}

"{"	{
	return CUR_BRACE_OPEN;
}

"}"	{
	return CUR_BRACE_CLOSE;
}

"("	{
	return PAR_OPEN;
}

")"	{
	return PAR_CLOSE;
}

[.]	{
	return DOT;
}

";"	{
	return SEMI_COLON;
}

","	{
	return COMMA;
}

[0-9]+	{
	lastLexeme = std::string(currentLexeme);
	currentLexeme = std::string(yytext);
	return INTEGER;
}

[a-zA-Z][a-zA-Z0-9_]*	{
	lastLexeme = std::string(currentLexeme);
	currentLexeme = std::string(yytext);
	return ID;
}

["][^"]*["]	{
	lastLexeme = std::string(currentLexeme);
	currentLexeme = std::string(yytext).substr(1, std::string(yytext).size() - 2);
	return STRING;
}

<<EOF>>	{
	if(openBuffers.size() > 0){
		yy_delete_buffer(YY_CURRENT_BUFFER);
		YY_BUFFER_STATE buffer = openBuffers.top();
		openBuffers.pop();
		yy_switch_to_buffer(buffer);
	}else{
		yyterminate();
	}
}

[^ \n\t.,;(){}\/=]+	{
	printf("Error on : %s\n", yytext);
}

%%

int yywrap(){
	return 1;
}

void parseFile(std::string fileName){
	fopen_s(&yyin, ("Input/" + fileName).c_str(), "r");
	
	openBuffers.push(YY_CURRENT_BUFFER);
	
	yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
	
	BEGIN(INITIAL);
}
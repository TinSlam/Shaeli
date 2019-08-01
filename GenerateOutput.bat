mkdir Output
flex Parser/scanner.l
bison Parser/parser.y
g++ lex.yy.c parser.tab.c -o target
target
gcc Output/output.c -o target
move target.exe Output
del /f lex.yy.c
del /f parser.tab.c
cd Output
del /f FunctionHeaders.h
del /f output.c
del /f ShaeliReferences.h
PAUSE
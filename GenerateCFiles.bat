mkdir Output
flex Parser/scanner.l
bison Parser/parser.y
g++ lex.yy.c parser.tab.c -o target
target
del /f lex.yy.c
del /f parser.tab.c
del /f target.exe
cd Output
del /f target.exe
PAUSE
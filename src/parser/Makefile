all: ram_parser

./bin/c/bison.tab.c ./bin/c/bison.tab.h: src/bison.y
	bison -Wcounterexamples -d -b ./bin/c/bison src/bison.y

./bin/c/lex.yy.c: src/flex.l ./bin/c/bison.tab.h
	flex  -o ./bin/c/lex.yy.c src/flex.l

ram_parser: ./bin/c/lex.yy.c ./bin/c/bison.tab.c ./bin/c/bison.tab.h

CC = g++ -O2 -Wno-deprecated

tag = -i

CURR_DIR = $(shell pwd)
BIN_DIR = $(CURR_DIR)/bin
SRC_DIR = $(CURR_DIR)/src

ifdef linux
tag = -n
endif

test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o Pipe.o y.tab.o lex.yy.o test.o
	$(CC) -o test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o Pipe.o y.tab.o lex.yy.o test.o -lfl -lpthread
	mv *.o $(BIN_DIR)

test.o: $(SRC_DIR)/test.cc
	$(CC) -g -c $(SRC_DIR)/test.cc

Comparison.o: $(SRC_DIR)/Comparison.cc
	$(CC) -g -c $(SRC_DIR)/Comparison.cc

ComparisonEngine.o: $(SRC_DIR)/ComparisonEngine.cc
	$(CC) -g -c $(SRC_DIR)/ComparisonEngine.cc

Pipe.o: $(SRC_DIR)/Pipe.cc
	$(CC) -g -c $(SRC_DIR)/Pipe.cc

BigQ.o: $(SRC_DIR)/BigQ.cc
	$(CC) -g -c $(SRC_DIR)/BigQ.cc

DBFile.o: $(SRC_DIR)/DBFile.cc
	$(CC) -g -c $(SRC_DIR)/DBFile.cc

File.o: $(SRC_DIR)/File.cc
	$(CC) -g -c $(SRC_DIR)/File.cc

Record.o: $(SRC_DIR)/Record.cc
	$(CC) -g -c $(SRC_DIR)/Record.cc

Schema.o: $(SRC_DIR)/Schema.cc
	$(CC) -g -c $(SRC_DIR)/Schema.cc

y.tab.o: $(SRC_DIR)/Parser.y
	yacc -d $(SRC_DIR)/Parser.y
	sed $(tag) $(SRC_DIR)/y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	g++ -c $(SRC_DIR)/y.tab.c

lex.yy.o: $(SRC_DIR)/Lexer.l
	lex  $(SRC_DIR)/Lexer.l
	gcc -c $(SRC_DIR)/lex.yy.c

clean:
	rm -f *.o
	rm -f *.out
	rm -f y.tab.c
	rm -f lex.yy.c
	rm -f y.tab.h
	rm -f $(BIN_DIR)/*.o
	rm -f $(BIN_DIR)/*.out


CC = g++ -std=c++0x -O2 -Wno-deprecated

tag = -i

CURR_DIR = $(shell pwd)
GTEST_DIR = $(CURR_DIR)/gtest
BIN_DIR = $(CURR_DIR)/bin
SRC_DIR = $(CURR_DIR)/src
DD = $(CC) -isystem $(GTEST_DIR)/include -Wall -Wextra -pthread

ifdef linux
tag = -n
endif

gTest21: libgtest.a sampletest21
	$(DD) $(SRC_DIR)/gTestProj21.cc $(BIN_DIR)/libgtest.a -o $(BIN_DIR)/gTestProj21 $(BIN_DIR)/Record.o $(BIN_DIR)/Comparison.o $(BIN_DIR)/ComparisonEngine.o $(BIN_DIR)/Schema.o $(BIN_DIR)/BigQ.o $(BIN_DIR)/Pipe.o $(BIN_DIR)/File.o $(BIN_DIR)/GenericDBFile.o $(BIN_DIR)/DBFile.o $(BIN_DIR)/HeapFile.o $(BIN_DIR)/SortedFile.o $(BIN_DIR)/y.tab.o $(BIN_DIR)/lex.yy.o

sampletest21: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o GenericDBFile.o DBFile.o HeapFile.o SortedFile.o Pipe.o gTestProj21.o y.tab.o lex.yy.o 
	mv *.o $(BIN_DIR)

gTest2: libgtest.a sampletest2
	$(DD) $(SRC_DIR)/gTestProj2.cc $(BIN_DIR)/libgtest.a -o $(BIN_DIR)/gTestProj2 $(BIN_DIR)/Record.o $(BIN_DIR)/Comparison.o $(BIN_DIR)/ComparisonEngine.o $(BIN_DIR)/Schema.o $(BIN_DIR)/BigQ.o $(BIN_DIR)/Pipe.o $(BIN_DIR)/File.o $(BIN_DIR)/GenericDBFile.o $(BIN_DIR)/DBFile.o $(BIN_DIR)/HeapFile.o $(BIN_DIR)/SortedFile.o $(BIN_DIR)/y.tab.o $(BIN_DIR)/lex.yy.o

sampletest2: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o GenericDBFile.o DBFile.o HeapFile.o SortedFile.o Pipe.o gTestProj21.o y.tab.o lex.yy.o 
	mv *.o $(BIN_DIR)

test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o GenericDBFile.o DBFile.o HeapFile.o SortedFile.o Pipe.o y.tab.o lex.yy.o test.o
	$(CC) -o test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o GenericDBFile.o DBFile.o HeapFile.o SortedFile.o Pipe.o y.tab.o lex.yy.o test.o -lfl -lpthread
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

HeapFile.o: $(SRC_DIR)/HeapFile.cc
	$(CC) -g -c $(SRC_DIR)/HeapFile.cc

SortedFile.o: $(SRC_DIR)/SortedFile.cc
	$(CC) -g -c $(SRC_DIR)/SortedFile.cc

GenericDBFile.o: $(SRC_DIR)/GenericDBFile.cc
	$(CC) -g -c $(SRC_DIR)/GenericDBFile.cc
 
File.o: $(SRC_DIR)/File.cc
	$(CC) -g -c $(SRC_DIR)/File.cc

Record.o: $(SRC_DIR)/Record.cc
	$(CC) -g -c $(SRC_DIR)/Record.cc

Schema.o: $(SRC_DIR)/Schema.cc
	$(CC) -g -c $(SRC_DIR)/Schema.cc

gTestProj21.o : $(SRC_DIR)/gTestProj21.cc
	$(CC) -g -c $(SRC_DIR)/gTestProj21.cc

libgtest.a: 
	g++ -w -isystem $(GTEST_DIR)/include -I$(GTEST_DIR) -pthread -c $(GTEST_DIR)/src/gtest-all.cc
	ar -rv libgtest.a gtest-all.o	
	mv libgtest.a $(CURR_DIR)/bin

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

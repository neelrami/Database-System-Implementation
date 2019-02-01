CC = g++ -O2 -Wno-deprecated

tag = -i

CURR_DIR = $(shell pwd)
GTEST_DIR = $(CURR_DIR)/gtest
BIN_DIR = $(CURR_DIR)/bin
SRC_DIR = $(CURR_DIR)/src
DD = $(CC) -isystem $(GTEST_DIR)/include -Wall -Wextra -pthread

ifdef linux
tag = -n
endif

gTest: libgtest.a sampletest
	$(DD) $(SRC_DIR)/gTestProj1.cc $(BIN_DIR)/libgtest.a -o $(BIN_DIR)/gTestProj1 $(BIN_DIR)/Record.o $(BIN_DIR)/Comparison.o $(BIN_DIR)/ComparisonEngine.o $(BIN_DIR)/Schema.o $(BIN_DIR)/File.o $(BIN_DIR)/DBFile.o  $(BIN_DIR)/y.tab.o $(BIN_DIR)/lex.yy.o

sampletest: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o gTestProj1.o y.tab.o lex.yy.o 
	mv *.o $(BIN_DIR)

test.out: Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o y.tab.o lex.yy.o test.o
	$(CC) -o test.out Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o y.tab.o lex.yy.o test.o -lfl
	
main: Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o
	$(CC) -o main Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o -lfl
	
test.o: $(SRC_DIR)/test.cc
	$(CC) -g -c $(SRC_DIR)/test.cc

main.o: $(SRC_DIR)/main.cc
	$(CC) -g -c $(SRC_DIR)/main.cc
	
Comparison.o: $(SRC_DIR)/Comparison.cc
	$(CC) -g -c $(SRC_DIR)/Comparison.cc
	
ComparisonEngine.o: $(SRC_DIR)/ComparisonEngine.cc
	$(CC) -g -c $(SRC_DIR)/ComparisonEngine.cc
	
DBFile.o: $(SRC_DIR)/DBFile.cc
	$(CC) -g -c $(SRC_DIR)/DBFile.cc

File.o: $(SRC_DIR)/File.cc
	$(CC) -g -c $(SRC_DIR)/File.cc

Record.o: $(SRC_DIR)/Record.cc
	$(CC) -g -c $(SRC_DIR)/Record.cc

Schema.o: $(SRC_DIR)/Schema.cc
	$(CC) -g -c $(SRC_DIR)/Schema.cc

gTestProj1.o : $(SRC_DIR)/gTestProj1.cc
	$(CC) -g -c $(SRC_DIR)/gTestProj1.cc

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
	gcc  -c $(SRC_DIR)/lex.yy.c

clean: 
	rm -f *.o
	rm -f *.out
	rm -f y.tab.c
	rm -f lex.yy.c
	rm -f y.tab.h
	rm -f $(BIN_DIR)/*.o
	rm -f $(BIN_DIR)/*.out
	rm -f $(BIN_DIR)/*.bin
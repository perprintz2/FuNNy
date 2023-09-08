# LEX YACC maker

LEX= flex
YACC= bison
LIBS=   -lfl -lm 
MLIBS = -lm 
CC= gcc 
CPPFLAGS=  -O -I./include 
VPATH = src
all: funny adapt.o matlib.o

funny: fuzzy.tab.o lex.yy.o codegen.o
	$(CC) -o $@ fuzzy.tab.o lex.yy.o codegen.o $(LIBS) 

codegen.o: src/codegen.c fuzzy-code include/symtab.h

fuzzy.tab.c "fuzzy.tab.h: src/fuzzy.y include/symtab.h include/codegen.h
	$(YACC) -t -d -v src/fuzzy.y

lex.yy.c: src/fuzzy.l fuzzy.tab.h include/symtab.h include/errnr.h
	$(LEX) src/fuzzy.l

adapt.o: src/adapt.c
matlib.o: src/matlib.c

tstapp: fuzout.o tstapp.o adapt.o matlib.o
	$(CC) -o $@ fuzout.o tstapp.o adapt.o matlib.o $(MLIBS) 

adapt.o: src/adapt.c include/adapt.h

bruapp: fuzout.o brusdemo.o adapt.o matlib.o
	$(CC) -o $@ fuzout.o brusdemo.o adapt.o matlib.o $(MLIBS) 

anfislearn.o: anfislearn.c fuzzy.h symtab.h

anfis: fuzout.o anfislearn.o adapt.o matlib.o
	$(CC) -o $@  fuzout.o anfislearn.o adapt.o  matlib.o $(MLIBS)

matlabapp.o: matlabapp.c  matx.h

genmat: fuzout.o matlabapp.o adapt.o matlib.o
	$(CC) -o $@ fuzout.o matlabapp.o adapt.o  matlib.o $(MLIBS)

clean:
	rm *.o fuzzy.tab.c lex.yy.c fuzzy.tab.h *.dat fuzzy.output

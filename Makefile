all: bstr-test-c bstr-test-cxx

CFLAGS=-Wall -std=c99 -g
OBJECTS=bstr.o bstr_utf.o
CC = gcc 

CXXFLAGS=-g -std=c++11 -g 
CXX = g++ 

bstr.o: bstr.c bstr.h bstr_utf.c bstr_utf.h
	$(CC) $(CFLAGS) -c bstr.c

bstr_utf.o: bstr_utf.c bstr_utf.h
	$(CC) $(CFLAGS) -c bstr_utf.c

bstr-test-c: bstr.o bstr_test.c 
	$(CC) $(OBJECTS) $(CFLAGS) -o bstr-test-c  bstr_test.c  

bstr-test-cxx: bstr.o bstr_test.cpp 
	$(CXX) $(OBJECTS) $(CXXFLAGS) -o bstr-test-cxx bstr_test.cpp 





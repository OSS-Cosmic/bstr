all: bstr-test

bstr-test: bstr.c bstr_test.c bstr.h bstr_utf8.c bstr_utf8.h 
	$(CC) -o bstr-test bstr_test.c bstr.c bstr_utf8.c -Wall -std=c99 -O2

bstr-test-debug: bstr.c bstr_test.c bstr.h  
	$(CC) -o bstr-test bstr_test.c bstr.c bstr_utf8.c -Wall -std=c99 -g


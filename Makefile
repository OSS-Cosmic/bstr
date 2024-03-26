all: bstr-test

bstr-test: bstr.c bstr_test.c bstr.h  
	$(CC) -o bstr-test bstr_test.c bstr.c -Wall -std=c99 -pedantic -fsanitize=address,undefined  -O1 
	@echo ">>> Type ./bstr-test to run the sds.c unit tests."

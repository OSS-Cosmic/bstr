all: bstr-test

bstr-test: bstr.c bstr_test.c bstr.h  
	$(CC) -o bstr-test bstr_test.c bstr.c -Wall -std=c99 -O2
	@echo ">>> Type ./bstr-test to run the sds.c unit tests."

bstr-test-debug: bstr.c bstr_test.c bstr.h  
	$(CC) -o bstr-test bstr_test.c bstr.c -Wall -std=c99 -g
	@echo ">>> Type ./bstr-test to run the sds.c unit tests."

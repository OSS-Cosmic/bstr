#include "bstr.h"
#include "utest.h"
#include "bstr.c"

UTEST(compare, bstrCaselessCompare ) {
  EXPECT_EQ(bstrCaselessCompare(BSTR_C_SLICE("test"), BSTR_C_SLICE("TEST")), 0);
  EXPECT_EQ(bstrCaselessCompare(BSTR_C_SLICE("testAA"), BSTR_C_SLICE("TEST")), 1);
}

UTEST(compare, bstrCompare  ) {
  EXPECT_EQ(bstrCompare(BSTR_C_SLICE("test"), BSTR_C_SLICE("TEST")), 32);
  EXPECT_EQ(bstrCompare(BSTR_C_SLICE("Test"), BSTR_C_SLICE("test")), -32);
}


UTEST(indexOf, bstrIndexOf) {
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("foo foo"), BSTR_C_SLICE("foo")), 0);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("banana"), BSTR_C_SLICE("ana")), 1);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("textbook"), BSTR_C_SLICE("book")), 4);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("abababcabababcbab"), BSTR_C_SLICE("ababc")), 2); 
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("This is a test string to see how Boyer-Moore handles longer searches"), BSTR_C_SLICE("searches")), 60);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("hello world"), BSTR_C_SLICE("hello")), 0);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("apple banana"), BSTR_C_SLICE("nana")), 8);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("goodbye"), BSTR_C_SLICE("bye")), 4);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("cat"), BSTR_C_SLICE("dog")), -1);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE(""), BSTR_C_SLICE("abc")), -1);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("hello"), BSTR_C_SLICE("")), -1);
  EXPECT_EQ(bstrIndexOf(BSTR_C_SLICE("abc"), BSTR_C_SLICE("abcd")), -1);

}

UTEST(indexOf, bstrLastIndexOf) {
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("foo foo"), BSTR_C_SLICE("foo")), 4);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("banana"), BSTR_C_SLICE("ana")), 3);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("textbook"), BSTR_C_SLICE("book")), 4);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("abababcabababcbab"), BSTR_C_SLICE("ababc")), 9); 
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("This is a test string to see how Boyer-Moore handles longer searches"), BSTR_C_SLICE("searches")), 60);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("hello world"), BSTR_C_SLICE("hello")), 0);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("apple banana"), BSTR_C_SLICE("nana")), 8);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("goodbye"), BSTR_C_SLICE("bye")), 4);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("cat"), BSTR_C_SLICE("dog")), -1);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE(""), BSTR_C_SLICE("abc")), -1);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("hello"), BSTR_C_SLICE("")), -1);
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("abc"), BSTR_C_SLICE("abcd")), -1);
}

UTEST_MAIN()

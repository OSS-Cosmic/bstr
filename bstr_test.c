#include "bstr.h"
#include "utest.h"
#include "bstr.c"

UTEST(bstr, bstrCaselessCompare ) {
  EXPECT_EQ(bstrCaselessCompare(BSTR_C_SLICE("test"), BSTR_C_SLICE("TEST")), 0);
  EXPECT_EQ(bstrCaselessCompare(BSTR_C_SLICE("testAA"), BSTR_C_SLICE("TEST")), 1);
}

UTEST(bstr, bstrCompare  ) {
  EXPECT_EQ(bstrCompare(BSTR_C_SLICE("test"), BSTR_C_SLICE("TEST")), 32);
  EXPECT_EQ(bstrCompare(BSTR_C_SLICE("Test"), BSTR_C_SLICE("test")), -32);
}

UTEST(bstr, bstrIndexOf) {
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

UTEST(bstr, bstrLastIndexOf) {
  EXPECT_EQ(bstrLastIndexOf(BSTR_C_SLICE("foo foo"), BSTR_C_SLICE(" ")), 3);
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

UTEST(bstr, bstrEq) {
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("Hello world"), BSTR_C_SLICE("Hello world")), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("Helloworld"), BSTR_C_SLICE("Hello world")), 0);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("Hello World"), BSTR_C_SLICE("Hello world")), 0);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("Hello"), BSTR_C_SLICE("Hello ")), 0);
}


UTEST(bstr, bstrIterateRev) {
  struct bstr_slice_s buf = BSTR_C_SLICE("one two three four five");
  struct bstr_split_iterable_s iterable = {
      .delim = BSTR_C_SLICE(" "),
      .buffer = buf,
      .cursor = buf.len 
  };
  struct bstr_slice_s s = {};
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("five"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("four"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("three"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("two"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("one"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 0);

}

UTEST(bstr, bstrIterate) {
  struct bstr_split_iterable_s iterable = {
      .delim = BSTR_C_SLICE(" "),
      .buffer = BSTR_C_SLICE("one two three four five"),
      .cursor = 0
  };
  struct bstr_slice_s s = {};
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("one"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("two"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("three"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("four"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(BSTR_C_SLICE("five"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 0);
}

UTEST(bstr, bstrCaselessEq) {
  EXPECT_EQ(bstrCaselessEq(BSTR_C_SLICE("Hello world"), BSTR_C_SLICE("Hello world")), 1);
  EXPECT_EQ(bstrCaselessEq(BSTR_C_SLICE("Helloworld"), BSTR_C_SLICE("Hello world")), 0);
  EXPECT_EQ(bstrCaselessEq(BSTR_C_SLICE("Hello World"), BSTR_C_SLICE("Hello world")), 1);
  EXPECT_EQ(bstrCaselessEq(BSTR_C_SLICE("Hello"), BSTR_C_SLICE("Hello ")), 0);
}

UTEST(bstr, appendSlice) {
  struct bstr_s buf = bstrCreate("Hello");
  EXPECT_EQ(bstrAppendSlice(&buf, BSTR_C_SLICE(" world")), BSTR_OK);
  EXPECT_EQ(bstrEq(BSTR_TO_SLICE(&buf), BSTR_C_SLICE("Hello world")), 1);
  bstrFree(&buf);
}

UTEST(bstr, bstrcatprintf) {
  struct bstr_s buf = bstrEmpty();
  EXPECT_EQ(bstrcatprintf(&buf, "Hello %s", "world"), BSTR_OK);
  EXPECT_EQ(bstrEq(BSTR_TO_SLICE(&buf), BSTR_C_SLICE("Hello world")), 1);
  bstrFree(&buf);

  EXPECT_EQ(bstrcatprintf(&buf, "%d", 123), BSTR_OK);
  EXPECT_EQ(bstrEq(BSTR_TO_SLICE(&buf), BSTR_C_SLICE("123")), 1);
  
  EXPECT_EQ(bstrcatprintf(&buf, " %lu", 156), BSTR_OK);
  EXPECT_EQ(bstrEq(BSTR_TO_SLICE(&buf), BSTR_C_SLICE("123 156")), 1);
  bstrFree(&buf);

  EXPECT_EQ(bstrcatprintf(&buf,"a%cb",0), BSTR_OK);
  EXPECT_EQ(bstrEq(BSTR_TO_SLICE(&buf), (struct bstr_slice_s){.buf = "a\0""b", .len = 3}), 1);
}

UTEST(bstr, updateLen) {
  struct bstr_s buf = bstrCreate("Hello World");
  buf.buf[5] = '\0';
  bstrUpdateLen(&buf);
  EXPECT_EQ(buf.len, 5);
  bstrFree(&buf);
}

UTEST(bstr, assign) {
  struct bstr_s buf = bstrCreate("");
  bstrAssign(&buf, BSTR_C_SLICE("Hello World"));
  EXPECT_EQ(bstrEq(BSTR_TO_SLICE(&buf), BSTR_C_SLICE("Hello world")), BSTR_OK);

  bstrFree(&buf);
}


UTEST_MAIN()

#include "bstr.h"
#include "bstr.c"

#include "utest.h"

UTEST(bstr, bstrCaselessCompare ) {
  EXPECT_EQ(bstrCaselessCompare(CSTR_TO_CONSTSLICE("test"), CSTR_TO_CONSTSLICE("TEST")), 0);
  EXPECT_EQ(bstrCaselessCompare(CSTR_TO_CONSTSLICE("testAA"), CSTR_TO_CONSTSLICE("TEST")), 1);
}

UTEST(bstr, bstrCompare  ) {
  EXPECT_EQ(bstrCompare(CSTR_TO_CONSTSLICE("test"), CSTR_TO_CONSTSLICE("TEST")), 32);
  EXPECT_EQ(bstrCompare(CSTR_TO_CONSTSLICE("Test"), CSTR_TO_CONSTSLICE("test")), -32);
}

UTEST(bstr, bstrIndexOf) {
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("foo foo"), CSTR_TO_CONSTSLICE("foo")), 0);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("banana"), CSTR_TO_CONSTSLICE("ana")), 1);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("textbook"), CSTR_TO_CONSTSLICE("book")), 4);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("abababcabababcbab"), CSTR_TO_CONSTSLICE("ababc")), 2); 
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("This is a test string to see how Boyer-Moore handles longer searches"), CSTR_TO_CONSTSLICE("searches")), 60);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("hello world"), CSTR_TO_CONSTSLICE("hello")), 0);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("apple banana"), CSTR_TO_CONSTSLICE("nana")), 8);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("goodbye"), CSTR_TO_CONSTSLICE("bye")), 4);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("cat"), CSTR_TO_CONSTSLICE("dog")), -1);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE(""), CSTR_TO_CONSTSLICE("abc")), -1);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("hello"), CSTR_TO_CONSTSLICE("")), -1);
  EXPECT_EQ(bstrIndexOf(CSTR_TO_CONSTSLICE("abc"), CSTR_TO_CONSTSLICE("abcd")), -1);

}

UTEST(bstr, bstrLastIndexOf) {
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("foo foo"), CSTR_TO_CONSTSLICE(" ")), 3);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("foo foo"), CSTR_TO_CONSTSLICE("foo")), 4);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("banana"), CSTR_TO_CONSTSLICE("ana")), 3);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("textbook"), CSTR_TO_CONSTSLICE("book")), 4);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("abababcabababcbab"), CSTR_TO_CONSTSLICE("ababc")), 9); 
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("This is a test string to see how Boyer-Moore handles longer searches"), CSTR_TO_CONSTSLICE("searches")), 60);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("hello world"), CSTR_TO_CONSTSLICE("hello")), 0);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("apple banana"), CSTR_TO_CONSTSLICE("nana")), 8);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("goodbye"), CSTR_TO_CONSTSLICE("bye")), 4);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("cat"), CSTR_TO_CONSTSLICE("dog")), -1);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE(""), CSTR_TO_CONSTSLICE("abc")), -1);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("hello"), CSTR_TO_CONSTSLICE("")), -1);
  EXPECT_EQ(bstrLastIndexOf(CSTR_TO_CONSTSLICE("abc"), CSTR_TO_CONSTSLICE("abcd")), -1);
}

UTEST(bstr, bstrEq) {
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("Hello world"), CSTR_TO_CONSTSLICE("Hello world")), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("Helloworld"), CSTR_TO_CONSTSLICE("Hello world")), 0);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("Hello World"), CSTR_TO_CONSTSLICE("Hello world")), 0);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("Hello"), CSTR_TO_CONSTSLICE("Hello ")), 0);
}


UTEST(bstr, bstrIterateRev) {
  struct bstr_const_slice_s buf = CSTR_TO_CONSTSLICE("one two three four five");
  struct bstr_split_iterable_s iterable = {
      .delim = CSTR_TO_CONSTSLICE(" "),
      .buffer = buf,
      .cursor = buf.len 
  };
  struct bstr_const_slice_s s = {};
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("five"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("four"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("three"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("two"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("one"), s), 1);
  s = bstrSplitIterReverse(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 0);

}

UTEST(bstr, bstrReadull) {
  unsigned long long res = 0;
  EXPECT_EQ(bstrReadull(CSTR_TO_CONSTSLICE("123456"), 10, &res), 1);
  EXPECT_EQ(res, 123456);
}

UTEST(bstr, bstrReadu32) {
  long long res = 0;
  EXPECT_EQ(bstrReadll(CSTR_TO_CONSTSLICE("123456"), 10, &res), 1);
  EXPECT_EQ(res, 123456);
}

UTEST(bstr, bstrIterate) {
  struct bstr_split_iterable_s iterable = {
      .delim = CSTR_TO_CONSTSLICE(" "),
      .buffer = CSTR_TO_CONSTSLICE("one two three four five"),
      .cursor = 0
  };
  struct bstr_const_slice_s s = {};
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("one"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("two"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("three"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("four"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 1);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("five"), s), 1);
  s = bstrSplitItr(&iterable);
  EXPECT_EQ(bstrSliceValid(s), 0);
}

UTEST(bstr, bstrfmtWriteLongLong) {
  char test_buffer[BSTR_LLSTR_SIZE];
  size_t len = bstrfmtll((struct bstr_slice_s){
    .buf = test_buffer,
    .len = BSTR_LLSTR_SIZE
  }, 12481);
  EXPECT_EQ(len, 5);
  EXPECT_EQ(bstrEq(CSTR_TO_CONSTSLICE("12481"), (struct bstr_const_slice_s){
    .buf = test_buffer,
    .len = len
  }), true);
}

UTEST(bstr, bstrCaselessEq) {
  EXPECT_EQ(bstrCaselessEq(CSTR_TO_CONSTSLICE("Hello world"), CSTR_TO_CONSTSLICE("Hello world")), 1);
  EXPECT_EQ(bstrCaselessEq(CSTR_TO_CONSTSLICE("Helloworld"), CSTR_TO_CONSTSLICE("Hello world")), 0);
  EXPECT_EQ(bstrCaselessEq(CSTR_TO_CONSTSLICE("Hello World"), CSTR_TO_CONSTSLICE("Hello world")), 1);
  EXPECT_EQ(bstrCaselessEq(CSTR_TO_CONSTSLICE("Hello"), CSTR_TO_CONSTSLICE("Hello ")), 0);
}

UTEST(bstr, bstrCatJoin) {
  struct bstr_s buf ={};
  struct bstr_const_slice_s slices[] = {
    CSTR_TO_CONSTSLICE("one"),
    CSTR_TO_CONSTSLICE("two"),
    CSTR_TO_CONSTSLICE("three"),
    CSTR_TO_CONSTSLICE("four"),
  };
  EXPECT_EQ(bstrCatJoin(&buf, slices, 4, CSTR_TO_CONSTSLICE(" ")), true);
  EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("one two three four")), true);
  bstrFree(&buf);
}

UTEST(bstr, bstrCatJoinCstr) {
  struct bstr_s buf ={};
  char* slices[] = {
    "one",
    "two",
    "three",
    "four",
  };
  EXPECT_EQ(bstrCatJoinCStr(&buf, slices, 4, CSTR_TO_CONSTSLICE(" ")), true);
  EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("one two three four")), true);
  bstrFree(&buf);
}


UTEST(bstr, appendSlice) {
  struct bstr_s buf = {};
  bstrAssign(&buf, CSTR_TO_CONSTSLICE("Hello"));
  EXPECT_EQ(bstrAppendSlice(&buf, CSTR_TO_CONSTSLICE(" world")), true);
  EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("Hello world")), true);
  bstrFree(&buf);
}

UTEST(bstr, bstrcatprintf) {
  struct bstr_s buf = {0};
  EXPECT_EQ(bstrcatprintf(&buf, "Hello %s", "world"), true);
  EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("Hello world")), true);
  bstrFree(&buf);

  EXPECT_EQ(bstrcatprintf(&buf, "%d", 123), true);
  EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("123")), true);
  
  EXPECT_EQ(bstrcatprintf(&buf, " %lu", 156), true);
  EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("123 156")), true);
  bstrFree(&buf);

  EXPECT_EQ(bstrcatprintf(&buf,"a%cb",0), true);
  EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), (struct bstr_const_slice_s){.buf = "a\0""b", .len = 3}), 1);
}

UTEST(bstr, bstrcatfmt) {
   struct bstr_s s = {0};
   {
     bstrcatfmt(&s, "%S\n", CSTR_TO_CONSTSLICE("Hello World"));
     EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(s), CSTR_TO_CONSTSLICE("Hello World\n")), true );
     bstrClear(&s);
   }
   {
     bstrcatfmt(&s, "%i\n", 125);
     EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(s), CSTR_TO_CONSTSLICE("125\n")), true );
     bstrClear(&s);
   }
   {
     bstrcatfmt(&s, "%i\n", -125);
     EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(s), CSTR_TO_CONSTSLICE("-125\n")), true );
     bstrClear(&s);
   }
   bstrFree(&s);
}

UTEST(bstr, updateLen) {
  struct bstr_s buf = {};
  bstrAssign(&buf, CSTR_TO_CONSTSLICE("Hello World"));
  buf.buf[5] = '\0';
  bstrUpdateLen(&buf);
 
  EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("Hello")), true );
  EXPECT_EQ(buf.len, 5);

  bstrFree(&buf);
}

UTEST(bstr, bstrAssign) {
  struct bstr_s buf = {};
  {
    bstrAssign(&buf, CSTR_TO_CONSTSLICE("Hello World"));
    EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("Hello World")), true);
  }
  {
    bstrAssign(&buf, bstrTrim(CSTR_TO_CONSTSLICE("   Hello World   ")));
    EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("Hello World")), true);
  }
  bstrFree(&buf);

}

UTEST(bstr, bstrRightTrim) {
  EXPECT_EQ(bstrEq(bstrRightTrim(CSTR_TO_CONSTSLICE("Hello world  ")), CSTR_TO_CONSTSLICE("Hello world")), true);
  EXPECT_EQ(bstrEq(bstrRightTrim(CSTR_TO_CONSTSLICE("  Hello world  ")), CSTR_TO_CONSTSLICE("  Hello world")), true);
  EXPECT_EQ(bstrEq(bstrRightTrim(CSTR_TO_CONSTSLICE("  ")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrRightTrim(CSTR_TO_CONSTSLICE(" ")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrRightTrim(CSTR_TO_CONSTSLICE("\n")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrRightTrim(CSTR_TO_CONSTSLICE("\t")), CSTR_TO_CONSTSLICE("")), true);
}

UTEST(bstr, bstrLeftTrim) {
  EXPECT_EQ(bstrEq(bstrLeftTrim(CSTR_TO_CONSTSLICE("Hello world  ")), CSTR_TO_CONSTSLICE("Hello world  ")), true);
  EXPECT_EQ(bstrEq(bstrLeftTrim(CSTR_TO_CONSTSLICE("  ")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrLeftTrim(CSTR_TO_CONSTSLICE(" ")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrLeftTrim(CSTR_TO_CONSTSLICE("\n")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrLeftTrim(CSTR_TO_CONSTSLICE("\t")), CSTR_TO_CONSTSLICE("")), true);
}

UTEST(bstr, bstrTrim) {
  {
    struct bstr_s buf = {};
    bstrAssign(&buf, CSTR_TO_CONSTSLICE("  Hello World "));
    bstrAssign(&buf, bstrTrim(BSTR_TO_CONSTSLICE(buf)));
    EXPECT_EQ(bstrEq(BSTR_TO_CONSTSLICE(buf), CSTR_TO_CONSTSLICE("Hello World")), true);
    bstrFree(&buf);
  }


  EXPECT_EQ(bstrEq(bstrTrim(CSTR_TO_CONSTSLICE("Hello world  ")), CSTR_TO_CONSTSLICE("Hello world")), true);
  EXPECT_EQ(bstrEq(bstrTrim(CSTR_TO_CONSTSLICE("  \t Hello world  ")), CSTR_TO_CONSTSLICE("Hello world")), true);
  EXPECT_EQ(bstrEq(bstrTrim(CSTR_TO_CONSTSLICE("  ")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrTrim(CSTR_TO_CONSTSLICE(" ")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrTrim(CSTR_TO_CONSTSLICE("\n")), CSTR_TO_CONSTSLICE("")), true);
  EXPECT_EQ(bstrEq(bstrTrim(CSTR_TO_CONSTSLICE("\t")), CSTR_TO_CONSTSLICE("")), true);
}
UTEST_MAIN()

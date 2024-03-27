#include "utest.h"

#include "bstr.h"

UTEST(bstr, bstrCaselessCompare ) {
  EXPECT_EQ(bstrCaselessCompare(bstr_const_ref("test"), bstr_const_ref("TEST")), 0);
  EXPECT_EQ(bstrCaselessCompare(bstr_const_ref("testAA"), bstr_const_ref("TEST")), 1);
}

UTEST(bstr, bstrCompare  ) {
  EXPECT_EQ(bstrCompare(bstr_const_ref("test"), bstr_const_ref("TEST")), 32);
  EXPECT_EQ(bstrCompare(bstr_const_ref("Test"), bstr_const_ref("test")), -32);
}

UTEST(bstr, bstrIndexOf) {
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("foo foo"), bstr_const_ref("foo")), 0);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("banana"), bstr_const_ref("ana")), 1);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("textbook"), bstr_const_ref("book")), 4);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("abababcabababcbab"), bstr_const_ref("ababc")), 2); 
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("This is a test string to see how Boyer-Moore handles longer searches"), bstr_const_ref("searches")), 60);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("hello world"), bstr_const_ref("hello")), 0);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("apple banana"), bstr_const_ref("nana")), 8);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("goodbye"), bstr_const_ref("bye")), 4);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("cat"), bstr_const_ref("dog")), -1);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref(""), bstr_const_ref("abc")), -1);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("hello"), bstr_const_ref("")), -1);
  EXPECT_EQ(bstrIndexOf(bstr_const_ref("abc"), bstr_const_ref("abcd")), -1);

}

UTEST(bstr, bstrLastIndexOf) {
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("foo foo"), bstr_const_ref(" ")), 3);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("foo foo"), bstr_const_ref("foo")), 4);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("banana"), bstr_const_ref("ana")), 3);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("textbook"), bstr_const_ref("book")), 4);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("abababcabababcbab"), bstr_const_ref("ababc")), 9); 
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("This is a test string to see how Boyer-Moore handles longer searches"), bstr_const_ref("searches")), 60);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("hello world"), bstr_const_ref("hello")), 0);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("apple banana"), bstr_const_ref("nana")), 8);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("goodbye"), bstr_const_ref("bye")), 4);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("cat"), bstr_const_ref("dog")), -1);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref(""), bstr_const_ref("abc")), -1);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("hello"), bstr_const_ref("")), -1);
  EXPECT_EQ(bstrLastIndexOf(bstr_const_ref("abc"), bstr_const_ref("abcd")), -1);
}

UTEST(bstr, bstrEq) {
  EXPECT_EQ(bstreq(bstr_const_ref("Hello world"), bstr_const_ref("Hello world")), 1);
  EXPECT_EQ(bstreq(bstr_const_ref("Helloworld"), bstr_const_ref("Hello world")), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("Hello World"), bstr_const_ref("Hello world")), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("Hello"), bstr_const_ref("Hello ")), 0);
}

UTEST(bstr, bstrDuplicate) {
  struct bstr_s s1 = {0};
  EXPECT_EQ(bstrAssign(&s1, bstr_const_ref("Hello World")), true);
  struct bstr_s s2 = bstrDup(&s1);
  EXPECT_NE(s1.buf, s2.buf);
  bstrfree(&s1);
  bstrfree(&s2);
}

UTEST(bstr, bstrIterateRev) {
  struct bstr_const_slice_s buf = bstr_const_ref("one two three four five");
  struct bstr_split_iterable_s iterable = {
      .delim = bstr_const_ref(" "),
      .buffer = buf,
      .cursor = buf.len 
  };
  struct bstr_const_slice_s s = {0};
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("five"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("four"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("three"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("two"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("one"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 1);

}

UTEST(bstr, bstrReadull) {
  unsigned long long res = 0;
  EXPECT_EQ(bstrReadull(bstr_const_ref("123456"), 10, &res), 1);
  EXPECT_EQ(res, 123456);
}

UTEST(bstr, bstrReadu32) {
  long long res = 0;
  EXPECT_EQ(bstrReadll(bstr_const_ref("123456"), 10, &res), 1);
  EXPECT_EQ(res, 123456);
}

UTEST(bstr, bstrIterate) {
  struct bstr_split_iterable_s iterable = {
      .delim = bstr_const_ref(" "),
      .buffer = bstr_const_ref("one two three four five"),
      .cursor = 0
  };
  struct bstr_const_slice_s s = {0};
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("one"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("two"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("three"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("four"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstreq(bstr_const_ref("five"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 1);
}

UTEST(bstr, bstrfmtWriteLongLong) {
  char test_buffer[BSTR_LLSTR_SIZE];
  size_t len = bstrfmtll((struct bstr_slice_s){
    .buf = test_buffer,
    .len = BSTR_LLSTR_SIZE
  }, 12481);
  EXPECT_EQ(len, 5);
  EXPECT_EQ(bstreq(bstr_const_ref("12481"), (struct bstr_const_slice_s){
    .buf = test_buffer,
    .len = len
  }), true);
}

UTEST(bstr, bstrCaselessEq) {
  EXPECT_EQ(bstrCaselessEq(bstr_const_ref("Hello world"), bstr_const_ref("Hello world")), 1);
  EXPECT_EQ(bstrCaselessEq(bstr_const_ref("Helloworld"), bstr_const_ref("Hello world")), 0);
  EXPECT_EQ(bstrCaselessEq(bstr_const_ref("Hello World"), bstr_const_ref("Hello world")), 1);
  EXPECT_EQ(bstrCaselessEq(bstr_const_ref("Hello"), bstr_const_ref("Hello ")), 0);
}

UTEST(bstr, bstrCatJoin) {
  struct bstr_s buf ={0};
  struct bstr_const_slice_s slices[] = {
    bstr_const_ref("one"),
    bstr_const_ref("two"),
    bstr_const_ref("three"),
    bstr_const_ref("four"),
  };
  EXPECT_EQ(bstrcatjoin(&buf, slices, 4, bstr_const_ref(" ")), true);
  EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("one two three four")), true);
  bstrfree(&buf);
}

UTEST(bstr, bstrCatJoinCstr) {
  struct bstr_s buf ={0};
  char* slices[] = {
    "one",
    "two",
    "three",
    "four",
  };
  EXPECT_EQ(bstrCatJoinCStr(&buf, slices, 4, bstr_const_ref(" ")), true);
  EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("one two three four")), true);
  bstrfree(&buf);
}


UTEST(bstr, appendSlice) {
  struct bstr_s buf = {0};
  bstrAssign(&buf, bstr_const_ref("Hello"));
  EXPECT_EQ(bstrAppendSlice(&buf, bstr_const_ref(" world")), true);
  EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("Hello world")), true);
  bstrfree(&buf);
}

UTEST(bstr, bstrcatprintf) {
  struct bstr_s buf = {0};
  EXPECT_EQ(bstrcatprintf(&buf, "Hello %s", "world"), true);
  EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("Hello world")), true);

  bstrsetlen(&buf, 0);
  EXPECT_EQ(bstrcatprintf(&buf, "%d", 123), true);
  EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("123")), true);
  
  EXPECT_EQ(bstrcatprintf(&buf, " %lu", 156), true);
  EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("123 156")), true);

  bstrsetlen(&buf, 0);
  EXPECT_EQ(bstrcatprintf(&buf,"a%cb",0), true);
  EXPECT_EQ(bstreq(bstr_const_ref(buf), (struct bstr_const_slice_s){.buf = "a\0""b", .len = 3}), 1);
  bstrfree(&buf);
}

UTEST(bstr, bstrcatfmt) {
   struct bstr_s s = {0};
   {
     bstrcatfmt(&s, "%S\n", bstr_const_ref("Hello World"));
     EXPECT_EQ(bstreq(bstr_const_ref(s), bstr_const_ref("Hello World\n")), true );
     bstr_clear(&s);
   }
   {
     bstrcatfmt(&s, "%i\n", 125);
     EXPECT_EQ(bstreq(bstr_const_ref(s), bstr_const_ref("125\n")), true );
     bstr_clear(&s);
   }
   {
     bstrcatfmt(&s, "%i\n", -125);
     EXPECT_EQ(bstreq(bstr_const_ref(s), bstr_const_ref("-125\n")), true );
     bstr_clear(&s);
   }
   bstrfree(&s);
}

UTEST(bstr, updateLen) {
  struct bstr_s buf = {0};
  bstrAssign(&buf, bstr_const_ref("Hello World"));
  buf.buf[5] = '\0';
  bstrUpdateLen(&buf);
 
  EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("Hello")), true );
  EXPECT_EQ(buf.len, 5);

  bstrfree(&buf);
}

UTEST(bstr, bstrAssign) {
  struct bstr_s buf = {0};
  {
    bstrAssign(&buf, bstr_const_ref("Hello World"));
    EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("Hello World")), true);
  }
  {
    bstrAssign(&buf, bstrtrim(bstr_const_ref("   Hello World   ")));
    EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("Hello World")), true);
  }
  bstrfree(&buf);

}

UTEST(bstr, bstr_rtrim) {
  EXPECT_EQ(bstreq(bstrrtrim(bstr_const_ref("Hello world  ")), bstr_const_ref("Hello world")), true);
  EXPECT_EQ(bstreq(bstrrtrim(bstr_const_ref("  Hello world  ")), bstr_const_ref("  Hello world")), true);
  EXPECT_EQ(bstreq(bstrrtrim(bstr_const_ref("  ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrrtrim(bstr_const_ref(" ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrrtrim(bstr_const_ref("\n")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrrtrim(bstr_const_ref("\t")), bstr_const_ref("")), true);
}

UTEST(bstr, bstr_ltrim) {
  EXPECT_EQ(bstreq(bstrltrim(bstr_const_ref("Hello world  ")), bstr_const_ref("Hello world  ")), true);
  EXPECT_EQ(bstreq(bstrltrim(bstr_const_ref("  ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrltrim(bstr_const_ref(" ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrltrim(bstr_const_ref("\n")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrltrim(bstr_const_ref("\t")), bstr_const_ref("")), true);
}

UTEST(bstr, bstrTrim) {
  {
    struct bstr_s buf = {0};
    bstrAssign(&buf, bstr_const_ref("  Hello World "));
    bstrAssign(&buf, bstrtrim(bstr_const_ref(buf)));
    EXPECT_EQ(bstreq(bstr_const_ref(buf), bstr_const_ref("Hello World")), true);
    bstrfree(&buf);
  }

  EXPECT_EQ(bstreq(bstrtrim(bstr_const_ref("Hello world  ")), bstr_const_ref("Hello world")), true);
  EXPECT_EQ(bstreq(bstrtrim(bstr_const_ref("  \t Hello world  ")), bstr_const_ref("Hello world")), true);
  EXPECT_EQ(bstreq(bstrtrim(bstr_const_ref("  ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrtrim(bstr_const_ref(" ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrtrim(bstr_const_ref("\n")), bstr_const_ref("")), true);
  EXPECT_EQ(bstreq(bstrtrim(bstr_const_ref("\t")), bstr_const_ref("")), true);
}
UTEST_MAIN()

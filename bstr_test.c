#include "utest.h"

#include "bstr.h"
#include "bstr_utf.h"
#include <stdbool.h>

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
  EXPECT_EQ(bstrEqual(bstr_const_ref("Hello world"), bstr_const_ref("Hello world")), 1);
  EXPECT_EQ(bstrEqual(bstr_const_ref("Helloworld"), bstr_const_ref("Hello world")), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("Hello World"), bstr_const_ref("Hello world")), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("Hello"), bstr_const_ref("Hello ")), 0);
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
      .buffer = buf,
      .delim = bstr_const_ref(" "),
      .cursor = buf.len 
  };
  struct bstr_const_slice_s s = {0};
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("five"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("four"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("three"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("two"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("one"), s), 1);
  s = bstrSplitRevIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 1);

}

UTEST(bstr, bstrReadull) {
  unsigned long long res = 0;
  EXPECT_EQ(bstrReadull(bstr_const_ref("123456"), &res), 1);
  EXPECT_EQ(res, 123456);
}

UTEST(bstr, bstrReadu32) {
  long long res = 0;
  EXPECT_EQ(bstrReadll(bstr_const_ref("123456"), &res), 1);
  EXPECT_EQ(res, 123456);
}

UTEST(bstr, bstrIterateWhiteSpace) {
  struct bstr_split_iterable_s iterable = {
      .buffer = bstr_const_ref("one  two"),
      .delim = bstr_const_ref(" "),
      .cursor = 0
  };
  struct bstr_const_slice_s s = {0};
  
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstr_iter_has_more(iterable), 1);
  EXPECT_EQ(bstrEqual(bstr_const_ref("one"), s), 1);

  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 1);
  EXPECT_EQ(bstr_iter_has_more(iterable), 1);
  
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstrEqual(bstr_const_ref("two"), s), 1);
  EXPECT_EQ(bstr_iter_has_more(iterable), 0);
  
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 1);
  EXPECT_EQ(bstr_iter_has_more(iterable), 0);

}

UTEST(bstr, bstrIterateCount) { 
  {
    struct bstr_split_iterable_s iterable = {
        .buffer = bstr_const_ref("one two three four five"),
        .delim = bstr_const_ref(" "),
        .cursor = 0
    };
    struct bstr_const_slice_s slices[] = {
      bstr_const_ref("one"),
      bstr_const_ref("two"),
      bstr_const_ref("three"),
      bstr_const_ref("four"),
      bstr_const_ref("five"),
    };
    size_t i = 0;
    for(struct bstr_const_slice_s it = bstrSplitIter(&iterable);
        bstr_iter_has_more(iterable);  it = bstrSplitIter(&iterable)) {
      EXPECT_EQ(bstrEqual(slices[i++], it), 1);
    }
  }
  {
    struct bstr_split_iterable_s iterable = {
        .buffer = bstr_const_ref("one   two"),
        .delim = bstr_const_ref(" "),
        .cursor = 0
    };
    struct bstr_const_slice_s slices[] = {
      bstr_const_ref("one"),
      bstr_const_ref(""),
      bstr_const_ref(""),
      bstr_const_ref("two"),
    };
    size_t i = 0;
    for(struct bstr_const_slice_s it = bstrSplitIter(&iterable);
        bstr_iter_has_more(iterable);  it = bstrSplitIter(&iterable)) {
      EXPECT_EQ(bstrEqual(slices[i++], it), 1);
    }
  }
}

UTEST(bstr, bstrIterate) {
  struct bstr_split_iterable_s iterable = {
      .buffer = bstr_const_ref("one two three four five"),
      .delim = bstr_const_ref(" "),
      .cursor = 0
  };


  struct bstr_const_slice_s s = {0};
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("one"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("two"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("three"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("four"), s), 1);
  s = bstrSplitIter(&iterable);
  EXPECT_EQ(bstr_is_empty(s), 0);
  EXPECT_EQ(bstrEqual(bstr_const_ref("five"), s), 1);
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
  EXPECT_EQ(bstrEqual(bstr_const_ref("12481"), (struct bstr_const_slice_s){
    .buf = test_buffer,
    .len = len
  }), true);
}

UTEST(bstr, bstrCaselessEq) {
  EXPECT_EQ(bstrCaselessEqual(bstr_const_ref("Hello world"), bstr_const_ref("Hello world")), 1);
  EXPECT_EQ(bstrCaselessEqual(bstr_const_ref("Helloworld"), bstr_const_ref("Hello world")), 0);
  EXPECT_EQ(bstrCaselessEqual(bstr_const_ref("Hello World"), bstr_const_ref("Hello world")), 1);
  EXPECT_EQ(bstrCaselessEqual(bstr_const_ref("Hello"), bstr_const_ref("Hello ")), 0);
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
  EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("one two three four")), true);
  bstrfree(&buf);
}

UTEST(bstr, bstrCatJoinCstr) {
  struct bstr_s buf ={0};
  const char* slices[] = {
    "one",
    "two",
    "three",
    "four",
  };
  EXPECT_EQ(bstrCatJoinCStr(&buf, slices, 4, bstr_const_ref(" ")), true);
  EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("one two three four")), true);
  bstrfree(&buf);
}

UTEST(bstr, appendSlice) {
  struct bstr_s buf = {0};
  bstrAssign(&buf, bstr_const_ref("Hello"));
  EXPECT_EQ(bstrAppendSlice(&buf, bstr_const_ref(" world")), true);
  EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("Hello world")), true);
  bstrfree(&buf);
}

UTEST(bstr, bstrcatprintf) {
  struct bstr_s buf = {0};
  EXPECT_EQ(bstrcatprintf(&buf, "Hello %s", "world"), true);
  EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("Hello world")), true);

  bstrsetlen(&buf, 0);
  EXPECT_EQ(bstrcatprintf(&buf, "%d", 123), true);
  EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("123")), true);
  
  EXPECT_EQ(bstrcatprintf(&buf, " %lu", 156), true);
  EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("123 156")), true);

  bstrsetlen(&buf, 0);
  EXPECT_EQ(bstrcatprintf(&buf,"a%cb",0), true);
  EXPECT_EQ(bstrEqual(bstr_const_ref(buf), (struct bstr_const_slice_s){.buf = "a\0""b", .len = 3}), 1);
  bstrfree(&buf);
}

UTEST(bstr, bstrcatfmt) {
   struct bstr_s s = {0};
   {
     bstrcatfmt(&s, "%S\n", bstr_const_ref("Hello World"));
     EXPECT_EQ(bstrEqual(bstr_const_ref(s), bstr_const_ref("Hello World\n")), true );
     bstr_clear(&s);
   }
   {
     bstrcatfmt(&s, "%i\n", 125);
     EXPECT_EQ(bstrEqual(bstr_const_ref(s), bstr_const_ref("125\n")), true );
     bstr_clear(&s);
   }
   {
     bstrcatfmt(&s, "%i\n", -125);
     EXPECT_EQ(bstrEqual(bstr_const_ref(s), bstr_const_ref("-125\n")), true );
     bstr_clear(&s);
   }
   bstrfree(&s);
}

UTEST(bstr, updateLen) {
  struct bstr_s buf = {0};
  bstrAssign(&buf, bstr_const_ref("Hello World"));
  buf.buf[5] = '\0';
  bstrUpdateLen(&buf);
 
  EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("Hello")), true );
  EXPECT_EQ(buf.len, 5);

  bstrfree(&buf);
}

UTEST(bstr, bstrAssign) {
  struct bstr_s buf = {0};
  {
    bstrAssign(&buf, bstr_const_ref("Hello World"));
    EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("Hello World")), true);
  }
  {
    bstrAssign(&buf, bstrtrim(bstr_const_ref("   Hello World   ")));
    EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("Hello World")), true);
  }
  bstrfree(&buf);

}

UTEST(bstr, bstr_rtrim) {
  EXPECT_EQ(bstrEqual(bstrrtrim(bstr_const_ref("Hello world  ")), bstr_const_ref("Hello world")), true);
  EXPECT_EQ(bstrEqual(bstrrtrim(bstr_const_ref("  Hello world  ")), bstr_const_ref("  Hello world")), true);
  EXPECT_EQ(bstrEqual(bstrrtrim(bstr_const_ref("  ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrrtrim(bstr_const_ref(" ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrrtrim(bstr_const_ref("\n")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrrtrim(bstr_const_ref("\t")), bstr_const_ref("")), true);
}

UTEST(bstr, bstr_ltrim) {
  EXPECT_EQ(bstrEqual(bstrltrim(bstr_const_ref("Hello world  ")), bstr_const_ref("Hello world  ")), true);
  EXPECT_EQ(bstrEqual(bstrltrim(bstr_const_ref("  ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrltrim(bstr_const_ref(" ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrltrim(bstr_const_ref("\n")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrltrim(bstr_const_ref("\t")), bstr_const_ref("")), true);
}

UTEST(bstr, bstrTrim) {
  {
    struct bstr_s buf = {0};
    bstrAssign(&buf, bstr_const_ref("  Hello World "));
    bstrAssign(&buf, bstrtrim(bstr_const_ref(buf)));
    EXPECT_EQ(bstrEqual(bstr_const_ref(buf), bstr_const_ref("Hello World")), true);
    bstrfree(&buf);
  }

  EXPECT_EQ(bstrEqual(bstrtrim(bstr_const_ref("Hello world  ")), bstr_const_ref("Hello world")), true);
  EXPECT_EQ(bstrEqual(bstrtrim(bstr_const_ref("  \t Hello world  ")), bstr_const_ref("Hello world")), true);
  EXPECT_EQ(bstrEqual(bstrtrim(bstr_const_ref("  ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrtrim(bstr_const_ref(" ")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrtrim(bstr_const_ref("\n")), bstr_const_ref("")), true);
  EXPECT_EQ(bstrEqual(bstrtrim(bstr_const_ref("\t")), bstr_const_ref("")), true);
}

UTEST(bstr, bstrsscanf) {
  {
    uint32_t a = 0;
    uint32_t b = 0;
    int read = bstrsscanf(bstr_const_ref("10.132"), "%u.%u", &a, &b);
    EXPECT_EQ(read, 2);
    EXPECT_EQ(a, 10);
    EXPECT_EQ(b, 132);

  }
}

UTEST(bstr, bstrSliceToUtf16CodePoint) {
  unsigned char leftPointingMagnify[] = {0x3D,0xD8,0x0D,0xDD};

  struct bstr_utf_result_s res;
  {
    struct bstr_utf_iterable_s iter = {
      .buffer = (struct bstr_const_slice_s) {
        .buf = (const char*)leftPointingMagnify,
        .len = 2,
      },
      .cursor = 0
    };
    res = bstrUtf16NextCodePoint(&iter);
    EXPECT_EQ((bool)res.invalid, true);
    EXPECT_EQ((bool)res.finished, true);
  }
  {
    struct bstr_utf_iterable_s iter = {
      .buffer = (struct bstr_const_slice_s) {
        .buf = (const char*)leftPointingMagnify,
        .len = sizeof(leftPointingMagnify),
      },
      .cursor = 0
    };
    res = bstrUtf16NextCodePoint(&iter);
    EXPECT_EQ(res.codePoint, 0x1F50D);
    EXPECT_EQ((bool)res.finished, true);
  }
  {
    struct bstr_utf_iterable_s iter = (struct bstr_utf_iterable_s){
      .buffer = (struct bstr_const_slice_s) {
        .buf = NULL, 
        .len = 0,
      },
      .cursor = 0
    };
    res = bstrUtf16NextCodePoint(&iter);
    EXPECT_EQ((bool)res.invalid, true);
    EXPECT_EQ((bool)res.finished, true);
  }
  {
    char badInput[] = {0};
    struct bstr_utf_iterable_s iter = (struct bstr_utf_iterable_s){
        .buffer = (struct bstr_const_slice_s){
          .buf = badInput,
          .len = 0, 
        },
        .cursor = 0};
    res = bstrUtf16NextCodePoint(&iter);
    EXPECT_EQ((bool)res.invalid, true);
    EXPECT_EQ((bool)res.finished, true);
  }

}

//UTEST(bstr, bstrSliceToUtf8CodePoint) {
//  char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
//  struct bstr_utf_iterable_s iter = {
//    .buffer = (struct bstr_const_slice_s) {
//      .len = sizeof(smilyCat),
//      .buf = smilyCat
//    },
//    .cursor = 0
//  };
//
//  struct bstr_utf_result_s res = bstrUtf8NextCodePoint(&iter);
//  EXPECT_EQ(res.codePoint, 0x0001f63c);
//  
//  char charU[] = {'U'};
//  EXPECT_EQ(bstrSliceToUtf8CodePoint((struct bstr_const_slice_s) {
//    .len = sizeof(charU),
//    .buf = charU 
//  }, 0), 'U');
//
//  char ringOperator[] = {0xe2, 0x88, 0x98};
//  EXPECT_EQ(bstrSliceToUtf8CodePoint((struct bstr_const_slice_s) {
//    .len = sizeof(ringOperator),
//    .buf = ringOperator 
//  }, 0), 0x2218);
// 
//  // this has an extra byte 
//  char badRingOperator[] = {0xe2, 0x88, 0x98, 0x1};
//  EXPECT_EQ(bstrSliceToUtf8CodePoint((struct bstr_const_slice_s) {
//    .len = sizeof(badRingOperator),
//    .buf = badRingOperator 
//  }, 1), 1);
//}

UTEST(bstr, bstrUtf8CodePointIter) {
  {
    unsigned char smilyCat[] = {0xF0, 0x9F, 0x98, 0xBC};
    struct bstr_utf_iterable_s iter = {
      .buffer = (struct bstr_const_slice_s) {
        .buf = (const char*)smilyCat,
        .len = sizeof(smilyCat),
      },
      .cursor = 0
    };
    struct bstr_utf_result_s res = bstrUtf8NextCodePoint(&iter);
    EXPECT_EQ(res.codePoint, 0x0001f63c);
    EXPECT_EQ((bool)res.finished, true);
    EXPECT_EQ((bool)res.invalid, false);
  }
{
  // Ḽơᶉëᶆ
  unsigned char buffer[] = {0xE1, 0xB8, 0xBC, 0xC6, 0xA1, 0xE1, 0xB6, 0x89,0xC3, 0xAB,0xE1, 0xB6,0x86 };
  struct bstr_utf_iterable_s iterable = {
    .buffer = {
      .buf = (const char*)buffer,
      .len = sizeof(buffer)
    },
    .cursor = 0,
  };
  struct bstr_utf_result_s s = {0};
  s = bstrUtf8NextCodePoint(&iterable);
  EXPECT_EQ(s.codePoint, 0x00001E3C); // 0xE1, 0xB8, 0xBC
  EXPECT_EQ((bool)s.finished, false);
  EXPECT_EQ((bool)s.invalid, false);
  s = bstrUtf8NextCodePoint(&iterable);
  EXPECT_EQ(s.codePoint,0x1a1); // 0xBC, 0xC6
  EXPECT_EQ((bool)s.finished, false);
  EXPECT_EQ((bool)s.invalid, false);
  s = bstrUtf8NextCodePoint(&iterable);
  EXPECT_EQ(s.codePoint,0x1D89); // 0xE1, 0xB6, 0x89
  EXPECT_EQ((bool)s.finished, false);
  EXPECT_EQ((bool)s.invalid, false);
  s = bstrUtf8NextCodePoint(&iterable);
  EXPECT_EQ(s.codePoint,0x00EB); // 0xC3 0xAB
  EXPECT_EQ((bool)s.finished, false);
  EXPECT_EQ((bool)s.invalid, false);
  s = bstrUtf8NextCodePoint(&iterable);
  EXPECT_EQ(s.codePoint,0x1D86); //0xE1 0xB6 0x86
  EXPECT_EQ((bool)s.finished, true);
  EXPECT_EQ((bool)s.invalid, false);
  s = bstrUtf8NextCodePoint(&iterable);
  EXPECT_EQ((bool)s.finished, true);
  EXPECT_EQ((bool)s.invalid, true);
  }
}


UTEST_MAIN()

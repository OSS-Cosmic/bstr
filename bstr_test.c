#include "bstr.h"
#include "utest.h"
#include "bstr.c"

UTEST(compare, bstrCaselessCompare ) {
  EXPECT_EQ(bstrCaselessCompare(bstr_slice_literal("test"), bstr_slice_literal("TEST")), 0);
  EXPECT_EQ(bstrCaselessCompare(bstr_slice_literal("testAA"), bstr_slice_literal("TEST")), 1);
}

UTEST(compare, bstrCompare  ) {
  EXPECT_EQ(bstrCompare(bstr_slice_literal("test"), bstr_slice_literal("TEST")), 32);
  EXPECT_EQ(bstrCompare(bstr_slice_literal("Test"), bstr_slice_literal("test")), -32);
}


UTEST(indexOf, bstrIndexOf) {
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("foo foo"), bstr_slice_literal("foo")), 0);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("banana"), bstr_slice_literal("ana")), 1);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("textbook"), bstr_slice_literal("book")), 4);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("abababcabababcbab"), bstr_slice_literal("ababc")), 2); 
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("This is a test string to see how Boyer-Moore handles longer searches"), bstr_slice_literal("searches")), 60);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("hello world"), bstr_slice_literal("hello")), 0);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("apple banana"), bstr_slice_literal("nana")), 8);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("goodbye"), bstr_slice_literal("bye")), 4);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("cat"), bstr_slice_literal("dog")), -1);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal(""), bstr_slice_literal("abc")), -1);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("hello"), bstr_slice_literal("")), -1);
  EXPECT_EQ(bstrIndexOf(bstr_slice_literal("abc"), bstr_slice_literal("abcd")), -1);

}

 //   try testing.expect(indexOf(u8, "one two three four five six seven eight nine ten eleven", "three four").? == 8);
 //   try testing.expect(lastIndexOf(u8, "one two three four five six seven eight nine ten eleven", "three four").? == 8);
 //   try testing.expect(indexOf(u8, "one two three four five six seven eight nine ten eleven", "two two") == null);
 //   try testing.expect(lastIndexOf(u8, "one two three four five six seven eight nine ten eleven", "two two") == null);

 //   try testing.expect(indexOf(u8, "one two three four five six seven eight nine ten", "").? == 0);
 //   try testing.expect(lastIndexOf(u8, "one two three four five six seven eight nine ten", "").? == 48);

 //   try testing.expect(indexOf(u8, "one two three four", "four").? == 14);
 //   try testing.expect(lastIndexOf(u8, "one two three two four", "two").? == 14);
 //   try testing.expect(indexOf(u8, "one two three four", "gour") == null);
 //   try testing.expect(lastIndexOf(u8, "one two three four", "gour") == null);
 //   try testing.expect(indexOf(u8, "foo", "foo").? == 0);
 //   try testing.expect(lastIndexOf(u8, "foo", "foo").? == 0);
 //   try testing.expect(indexOf(u8, "foo", "fool") == null);
 //   try testing.expect(lastIndexOf(u8, "foo", "lfoo") == null);
 //   try testing.expect(lastIndexOf(u8, "foo", "fool") == null);

 //   try testing.expect(indexOf(u8, "foo foo", "foo").? == 0);
 //   try testing.expect(lastIndexOf(u8, "foo foo", "foo").? == 4);
 //   try testing.expect(lastIndexOfAny(u8, "boo, cat", "abo").? == 6);
 //   try testing.expect(lastIndexOfScalar(u8, "boo", 'o').? == 2);


UTEST_MAIN()

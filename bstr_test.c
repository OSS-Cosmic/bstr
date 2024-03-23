#include "bstr.h"
#include "utest.h"
#include "bstr.c"


UTEST(bstricmp , bstricmp_1 ) {
  EXPECT_EQ(bstricmp(bstr_slice_literal("test"), bstr_slice_literal("TEST")), BSTR_OK);
}
UTEST_MAIN()

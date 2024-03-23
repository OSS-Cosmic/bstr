#include "bstr.h"
#include <assert.h>
#include <ctype.h>


#define s_malloc malloc
#define s_realloc realloc
#define s_free free

#define   upcase(c) (toupper ((unsigned char) c))
#define downcase(c) (tolower ((unsigned char) c))
#define   wspace(c) (isspace ((unsigned char) c))

struct bstr_s bstr_create(const char *init) {
  size_t initlen = (init == NULL) ? 0 : strlen(init);
  return bstr_createlen(init, initlen);
}

struct bstr_s bstr_createlen(const char *init, size_t len) {
    struct bstr_s s = {};
    s.buf = s_malloc(len+1);
    if(s.buf == NULL) return s;
    if (init==BSTR_NOINIT)
        init = NULL;
    else if (!init)
        memset(s.buf, 0, len+1);
    if (len && init)
        memcpy(s.buf, init, len);
    s.buf[len] = '\0';
    s.alloc = len;
    return s;
}


int bstricmp (const struct bstr_slice_s b0, const struct bstr_slice_s b1) {
  const char* c1 = b0.begin;
  const char* c2 = b1.begin;
  for(;c1 < b0.end && c2 < b1.end; c1++,c2++) {
    const char v  = (char) downcase (*c1)
	           - (char) downcase (*c2);
    if (0 != v) 
	    return v;
  }

  for(;c1 < b0.end; c1++) {
		const char v = (char) downcase (*c1);
		if (v) 
			return v;
		return BSTR_CMP_EXTRA_NULL;
  }

  for(;c2 < b1.end; c2++) {
		const char v = (char) downcase (*c2);
		if (v) 
			return v;
		return BSTR_CMP_EXTRA_NULL;
  }
  return BSTR_OK;
}
//int bstrnicmp (const struct bstr_slice_s b0, const struct bstr_slice_s b1, int n) {
//
//}
//int bstrcmp (const struct bstr_slice_s b0, const struct bstr_slice_s b1) {
//
//}
//int bstrncmp (const struct bstr_slice_s b0, const struct bstr_slice_s b1, int n) {
//
//}

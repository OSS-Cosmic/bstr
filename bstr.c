#include "bstr.h"
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

/*  
 *  Compare two strings without differentiating between case. The return
 *  value is the difference of the values of the characters where the two
 *  strings first differ after lower case transformation, otherwise 0 is
 *  returned indicating that the strings are equal. If the lengths are
 *  different, then a difference from 0 is given. 
 */
int bstrCaselessCompare(const struct bstr_slice_s b0, const struct bstr_slice_s b1) {
  if(b0.buf == b1.buf) {
    goto same_str; // same string but length is different 
  }
  
  size_t i0 = 0;
  size_t i1 = 0;
  for (; i0 < b0.len && i1 < b1.len; i0++, i1++) {
    const int v = (int)downcase(b0.buf[i0]) - (int)downcase(b1.buf[i1]);
    if (0 != v)
      return v;
  }

same_str:
  if (b0.len > b1.len)
    return 1;
  if (b1.len > b0.len)
    return -1;
  return 0;
}

int bstrCompare (const struct bstr_slice_s b0, const struct bstr_slice_s b1) {
  if(b0.buf == b1.buf) {
    goto same_str; // same string but length is different 
  }

  size_t i0 = 0;
  size_t i1 = 0;
  for (; i0 < b0.len && i1 < b1.len; i0++, i1++) {
    const int v = (int)b0.buf[i0] - (int)b1.buf[i1];
    if (0 != v) {
      return v;
    }
  }
  
same_str:
  if (b0.len > b1.len)
    return 1;
  if (b1.len > b0.len)
    return -1;
  return 0;
}

int bstrCaselessEq (const struct bstr_slice_s b0, const struct bstr_slice_s b1) {
  if(b0.len != b1.len) 
    return 0;

  size_t i0 = 0;
  size_t i1 = 0;
  for (; i0 < b0.len && i1 < b1.len; i0++, i1++) {
    if((char)downcase(b0.buf[i0]) != (char)downcase(b1.buf[i1])) {
      return 0;
    }
  }
  return 1;
}

int bstrEq (const struct bstr_slice_s b0, const struct bstr_slice_s b1) {
  if(b0.len != b1.len) 
    return 0;
  
  size_t i0 = 0;
  size_t i1 = 0;
  for (; i0 < b0.len && i1 < b1.len; i0++, i1++) {
    if(b0.buf[i0] != b1.buf[i1]) {
      return 0;
    }
  }
  return 1;

}

int bstrIndexOfCmp(const struct bstr_slice_s haystack, const struct bstr_slice_s needle, bstr_cmp_handle handle) {
  if(needle.len > haystack.len || needle.len == 0)
    return BSTR_ERR;

  if(haystack.len < 52 && needle.len <= 4) {
    for(size_t i = 0; i < (haystack.len - (needle.len - 1)); i++){
      for(size_t j = 0; j < needle.len; j++) {
        if(handle((struct bstr_slice_s){
          .buf = haystack.buf + i,
          .len = needle.len
        }, needle)) {
          return i;
        }
      }
    }
    return BSTR_ERR;
  }

  // Boyer–Moore–Horspool_algorithm
  char skip_table[256] = {0};
  for(size_t i = 0; i < 256; i++) {
    skip_table[i] = needle.len;
  }
  for(size_t i = 0; i < needle.len - 1; i++) {
    skip_table[needle.buf[i]] = needle.len - i - 1; 
  }

  size_t i = 0;
  while(i <= haystack.len - needle.len) {
    if (bstrEq((struct bstr_slice_s){
      .buf = haystack.buf + i,
      .len = needle.len
    }, needle)) {
        return i;
    }
    i += skip_table[haystack.buf[i + needle.len - 1]];
  }
  return BSTR_ERR;

}
int bstrLastIndexOfCmp(const struct bstr_slice_s haystack, const struct bstr_slice_s needle, bstr_cmp_handle handle) {
  if(needle.len > haystack.len || needle.len == 0)
    return BSTR_ERR;

  if(haystack.len < 52 && needle.len <= 4) {
    for(size_t i = (haystack.len - needle.len) - 1;; i--){
      for(size_t j = 0; j < needle.len; j++) {
        if(handle((struct bstr_slice_s){
          .buf = haystack.buf + i,
          .len = needle.len
        }, needle)) {
          return i;
        }
      }
      if(i == 0) break;
    }
    return BSTR_ERR;
  }

  // Boyer–Moore–Horspool_algorithm
  size_t table_len = 0;
  char skip_table[256] = {0};
  for(size_t i = 0; i < 256; i++) {
    skip_table[i] = needle.len;
  }
  
  for(size_t i = needle.len - 1;; i--){
    skip_table[needle.buf[i]] = needle.len - i - 1; 
    if(i == 0) break;
  }

  size_t i = haystack.len - needle.len;
  while(1) {
    if (handle((struct bstr_slice_s){
      .buf = haystack.buf + i,
      .len = needle.len
    }, needle)) {
        return i;
    }
    const size_t skip = skip_table[haystack.buf[i]];
    if(skip > i) break;
    i -= skip;
  }
  return BSTR_ERR;
  
}
int bstrLastIndexOf(const struct bstr_slice_s haystack,
                    const struct bstr_slice_s needle) {
  return bstrLastIndexOfCmp(haystack, needle, bstrEq);
}

int bstrIndexOf(const struct bstr_slice_s haystack,
                const struct bstr_slice_s needle) {
  return bstrIndexOfCmp(haystack, needle, bstrEq);
}

int bstrIndexOfCaseless(const struct bstr_slice_s haystack,
                        const struct bstr_slice_s needle) {
  return bstrIndexOfCmp(haystack, needle, bstrCaselessEq);
}
int bstrLastIndexOfCaseless(const struct bstr_slice_s haystack,
                            const struct bstr_slice_s needle) {
  return bstrLastIndexOfCmp(haystack, needle, bstrCaselessEq);
}

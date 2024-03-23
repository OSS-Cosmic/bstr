#include "bstr.h"
#include <ctype.h>
#include <stdarg.h>

#define s_malloc malloc
#define s_realloc realloc
#define s_free free

#define   upcase(c) (toupper ((unsigned char) c))
#define downcase(c) (tolower ((unsigned char) c))
#define   wspace(c) (isspace ((unsigned char) c))

#define CHAR_TABLE_LEN (1 << CHAR_BIT)
#define BSTR_MAX_PREALLOC (1024*1024)

typedef int(*bstr_cmp_handle) (struct bstr_slice_s s0, struct bstr_slice_s s1);

struct bstr_s bstrCreate(const char *init) {
  size_t initlen = (init == NULL) ? 0 : strlen(init);
  return bstrCreateLen(init, initlen);
}


int bstrAssign(struct bstr_s* str, const struct bstr_slice_s slice) {
  return BSTR_OK;
}

int bstrSetLen(struct bstr_s* str, size_t len) {
  if(len > str->alloc) {
    assert(len > str->len);
    size_t reqSize = len + 1;
    if(reqSize < BSTR_MAX_PREALLOC) {
      reqSize *= 2; 
    } else {
      reqSize += BSTR_MAX_PREALLOC;
    }
    str->buf = s_realloc(str->buf, reqSize);
    if(str->buf == NULL)
      return BSTR_ERR;
    str->alloc = reqSize;
  }
  str->len = len;
  str->buf[str->len] = '\0';
  return BSTR_OK;
}

int bstrUpdateLen(struct bstr_s* str) {
  size_t len = strlen(str->buf);
  str->len = len;
  assert(str->len <= str->alloc); // the buffer has overrun
  return BSTR_OK; 
}

int bstrClear(struct bstr_s* str) {
  if(bstrSetLen(str, 0) != BSTR_OK)
    return BSTR_ERR;
  str->buf[0] = 0;
  return BSTR_OK;
}
struct bstr_s bstrCreateLen(const char *init, size_t len) {
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
    s.len = len;
    s.alloc = len;
    return s;
}

void bstrFree(struct bstr_s* str) {
  s_free(str->buf);
  str->len = 0;
  str->alloc = 0;
  str->buf = NULL;
}

int bstrAppendSlice(struct bstr_s* str, const struct bstr_slice_s slice) {
  if(bstrMakeRoomFor(str, slice.len + 1) != BSTR_OK)
    return BSTR_ERR;
  for(size_t i = 0; i < slice.len; i++) {
    str->buf[str->len + i] = slice.buf[i];
  }
  str->len += slice.len;
  str->buf[str->len] = '\0';
  return BSTR_OK;
}

int bstrcatvprintf(struct bstr_s* str, const char* fmt, va_list ap) {
  va_list cpy;
  char staticbuf[1024], *buf = staticbuf, *t;
  size_t buflen = strlen(fmt) * 2;
  int bufstrlen;

  /* We try to start using a static buffer for speed.
   * If not possible we revert to heap allocation. */
  if (buflen > sizeof(staticbuf)) {
    buf = s_malloc(buflen);
    if (buf == NULL)
      return BSTR_ERR;
  } else {
    buflen = sizeof(staticbuf);
  }

  /* Alloc enough space for buffer and \0 after failing to
   * fit the string in the current buffer size. */
  while (1) {
    va_copy(cpy, ap);
    bufstrlen = vsnprintf(buf, buflen, fmt, cpy);
    va_end(cpy);
    if (bufstrlen < 0) {
      if (buf != staticbuf)
        s_free(buf);
      return BSTR_ERR;
    }
    if (((size_t)bufstrlen) >= buflen) {
      if (buf != staticbuf)
        s_free(buf);
      buflen = ((size_t)bufstrlen) + 1;
      buf = s_malloc(buflen);
      if (buf == NULL)
        return BSTR_ERR;
      continue;
    }
    break;
  }

  /* Finally concat the obtained string to the SDS string and return it. */
  bstrAppendSlice(str, (struct bstr_slice_s){.buf = buf, .len = bufstrlen});
  if (buf != staticbuf)
    s_free(buf);
  return BSTR_OK;
}

struct bstr_s bstrEmpty() {
  return (struct bstr_s) {0};
}

int bstrcatprintf(struct bstr_s* s, const char *fmt, ...) {
    int result = 0;
    va_list ap;
    char *t;
    va_start(ap, fmt);
    result = bstrcatvprintf(s,fmt,ap);
    va_end(ap);
    return result;
}

int bstrInsertSlice(struct bstr_s* str, size_t offset, const struct bstr_slice_s slice) {
  assert(offset <= str->len);
  if(bstrMakeRoomFor(str, slice.len + 1) != BSTR_OK)
    return BSTR_ERR;
  memmove(str->buf + offset + slice.len, str->buf + offset, str->len - offset);
  for (size_t i = 0; i < slice.len; i++) {
    str->buf[offset + i] = slice.buf[i];
  }
  str->len += slice.len;
  str->buf[str->len] = '\0';
  return BSTR_OK;
}

int bstrAppendChar(struct bstr_s* str, char b) {
  return bstrAppendSlice(str, (struct bstr_slice_s){ .buf = &b, .len = 1});
}

int bstrInsertChar(struct bstr_s* str, size_t i, char b) {
  return bstrInsertSlice(str, i, (struct bstr_slice_s){ .buf = &b, .len = 1});
}

int bstrMakeRoomFor(struct bstr_s* str, size_t addlen) {
  const size_t avail = str->alloc - str->len;
  if(avail >= addlen) return BSTR_OK;
  
  size_t reqSize = str->len + addlen;
  if(reqSize < BSTR_MAX_PREALLOC) {
    reqSize *= 2; 
  } else {
    reqSize += BSTR_MAX_PREALLOC;
  }
  str->buf = s_realloc(str->buf, reqSize);
  str->alloc = reqSize;
  return BSTR_OK;
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

static inline int bstrIndexOfCmp(const struct bstr_slice_s haystack, const struct bstr_slice_s needle, bstr_cmp_handle handle) {
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
  char skip_table[CHAR_TABLE_LEN] = {0};
  for(size_t i = 0; i < CHAR_TABLE_LEN; i++) {
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
static inline int bstrLastIndexOfCmp(const struct bstr_slice_s haystack, const struct bstr_slice_s needle, bstr_cmp_handle handle) {
  if(needle.len > haystack.len || needle.len == 0)
    return BSTR_ERR;

  if(haystack.len < 52 && needle.len <= 4) {
    for(size_t i = (haystack.len - needle.len);; i--){
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
  // skip table to speed up searching for substrings
  char skip_table[256] = {0};
  for(size_t i = 0; i < 256; i++) {
    skip_table[i] = needle.len;
  }
  
  for(size_t i = needle.len - 1;; i--){
    skip_table[needle.buf[i]] = i; 
    if(i == 1) break;
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

int bstrIndexOfAny(const struct bstr_slice_s haystack, const struct bstr_slice_s characters) {
  for(size_t i = 0; i < haystack.len; i++) {
    for(size_t j = 0; characters.len; j++) {
      if(haystack.buf[i] == characters.buf[j]) {
        return i;
      } 
    }
  }
  return BSTR_ERR;
}

int bstrLastIndexOfAny(const struct bstr_slice_s haystack, const struct bstr_slice_s characters) {
  for(size_t i = haystack.len;; i--){
    for(size_t j = 0; characters.len; j++) {
      if(haystack.buf[i] == characters.buf[j]) {
        return i;
      } 
    }
  }
  return BSTR_ERR;

}

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

static inline bool charToDigit(uint8_t c , uint8_t base, uint8_t* res) {
  uint8_t val = 0;
  if(c >= '0' && c <= '9')
    val = c - '0';
  if(c >= 'a' && c <= 'z')
    val = c - 'a' + 10;
  if(c >= 'A' && c <= 'Z') 
    val = c - 'A' + 10;
  if(val >= base) 
    return false;
  (*res) = val;
  return true;
}


typedef bool(*bstr_cmp_handle) (struct bstr_const_slice_s s0, struct bstr_const_slice_s s1);

void bstrToUpper(struct bstr_slice_s slice) {
    for(size_t i = 0; i < slice.len; i++) {
        slice.buf[i] = toupper(slice.buf[i]);
    }
}
void bstrToLower(struct bstr_slice_s slice) {
    for(size_t i = 0; i < slice.len; i++) {
        slice.buf[i] = tolower(slice.buf[i]);
    }
}

struct bstr_const_slice_s bstrTrim(struct bstr_const_slice_s slice) {
  return bstrLeftTrim(bstrRightTrim(slice));
}
struct bstr_const_slice_s bstrRightTrim(struct bstr_const_slice_s slice) {
  if (BSTR_SLICE_EMPTY(slice))
    return (struct bstr_const_slice_s){slice.buf, 0};
  for (size_t i = slice.len - 1;; i--) {
    if (!isspace(slice.buf[i])) {
      return (struct bstr_const_slice_s){slice.buf, i + 1};
    }
    if(i == 0) break;
  }
  return (struct bstr_const_slice_s){slice.buf, 0};
}

struct bstr_const_slice_s bstrLeftTrim(struct bstr_const_slice_s slice) {
  if (BSTR_SLICE_EMPTY(slice))
    return (struct bstr_const_slice_s){slice.buf, 0};
  for (size_t i = 0; i < slice.len; i++) {
    if (!isspace(slice.buf[i])) {
      return (struct bstr_const_slice_s){slice.buf + i, slice.len - i};
    }
  }
  return (struct bstr_const_slice_s){slice.buf, 0};
}

bool bstrAssign(struct bstr_s* str, struct bstr_const_slice_s slice) {
    // set the length of the string alloc will 
    if(!bstrSetLen(str, slice.len))
        return false;
    // slices can potentially overlap to the dest string 
    //    trimming and assigning
    memmove(str->buf, slice.buf, slice.len); 
    return true;
}

bool bstrSetLen(struct bstr_s* str, size_t len) {
  if(len + 1 > str->alloc) {
    assert(len > str->len);
    size_t reqSize = len + 1;
    if(reqSize < BSTR_MAX_PREALLOC) {
      reqSize *= 2; 
    } else {
      reqSize += BSTR_MAX_PREALLOC;
    }
    str->buf = s_realloc(str->buf, reqSize);
    if(str->buf == NULL)
      return false;
    str->alloc = reqSize;
  }
  str->len = len;
  str->buf[str->len] = '\0';
  return true;
}

bool bstrUpdateLen(struct bstr_s* str) {
  size_t len = strlen(str->buf);
  str->len = len;
  assert(str->len < str->alloc); // the buffer has overrun
  return true; 
}

bool bstrClear(struct bstr_s* str) {
  if(!bstrSetLen(str, 0))
    return false;
  str->buf[0] = 0;
  return true;
}

void bstrFree(struct bstr_s* str) {
  s_free(str->buf);
  str->len = 0;
  str->alloc = 0;
  str->buf = NULL;
}

bool bstrSetReserve(struct bstr_s* str, size_t reserveLen) {
  if(reserveLen > str->alloc) {
    str->alloc = reserveLen;
    str->buf = s_realloc(str->buf, str->alloc);
    if(str->buf == NULL)
      return false;
  }
  return true;
}

struct bstr_s bstrDuplicate(const struct bstr_s* str) {
  assert(str);
  struct bstr_s result = {};
  if(str->buf == NULL) 
    return result; 
  result.buf = s_malloc(str->len + 1);
  if(result.buf == NULL) 
    return result;
  memcpy(result.buf, str->buf, str->len);
  result.buf[result.len] = str->len;
  return result;
}

bool bstrAppendSlice(struct bstr_s* str, const struct bstr_const_slice_s slice) {
  if(!bstrMakeRoomFor(str, slice.len + 1))
    return false;
  for(size_t i = 0; i < slice.len; i++) {
    str->buf[str->len + i] = slice.buf[i];
  }
  str->len += slice.len;
  str->buf[str->len] = '\0';
  return true;
}

int bstrfmtll(struct bstr_slice_s slice, long long value) { 
    unsigned long long v;
    /* Generate the string representation, this method produces
     * a reversed string. */
    if (value < 0) {
        /* Since v is unsigned, if value==LLONG_MIN then
         * -LLONG_MIN will overflow. */
        if (value != LLONG_MIN) {
            v = -value;
        } else {
            v = ((unsigned long long)LLONG_MAX) + 1;
        }
    } else {
        v = value;
    }
    char* p = slice.buf;
    do {
        // early escape if we write past the end of the slice
        if(p + 1 >= slice.buf + slice.len)
            return -1;
        *p++ = '0'+(v%10);
        v /= 10;
    } while(v);
    if (value < 0) *p++ = '-';

    /* Compute length and add null term. */
    const int len = p - slice.buf;

    /* Reverse the string. */
    p--;
    char* s = slice.buf;
    while(s < p) {
        const char aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return len;
}

int bstrfmtull(struct bstr_slice_s slice, unsigned long long value) {
    unsigned long long v;
    /* Generate the string representation, this method produces
     * a reversed string. */
    char* p = slice.buf;
    do {
        // early escape if we write past the end of the slice
        if(p + 1 >= slice.buf + slice.len)
            return -1;
        *p++ = '0'+(value%10);
        value /= 10;
    } while(v);

    /* Compute length and add null term. */
    const int len = p - slice.buf;

    /* Reverse the string. */
    p--;
    char* s = slice.buf;
    while(s < p) {
        const char aux = *s;
        *s = *p;
        *p = aux;
        s++;
        p--;
    }
    return len;
}

bool bstrcatfmt(struct bstr_s* s, char const *fmt, ...) {
    const char *f = fmt;
    va_list ap;

    /* To avoid continuous reallocations, let's start with a buffer that
     * can hold at least two times the format string itself. It's not the
     * best heuristic but seems to work in practice. */
    if(!bstrMakeRoomFor(s, strlen(fmt)*2)) 
        return false; // we failed to make room for
    va_start(ap,fmt);
    f = fmt;    /* Next format specifier byte to process. */
    while(*f) {
        char next;
        long long num;

        /* Make sure there is always space for at least 1 char. */
        if (BSTR_AVIL(s)==0) {
            if(!bstrMakeRoomFor(s,1))
                return false;
        }
        assert(s->len <= s->alloc);

        switch(*f) {
        case '%':
            next = *(f+1);
            if (next == '\0') break;
            f++;
            switch(next) {
            case 'c':
                num = va_arg(ap,int);
                s->buf[s->len++] = num;
                break;
            case 's': {
              char *str = va_arg(ap, char *);
              if (!bstrMakeRoomFor(s, strlen(str)))
                return false;
              break;
            }
            case 'S': {
                const struct bstr_const_slice_s str = va_arg(ap,struct bstr_const_slice_s);
                if(!bstrMakeRoomFor(s, str.len))
                  return false;
                memcpy(s->buf + s->len, str.buf, str.len);
                s->len += str.len;
                break;
            }
            case 'i':
            case 'I':
            case 'l':
                if (next == 'i') {
                    num = va_arg(ap,int);
                    if(!bstrMakeRoomFor(s, BSTR_LSTR_SIZE))
                      return false;
                } else if (next == 'l') {
                    num = va_arg(ap,long);
                    if(!bstrMakeRoomFor(s, BSTR_LLSTR_SIZE))
                      return false;
                } else {
                    num = va_arg(ap,long long);
                    if(!bstrMakeRoomFor(s, BSTR_LLSTR_SIZE))
                      return false;
                }
                const int len = bstrfmtll(BSTR_AVAIL_SLICE(s), num);
                if(len == -1)
                  return false;
                s->len += len;
                
                break;
            case 'u':
            case 'U':
            case 'L': {
                unsigned long long unum;
                if (next == 'u') {
                    unum = va_arg(ap,unsigned int);
                    if(!bstrMakeRoomFor(s, BSTR_LSTR_SIZE))
                      return false;
                } else if (next == 'L') {
                    unum = va_arg(ap,unsigned long);
                    if(!bstrMakeRoomFor(s, BSTR_LLSTR_SIZE))
                      return false;
                } else {
                    unum = va_arg(ap,unsigned long long);
                    if(!bstrMakeRoomFor(s, BSTR_LLSTR_SIZE))
                      return false;
                }
                const int len = bstrfmtull(BSTR_AVAIL_SLICE(s),unum);
                if(len == -1)
                  return false;
                s->len += len;
                
                break;
            }
            default: /* Handle %% and generally %<unknown>. */
                s->buf[s->len++] = next;
                break;
            }
            break;
        default:
            s->buf[s->len++] = *f;
            break;
        }
        f++;
    }
    va_end(ap);

    /* Add null-term */
    s->buf[s->len] = '\0';
    return s;

} 

static inline size_t readNumberSign(const char* buf, size_t pos, size_t len , int* result_sign) {
  assert(result_sign);
  if(pos + 1 >= len) {
		switch(buf[0]) {
			case '+':
				(*result_sign) = 1;
				return 1;
			case '-':
				(*result_sign) = -1;
				return 1;
		}
  }
  return 0;
}

static inline size_t readNumberBase(const char* buf, size_t pos, size_t len , int* base) { 
	assert(base);
	(*base) = 10;
	if ((*base) == 0) {
		if (pos + 2 >= len && buf[0] == '0') {
			switch (downcase(buf[1])) {
				case 'b':
					(*base) = 2;
					return 2;
				case 'o':
					(*base) = 8;
					return 2;
				case 'x':
					(*base) = 16;
					return 2;
			}
		}
	}
	return 0;
}

bool bstrReadull(struct bstr_const_slice_s slice, int base, unsigned long long *result) {
  assert(result);
  size_t pos = 0;
  int numBase = 0;
  pos += readNumberBase(slice.buf, pos, slice.len, &numBase);
  unsigned long long val = 0;
  for (; pos < slice.len; pos++) {
    uint8_t digit = 0;
    if (!charToDigit(slice.buf[pos], numBase, &digit))
      return false;
    if (val != 0) {
      val *= numBase;
    }
    val += digit;
  }
  (*result) = val;
  return true;
}
bool bstrReadll(struct bstr_const_slice_s slice, int base, long long*result) {
  assert(result);
  size_t pos = 0;
  int sign = 1;
  int numBase = 0;
  pos += readNumberSign(slice.buf, pos, slice.len, &sign);
  pos += readNumberBase(slice.buf, pos, slice.len, &numBase);
  int64_t val = 0;
  for (; pos < slice.len; pos++) {
    uint8_t digit = 0;
    if (!charToDigit(slice.buf[pos], numBase, &digit))
      return false;
    
    if (val != 0) {
      val *= numBase;
    }
    val += (digit * sign);
  }
  (*result) = val;
  return true;
}

bool bstrcatvprintf(struct bstr_s* str, const char* fmt, va_list ap) {
  va_list cpy;
  char staticbuf[1024], *buf = staticbuf, *t;
  size_t buflen = strlen(fmt) * 2;
  int bufstrlen;

  /* We try to start using a static buffer for speed.
   * If not possible we revert to heap allocation. */
  if (buflen > sizeof(staticbuf)) {
    buf = s_malloc(buflen);
    if (buf == NULL)
      return false;
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
      return false;
    }
    if (((size_t)bufstrlen) >= buflen) {
      if (buf != staticbuf)
        s_free(buf);
      buflen = ((size_t)bufstrlen) + 1;
      buf = s_malloc(buflen);
      if (buf == NULL)
        return false;
      continue;
    }
    break;
  }

  /* Finally concat the obtained string to the bstr string and return it. */
  bstrAppendSlice(str, (struct bstr_const_slice_s){.buf = buf, .len = bufstrlen});
  if (buf != staticbuf)
    s_free(buf);
  return true;
}

bool bstrcatprintf(struct bstr_s* s, const char *fmt, ...) {
    va_list ap;
    char *t;
    va_start(ap, fmt);
    const bool result = bstrcatvprintf(s,fmt,ap);
    va_end(ap);
    return result;
}

bool bstrInsertSlice(struct bstr_s* str, size_t offset, const struct bstr_const_slice_s slice) {
  assert(offset <= str->len);
  if(!bstrMakeRoomFor(str, slice.len + 1))
    return false;
  memmove(str->buf + offset + slice.len, str->buf + offset, str->len - offset);
  for (size_t i = 0; i < slice.len; i++) {
    str->buf[offset + i] = slice.buf[i];
  }
  str->len += slice.len;
  str->buf[str->len] = '\0';
  return true;
}

bool bstrAppendChar(struct bstr_s* str, char b) {
  return bstrAppendSlice(str, (struct bstr_const_slice_s){ .buf = &b, .len = 1});
}

bool bstrInsertChar(struct bstr_s* str, size_t i, char b) {
  return bstrInsertSlice(str, i, (struct bstr_const_slice_s){ .buf = &b, .len = 1});
}

bool bstrMakeRoomFor(struct bstr_s* str, size_t addlen) {
  const size_t avail = str->alloc - str->len;
  if(avail >= addlen) return true;
  
  size_t reqSize = str->len + addlen;
  if(reqSize < BSTR_MAX_PREALLOC) {
    reqSize *= 2; 
  } else {
    reqSize += BSTR_MAX_PREALLOC;
  }
  str->buf = s_realloc(str->buf, reqSize);
  if(!str->buf)
    return false;
  str->alloc = reqSize;
  return true;
}

struct bstr_const_slice_s bstrSplitItr(struct bstr_split_iterable_s* iterable) {
  assert(bstrSliceValid(iterable->buffer));
  assert(bstrSliceValid(iterable->delim));
  if(iterable->cursor == iterable->buffer.len) 
    return (struct bstr_const_slice_s){0};

  const char* begin = iterable->buffer.buf + iterable->cursor;
  const int offset = bstrIndexOfOffset(BSTR_TO_CONSTSLICE((iterable->buffer)), iterable->cursor, iterable->delim);
  if(offset == -1) {
    struct bstr_const_slice_s res =  BSTR_CONSTSLICE_SUB(&(iterable->buffer), iterable->cursor, iterable->buffer.len);
    iterable->cursor = iterable->buffer.len; // move the cursor to the very end of the buffer
    return res;
  }
  struct bstr_const_slice_s res =  BSTR_CONSTSLICE_SUB(&(iterable->buffer), iterable->cursor, offset);
  iterable->cursor = offset + iterable->delim.len; 
  return res;
}

struct bstr_const_slice_s bstrSplitIterReverse(struct bstr_split_iterable_s* iterable) {
  assert(bstrSliceValid(iterable->buffer));
  assert(bstrSliceValid(iterable->delim));
  if(iterable->cursor == 0) 
    return (struct bstr_const_slice_s){0};
  
  const char* begin = iterable->buffer.buf + iterable->cursor;
  const int offset = bstrLastIndexOfOffset(BSTR_TO_CONSTSLICE((iterable->buffer)), iterable->cursor - 1, iterable->delim);
  if(offset == -1) {
    struct bstr_const_slice_s res =  BSTR_CONSTSLICE_SUB(&(iterable->buffer), 0, iterable->cursor);
    iterable->cursor = 0; // move the cursor to the very beginning of the buffer
    return res;
  }
  assert(offset + iterable->delim.len <= iterable->cursor);
  struct bstr_const_slice_s res =  BSTR_CONSTSLICE_SUB(&(iterable->buffer), offset + iterable->delim.len, iterable->cursor);
  iterable->cursor = offset; 
  return res;
}


int bstrCaselessCompare(const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1) {
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

int bstrCompare (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1) {
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

bool bstrCaselessEq (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1) {
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

bool bstrEq (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1) {
  //printf("EQ: \"%.*s\" -- \"%.*s\"\n", (int)b0.len, b0.buf, (int)b1.len, b1.buf);
  if(b0.len != b1.len) 
    return 0;
  size_t i0 = 0;
  size_t i1 = 0;
  for (; i0 < b0.len && i1 < b1.len; i0++, i1++) {
    if(b0.buf[i0] != b1.buf[i1]) {
      return false;
    }
  }
  return true;
}

static inline int bstrIndexOfCmp(const struct bstr_const_slice_s haystack, size_t offset, const struct bstr_const_slice_s needle, bstr_cmp_handle handle) {
  if(needle.len > haystack.len || needle.len == 0)
    return -1;

  if(haystack.len < 52 && needle.len <= 4) {
    for(size_t i = offset; i < (haystack.len - (needle.len - 1)); i++){
      for(size_t j = 0; j < needle.len; j++) {
        if(handle((struct bstr_const_slice_s){
          .buf = haystack.buf + i,
          .len = needle.len
        }, needle)) {
          return i;
        }
      }
    }
    return -1;
  }

  // Boyer–Moore–Horspool_algorithm
  char skip_table[CHAR_TABLE_LEN] = {0};
  for(size_t i = 0; i < CHAR_TABLE_LEN; i++) {
    skip_table[i] = needle.len;
  }
  for(size_t i = 0; i < needle.len - 1; i++) {
    skip_table[needle.buf[i]] = needle.len - i - 1; 
  }

  size_t i = offset;
  while(i <= haystack.len - needle.len) {
    if (bstrEq((struct bstr_const_slice_s){
      .buf = haystack.buf + i,
      .len = needle.len
    }, needle)) {
        return i;
    }
    i += skip_table[haystack.buf[i + needle.len - 1]];
  }
  return -1;

}

static inline int bstrLastIndexOfCmp(const struct bstr_const_slice_s haystack, size_t offset, const struct bstr_const_slice_s needle, bstr_cmp_handle handle) {
  if(needle.len > haystack.len || needle.len == 0)
    return -1;

  assert(offset <= haystack.len);
  const size_t startIndex = (offset <= (haystack.len - needle.len)) ? offset : (haystack.len - needle.len); 
  
  if(haystack.len < 52 && needle.len <= 4) {
    for(size_t i = startIndex;; i--) {
      assert(i < haystack.len);
      for(size_t j = 0; j < needle.len; j++) {
        if(handle((struct bstr_const_slice_s){
          .buf = haystack.buf + i,
          .len = needle.len
        }, needle)) {
          return i;
        }
      }
      if(i == 0) break;
    }
    return -1;
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

  size_t i = startIndex; 
  while(1) {
    if (handle((struct bstr_const_slice_s){
      .buf = haystack.buf + i,
      .len = needle.len
    }, needle)) {
        return i;
    }
    const size_t skip = skip_table[haystack.buf[i]];
    if(skip > i) break;
    i -= skip;
  }
  return -1;
  
}

int bstrIndexOfOffset(const struct bstr_const_slice_s haystack, size_t offset,
                      const struct bstr_const_slice_s needle) {
  return bstrIndexOfCmp(haystack, offset, needle, bstrEq);
}

int bstrIndexOf(const struct bstr_const_slice_s haystack,
                const struct bstr_const_slice_s needle) {
  return bstrIndexOfCmp(haystack, 0, needle, bstrEq);
}

int bstrIndexOfCaseless(const struct bstr_const_slice_s haystack,
                        const struct bstr_const_slice_s needle) {
  return bstrIndexOfCmp(haystack, 0, needle, bstrCaselessEq);
}

int bstrIndexOfCaselessOffset(const struct bstr_const_slice_s haystack, size_t offset, const struct bstr_const_slice_s needle) {
  return bstrIndexOfCmp(haystack, offset, needle, bstrCaselessEq);
}

int bstrLastIndexOf(const struct bstr_const_slice_s haystack,
                    const struct bstr_const_slice_s needle) {
  return bstrLastIndexOfCmp(haystack, haystack.len, needle, bstrEq);
}

int bstrLastIndexOfOffset(const struct bstr_const_slice_s haystack, size_t offset, const struct bstr_const_slice_s needle) {
  return bstrLastIndexOfCmp(haystack, offset, needle, bstrEq);
}

int bstrLastIndexOfCaseless(const struct bstr_const_slice_s haystack,
                            const struct bstr_const_slice_s needle) {
  return bstrLastIndexOfCmp(haystack, haystack.len, needle, bstrCaselessEq);
}

int bstrLastIndexOfCaselessOffset(const struct bstr_const_slice_s haystack, size_t offset, const struct bstr_const_slice_s needle) {
  return bstrLastIndexOfCmp(haystack, offset, needle, bstrCaselessEq);
}

int bstrIndexOfAny(const struct bstr_const_slice_s haystack, const struct bstr_const_slice_s characters) {
  for(size_t i = 0; i < haystack.len; i++) {
    for(size_t j = 0; characters.len; j++) {
      if(haystack.buf[i] == characters.buf[j]) {
        return i;
      } 
    }
  }
  return false;
}

bool bstrCatJoin(struct bstr_s *str, struct bstr_const_slice_s *slices,
                 size_t numSlices, struct bstr_const_slice_s sep) {
{
    assert(str);
    assert(slices);
    if (numSlices == 0)
      return true; // there is nothing to join just exit

    // calculating the reserve length
    size_t reserveLen = 0;
    for (size_t i = 0; i < numSlices; i++) {
      reserveLen += slices[i].len;
      reserveLen += sep.len;
    }
    reserveLen += 1; // space for the null terminator
    if(!bstrMakeRoomFor(str, reserveLen)) 
      return false;
  }
  for (size_t i = 0; i < numSlices; i++) {
    memcpy(str->buf + str->len, slices[i].buf, slices[i].len);
    str->len += slices[i].len;
    if (numSlices > 1 && i < numSlices - 1 && sep.len >= 1) {
      memcpy(str->buf + str->len, sep.buf, sep.len);
      str->len += sep.len;
    }
  }
  str->buf[str->len] = '\0';

  assert(str->len <= str->alloc); // we've overrun the buffer
  return true;
}


bool bstrCatJoinCStr(struct bstr_s *str, char **argv, size_t argc, struct bstr_const_slice_s sep) {
  for(size_t i = 0; i < argc; i++) {
    const size_t argLen = strlen(argv[i]);
    if(!bstrMakeRoomFor(str, argLen + sep.len)) 
      return false;
    memcpy(str->buf + str->len, argv[i], argLen);
    str->len += argLen;
    if (argc > 1 && i < argc - 1 && sep.len >= 1) {
      memcpy(str->buf + str->len, sep.buf, sep.len);
      str->len += sep.len;
    }
  }
  str->buf[str->len] = '\0';
  assert(str->len <= str->alloc); // we've overrun the buffer
  return true;
}

int bstrLastIndexOfAny(const struct bstr_const_slice_s haystack, const struct bstr_const_slice_s characters) {
  for(size_t i = haystack.len;; i--){
    for(size_t j = 0; characters.len; j++) {
      if(haystack.buf[i] == characters.buf[j]) {
        return i;
      } 
    }
  }
  return false;
}

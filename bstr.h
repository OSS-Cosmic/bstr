#ifndef _BSTR_H_
#define _BSTR_H_ 1

#include <stdio.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define BSTR_ERR (-1)
#define BSTR_OK (0)
#define BSTR_BS_BUFF_LENGTH_GET (0)
#define BSTR_CMP_EXTRA_NULL ((int)UCHAR_MAX + 1)

const char *BSTR_NOINIT = "BSTR_NOINIT";

struct bstr_s {
  size_t alloc;
  size_t len;
  char* buf;
};

struct bstr_slice_s {
  const char* buf;
  size_t len;
}; 
#define BSTR_TO_SLICE(a) (struct bstr_slice_s){(a)->buf, (a)->buf + (a)->len}
#define BSTR_C_SLICE(c) (struct bstr_slice_s){(const char*)(c), strlen(c)}

inline struct bstr_slice_s bstrSliceSub(const struct bstr_slice_s, size_t begin, size_t end);

struct bstr_s bstrCreate(const char *init);
struct bstr_s bstrCreateLen(const char *init, size_t len);
void bstrFree(struct bstr_s* str);

int bstrAppendSlice(struct bstr_s* str, const struct bstr_slice_s slice);
int bstrAppendChar(struct bstr_s* str, char b);
int bstrInsertChar(struct bstr_s* str, char b);
int bstrInsertSlice(struct bstr_s* str, const struct bstr_slice_s slice);

int bstrMakeRoomFor(struct bstr_s* str, size_t addlen);

/* Scan/search functions */
int bstrCaselessCompare (const struct bstr_slice_s b0, const struct bstr_slice_s b1);
int bstrCompare  (const struct bstr_slice_s b0, const struct bstr_slice_s b1);

int bstrCaselessEq (const struct bstr_slice_s b0, const struct bstr_slice_s b1);
int bstrEq (const struct bstr_slice_s b0, const struct bstr_slice_s b1);

int bstrIndexOf(const struct bstr_slice_s haystack, const struct bstr_slice_s needle);
int bstrLastIndexOf(const struct bstr_slice_s str, const struct bstr_slice_s needle);

int bstrIndexOfCaseless(const struct bstr_slice_s haystack, const struct bstr_slice_s needle);
int bstrLastIndexOfCaseless(const struct bstr_slice_s str, const struct bstr_slice_s needle);

int bstrIndexOfAny(const struct bstr_slice_s haystack, const struct bstr_slice_s characters);
int bstrLastIndexOfAny(const struct bstr_slice_s haystack, const struct bstr_slice_s characters);

struct bstr_slice_s bstrSliceSub(const struct bstr_slice_s s0, size_t begin, size_t end) {
  assert(begin <= end); 
  struct bstr_slice_s res = {
    .buf = s0.buf + begin,
    .len = end - begin
  };
  // the new slice has to be inbetween the incoming slice.
  assert(begin + end <= s0.len); 
  return res;
}
#endif

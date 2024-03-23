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
#define bstr_to_slice(a) (struct bstr_slice_s){(a)->buf, (a)->buf + (a)->len}
#define bstr_slice_literal(c) (const struct bstr_slice_s){(const char*)(c), strlen(c)}
#define bstr_c_literal(c) (const struct bstr_slice_s){(const char*)(c), strlen(c)}

inline size_t bstr_slice_len(const struct bstr_slice_s*);
inline struct bstr_slice_s bstr_slice_sub(const struct bstr_slice_s, size_t begin, size_t end);

struct bstr_s bstr_create(const char *init);
struct bstr_s bstr_createlen(const char *init, size_t len);


void bstr_free(struct bstr_s* str);

/* Scan/search functions */
int bstrCaselessCompare (const struct bstr_slice_s b0, const struct bstr_slice_s b1);
int bstrCompare  (const struct bstr_slice_s b0, const struct bstr_slice_s b1);

int bstrCaselessEq (const struct bstr_slice_s b0, const struct bstr_slice_s b1);
int bstrEq (const struct bstr_slice_s b0, const struct bstr_slice_s b1);

int bstrIndexOf(const struct bstr_slice_s haystack, const struct bstr_slice_s needle);
int bstrLastIndexOf(const struct bstr_slice_s str, const struct bstr_slice_s needle);
int bstrIndexOfCaseless(const struct bstr_slice_s haystack, const struct bstr_slice_s needle);
int bstrLastIndexOfCaseless(const struct bstr_slice_s str, const struct bstr_slice_s needle);

typedef int(*bstr_cmp_handle) (struct bstr_slice_s s0, struct bstr_slice_s s1);
int bstrIndexOfCmp(const struct bstr_slice_s haystack, const struct bstr_slice_s needle, bstr_cmp_handle handle);
int bstrLastIndexOfCmp(const struct bstr_slice_s haystack, const struct bstr_slice_s needle, bstr_cmp_handle handle);

int binstrcaseless (const struct bstr_slice_s s1, int pos, const struct bstr_slice_s s2);
int binstrrcaseless (const struct bstr_slice_s s1, int pos, const struct bstr_slice_s s2);

int bstrchrp (const struct bstr_slice_s b, int c, int pos);
int bstrrchrp (const struct bstr_slice_s b, int c, int pos);

struct bstr_slice_s bstr_slice_sub(const struct bstr_slice_s s0, size_t begin, size_t end) {
  assert(begin <= end); 
  struct bstr_slice_s res = {
    .buf = s0.buf + begin,
    .len = end - begin
  };
  // the new slice has to be inbetween the incoming slice.
  assert(begin + end <= s0.len); 
  return res;
}

//size_t bstr_slice_len(const struct bstr_slice_s* s0) {
//  assert(bstr_slice_valid(s0));
//  return s0->end - s0->begin;
//}


//int biseqcaseless (const bstring* b0, const bstring* b1);
//int biseqcaselessblk (const bstring* b, const void * blk, int len);
//int bisstemeqcaselessblk (const bstring* b0, const void * blk, int len);
//int biseq (const bstring* b0, const bstring* b1);
//int biseqblk (const bstring* b, const void * blk, int len);
//int bisstemeqblk (const bstring* b0, const void * blk, int len);
//int biseqcstr (const bstring* b, const char * s);
//int biseqcstrcaseless (const bstring* b, const char * s);
//int binstr (const bstring* s1, int pos, const bstring* s2);
//int binstrr (const bstring* s1, int pos, const bstring* s2);
//int binstrcaseless (const bstring* s1, int pos, const bstring* s2);
//int binstrrcaseless (const bstring* s1, int pos, const bstring* s2);
//int bstrchrp (const bstring* b, int c, int pos);
//int bstrrchrp (const bstring* b, int c, int pos);
//int binchr (const bstring* b0, int pos, const bstring* b1);
//int binchrr (const bstring* b0, int pos, const bstring* b1);
//int bninchr (const bstring* b0, int pos, const bstring* b1);
//int bninchrr (const bstring* b0, int pos, const bstring* b1);
#endif

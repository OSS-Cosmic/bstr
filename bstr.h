#ifndef _BSTR_H_
#define _BSTR_H_ 1

#include <stdio.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#define BSTR_ERR (-1)
#define BSTR_OK (0)
#define BSTR_BS_BUFF_LENGTH_GET (0)
#define BSTR_CMP_EXTRA_NULL ((int)UCHAR_MAX + 1)

struct bstr_s {
  size_t alloc;
  size_t len;
  char* buf;
};

struct bstr_slice_s {
  const char* begin;
  const char* end;
}; 

const char *BSTR_NOINIT = "BSTR_NOINIT";

#define bstr_sub(a, begin, end) (struct bstr_slice_s){a->buf + begin, a->buf + begin + end}
#define bstr_slice_literal(c) (struct bstr_slice_s){(const char*)c, (const char*)c + strlen(c)}

struct bstr_s bstr_create(const char *init);
struct bstr_s bstr_createlen(const char *init, size_t len);

void bstr_free(struct bstr_s* str);

/* Scan/search functions */
int bstricmp (const struct bstr_slice_s b0, const struct bstr_slice_s b1);
//int bstrnicmp (const struct bstr_slice_s b0, const struct bstr_slice_s b1, int n);
//int bstrcmp (const struct bstr_slice_s b0, const struct bstr_slice_s b1);
//int bstrncmp (const struct bstr_slice_s b0, const struct bstr_slice_s b1, int n);



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

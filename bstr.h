/* Bstr 2.0 -- A C dynamic strings library
 *
 * Copyright (c) 2006-2015, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2015, Oran Agra
 * Copyright (c) 2015, Redis Labs, Inc
 * Paul Hsieh in 2002-2015
 * Michael Pollind 2024-* <mpollind at gmail dot com>
 * 
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of Redis nor the names of its contributors may be used
 *     to endorse or promote products derived from this software without
 *     specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

// this is a modified string library that implements features from both bstring and sds


#ifndef BSTR_H_INCLUDED
#define BSTR_H_INCLUDED

#include <stdio.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

struct bstr_s {
  size_t alloc;
  size_t len;
  char* buf;
};

// a const slice
struct bstr_const_slice_s {
  const char * buf;
  size_t len;
}; 
struct bstr_slice_s {
  char * buf;
  size_t len;
}; 

#ifdef __cplusplus
static inline struct bstr_const_slice_s bstr_const_ref(const char* c) { return (struct bstr_const_slice_s){c, strlen(c)}; }
static inline struct bstr_const_slice_s bstr_const_ref(struct bstr_s str) { return (struct bstr_const_slice_s){str.buf, str.len};}
static inline struct bstr_const_slice_s bstr_const_ref(struct bstr_slice_s slice){return (struct bstr_const_slice_s){slice.buf, slice.len};}

static inline struct bstr_slice_s bstr_ref(char *c) { return (struct bstr_slice_s){c, strlen(c)}; }
static inline struct bstr_slice_s bstr_ref(struct bstr_s str) { return (struct bstr_slice_s){str.buf, str.len}; }

static inline struct bstr_slice_s bstr_sub(struct bstr_slice_s slice, size_t a, size_t b) {
  assert((b - a) <= slice.len);
  return (struct bstr_slice_s){slice.buf + a, b - a};
}
static inline struct bstr_const_slice_s bstr_sub(struct bstr_const_slice_s slice, size_t a, size_t b) {
  assert((b - a) <= slice.len);
  return (struct bstr_const_slice_s){slice.buf + a, b - a};
}
#endif


#define BSTR_LLSTR_SIZE 21
#define BSTR_LSTR_SIZE 16 

#define bstr_avail_len(b) ((b).alloc - (b).len)
#define bstr_avail_slice(b)((struct bstr_slice_s){(b).buf + (b).len, bstr_avail_len(b)})
#define bstr_is_empty(b) ((b).buf == NULL || (b).len == 0)

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
static inline struct bstr_const_slice_s cstr_to_const_slice(const char* c) { return (struct bstr_const_slice_s){c, strlen(c)}; }
static inline struct bstr_const_slice_s bstr_to_const_slice(struct bstr_s str) { return (struct bstr_const_slice_s){str.buf, str.len};}
static inline struct bstr_const_slice_s bstr_slice_to_const_slice(struct bstr_slice_s slice){return (struct bstr_const_slice_s){slice.buf, slice.len};}

static inline struct bstr_slice_s cstr_to_slice(char *c) { return (struct bstr_slice_s){c, strlen(c)}; }
static inline struct bstr_slice_s bstr_to_slice(struct bstr_s str) { return (struct bstr_slice_s){str.buf, str.len}; }

static inline struct bstr_slice_s bstr_sub_slice(struct bstr_slice_s slice, size_t a, size_t b) {
  assert((b - a) <= slice.len);
  return (struct bstr_slice_s){slice.buf + a, b - a};
}
static inline struct bstr_const_slice_s bstr_sub_const_slice(struct bstr_const_slice_s slice, size_t a, size_t b) {
  assert((b - a) <= slice.len);
  return (struct bstr_const_slice_s){slice.buf + a, b - a};
}

#define bstr_const_ref(T) \
    _Generic((T), \
      struct bstr_s: bstr_to_const_slice, \
      struct bstr_slice_s: bstr_slice_to_const_slice, \
      char*: cstr_to_const_slice)(T)

#define bstr_ref(T) \
    _Generic((T), \
      struct bstr_s: bstr_to_slice, \
      char*: cstr_to_slice)(T)

#define bstr_sub(T,a,b) \
    _Generic((T), \
      struct bstr_const_slice_s: bstr_sub_const_slice, \
      struct bstr_slice_s: bstr_sub_slice)(T, a, b)

#endif


/**
 * Creates a string from a slice 
 **/
void bstrfree(struct bstr_s* str);
void bstrupper(struct bstr_slice_s slice);
void bstrlower(struct bstr_slice_s slice);

struct bstr_const_slice_s bstrtrim(struct bstr_const_slice_s slice);
struct bstr_const_slice_s bstrrtrim(struct bstr_const_slice_s slice);
struct bstr_const_slice_s bstrltrim(struct bstr_const_slice_s slice);

/* Enlarge the free space at the end of the bstr string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 *
 * Note: this does not change the *length* of the bstr string as len 
 * but only the free buffer space we have. */
bool bstrMakeRoomFor(struct bstr_s* str, size_t addlen);
/* 
 * set the length of the buffer to the length specified. this
 * will also trigger a realloction if the length is greater then the size
 * reserved. 
 *
 * Note: this does not set the null terminator for the string.
 * this will corrupt slices that are referencing a slice out of this buffer.
 **/
bool bstrsetlen(struct bstr_s* str, size_t len);
/**
 * set the amount of memory reserved by the bstr. will only ever increase
 * the size of the string 
 * 
 * A reserved string can be assigned with bstrAssign
 **/
bool bstrsetresv(struct bstr_s* str, size_t reserveLen); 

/** 
 * Modify an bstr string in-place to make it empty (zero length) set null terminator.
 * However all the existing buffer is not discarded but set as free space
 * so that next append operations will not require allocations up to the
 * number of bytes previously available. 
 **/
bool bstr_clear(struct bstr_s* str);

/**
 * takes a bstr and duplicates the underlying buffer.
 *
 * the buffer is trimmed down to the length of the string.
 *
 * if the buffer fails to allocate then BSTR_IS_EMPTY(b) will be true
 **/
struct bstr_s bstrDup(const struct bstr_s* str);
bool bstrAppendSlice(struct bstr_s* str, const struct bstr_const_slice_s slice);
bool bstrAppendChar(struct bstr_s* str, char b);
bool bstrInsertChar(struct bstr_s* str, size_t i, char b);
bool bstrInserSlice(struct bstr_s* str, size_t i, const struct bstr_const_slice_s slice);
bool bstrAssign(struct bstr_s* str, struct bstr_const_slice_s slice);
/**
 * resizes the allocation of the bstr will truncate if the allocation is less then the size 
 *
 * the buffer is trimmed down to the length of the string.
 *
 * If the buffer fails to reallocate then false is returned
 **/
bool bstrresize(struct bstr_s* str, size_t len);


struct bstr_split_iterable_s {
  const struct bstr_const_slice_s buffer; // the buffer to iterrate over
  const struct bstr_const_slice_s delim; // delim to split across 
  size_t cursor; // the current position in the buffer
};

/** 
 * splits a string using an iterator and returns a slice. a valid slice means there are 
 * are more slices.
 *
 * The the slice does not have a null terminator.
 *
 * struct bstr_split_iterator_s iterable = {
 *     .delim = bstr_ref(" "),
 *     .buffer = bstr_ref("one two three four five"),
 *     .cursor = 0
 * };
 * for(struct bstr_slice_s it = bstrSplitIterate(&iterable); 
 *  !bstr_is_empty(it); 
 *  it = bstrSplitIterate(&iterable)) {
 *   printf("Next substring: %.*s\n", slice.len, slice.buf); 
 * }
 *
 **/
struct bstr_const_slice_s bstrSplitIter(struct bstr_split_iterable_s*);

/** 
 * splits a string using an iterator and returns a slice. a valid slice means there are 
 * are more slices.
 *
 * For the reverse case move the cursor to the end of the string
 *
 * The the slice does not have a null terminator.
 *
 * struct bstr_split_iterator_s iterable = {
 *     .delim = bstr_ref(" "),
 *     .buffer = bstr_ref("one two three four five"),
 *     .cursor = 0
 * };
 * for(struct bstr_slice_s it = bstrSplitIterate(&iterable); 
 *  !bstr_is_empty(it); 
 *  it = bstrSplitIterate(&iterable)) {
 *   printf("Next substring: %.*s\n", slice.len, slice.buf); 
 * }
 *
 **/
struct bstr_const_slice_s bstrSplitRevIter(struct bstr_split_iterable_s*);

/* Set the bstr string length to the length as obtained with strlen(), so
 * considering as content only up to the first null term character.
 *
 * This function is useful when the bstr string has been changed where
 * the length is not correctly updated. using vsprintf for instance.
 *
 * After the call, slices are not valid if they reference this bstr 
 * 
 * s = bstrEmpty();
 * s[2] = '\0';
 * bstrUpdateLen(s);
 * printf("%d\n", s.len);
 *
 * The output will be "2", but if we comment out the call to bstrUpdateLen()
 * the output will be "6" as the string was modified but the logical length
 * remains 6 bytes. 
 ** */
bool bstrUpdateLen(struct bstr_s* str);

/* Append to the bstr string 's' a string obtained using printf-alike format
 * specifier.
 *
 * After the call, the modified bstr string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = bstrCreate("Sum is: ");
 * bstrcatprintf(s,"%d+%d = %d",a,b,a+b)
 *
 * if valid BSTR_OK else BSTR_ERR
 */
bool bstrcatprintf(struct bstr_s* s, const char *fmt, ...); 
bool bstrcatvprintf(struct bstr_s* str, const char* fmt, va_list ap);

int bstrsscanf(struct bstr_const_slice_s slice, const char* fmt, ...);
int bstrvsscanf(struct bstr_const_slice_s slice, const char* fmt, va_list ap);

/* This function is similar to bstrcatprintf, but much faster as it does
 * not rely on sprintf() family functions implemented by the libc that
 * are often very slow. Moreover directly handling the bstr as
 * new data is concatenated provides a performance improvement.
 *
 * However this function only handles an incompatible subset of printf-alike
 * format specifiers:
 *
 * %c - char
 * %s - C String
 * %S - struct bstr_const_slice_s slice 
 * %i - signed int
 * %l - signed long
 * %I - 64 bit signed integer (long long, int64_t)
 * %u - unsigned int
 * %L - unsigned long
 * %U - 64 bit unsigned integer (unsigned long long, uint64_t)
 * %% - Verbatim "%" character.
 */
bool bstrcatfmt(struct bstr_s*, char const *fmt, ...);


/*
 * join an array of slices and cat them to bstr. faster since the lengths are known ahead of time.
 * the buffer can be pre-reserved upfront.
 *
 * this modifies bstr so slices that reference this bstr can become invalid.
 **/
bool bstrcatjoin(struct bstr_s*, struct bstr_const_slice_s* slices, size_t numSlices, struct bstr_const_slice_s sep);
/*
 * join an array of strings and cat them to bstr 
 **/
bool bstrCatJoinCStr(struct bstr_s*, const char** argv, size_t argc, struct bstr_const_slice_s sep);

/**
 * this should fit safetly within BSTR_LLSTR_SIZE. 
 *
 * the number of bytes written to the slice is returned else -1 if the 
 * value is unable to be written or the length of the slice is greater
 *
 **/
int bstrfmtll(struct bstr_slice_s slice, long long value); 
int bstrfmtull(struct bstr_slice_s slice, unsigned long long value); 

/*  
 * Parse a string into a 64-bit integer.
 *
 * when base is 0, the function will try to determine the base from the string
 *   * if the string starts with 0x, the base will be 16
 *   * if the string starts with 0b, the base will be 2
 *   * if the string starts with 0o, the base will be 8
 *   * otherwise the base will be 10
 */
bool bstrReadll(struct bstr_const_slice_s, int base, long long* result);
bool bstrReadull(struct bstr_const_slice_s, int base, unsigned long long* result);

/* Scan/search functions */
/*  
 *  Compare two strings without differentiating between case. The return
 *  value is the difference of the values of the characters where the two
 *  strings first differ after lower case transformation, otherwise 0 is
 *  returned indicating that the strings are equal. If the lengths are
 *  different, if the first slice is longer 1 else -1. 
 */
int bstrCaselessCompare (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1);
/*
 *  The return value is the difference of the values of the characters where the
 *  two strings first differ after lower case transformation, otherwise 0 is
 *  returned indicating that the strings are equal. If the lengths are
 *  different, if the first slice is longer 1 else -1.
 */
int bstrCompare  (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1);
/**
*  Test if two strings are equal ignores case true else false.  
**/
bool bstrCaselessEqual (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1);
/**
*  Test if two strings are equal return true else false.  
**/
bool bstrEqual (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1);

int bstrIndexOfOffset(const struct bstr_const_slice_s haystack, size_t offset, const struct bstr_const_slice_s needle);
int bstrIndexOf(const struct bstr_const_slice_s haystack, const struct bstr_const_slice_s needle);
int bstrLastIndexOfOffset(const struct bstr_const_slice_s str, size_t offset, const struct bstr_const_slice_s needle);
int bstrLastIndexOf(const struct bstr_const_slice_s str, const struct bstr_const_slice_s needle);

int bstrIndexOfCaselessOffset(const struct bstr_const_slice_s haystack, size_t offset, const struct bstr_const_slice_s needle);
int bstrIndexOfCaseless(const struct bstr_const_slice_s haystack, const struct bstr_const_slice_s needle);
int bstrLastIndexOfCaseless(const struct bstr_const_slice_s haystack, const struct bstr_const_slice_s needle);
int bstrLastIndexOfCaselessOffset(const struct bstr_const_slice_s haystack, size_t offset, const struct bstr_const_slice_s needle);

int bstrIndexOfAny(const struct bstr_const_slice_s haystack, const struct bstr_const_slice_s characters);
int bstrLastIndexOfAny(const struct bstr_const_slice_s haystack, const struct bstr_const_slice_s characters);

#ifdef __cplusplus
}
#endif

#endif

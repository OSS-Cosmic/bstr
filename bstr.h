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


#ifndef _BSTR_H_
#define _BSTR_H_ 1

#include <stdio.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>

#define BSTR_LLSTR_SIZE 21
#define BSTR_LSTR_SIZE 16 

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
#define BSTR_TO_CONSTSLICE(a) (struct bstr_const_slice_s){(a).buf, (a).len}
#define CSTR_TO_CONSTSLICE(c) (struct bstr_const_slice_s){c, strlen(c)}
#define BSTR_CONSTSLICE_SUB(s, b, e) (struct bstr_const_slice_s){(s)->buf + (b), (e) - (b)} 
#define BSTR_TO_SLICE(a) (struct bstr_slice_s){(a)->buf, (a)->len}
#define CSTR_TO_SLICE(c) (struct bstr_slice_s){(const char*)(c), strlen(c)}
#define BSTR_AVIL(b) ((b)->alloc - (b)->len)
#define BSTR_AVAIL_SLICE(b)((struct bstr_slice_s){(b)->buf + (b)->len, BSTR_AVIL(b)})

#define BSTR_SLICE_EMPTY(b) ((b).len == 0 || (b).buf == NULL)
#define BSTR_IS_EMPTY(b) ((b).buff == NULL)

inline int bstrSliceValid(const struct bstr_const_slice_s slice);

/**
 * Creates a string from a slice 
 **/
void bstrFree(struct bstr_s* str);
void bstrToUpper(struct bstr_slice_s slice);
void bstrToLower(struct bstr_slice_s slice);

struct bstr_const_slice_s bstrTrim(struct bstr_const_slice_s slice);
struct bstr_const_slice_s bstrRightTrim(struct bstr_const_slice_s slice);
struct bstr_const_slice_s bstrLeftTrim(struct bstr_const_slice_s slice);

/* Enlarge the free space at the end of the bstr string so that the caller
 * is sure that after calling this function can overwrite up to addlen
 * bytes after the end of the string, plus one more byte for nul term.
 *
 * Note: this does not change the *length* of the bstr string as len 
 * but only the free buffer space we have. */
bool bstrMakeRoomFor(struct bstr_s* str, size_t addlen);
/* 
 * set the lenght of the buffer to the length specified. this
 * will also trigger a realloction if the lenght is greater then the size
 * reserved. 
 *
 * Note: this does not set the null terminator for the string.
 * this will corrupt slices that are referencing a slice out of this buffer.
 **/
bool bstrSetLen(struct bstr_s* str, size_t len);
/**
 * set the amount of memory reserved by the bstr. will only ever increase
 * the size of the string 
 * 
 * A reserved string can be assigned with bstrAssign
 **/
bool bstrSetReserve(struct bstr_s* str, size_t reserveLen); 

/**
 * takes a bstr and duplicates the underlying buffer.
 *
 * the buffer is trimmed down to the length of the string.
 *
 * if the buffer fails to allocate then BSTR_IS_EMPTY(b) will be true
 **/
struct bstr_s bstrDuplicate(const struct bstr_s* str);
bool bstrAppendSlice(struct bstr_s* str, const struct bstr_const_slice_s slice);
bool bstrAppendChar(struct bstr_s* str, char b);
bool bstrInsertChar(struct bstr_s* str, size_t i, char b);
bool bstrInsertSlice(struct bstr_s* str, size_t i, const struct bstr_const_slice_s slice);
bool bstrAssign(struct bstr_s* str, struct bstr_const_slice_s slice);

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
 *     .delim = BSTR_C_SLICE(" "),
 *     .buffer = BSTR_C_SLICE("one two three four five"),
 *     .cursor = 0
 * };
 * for(struct bstr_slice_s it = bstrSplitIterate(&iterable); 
 *  bstrSliceValid(it); 
 *  it = bstrSplitIterate(&iterable)) {
 *   printf("Next substring: %.*s\n", slice.len, slice.buf); 
 * }
 *
 **/
struct bstr_const_slice_s bstrSplitItr(struct bstr_split_iterable_s*);

/** 
 * splits a string using an iterator and returns a slice. a valid slice means there are 
 * are more slices.
 *
 * For the reverse case move the cursor to the end of the string
 *
 * The the slice does not have a null terminator.
 *
 * struct bstr_split_iterator_s iterable = {
 *     .delim = BSTR_C_SLICE(" "),
 *     .buffer = BSTR_C_SLICE("one two three four five"),
 *     .cursor = 0
 * };
 * for(struct bstr_slice_s it = bstrSplitIterate(&iterable); 
 *  bstrSliceValid(it); 
 *  it = bstrSplitIterate(&iterable)) {
 *   printf("Next substring: %.*s\n", slice.len, slice.buf); 
 * }
 *
 **/
struct bstr_const_slice_s bstrSplitIterReverse(struct bstr_split_iterable_s*);

/* Set the bstr string length to the length as obtained with strlen(), so
 * considering as content only up to the first null term character.
 *
 * This function is useful when the bstr string has been changed where
 * the lenght is not correctly updated. using vsprintf for instance.
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
 * remains 6 bytes. */
bool bstrUpdateLen(struct bstr_s* str);


/** 
 * Modify an bstr string in-place to make it empty (zero length) set null terminator.
 * However all the existing buffer is not discarded but set as free space
 * so that next append operations will not require allocations up to the
 * number of bytes previously available. 
 **/
bool bstrClear(struct bstr_s* str);


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
 * join an array of slices and cat them to bstr. faster since the lenghts are known ahead of time.
 * the buffer can be pre-reserved upfront.
 *
 * this modifies bstr so slices that reference this bstr can become invalid.
 **/
bool bstrCatJoin(struct bstr_s*, struct bstr_const_slice_s* slices, size_t numSlices, struct bstr_const_slice_s sep);
/*
 * join an array of strings and cat them to bstr 
 **/
bool bstrCatJoinCStr(struct bstr_s*, char** argv, size_t argc, struct bstr_const_slice_s sep);

/**
 * this should fit safetly within BSTR_LLSTR_SIZE. 
 *
 * the number of bytes written to the slice is returned else -1 if the 
 * value is unable to be written or the lenght of the slice is greater
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
bool bstrCaselessEq (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1);
/**
*  Test if two strings are equal return true else false.  
**/
bool bstrEq (const struct bstr_const_slice_s b0, const struct bstr_const_slice_s b1);

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

int bstrSliceValid(const struct bstr_const_slice_s slice) {
  return slice.buf != NULL;
}
#endif

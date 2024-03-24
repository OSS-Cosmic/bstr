/* Bstr 2.0 -- A C dynamic strings library
 *
 * Copyright (c) 2006-2015, Salvatore Sanfilippo <antirez at gmail dot com>
 * Copyright (c) 2015, Oran Agra
 * Copyright (c) 2015, Redis Labs, Inc
 * Paul Hsieh in 2002-2015
 * Michael Pollind2024 <mpollind at gmail dot com>
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

#define BSTR_ERR (-1)
#define BSTR_OK (0)
#define BSTR_BS_BUFF_LENGTH_GET (0)

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
#define BSTR_TO_SLICE(a) (struct bstr_slice_s){(a)->buf, (a)->len}
#define BSTR_C_SLICE(c) (struct bstr_slice_s){(const char*)(c), strlen(c)}
inline int bstrSliceValid(const struct bstr_slice_s slice);
inline struct bstr_slice_s bstrSliceSub(const struct bstr_slice_s, size_t begin, size_t end);

struct bstr_s bstrEmpty();
struct bstr_s bstrCreate(const char *init);
struct bstr_s bstrCreateLen(const char *init, size_t len);
void bstrFree(struct bstr_s* str);

int bstrAppendSlice(struct bstr_s* str, const struct bstr_slice_s slice);
int bstrAppendChar(struct bstr_s* str, char b);
int bstrInsertChar(struct bstr_s* str, size_t i, char b);
int bstrInsertSlice(struct bstr_s* str, size_t i, const struct bstr_slice_s slice);
int bstrAssign(struct bstr_s* str, const struct bstr_slice_s slice);

struct bstr_split_iterable_s {
  const struct bstr_slice_s buffer; // the buffer to iterrate over
  const struct bstr_slice_s delim; // delim to split across 
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
struct bstr_slice_s bstrSplitItr(struct bstr_split_iterable_s*);
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

struct bstr_slice_s bstrSplitIterReverse(struct bstr_split_iterable_s*);

/* Set the bstr string length to the length as obtained with strlen(), so
 * considering as content only up to the first null term character.
 *
 * This function is useful when the sds string is hacked manually in some
 * way, like in the following example:
 *
 * s = bstrEmpty();
 * s[2] = '\0';
 * bstrUpdateLen(s);
 * printf("%d\n", s.len);
 *
 * The output will be "2", but if we comment out the call to sdsupdatelen()
 * the output will be "6" as the string was modified but the logical length
 * remains 6 bytes. */
int bstrUpdateLen(struct bstr_s* str);

int bstrMakeRoomFor(struct bstr_s* str, size_t addlen);
int bstrSetLen(struct bstr_s* str, size_t len);

/* Modify an bstr string in-place to make it empty (zero length).
 * However all the existing buffer is not discarded but set as free space
 * so that next append operations will not require allocations up to the
 * number of bytes previously available. */
int bstrClear(struct bstr_s* str);

int bstrcatvprintf(struct bstr_s* str, const char* fmt, va_list ap);
int bstrcatprintf(struct bstr_s* s, const char *fmt, ...); 

/* Scan/search functions */
int bstrCaselessCompare (const struct bstr_slice_s b0, const struct bstr_slice_s b1);
int bstrCompare  (const struct bstr_slice_s b0, const struct bstr_slice_s b1);

int bstrCaselessEq (const struct bstr_slice_s b0, const struct bstr_slice_s b1);
int bstrEq (const struct bstr_slice_s b0, const struct bstr_slice_s b1);

int bstrIndexOfOffset(const struct bstr_slice_s haystack, size_t offset, const struct bstr_slice_s needle);
int bstrIndexOf(const struct bstr_slice_s haystack, const struct bstr_slice_s needle);
int bstrLastIndexOfOffset(const struct bstr_slice_s str, size_t offset, const struct bstr_slice_s needle);
int bstrLastIndexOf(const struct bstr_slice_s str, const struct bstr_slice_s needle);

int bstrIndexOfCaselessOffset(const struct bstr_slice_s haystack, size_t offset, const struct bstr_slice_s needle);
int bstrIndexOfCaseless(const struct bstr_slice_s haystack, const struct bstr_slice_s needle);
int bstrLastIndexOfCaseless(const struct bstr_slice_s haystack, const struct bstr_slice_s needle);
int bstrLastIndexOfCaselessOffset(const struct bstr_slice_s haystack, size_t offset, const struct bstr_slice_s needle);

int bstrIndexOfAny(const struct bstr_slice_s haystack, const struct bstr_slice_s characters);
int bstrLastIndexOfAny(const struct bstr_slice_s haystack, const struct bstr_slice_s characters);

struct bstr_slice_s bstrSliceSub(const struct bstr_slice_s s0, size_t begin, size_t end) {
  assert(begin <= end); 
  struct bstr_slice_s res = {
    .buf = s0.buf + begin,
    .len = end - begin
  };
  // the new slice has to be inbetween the incoming slice.
  assert(end <= s0.len); 
  assert(begin <= s0.len); 
  return res;
}
int bstrSliceValid(const struct bstr_slice_s slice) {
  return slice.len > 0 && slice.buf != NULL;
}
#endif

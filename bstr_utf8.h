/*
 * This source file is part of the bstring string library.  This code was
 * written by Paul Hsieh in 2002-2015, and is covered by the BSD open source
 * license and the GPL. Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef _BSTR_UTF8_H_
#define _BSTR_UTF8_H_ 1

#include "bstr.h"


#define isLegalUnicodeCodePoint(v)  ((((v) < 0xD800L) || ((v) > 0xDFFFL)) && (((unsigned long)(v)) <= 0x0010FFFFL) && (((v)|0x1F0001) != 0x1FFFFFL))

struct bstr_utf8_iterable_s {
  const struct bstr_const_slice_s buffer; // the buffer to iterrate over
  size_t cursor;
};

//struct size_t utf8CodePointLen(struct bstr_const_slice_s slice);
/**
 * iterates to the next chunk if its an invalid utf8 character then the invalid
 * chunk is returned
 **/
struct bstr_const_slice_s bstrUtf8CodePointIter(struct bstr_utf8_iterable_s* iter);
//struct bstr_const_slice_s utf8CodePointIterRev(struct bstr_utf8_iterable_s* iter);
/**
* converts a slice to utf8 codepoint.  
**/
uint32_t bstrSliceToUtf8CodePoint(struct bstr_const_slice_s slice, uint32_t errorcode);

#endif

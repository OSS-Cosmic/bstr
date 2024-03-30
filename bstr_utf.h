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

struct bstr_utf_iterable_s {
  const struct bstr_const_slice_s buffer; // the buffer to iterrate over
  size_t cursor;
};


struct bstr_utf_result_s {
  uint32_t codePoint;
  uint8_t invalid: 1;
  uint8_t finished: 1; // marks the final character in the sequence
};

#ifdef __cplusplus
extern "C" {
#endif

struct bstr_utf_result_s bstrUtf8NextCodePoint(struct bstr_utf_iterable_s* iter); 

// https://datatracker.ietf.org/doc/html/rfc2781
struct bstr_utf_result_s bstrUtf16NextCodePoint(struct bstr_utf_iterable_s* iter);
#ifdef __cplusplus
}
#endif

#endif

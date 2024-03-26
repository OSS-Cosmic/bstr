/*
 * This source file is part of the bstring string library.  This code was
 * written by Paul Hsieh in 2002-2015, and is covered by the BSD open source
 * license and the GPL. Refer to the accompanying documentation for details
 * on usage and license.
 */
#ifndef _BSTR_UTF8_H_
#define _BSTR_UTF8_H_ 1

#include "bstr.h"
#include <limits.h>

#if INT_MAX >= 0x7fffffffUL
typedef int 			cpUcs4;
#elif LONG_MAX >= 0x7fffffffUL
typedef long			cpUcs4;
#else
#error This compiler is not supported
#endif

#if UINT_MAX == 0xFFFF
typedef unsigned int	cpUcs2;
#elif USHRT_MAX == 0xFFFF
typedef unsigned short	cpUcs2;
#elif UCHAR_MAX == 0xFFFF
typedef unsigned char	cpUcs2;
#else
#error This compiler is not supported
#endif

#define isLegalUnicodeCodePoint(v) ((((v) < 0xD800L) || ((v) > 0xDFFFL)) && (((unsigned long)(v)) <= 0x0010FFFFL) && (((v)|0x1F0001) != 0x1FFFFFL))

struct bstr_utf8_iterable_s {
  const struct bstr_const_slice_s buffer; // the buffer to iterrate over

};

#endif

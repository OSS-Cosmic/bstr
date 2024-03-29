#include "bstr_utf.h"
#include "bstr.h"


struct bstr_const_slice_s bstrUtf8CodePointIter(struct bstr_utf8_iterable_s* iter){
  unsigned char c0 = iter->buffer.buf[iter->cursor];
  const size_t startCursor = iter->cursor;
  if (c0 < 0xC0 || c0 > 0xFD) {
    iter->cursor = (iter->cursor + 1 > iter->buffer.len) ? iter->buffer.len : iter->cursor + 1;
    return bstr_sub_const_slice(iter->buffer, startCursor, iter->cursor);
  } else if (c0 < 0xE0) {
    iter->cursor = (iter->cursor + 2 > iter->buffer.len) ? iter->buffer.len : iter->cursor + 2;
    return bstr_sub_const_slice(iter->buffer, startCursor, iter->cursor);
  } else if (c0 < 0xF0) {
    iter->cursor = (iter->cursor + 3 > iter->buffer.len) ? iter->buffer.len : iter->cursor + 3;
    return bstr_sub_const_slice(iter->buffer, startCursor, iter->cursor);
  } else if (c0 < 0xF8) {
    iter->cursor = (iter->cursor + 4 > iter->buffer.len) ? iter->buffer.len : iter->cursor + 4;
    return bstr_sub_const_slice(iter->buffer, startCursor, iter->cursor);
  }
  iter->cursor++;
  for (; iter->cursor < iter->buffer.len; iter->cursor++) {
    if ((iter->buffer.buf[iter->cursor] & 0xC0) != 0x80)
      break;
  }
  return bstr_sub_const_slice(iter->buffer, startCursor, iter->cursor);
}

uint32_t bstrSliceToUtf8CodePoint(struct bstr_const_slice_s slice, uint32_t errorcode) {
  const unsigned char c0 = slice.buf[0];
  if (c0 < 0xC0 || c0 > 0xFD) {
    if(slice.len != 1)
      return errorcode;
		if (c0 >= 0x80) return errorcode;
		return c0;
  } else if (c0 < 0xE0) {
    if(slice.len != 2)
      return errorcode;
    const unsigned char c1 = (unsigned char) ((unsigned) slice.buf[1] - 0x080); 
		const uint32_t codePoint = ((c0 << 6u) - (0x0C0 << 6u)) + c1;
		if (c1 >= 0x40 || codePoint < 0x80) return errorcode;
		return codePoint;
  } else if (c0 < 0xF0) {
    if(slice.len != 3)
      return errorcode;
    const unsigned char c1 = (unsigned char) ((unsigned) slice.buf[1] - 0x080); 
    const unsigned char c2 = (unsigned char) ((unsigned) slice.buf[2] - 0x080); 
		const uint32_t codePoint = ((c0 << 12) - (0x0E0 << 12u)) + (c1 << 6u) + c2;
		if ((c1|c2) >= 0x40 || codePoint < 0x80 || !isLegalUnicodeCodePoint (codePoint)) return errorcode;
		return codePoint;
  } else if (c0 < 0xF8) {
    if(slice.len != 4)
      return errorcode;
		const unsigned char c1 = (unsigned char) ((unsigned)  slice.buf[1] - 0x080);
		const unsigned char c2 = (unsigned char) ((unsigned)  slice.buf[2] - 0x080);
		const unsigned char c3 = (unsigned char) ((unsigned)  slice.buf[3] - 0x080);
    const uint32_t codePoint = ((c0 << 18) - (0x0F0 << 18u)) + (c1 << 12u) + (c2 << 6u) + c3; 
		if ((c1|c2|c3) >= 0x40 || codePoint < 0x10000 || !isLegalUnicodeCodePoint (codePoint)) return errorcode;
    return codePoint;
  }
  return errorcode;
}

uint32_t bstrSliceToUtf16CodePoint(struct bstr_const_slice_s slice, uint32_t errorcode) {
  const uint16_t w1 =  ((unsigned)slice.buf[1] << 8) | (unsigned) slice.buf[0];
  if(w1 < 0xD800 || w1 > 0xDFFF) {
    return w1;
  }
  if (w1 < 0xD800 || w1 > 0xDBFF)
    return errorcode;
  if (4 != slice.len)
    return errorcode;
  const uint16_t  w2 =  ((unsigned)slice.buf[3] << 8) | (unsigned)slice.buf[2];
  if (w2 < 0xDC00 || w2 > 0xDFFF)
    return errorcode;
  return (((w1 & 0x3FF) << 10) | (w2 & 0x3FF)) + 0x10000;
}




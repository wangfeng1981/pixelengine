
#ifndef WF_COMPRESS_H
#define WF_COMPRESS_H
#include "zlib.h"     // declare the external fns -- uses zconf.h, too


  int GetMaxCompressedLen(int nLenSrc);
  int CompressData(const Byte* abSrc, int nLenSrc, Byte* abDst, int nLenDst);

  int UncompressData(const Byte* abSrc, int nLenSrc, Byte* abDst, int nLenDst);





#endif

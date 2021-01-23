
#ifndef CXX
#define CXX

// ................................................ default header

#define _GNU_SOURCE

#define _XOPEN_SOURCE_EXTENDED 1

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>
#include <uchar.h>
#include <xmmintrin.h>
#include <locale.h>
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <float.h>
#include <stdarg.h>
#include <locale.h>
#include <stdbool.h>
#include <errno.h>
#include <wctype.h>
#include <time.h>
#include <errno.h>


#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <io.h>
    #include <fcntl.h>
#endif

#ifdef __linux__
    #include <wchar.h>
#endif

#ifdef __APPLE__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdollar-in-identifier-extension"
#endif

// ................................................ header cxx

#include "gc.h"
#include "stdio.h"
#include "argParse.h"
#include "hmap.h"
#include "iterator.h"
#include "mvector.h"
#include "mstack.h"
#include "string.h"

// .............................................. string.h

#ifndef MB_CUR_MAX
    #if defined(_MSC_VER)
        #define MB_CUR_MAX __mb_cur_max
    #else
        #define MB_CUR_MAX 2
    #endif
#endif


// ................................................ gcc

#ifdef __GNUC__
#pragma GCC diagnostic ignored "-Wunused-local-typedefs"
#endif

// ................................................ merge

#ifndef CONCATENATE
#define CONCATENATE(x,y)   x##y
#endif

#ifndef MERGE
#define MERGE(x,y)    CONCATENATE(x,y)
#endif

// ................................................ debug

// warning: ISO C does not support ‘__FUNCTION__’ '__PRETTY_FUNCTION__' predefined identifier [-Wpedantic]
#define debugInfo(...) fprintf ( stderr,"## file[%s] line[%d] fn[%s].\n",__FILE__,__LINE__,__func__);/*__VA_ARGS__*/

// ................................................ new

#define new(ID)        gcMalloc(sizeof(ID));

// ................................................ delete

#define delete(ID)    if((ID)!=NULL){gcFree(ID);(ID)=NULL;}

// ................................................ maxStringBuffer

#define gcMaxStringBuffer 4096

#define wdebug(...) fwprintf(stderr,__VA_ARGS__)
#define debug(...)  fprintf (stderr,__VA_ARGS__)

// questo per evitare warning comparazione ( signed wchar_t / unsigned wchar_t )
#define _WEOF (wchar_t)WEOF

// (not standard) : error: use of undeclared identifier 'errno_t'
typedef int errno_t;


#endif



/**/



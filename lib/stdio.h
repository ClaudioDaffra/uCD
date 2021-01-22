#ifndef cdSTDIO
#define cdSTDIO

#include "cxx.h"

// questo per evitare warning comparazione ( signed wchar_t / unsigned wchar_t )
#define _WEOF (wchar_t)WEOF

// (not standard ) : error: use of undeclared identifier 'errno_t'
typedef int errno_t;

typedef enum console_stream_e
{
    
    console_stream_ansi=100,
    console_stream_wide=200
    
} console_stream_t ;

extern console_stream_t std_stream;

extern void cdConsolesetUTF8    ( void ) ;
#define     consoleSetUTF8      cdConsolesetUTF8
#define     stdConsoleSetUTF8   cdConsolesetUTF8

void         consoleSetMBS      (void) ;
#define     stdConsoleSetMBS    consoleSetMBS

int           cdFileWOpen       ( FILE** pf, char* fileName, const char* flag , const char* ccs ) ;
#define     stdFileWOpen        cdFileWOpen

#endif 



/**/



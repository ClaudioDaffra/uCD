

#include "stdio.h"

// .................................................................... stream

console_stream_t std_stream = console_stream_ansi; 

// .................................................................... setUTF8

#if defined(_WIN32) || defined(_WIN64) 

    void cdConsolesetUTF8(void)
    {
        //const wchar_t*   _fontName=L"SimSun-ExtB";
        const wchar_t   _fontName[]=L"SimSun-ExtB";
        SHORT           _fontWeight=FW_NORMAL;
        SHORT           _fontSizeY=16;
        SHORT           _fontSizeX=0;    

        // set font
        
        CONSOLE_FONT_INFOEX info ;
        info.cbSize       = sizeof(info);
        info.dwFontSize.Y = _fontSizeY; // 16
        info.dwFontSize.X = _fontSizeX; // 0    
        info.FontWeight   = _fontWeight;

        wcscpy_s( info.FaceName, _countof(_fontName) , _fontName ) ;
        
        SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), false, &info);
        
        // set stream
        
         _wsetlocale(LC_ALL , L"it_IT.UTF-8" );  // set locale wide string
        _setmode(_fileno(stdout), _O_U8TEXT);    // set Locale for console
        _setmode(_fileno(stderr), _O_U8TEXT);    // set Locale for console    
        _setmode(_fileno(stdin ), _O_U8TEXT);    // set Locale for console      

        // set CHCP : 936 1252 > win7 : 65001
        
        int chcp=936;            
        SetConsoleCP( chcp ) ;             
        SetConsoleOutputCP( chcp ); 

        // questo per evitare errori di conversione delle stringhe esadecimali
        setlocale ( LC_NUMERIC , "C" ) ;
        std_stream = console_stream_wide ;

    } 
 
#else
   
    void cdConsolesetUTF8(void) 
    {
        setlocale(LC_ALL, "it_IT.UTF-8");

        // questo per evitare errori di conversione delle stringhe esadecimali   
        setlocale ( LC_NUMERIC , "C" ) ;
        
        std_stream = console_stream_wide ;
    }

#endif

void consoleSetMBS(void)
{
#ifdef _MSC_VER
    //const wchar_t*   _fontName=L"SimSun-ExtB";
    const wchar_t   _fontName[]=L"SimSun-ExtB";
    SHORT           _fontWeight=FW_NORMAL;
    SHORT           _fontSizeY=16;
    SHORT           _fontSizeX=0;    

    // set font
    
    CONSOLE_FONT_INFOEX info ;
    info.cbSize       = sizeof(info);
    info.dwFontSize.Y = _fontSizeY; // 16
    info.dwFontSize.X = _fontSizeX; // 0    
    info.FontWeight   = _fontWeight;

    wcscpy_s( info.FaceName, _countof(_fontName) , _fontName ) ;
    
    SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), false, &info);
    int chcp=65001;            
    SetConsoleCP( chcp ) ;             
    SetConsoleOutputCP( chcp ); 
    
    setlocale(LC_ALL,"it_IT.utf8");
    setlocale(LC_NUMERIC , "C" ) ; 
    
#else

    setlocale(LC_ALL,"");
    setlocale(LC_NUMERIC , "C" ) ;    
    
#endif
}


// .................................................................... open file wide stream

int cdFileWOpen( 
    FILE** pf,
    char* fileName, 
    const char* flag , 
    const char* ccs  
) 
{
    wchar_t buffer[128] ;
    buffer[0]=L'\0';

    wchar_t* _fileName  = cnvS8toWS ( fileName  ) ;
    wchar_t* _flag      = cnvS8toWS ( (char*)flag  ) ;
    wchar_t* _ccs       = cnvS8toWS ( (char*)ccs  ) ;
    
    if (_fileName==NULL) return 3 ;

    if ( wcslen(_flag)+wcslen(_ccs) > 14 )
    {
        return 2 ;
    }

    wcscat (buffer,_flag);
    wcscat (buffer,L",");
    wcscat (buffer,_ccs);   

#if defined(_WIN32) || defined(_WIN64)  

    errno_t err=_wfopen_s( pf,_fileName , buffer  );

    if ( err ) 
    {
        fwprintf(stdout,L"\n? Error Open File : [%ls] for [%ls].",_fileName,_flag);  
        return 1;
    }
    
#else
    
    for (uint32_t i=0;i<strlen(fileName);i++ )
    {
        if ( fileName[i]=='\\' ) fileName[i] = '/' ;
    }
     
    #ifdef __APPLE__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wself-assign"
    // per evitare unused parameter in linux
    #endif

    _ccs        = _ccs      ;
    buffer[0]   = L'\0'     ;
    _fileName   = _fileName ;

    #ifdef __APPLE__
    #pragma GCC diagnostic pop
    #endif

    *pf = fopen ( fileName , flag ) ;
    if ( *pf==NULL )
    {
        fwprintf(stdout,L"\n? Error Open File : [%ls] for [%ls].",_fileName,_flag);
        return 1;
    }
    
#endif

    return 0 ;
}


/**/



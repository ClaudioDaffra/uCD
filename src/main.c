
#include "../lib/cxx.h"
#include "global.h"
#include "error.h"
#include "lexer.h"

// *********
//  GLOBAL
// *********

global_t g = 
{
    0       ,    // fDebug
    NULL    ,    // file input name
    NULL    ,    // *
    NULL    ,    // file output name
    NULL    ,    // *
    NULL    ,    // file output stream
    //
    makeFileWithNewExt       	,
    outputSpecialCharInChar   	,
    outputSpecialCharInString	,
};

char* makeFileWithNewExt( char* pFileName , char* newExt )
{
    char*	pFileNameExt     = strrchr( pFileName,'.'	) ;
    int		fileNameLen      = strlen ( pFileName 		) ;
    
    if  ( pFileNameExt==NULL ) pFileNameExt = pFileName + fileNameLen ;

    int     newExtLen 		= strlen ( newExt	) ;
    int     fileNameNewLen	= ( pFileNameExt - pFileName ) + 1 + newExtLen ;
    char*	pFileNameNew	= gcMalloc( sizeof(char) * fileNameNewLen ) ;

    pFileNameNew[0] = '\0' ;
    strncat ( pFileNameNew , pFileName , ( pFileNameExt - pFileName ) ) ;
    strcat  ( pFileNameNew , newExt ) ;

    return pFileNameNew ;
}

wchar_t    outputSpecialCharInChar( wchar_t _wchar )
{
    if ( _wchar==0 ) return L'§';
    
    // \r viene saltato \f \v \t \f \' \" \\ vengono trasformati
    wchar_t wchar = _wchar ;
    
    switch ( _wchar ) 
    {
        case L'\a'     : wchar = L'A'; break ;		
        case L'\n'     : wchar = L'N'; break ;
        case L'\t'     : wchar = L'T'; break ;
        case L'\v'     : wchar = L'V'; break ;
        case L'\f'     : wchar = L'F'; break ;
        case L'\''     : wchar = L'\''; break ;
        case L'\"'     : wchar = L'\"'; break ;
        case L'\\'     : wchar = L'\\'; break ;
        default        : break;
    } 

    return wchar ;
}

wchar_t*    outputSpecialCharInString( wchar_t* token )
{
    if ( token == NULL ) return gcWcsDup(L"{null}");
    
    for ( size_t i=0; i < wcslen( token ) ; i++ )
    {
        token[i] = outputSpecialCharInChar( token[i] ) ;
    }
    return token ;
}

// *********
//  MAIN
// *********

#define $PRG  "CD"
#define $DATE "(1/2021)"
#define $REV  "alpha 0.0"

int main (int argc , const char** argv )
{
    gcStart();

    // *********
    //  ARGPARSE
    // *********
 
    argParseUsage ( usages ,
        $PRG" [options] [[-,/,--] args]",
        $PRG" [options]"
    ) ;
 
    argParseOption ( options ) 
    {
        OPT_HELP(),
        OPT_GROUP   ("Basic options:\n"),
            OPT_STRING  ('i', "input"   , &g.fileInputName      , "file input to read"  , NULL, 0, 0 ),
            OPT_STRING  ('o', "output"  , &g.fileOutputName     , "file output to write", NULL, 0, 0 ),
            OPT_BOOLEAN ('d', "debug"   , &g.fDebug             , "debug"               , NULL, 0, 0 ),     
        OPT_END()
    };    
 
    argparse_t argparse;

    argParseInit( &argparse, options, usages, 0);

    argParseDescribe ( &argparse, 
        "\n"$PRG" intepreter "$REV, 
        "\nby Claudio Daffra "$DATE"."
    );

    argParseStart ( &argparse ) ;

    // *********
    //  BEGIN
    // *********

    if ( g.fileInputName == NULL )
    {
        $loader( fatal , checkFileExists , noInputFiles , 0 , 0 , NULL , NULL );
    }
    else
    {
        if ( g.fDebug ) fwprintf ( stdout,L"\n# main  -> initialising file ...\n"  );
        
        stdFileWOpen ( &g.pFileInput , g.fileInputName , "r","ccs=UTF-8") ;    // ................... file input

        if ( g.fileOutputName == NULL ) // ..................................................... file output
        {
        g.fileOutputName          = makeFileWithNewExt( g.fileInputName , ".output" ) ;            
        }
        stdFileWOpen ( &g.pFileOutput , g.fileOutputName , "w+","ccs=UTF-8") ;

        if ( g.fDebug )
        {
            fwprintf ( stdout , L"\n%-20ls : [%018p] -> [%-20hs]" ,L"file input name"    ,g.pFileInput  	,g.fileInputName 	) ;
            fwprintf ( stdout , L"\n%-20ls : [%018p] -> [%-20hs]" ,L"file output name"   ,g.pFileOutput  	,g.fileOutputName 	) ;                                                    
        }

        stdConsoleSetUTF8();
        
        if (g.fDebug) fwprintf ( stdout,L"\n\nConsole set to UTF-8\n") ;

    }
    
	// *********
	//  LEXER
	// *********

	plexer_t lexer = lexerAlloc();

	lexerCtor(lexer);

	if ( g.fDebug == 1 ) lexer->fDebug = 1 ;

	// LEXER
	
		lexerInclude ( lexer , g.fileInputName ) ;
		
		for(;;)
		{
			lexerGetToken(lexer) ; // ottien il token
			if ( lexer->sym == sym_end ) break; 
		}
		
	// END LEXER
            
	lexerDtor(lexer);

	lexerDealloc(lexer);
		                 
    // *********
    //  END
    // *********
 
	if (g.pFileInput)	fclose(g.pFileInput);
	
	if (g.pFileOutput)	fclose(g.pFileOutput);
 
    printErrLog();

    gcStop();
    
    fwprintf ( stdout,L"\n" ) ;
  
    return 0 ;
}



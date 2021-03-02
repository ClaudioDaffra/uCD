#ifndef cdParser
#define cdParser

#include "../lib/cxx.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "ast.h"

// MATCH

wchar_t* doMatchMessage( wchar_t C0 , wchar_t* token )  ;

#define $MATCH( SYM_EXPECTED , WCHAR_EXPECTED )\
do {\
    if ( this->lexer->sym == SYM_EXPECTED )\
    {\
        parserGetToken(this);\
    }\
    else\
    {\
        $matchError( doMatchMessage(WCHAR_EXPECTED,this->lexer->token) ) ;\
        return NULL ;\
    }\
} while(0) ;


// parser : fields

struct parser_s
{
    int			fDebug        			;
    FILE*		pFileOutputParser		;  
    char*		fileNameOutputParser	;
    char*		fileInputName      		;
    plexer_t	lexer              		;
    past_t 		ast                     ; 
} ;

typedef struct parser_s     parser_t ;
typedef struct parser_s*    pparser_t ;

// parser.c

pparser_t	parserAlloc       	( void ) ;                
void		parserDealloc     	( pparser_t this ) ;
void		parserCtor        	( pparser_t this ) ;
void		parserDtor        	( pparser_t this ) ;
int         parserPrintToken	( pparser_t this ) ;
sym_t       parserGetToken     	( pparser_t this ) ;

pnode_t     parserProgram     	( pparser_t this , node_t* nBlock ) ;
pnode_t     parserScan         	( pparser_t this ) ;

// expr.c

node_t*     parserTerm    		( pparser_t this ) ;
node_t*     parserMulDivMod		( pparser_t this ) ;
node_t*     parserAddSub		( pparser_t this ) ;
node_t*		parserShiftLR		( pparser_t this ) ;
node_t*		parserOpRelLG		( pparser_t this ) ;
node_t*		parserOpRelEqNe		( pparser_t this ) ;
node_t* 	parserBitAnd		( pparser_t this ) ;
node_t* 	parserBitOr			( pparser_t this ) ;
node_t* 	parserBitXor		( pparser_t this ) ;
node_t* 	parserAnd			( pparser_t this ) ;
node_t* 	parserOr			( pparser_t this ) ;
node_t* 	parserXor			( pparser_t this ) ;
node_t* 	parserTerOp			( pparser_t this ) ;
node_t* 	parserAssign		( pparser_t this ) ;
node_t* 	parserExpr			( pparser_t this ) ;

// decl.c

node_t* 	parserPostFixArray	( pparser_t this , node_t* left ) ;
node_t* 	parserDeclT1		( pparser_t this , wchar_t* id ) ;
node_t* 	parserDeclT2		( pparser_t this , wchar_t* id ) ;
node_t* 	parserDeclT3		( pparser_t this , wchar_t* id ) ;
node_t* 	parserDeclT4		( pparser_t this , wchar_t* id ) ;
node_t*		parserDeclType		( pparser_t this , wchar_t* id ) ;
node_t* 	parserDecl			( pparser_t this ) ;

// statement.c

node_t*  	parserStatSub		( pparser_t this ) ;

#endif



/**/



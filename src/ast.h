#ifndef cdAST
#define cdAST

#include "../lib/cxx.h"
#include "global.h"
#include "error.h"
#include "lexer.h"

// .................................... forward declaration

typedef struct node_s     node_t ;

typedef struct node_s *   pnode_t ;

// .............................................. scope

enum stScope_e
{
    stScopeGlobal    ,
    stScopeLocal     ,
} ;

typedef enum stScope_e stScope_t ;

// .................................... enumerativi nodo

enum enodeType
{
    nTypeUndefined      ,   // 0
    
    nTypeTermInteger    ,   // 1
    nTypeTermReal       ,   // 2 
    nTypeTermChar       ,   // 3
    nTypeTermString     ,   // 4 
    nTypeTermID      	,   // 5 

    nTypeBinOp=10       ,   // 10
    nTypePrefix         ,   // 11
    nTypePostfix        ,   // 12 
    nTypeTerOp          ,   // 13       
    nTypeBlock          ,   // 14
    nTypeAssign         ,   // 15        

    nTypeDeclT1=20 		,   // 20 
    nTypeDeclT2         ,   // 21 
    nTypeDeclT3         ,   // 22     
    nTypeDeclT4         ,   // 23    
    nTypeDeclType		,   // 24
                        
} ;

typedef enum enodeType     enodeType_t;

// .................................... nodo terminale ( integer / real / id(caso speciale) )

typedef struct nodeTerm_s
{
        int64_t     integer  ;     // integer  / wchar_t  / byte
        double      real     ;     // float / double
        wchar_t     wchar    ;     // character
        wchar_t*    wstring  ;     // string    
        wchar_t*    id       ;     // id / keyword            
} 
nodeTerm_t ;

// .................................... nodo operatore binario

typedef struct nodeBinOp_s
{
    sym_t       sym     ;
    pnode_t     left    ;
    pnode_t     right   ;
}
nodeBinOp_t ;

// .................................... nodo operatore prefisso

typedef struct nodePrefix_s
{
    sym_t   sym     ;
    pnode_t right   ;   // ! 1 il nodo che c'è a destra
} 
nodePrefix_t ;

// .................................... nodo blocco

vectorTypeDef(pnode_t,next);

typedef struct nodeBlock_s
{
    vector_next_t	next ;    // vettore di nodi
    
} nodeBlock_t ;

// .................................... nodo operatore postfisso

typedef struct nodePostfix_s
{
    sym_t   sym    ;
    pnode_t left   ;   // ! 1 il nodo che c'è a sinistra
	union 
	{
		pnode_t array  	;   // [	... arr [1][2][3]
		pnode_t param  	;   // (	... sub ( 1,2,3 )   
		pnode_t vStruct ;   // .	... ___  a.b.c	  
	} ;
} 
nodePostfix_t ;

// .................................... nodo operatore ternario

typedef struct nodeTerOp_s
{
    sym_t       sym     ;
    pnode_t     cond    ;    
    pnode_t     left    ;
    pnode_t     right   ;
}
nodeTerOp_t ;

// ***********
// declaration
// ***********

// .................................... decl t1

typedef struct nodeDeclT1_s
{
    wchar_t*    id          ;        //  name 
    wchar_t*    type        ;        //  integer real char byte TYPE ?

} nodeDeclT1_t ;

// .................................... decl t2

typedef struct nodeDeclT2_s
{
    wchar_t*    id          ;        //  name 
    node_t*		array		;	     //  []
    node_t*		type        ;        //  integer real char byte TYPE ?

} nodeDeclT2_t ;

// .................................... decl t3

typedef struct nodeDeclT3_s
{
    wchar_t*    id          ;        //  name 
    node_t*		type        ;        //  return type integer real char byte or * type /function /array

} nodeDeclT3_t ;

// .................................... decl t4

typedef struct nodeDeclT4_s
{
	sym_t		sym			;		 // variabile array o funzione
    wchar_t*    id          ;        // pointer name 
    node_t*		type        ;        // return type integer real char byte or * type /function /array

} nodeDeclT4_t ;

// .................................... decl type

typedef struct nodeDeclType_s
{
    wchar_t*    id          ;        // type name
    node_t*		fields      ;        // campi della struttura

} nodeDeclType_t ;

// .................................... term : array dim [][][]

typedef struct nodePostFixArray_s
{
    pnode_t         dim     ;
    node_t*			left	;
    
} nodePostFixArray_t ;

// .................................... nodo  lhs := rhs

typedef struct nodeAssign_s
{
    pnode_t        lhs        ;
    pnode_t        rhs        ;
    
} nodeAssign_t ;

// *****************
//	NODO PRINCIPE
// *****************

struct node_s
{
    enodeType_t     type    ;
    uint32_t        row     ;
    uint32_t        col     ;
    wchar_t*        token   ;
        
    union 
    {
        nodeTerm_t              term          	;	// integer real wchar wstring id
            
        nodeBinOp_t             binOp         ;
        nodePrefix_t            prefix        ;
        nodePostfix_t           postfix       ;        
        nodeBlock_t             block         ;
        nodeTerOp_t             terOp         ;        
        nodeAssign_t            assign        ;
 
        nodeDeclT1_t			declT1        ; 
        nodeDeclT2_t   			declT2        ;
        nodeDeclT3_t     		declT3        ;
        nodeDeclT4_t     		declT4        ;
        nodeDeclType_t     		declType	  ;                                                         
    } ;
    
} ;

typedef struct node_s      node_t  ;

typedef struct node_s*     pnode_t ;

// ast : fields

struct ast_s
{
    int          fDebug                ;
    FILE*        pFileOutputAST        ;
    char*        fileNameOutputAST     ;  
    FILE*        pFileOutputNode       ;
    char*        fileNameOutputNode    ; 
} ;

typedef struct ast_s      ast_t     ;
typedef struct ast_s*     past_t ;

// struttura per i prefissi -> expr.c

struct sPrefixOp_s
{
    sym_t       sym      		;
    uint32_t    row_start 		;
    uint32_t    col_start 		;
    wchar_t*    token       	;
    wchar_t*    fileInputName	;
} ;

typedef struct sPrefixOp_s      sPrefixOp_t ;

typedef struct sPrefixOp_s*     psPrefixOp_t ;

// ast : methods

past_t      astAlloc                  ( void ) ;
void        astDealloc                ( past_t this ) ;
void        astCtor                   ( past_t this , char* fileInputName ) ;
void        astDtor                   ( past_t this ) ;

pnode_t     astNodeDebug              ( past_t this , pnode_t n ) ;
void        astDebug                  ( past_t this , pnode_t n ) ;

// ast.c expr.c

node_t*     astMakeNodeTermInteger    ( past_t this , plexer_t lexer , int64_t		_integer	) ;
node_t*     astMakeNodeTermReal       ( past_t this , plexer_t lexer , double		_real		) ;
node_t*     astMakeNodeTermChar       ( past_t this , plexer_t lexer , wchar_t		_wchar		) ;
node_t*     astMakeNodeTermString     ( past_t this , plexer_t lexer , wchar_t*		_wstring    ) ;
node_t* 	astMakeNodeTermID		  ( past_t this , plexer_t lexer , wchar_t* 	_id 		) ;

node_t*     astMakeNodeBinOP          ( past_t this , plexer_t lexer , sym_t sym , node_t* left , node_t* right ) ;
node_t*		astMakeNodeTerOP		  ( past_t this , plexer_t lexer , sym_t sym , node_t* cond , node_t* left , node_t* right  ) ;
node_t*     astMakeNodePrefix         ( past_t this , psPrefixOp_t prefix , node_t* left ) ;
node_t* 	astMakeNodePostfix		  ( past_t this , plexer_t lexer , node_t* left ) ;
node_t*     astMakeNodeBlock          ( past_t this ) ;
size_t      astPushNodeBlock          ( past_t this , node_t * nBlock     , node_t * next );

node_t*     astMakeNodeAssign         ( past_t this , plexer_t lexer ,  node_t * lhs , node_t * rhs ) ;

// ast.c decl.c

node_t* 	astMakeDeclT1			  ( past_t this , plexer_t lexer , wchar_t* _id  , wchar_t* _type ) ;
node_t* 	astMakeDeclT2			  ( past_t this , plexer_t lexer , node_t* array , node_t*   type ) ;
node_t* 	astMakeDeclT3			  ( past_t this , plexer_t lexer , node_t* type ) ;
node_t* 	astMakeDeclT4			  ( past_t this , plexer_t lexer , wchar_t* _id, node_t* _type ) ;
node_t* 	astMakeDeclType			  ( past_t this , plexer_t lexer , wchar_t* _id, node_t* _fields ) ;



#endif



/**/



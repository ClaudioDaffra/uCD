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
    stScopeLocal    ,
    //stScopeFuncParam
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
/* 
	nTypeTermStruct		,	// 6	+ campi della struttura
    nTypeTermVar        ,   // 7
*/
//    nTypeArray      	,   // 8
/*
    nTypeTermFunction   ,   // 9
*/
    nTypeBinOp          ,   // 10
    nTypePrefix         ,   // 11
    nTypePostfix        ,   // 12 
    nTypeTerOp          ,   // 13       
    nTypeBlock          ,   // 14
    //nTypeStruct         ,   // 15  
    
    nTypeAssign         ,    // 19        
/*    
    nTypeDeclConst      ,    // decl const global local 
    nTypeDeclVar        ,    // decl var     global local    
    nTypeDeclArray      ,    // decl array global local   
    nTypeArrayDim       ,    // array dim [][][] ...   
    nTypeDeclType       ,    // type declaration 
    nTypeDeclFunction   ,    // dichiarazione di funzione   
*/   
    nTypeDeclT1         ,   // 15 
    nTypeDeclT2         ,   // 16 
            
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


// declaration


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


/*
// .................................... nodo    costanti globali / locali

typedef struct nodeDeclConst_s
{
    
    wchar_t*    id          ;        //    c
    sym_t       sym         ;        //    sym_integer
    pnode_t     term        ;        //    makeNodeInteger ( 1 ) ;
    stScope_t   scope       ;        //    local global

} nodeDeclConst_t ;

// .................................... nodo    Var globali / locali

typedef struct nodeDeclVar_s
{
    wchar_t*    id          ;        //    v
    sym_t       sym         ;        //    sym_kw_integer , sym_kw_real , sym_kw_char , sym_kw_byte  , sym_id
    stScope_t   scope       ;        //    local global    
    pnode_t     expr        ;        //    expr
    int         size        ;        //    sizeof
} nodeDeclVar_t ;


// .................................... nodo    Var globali / locali

typedef struct nodeDeclType_s
{
    wchar_t*     id           ;        //    struct name
    stScope_t    scope        ;        //    local global
    vectorStruct( pnode_t , field ) ;
    //    initializer list    : var p1 : point := { }
} nodeDeclType_t ;

*/
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
        //nodeArray_t         	array     		; 
        //nodeTermFunction_t      termFunction  ;
        //nodeTermVar_t           termVar       ;
        //nodeTermField_t         termField     ; // 1 campo    
        //nodeTermStruct_t        termStruct    ; // + campi             
        nodeBinOp_t             binOp         ;
        nodePrefix_t            prefix        ;
        nodePostfix_t           postfix       ;        
        nodeBlock_t             block         ;
        nodeTerOp_t             terOp         ;        
        nodeAssign_t            assign        ;
        //nodeArrayDim_t          arrayDim      ; 
        //nodeDeclType_t          declType      ;
        //nodeDeclFunction_t      declFunction  ;   
        nodeDeclT1_t            	declT1       ; 
        nodeDeclT2_t            	declT2       ;                         
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


//pnode_t     astMakeNodeTermFunction   ( past_t this , wchar_t* id  , pnode_t pArrayParam ) ;
//node_t*     astMakeNodeTermField      ( past_t this , plexer_t lexer , wchar_t* id ) ;
//node_t* 	astMakeNodeTermStruct	  ( past_t this ) ;

node_t*     astMakeNodeBinOP          ( past_t this , plexer_t lexer , sym_t sym , node_t* left , node_t* right ) ;
node_t*		astMakeNodeTerOP		  ( past_t this , plexer_t lexer , sym_t sym , node_t* cond , node_t* left , node_t* right  ) ;
node_t*     astMakeNodePrefix         ( past_t this , psPrefixOp_t prefix , node_t* left ) ;
node_t* 	astMakeNodePostfix		  ( past_t this , plexer_t lexer , node_t* left ) ;
node_t*     astMakeNodeBlock          ( past_t this ) ;
size_t      astPushNodeBlock          ( past_t this , node_t * nBlock     , node_t * next );

//size_t      astPushAllNodeBlock       ( past_t this , node_t * nBlockDest , node_t * nBlockSource ) ;


//pnode_t     astMakeNodeDeclType       ( past_t this , wchar_t* id , stScope_t    scope )  ;
//pnode_t     astMakeNodeDeclFunction   ( past_t this , wchar_t* id , sym_t retType , pnode_t pParamList , pnode_t pBlockCode ) ;

node_t*     astMakeNodeAssign         ( past_t this , plexer_t lexer ,  node_t * lhs , node_t * rhs ) ;

// ast.c decl.c

node_t* 	astMakeDeclT1			  ( past_t this , plexer_t lexer , wchar_t* _id  , wchar_t* _type ) ;
node_t* 	astMakeDeclT2			  ( past_t this , plexer_t lexer , node_t* array , node_t*   type ) ;

#endif



/**/



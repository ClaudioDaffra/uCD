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
    nTypeTermArray      ,   // 8
    nTypeTermFunction   ,   // 9
*/
    nTypeBinOp          ,   // 10
    nTypePrefix         ,   // 11
    nTypePostfix        ,   // 12       
    nTypeBlock          ,   // 13
/*    
    nTypeDeclConst      ,    // decl const global local 
    nTypeDeclVar        ,    // decl var     global local    
    nTypeDeclArray      ,    // decl array global local   
    nTypeArrayDim       ,    // array dim [][][] ...   
    nTypeDeclType       ,    // type declaration 
    nTypeDeclFunction   ,    // dichiarazione di funzione   
*/   
    nTypeAssign         ,    // 19 := 
        
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

// .................................... nodo operatore postfisso

typedef struct nodePostfix_s
{
    sym_t   sym     ;
    pnode_t left   ;   // ! 1 il nodo che c'è a sinistra
} 
nodePostfix_t ;

// .................................... nodo blocco

vectorTypeDef(pnode_t,next);

typedef struct nodeBlock_s
{
    vector_next_t	next ;    // vettore di nodi
    
} nodeBlock_t ;
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

// .................................... array dim [][][]

typedef struct nodeArrayDim_s
{
    
    vectorStruct(pnode_t,ndx);        // vettore di indici
    
} nodeArrayDim_t ;

// .................................... nodo    Var globali / locali

typedef struct nodeDeclArray_s
{
    wchar_t*     id            ;        //    v
    sym_t        sym           ;        //    sym_kw_integer , sym_kw_real , sym_kw_char , sym_kw_byte , sym_id
    stScope_t    scope         ;        //    local global    
    pnode_t      dim           ;        //  vettore dimensioni
    pnode_t      il            ;        //    initializer list
    int          size          ;        //  sizeof
} nodeDeclArray_t ;

// .................................... nodo    Var globali / locali

typedef struct nodeDeclType_s
{
    wchar_t*     id           ;        //    struct name
    stScope_t    scope        ;        //    local global
    vectorStruct( pnode_t , field ) ;
    //    initializer list    : var p1 : point := { }
} nodeDeclType_t ;

// .................................... term : array dim [][][]

typedef struct nodeTermArray_s
{
    wchar_t*        id      ;
    pnode_t         dim     ;
    
} nodeTermArray_t ;

// .................................... term : function f1(,,)

typedef struct nodeTermFunction_s
{
    wchar_t*        id        ;
    pnode_t         param     ;
    
} nodeTermFunction_t ;


// .................................... nodo variabili semplici    :    var

typedef struct nodeTermVar_s
{
    wchar_t*    id            ;
    
} nodeTermVar_t ;

// .................................... nodo campo della stuttura :    var or array of type of not : a[10] ; x ; ( single ) 

typedef struct nodeTermField_s
{
    wchar_t*    id            ;
    
} nodeTermField_t ;


// .................................... nodo campo della stuttura :    var or array of type of not : a[10].x ; ( multi ) 

typedef struct nodeTermStruct_s
{
    wchar_t*    	id           		;	// struct name		utile per recuperare gli offset
    void*			pvst				;	// void pointer to sym table
    vectorStruct( pnode_t , vField )    ;	// vettore campi
    
} nodeTermStruct_t ;

*/
// .................................... nodo  lhs := rhs

typedef struct nodeAssign_s
{
    pnode_t        lhs        ;
    pnode_t        rhs        ;
    
} nodeAssign_t ;

/*
// .................................... nodo  decl function

typedef struct nodeDeclFunction_s
{
    wchar_t*     id                    ;    // name
    sym_t        retType               ;    // parameter return    
    vectorStruct( pnode_t , param )    ;    // parameter list
    //node_t*        paramList         ;    // param List
    node_t*        blockCode           ;    // block code
    
} nodeDeclFunction_t ;
*/

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
        nodeTerm_t              term          ;	// integer real wchar wstring id
        //nodeTermArray_t         termArray     ; 
        //nodeTermFunction_t      termFunction  ;
        //nodeTermVar_t           termVar       ;
        //nodeTermField_t         termField     ;	// 1 campo    
        //nodeTermStruct_t        termStruct    ; // + campi             
        nodeBinOp_t             binOp         ;
        nodePrefix_t            prefix        ;
        nodePostfix_t           postfix       ;        
        nodeBlock_t             block         ;
        //nodeDeclConst_t         declConst     ; 
        //nodeDeclVar_t           declVar       ; 
        nodeAssign_t            assign        ;
        //nodeDeclArray_t         declArray     ;
        //nodeArrayDim_t          arrayDim      ; 
        //nodeDeclType_t          declType      ;
        //nodeDeclFunction_t      declFunction  ;   
                  
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

node_t*     astMakeNodeTermInteger    ( past_t this , plexer_t lexer , int64_t		_integer	) ;
node_t*     astMakeNodeTermReal       ( past_t this , plexer_t lexer , double		_real		) ;
node_t*     astMakeNodeTermChar       ( past_t this , plexer_t lexer , wchar_t		_wchar		) ;
node_t*     astMakeNodeTermString     ( past_t this , plexer_t lexer , wchar_t*		_wstring    ) ;
node_t* 	astMakeNodeTermID		  ( past_t this , plexer_t lexer , wchar_t* 	_id 		) ;

//node_t*     astMakeNodeTermVar        ( past_t this , wchar_t* _name , uint32_t row , uint32_t col ) ;
//pnode_t     astMakeNodeTermArray      ( past_t this , wchar_t* id  , pnode_t pArrayDim )     ;
//pnode_t     astMakeNodeTermFunction   ( past_t this , wchar_t* id  , pnode_t pArrayParam ) ;
//node_t*     astMakeNodeTermField      ( past_t this , plexer_t lexer , wchar_t* id ) ;
//node_t* 	astMakeNodeTermStruct	  ( past_t this ) ;

node_t*     astMakeNodeBinOP          ( past_t this , plexer_t lexer , sym_t sym , node_t* left , node_t* right ) ;
node_t*     astMakeNodePrefix         ( past_t this , psPrefixOp_t prefix , node_t* left ) ;
node_t* 	astMakeNodePostfix		  ( past_t this , plexer_t lexer , node_t* left ) ;
node_t*     astMakeNodeBlock          ( past_t this ) ;
size_t      astPushNodeBlock          ( past_t this , node_t * nBlock     , node_t * next );

//size_t      astPushAllNodeBlock       ( past_t this , node_t * nBlockDest , node_t * nBlockSource ) ;

//pnode_t     astMakeNodeDeclConst      ( past_t this , wchar_t* id ,  sym_t sym , pnode_t _term     , stScope_t    scope ) ;
//pnode_t     astMakeNodeDeclVar        ( past_t this , wchar_t* id ,  sym_t sym , pnode_t _expr     , stScope_t    scope ) ;

//pnode_t     astMakeNodeDeclArray      ( past_t this , wchar_t* id ,  pnode_t _dim  , sym_t sym , pnode_t _il , stScope_t    scope ) ;
//pnode_t     astMakeNodeArrayDim       ( past_t this ) ;

//pnode_t     astMakeNodeDeclType       ( past_t this , wchar_t* id , stScope_t    scope )  ;
//pnode_t     astMakeNodeDeclFunction   ( past_t this , wchar_t* id , sym_t retType , pnode_t pParamList , pnode_t pBlockCode ) ;

node_t*     astMakeNodeAssign         ( past_t this , plexer_t lexer ,  node_t * lhs , node_t * rhs ) ;



#endif



/**/



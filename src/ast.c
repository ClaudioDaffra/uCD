#include "global.h"
#include "error.h"
#include "ast.h"
//#include "symTable.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wrestrict"
#endif

// ......................................................... ast alloc

past_t  astAlloc ( void )
{
    past_t past = gcMalloc( sizeof(ast_t) ) ;
    
    if ( past==NULL )
    {
        $astInternal( malloc , outOfMemory , NULL , NULL ) ;
    }
    else // init default parameter
    {
        past->fDebug              = 0         ;
        past->pFileOutputAST      = NULL      ;  
        past->fileNameOutputAST   = NULL      ; 
        past->pFileOutputNode     = NULL      ;  
        past->fileNameOutputNode  = NULL      ; 
    }
    return past ;
}

// ......................................................... ast dealloc

void      astDealloc ( past_t this )
{
    if ( this!=NULL )
    {
        gcFree(this);
    }
    else
    {
        $astInternal( dealloc , errUnknown , NULL , NULL );
    }
}

// ......................................................... ast constructor

void astCtor( past_t this , char* fileInputName )
{
    // file debug parser
    
    if ( this->fDebug ) 
    {
        if ( fileInputName!=NULL )
        {

            // .ast 
            
            this->fileNameOutputAST  = g.makeFileWithNewExt( fileInputName , ".ast"  ) ;
            stdFileWOpen ( &this->pFileOutputAST , this->fileNameOutputAST , "w+","ccs=UTF-8" ) ;
            
            if ( this->pFileOutputAST != NULL )
            {
                fwprintf ( this->pFileOutputAST , L"\n%-20ls : [%018p] -> [%-20hs]\n" 
                    ,L"file ast"        
                    ,this->pFileOutputAST 
                    ,this->fileNameOutputAST
                ) ;
            }
            
            // .node
            
            this->fileNameOutputNode  = g.makeFileWithNewExt( fileInputName , ".node"  ) ;
            stdFileWOpen ( &this->pFileOutputNode , this->fileNameOutputNode , "w+","ccs=UTF-8" ) ;
            
            if ( this->pFileOutputNode != NULL ) // \n\n per meglio visualizzare i blocchi
            {
                fwprintf ( this->pFileOutputNode , L"\n%-20ls : [%018p] -> [%-20hs]\n\n" 
                    ,L"file node"   
                    ,this->pFileOutputNode 
                    ,this->fileNameOutputNode
                ) ;
            }

        }
        else
        {
            $astInternal( checkFileExists , noInputFiles , L"{null}" , NULL );
        }
    }
}

// ......................................................... ast destructor

void astDtor ( past_t this )
{
    if (this->fDebug)     
    {
        fclose(this->pFileOutputAST);
        fclose(this->pFileOutputNode);
    } ;
}

// TERM : INTEGER

node_t* astMakeNodeTermInteger( past_t this , plexer_t lexer , int64_t _integer )
{
    #ifdef _MSC_VER
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%lld]\n",L"astMakeNodeTermInteger",_integer ) ;
    #else
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ld]\n",L"astMakeNodeTermInteger",_integer  ) ;
    #endif

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermInteger") ;

    nNew->type         = nTypeTermInteger ;
    nNew->term.integer = _integer ;
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;   

    return nNew ;
}

// TERM : REAL

node_t* astMakeNodeTermReal( past_t this , plexer_t lexer , double _real )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%lf]\n",L"astMakeNodeTermReal",_real ) ;
  
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermReal") ;

    nNew->type         = nTypeTermReal ;
    nNew->term.real    = _real ;

    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;  
    
    return nNew ;
}

// TERM : CHAR

node_t* astMakeNodeTermChar( past_t this , plexer_t lexer , wchar_t _wchar )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%lc]\n",L"astMakeNodeTermChar",g.outputSpecialCharInChar(_wchar) );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermChar") ;

    nNew->type             = nTypeTermChar ;
    nNew->term.wchar     = _wchar ;
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;    

    return nNew ;
}

// TERM : STRING

node_t* astMakeNodeTermString( past_t this , plexer_t lexer , wchar_t* _wstring )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeNodeTermString",g.outputSpecialCharInString(_wstring) );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermString") ;

    nNew->type         = nTypeTermString ;
    nNew->term.wstring = gcWcsDup(_wstring);
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;    

    return nNew ;
}

// TERM : ID

node_t* astMakeNodeTermID( past_t this , plexer_t lexer , wchar_t* _id )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeNodeTermID",_id );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTermID") ;

    nNew->type		= nTypeTermID ;
    nNew->term.id 	= gcWcsDup(_id);
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;    

    return nNew ;
}

// BINOP

node_t* astMakeNodeBinOP(  past_t this , plexer_t lexer , sym_t sym , node_t* left , node_t* right  )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: sym [%d]\n",L"astMakeNodeBinOP",sym );
  
    if ( left==NULL || right==NULL ) // se uno dei due operandi è nullo 
    {
        $pushErrLog( parser,error,parseExpr,expectedPrimaryExprBefore,lexer->row,lexer->col,lexer->fileInputName,lexer->token ) ;
    }
 
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeBinOP") ;

    nNew->type          = nTypeBinOp;
    nNew->binOp.sym     = sym       ;  
    nNew->binOp.left    = left      ;
    nNew->binOp.right   = right     ;

    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;  
    
    return nNew ;
}

// PREFIX

node_t* astMakeNodePrefix( past_t this , psPrefixOp_t prefix , node_t* left ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%d]\n",L"astMakeNodePrefix",prefix->sym );
    
     if ( left==NULL ) 
    {
        $pushErrLog( parser,error,parseExpr,expectedPrimaryExprAfter,prefix->row_start,prefix->col_start,prefix->fileInputName,prefix->token ) ;
    } 

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodePrefix") ;

    nNew->type         = nTypePrefix    ;
    nNew->prefix.sym   = prefix->sym    ;
    nNew->prefix.right = left             ;

    nNew->row    =    prefix->row_start ;
    nNew->col    =    prefix->col_start -1 ;
    nNew->token  =    gcWcsDup(prefix->token)  ; 

    return nNew ;
}

// POSTFIX

node_t* astMakeNodePostfix(  past_t this , plexer_t lexer , node_t* left ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%d]\n",L"astMakeNodePostfix",lexer->sym );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodePostfix") ;

    nNew->type         = nTypePostfix 	;
    nNew->postfix.sym  = lexer->sym    	;
    nNew->postfix.left = left     		;

    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start -1 ;
    nNew->token  =    gcWcsDup(lexer->token)  ; 

    return nNew ;
}

// TEROP

node_t* astMakeNodeTerOP( past_t this , plexer_t lexer , sym_t sym , node_t* cond , node_t* left , node_t* right  )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: sym [%d]\n",L"astMakeNodeTerOP",sym );
 
    if ( cond==NULL || left==NULL || right==NULL ) // se uno dei tre operandi è nullo 
    {
        $pushErrLog( parser,error,parseExpr,expectedPrimaryExprBefore,lexer->row,lexer->col,lexer->fileInputName,lexer->token ) ;
    }

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeTerOP") ;

    nNew->type          = nTypeTerOp;
    nNew->terOp.sym     = sym		;     
    nNew->terOp.cond    = cond      ;      
    nNew->terOp.left    = left      ;
    nNew->terOp.right   = right     ;

    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ;  
    
    return nNew ;
}

// BLOCK

node_t* astMakeNodeBlock( past_t this ) 
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeBlock" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeBlock") ;

    nNew->type = nTypeBlock  ;

    nNew->row    =    0;
    nNew->col    =    0;
    nNew->token  =    NULL ;

    vectorAlloc(nNew->block.next,128);

    return nNew ;
}
 
size_t astPushNodeBlock( past_t this , node_t * nBlock , node_t * next ) 
{
    if ( nBlock  == NULL ) return 0 ;
    if ( next    == NULL ) return 0 ;
    
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [block]:[%018p] -> [next]:[%018p]\n",L"astPushNodeBlock",nBlock,next );
    
    if ( next != NULL )    vectorPushBack( nBlock->block.next , next  ) ;
    
    return vectorSize(nBlock->block.next);
}
/*
size_t     astPushAllNodeBlock    ( past_t this , node_t * nBlockDest ,  node_t * nBlockSource )
{
    if ( nBlockDest   == NULL ) return 0 ;
    if ( nBlockSource == NULL ) return 0 ;
    
    size_t kBlockSize = vectorSize ( nBlockSource->block.next ) ;

    if (kBlockSize > 0 )
    {
        for ( uint32_t i = 0 ; i < kBlockSize ; i++ )
        {
            astPushNodeBlock( this , nBlockDest , nBlockSource->block.next.data[i] );
        }
    }
    return kBlockSize ;
} 

*/
// node terminale variabile semplice

node_t* astMakeNodeAssign  (  past_t this , plexer_t lexer ,  node_t * lhs , node_t * rhs )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls \n",L"astMakeNodeAssign" );
    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeNodeAssign") ;

    nNew->type = nTypeAssign  ;
    nNew->assign.lhs = lhs ;
    nNew->assign.rhs = rhs ;

    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(lexer->token)  ; ;

    return nNew ;
}

// decl t1

node_t* astMakeDeclT1( past_t this , plexer_t lexer , wchar_t* _id , wchar_t* _type )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeDeclT1",_id );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeDeclT1") ;

    nNew->type				= nTypeDeclT1 ;
    nNew->declT1.id 		= gcWcsDup(_id);
    nNew->declT1.type 		= gcWcsDup(_type);
    nNew->declT1.qualifier	= qualNull ;
    
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup(_id)  ;    

    return nNew ;
}

// decl t2

node_t* astMakeDeclT2( past_t this , plexer_t lexer , node_t* array , node_t* type )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls\n",L"astMakeDeclT2");

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeDeclT2") ;

    nNew->type				= nTypeDeclT2 ;
    nNew->declT2.array		= array ;
    nNew->declT2.type		= type ;
    nNew->declT2.id			= NULL ; 
    nNew->declT2.qualifier	= qualNull ;
	        
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    NULL;   

    return nNew ;
}

// decl t3

node_t* astMakeDeclT3( past_t this , plexer_t lexer , node_t* type )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls\n",L"astMakeDeclT3" );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeDeclT3") ;

    nNew->type				= nTypeDeclT3 ;
    nNew->declT3.type		= type ;
    nNew->declT3.qualifier	= qualNull ;
	        
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    NULL;    

    return nNew ;
}

// decl t4

node_t* astMakeDeclT4( past_t this , plexer_t lexer , wchar_t* _id, node_t* _type )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeDeclT4",_id );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeDeclT4") ;

    nNew->type			= nTypeDeclT4 ;
    nNew->declT4.id		= gcWcsDup(_id) ;
    nNew->declT4.type	= _type ;
     nNew->declT4.qualifier	= qualNull ;
                
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup( _id )  ;    

    return nNew ;
}

// decl type

node_t* astMakeDeclType( past_t this , plexer_t lexer , wchar_t* _id, node_t* _fields )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeDeclType",_id );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeDeclType") ;

    nNew->type				= nTypeDeclType ;
    nNew->declType.id		= gcWcsDup(_id) ;
    nNew->declType.fields	= _fields ; 
            
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup( _id )  ;    

    return nNew ;
}

// statement sub namespace label

node_t* astMakeStatSub( past_t this , plexer_t lexer , wchar_t* _id, node_t* _body )
{
    if ( this->fDebug ) fwprintf ( this->pFileOutputAST , L"%-30ls :: [%ls]\n",L"astMakeStatSub",_id );

    node_t* nNew   = NULL ; // new node
    
    nNew = gcMalloc ( sizeof(node_t) ) ;
    if ( nNew==NULL ) $astInternal ( malloc , outOfMemory , L"ast.c" , L"astMakeStatSub") ;

    nNew->type		= nTypeStatSub ;
    nNew->sub.id	= gcWcsDup(_id) ;
    nNew->sub.body	= _body ; 
            
    nNew->row    =    lexer->row_start ;
    nNew->col    =    lexer->col_start - 1;
    nNew->token  =    gcWcsDup( _id )  ;    

    return nNew ;
}

// ***********
// astDebug
// ***********

#define printTab\
	for(int i=0;i<nodeLevel;i++)\
		if ( this->fDebug )\
			fwprintf ( this->pFileOutputNode , L"   " ) ;

#define $astDebugRowColToken(FLAG)    if ( this->FLAG ) fwprintf ( this->pFileOutputNode , L" { %03d/%03d :: %ls }\n",n->row,n->col,n->token );

node_t* astNodeDebug( past_t this , node_t* n) 
{
	if ( n == NULL ) return NULL  ;

	// level

	static int nodeLevel=0;

	printTab;
 
	switch ( n->type )
	{
	  case  nTypeUndefined : // .............................................................................. nTypeUndefined

			if ( this->fDebug ) 
			{
				fwprintf ( this->pFileOutputNode , L"node [%018p] undefined  :: \n",(void*)n );
			}
			
			break;   
		  
	  case  nTypeTermInteger : // .............................................................................. nTypeTermInteger

			if ( this->fDebug ) 
			{
				fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%lld]",(void*)n,L"term integer"  ,n->term.integer );
				$astDebugRowColToken(fDebug);   
			}          
			
			break;
			
	   case  nTypeTermReal : // .............................................................................. nTypeTermReal

			if ( this->fDebug ) 
			{    
				fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%lf]",(void*)n,L"term real" ,n->term.real );
				$astDebugRowColToken(fDebug);
			}

			break;  

	  case  nTypeTermChar : // .............................................................................. nTypeTermChar

			if ( this->fDebug ) 
			{
				fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%lc]",(void*)n,L"term char"  ,g.outputSpecialCharInChar(n->term.wchar) );
				$astDebugRowColToken(fDebug);
			}             
			
			break;
			
	  case  nTypeTermString : // .............................................................................. nTypeTermString

			if ( this->fDebug ) 
			{
				fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%ls]",(void*)n,L"term string"  ,g.outputSpecialCharInString(n->term.wstring) );
				$astDebugRowColToken(fDebug);
			}             
			
			break;
			
	  case  nTypeTermID : // .............................................................................. nTypeTermID

			if ( this->fDebug ) 
			{
				fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%ls]",(void*)n,L"term id"  ,n->term.id );
				$astDebugRowColToken(fDebug);
			}             
			
			break;	
       
		case  nTypeBinOp : // .............................................................................. nTypeBinOp

			astNodeDebug( this,n->binOp.right ) ;
			astNodeDebug( this,n->binOp.left  ) ;

			 if ( this->fDebug ) 
			 {
				 printTab;
				 fwprintf ( 
					this->pFileOutputNode ,L"node [%018p] %-16ls :: Left[%018p] Right[%018p] :: sym [%d]",(void*)n,L"BinOp" 
					,(void*)n->binOp.left 
					,(void*)n->binOp.right
					,n->binOp.sym 
				);
				$astDebugRowColToken(fDebug);
			}

			break;             

		case  nTypePrefix : // .............................................................................. nTypePrefix

			astNodeDebug( this,n->prefix.right ) ;

			if ( this->fDebug ) 
			{
				printTab;
				fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%03d]",(void*)n,L"prefix" ,n->prefix.sym );
				$astDebugRowColToken(fDebug);
			}

			break;  

		case  nTypePostfix : // .............................................................................. nTypePostfix

			if ( this->fDebug ) 
			{
				switch ( n->postfix.sym )
				{
					case sym_inc :
					case sym_dec :
					
						astNodeDebug( this,n->postfix.left ) ;
						printTab;					
						fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%03d]",(void*)n,L"postfix" ,n->postfix.sym );
					break ;
					
					case sym_pq0 :
					
						astNodeDebug( this,n->postfix.left ) ;					
						astNodeDebug( this,n->postfix.array) ;
						printTab;
						fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%03d] dim[%03d]"
							,(void*)n,L"postfix" ,n->postfix.sym,n->postfix.array->block.next.size );
					break ;

					case sym_p0 :
					
						astNodeDebug( this,n->postfix.left ) ;					
						astNodeDebug( this,n->postfix.param) ;
						printTab;
						fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%03d] dim[%03d]"
							,(void*)n,L"postfix" ,n->postfix.sym,n->postfix.param->block.next.size );
					break ;
				
					case sym_dot :
					
						astNodeDebug( this,n->postfix.left ) ;					
						astNodeDebug( this,n->postfix.vStruct) ;
						printTab;
						fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%03d] dim[%03d]"
							,(void*)n,L"postfix" ,n->postfix.sym,n->postfix.vStruct->block.next.size );
					break ;			
											
					default:
						fwprintf ( this->pFileOutputNode , L"node [%018p] %-16ls :: [%03d]",(void*)n,L"postfix" ,n->postfix.sym );
						fwprintf ( this->pFileOutputNode , L"\n!! not implemented yet\n" ,n->postfix.sym );						
						exit(-1);
					break ;
				}
				$astDebugRowColToken(fDebug);
			}

			break; 

		case  nTypeTerOp : // .............................................................................. nTypeTerOp

			astNodeDebug( this,n->terOp.right ) ;
			astNodeDebug( this,n->terOp.left  ) ;
			astNodeDebug( this,n->terOp.cond ) ;
					
			 if ( this->fDebug ) 
			 {
				 printTab;
				 fwprintf ( 
					this->pFileOutputNode ,L"node [%018p] %-16ls :: cond[%018p] Left[%018p] Right[%018p] :: sym [%d]"
					,(void*)n
					,L"BinOp" 
					,(void*)n->terOp.cond 					
					,(void*)n->terOp.left 
					,(void*)n->terOp.right
					,n->terOp.sym 
				);
				$astDebugRowColToken(fDebug);
			}

			break;   
						
		case  nTypeBlock : // .............................................................................. nTypeBlock

			if ( this->fDebug ) 
			{
				size_t size = n->block.next.size ;
				if (size>0) size--;

				fwprintf ( this->pFileOutputNode , L"\n" ) ;
				printTab ;

				fwprintf ( this->pFileOutputNode , L"node [%018p] size[%03zu] %-16ls\n",(void*)n,size,L"block");
				printTab;
				fwprintf ( this->pFileOutputNode , L"{");				
				++nodeLevel ;
			}

			for (size_t i = 0 ; i<n->block.next.size; i++)
			{
				if ( this->fDebug ) 
				{    
					if ( n->block.next.data[i] != NULL )
					{
						fwprintf ( this->pFileOutputNode , L"\n" ) ;
						printTab ;
						fwprintf ( this->pFileOutputNode , L"node [%018p] -> [%03d/%03d]::[%018p] :: \n\n"
							,n,(int)i,(int)n->block.next.size-1,n->block.next.data[i] );
					}
				}
				if ( n->block.next.data[i] != NULL ) 
				{
					astNodeDebug( this,n->block.next.data[i] ) ; 
				}
			}
			
			if ( this->fDebug ) 
			{
				--nodeLevel ;
			}           
			printTab ;
			fwprintf ( this->pFileOutputNode , L"}\n\n"); 

			break; 

		case nTypeAssign : // .............................................................................. nTypeAssign

			astNodeDebug( this,n->assign.lhs ) ;
			
			astNodeDebug( this,n->assign.rhs ) ;

			if ( this->fDebug ) 
			{
				printTab;
				fwprintf 
					( 
						this->pFileOutputNode , L"node [%018p] %-16ls :: lhs [%018p] rhs [%018p]"
						,(void*)n
						,L"nTypeAssign" 
						,(void*)n->assign.lhs
						,(void*)n->assign.rhs  
					);
				$astDebugRowColToken(fDebug);
			}
		
		break ;  

		case nTypeDeclT1 : // .............................................................................. decl t1	id :: type

			if ( this->fDebug ) 
			{
				printTab;
				fwprintf 
					( 
						this->pFileOutputNode , L"node [%018p] %-16ls :: type [%ls] id [%ls] qual(%d)"
						,(void*)n
						,L"nDeclT1" 
						,(void*)n->declT1.type
						,(void*)n->declT1.id
						,(void*)n->declT1.qualifier 
					);
				$astDebugRowColToken(fDebug);
			}
		
		break ;

		case nTypeDeclT2 : // .............................................................................. decl t2	id :: [] type

			astNodeDebug( this , n->declT2.array ) ;
			
			if ( this->fDebug ) 
			{
				printTab;
				fwprintf 
					( 
						this->pFileOutputNode , L"node [%018p] %-16ls :: array[%p] type [%ls] id [%ls] qual(%d)"
						,(void*)n
						,L"nDeclT2" 
						,(void*)n->declT2.array
						,(void*)n->declT2.type->declT1.type
						,(void*)n->declT2.id 
						,(void*)n->declT2.qualifier						
					);
				$astDebugRowColToken(fDebug);
			}
		
		break ;

		case nTypeDeclT3 : // .............................................................................. decl t3	id :: () type

			if ( this->fDebug ) 
			{
				printTab;
				fwprintf 
					( 
						this->pFileOutputNode , L"node [%018p] %-16ls :: type [%ls] id (%ls) qual(%d)"
						,(void*)n
						,L"nDeclT3" 
						,(void*)n->declT3.type->declT1.type
						,(void*)n->declT3.id
						,(void*)n->declT3.qualifier	
					);
				$astDebugRowColToken(fDebug);
			}
		
		break ;

		case nTypeDeclT4 : // .............................................................................. decl t4 id :: * type

				astNodeDebug( this , n->declT4.type ) ;

				if ( this->fDebug ) 
				{
					printTab;
					fwprintf (	this->pFileOutputNode , L"node [%018p] %-16ls :: type [%018p] id [%ls] qual(%d)"
							,(void*)n
							,L"nDeclT4" 
							,(void*)n->declT4.type
							,(void*)n->declT4.id
							,(void*)n->declT4.qualifier	
					);
					switch ( n->declT4.sym )
					{
						case sym_ptr : fwprintf (	this->pFileOutputNode ,L" ptr to type"		); break ;
						case sym_pq0 : fwprintf (	this->pFileOutputNode ,L" ptr to array"		); break ;
						case sym_p0  : fwprintf (	this->pFileOutputNode ,L" ptr to function"	); break ;
						default      : fwprintf (	this->pFileOutputNode ,L" ptr to ??"		); break ;																								
					} 
											
				} 

				$astDebugRowColToken(fDebug);	
					
		break ;

		case nTypeDeclType : // .............................................................................. decl type

				astNodeDebug( this , n->declType.fields ) ;

				if ( this->fDebug ) 
				{
					printTab;
					fwprintf (	this->pFileOutputNode , L"node [%018p] %-16ls :: fields [%018p] id [%ls]"
							,(void*)n
							,L"nDeclType" 
							,(void*)n->declType.fields
							,(void*)n->declType.id 
					);						
				} 

				$astDebugRowColToken(fDebug);	
					
		break ;

		case nTypeStatSub : // .............................................................................. statement sub

				astNodeDebug( this , n->sub.body ) ;

				if ( this->fDebug ) 
				{
					printTab;
					fwprintf (	this->pFileOutputNode , L"node [%018p] %-16ls :: body [%018p] sub [%ls]"
							,(void*)n
							,L"nTypeStatSub" 
							,(void*)n->sub.body
							,(void*)n->sub.id
					);						
				} 

				$astDebugRowColToken(fDebug);	
					
		break ;
												
	  default : // .............................................................................. default

			$nodeInternal ( debug , errUnknown , L"ast.c" , L"node_t* astDebug(node_t* n) -> switch ( n->type )") ;
			return NULL ;

	  break;
	}

	//--nodeLevel;

	return NULL ;
}

void astDebug( past_t this , pnode_t n) 
{
    if ( this->fDebug ) 
    {
        fwprintf ( this->pFileOutputNode,L"\n# node   -> debugging ...\n\n"  );

        if ( n != NULL )
        {
            astNodeDebug(this,n);
        }
        else
        {
            fwprintf ( this->pFileOutputNode,L"\n# node   -> {null}...\n"  );
        }
    }
}


#undef $astDebugRowColToken

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

#undef printTab 


/**/



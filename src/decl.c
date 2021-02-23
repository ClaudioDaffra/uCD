#include "../lib/cxx.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

// id : type ;

node_t* parserDeclT1( pparser_t this , wchar_t* id )
{
	node_t *n=NULL;
	
	if ( this->lexer->sym == sym_id 
	|| 	 this->lexer->sym == sym_qm  )
	{
		n = astMakeDeclT1( this->ast , this->lexer , id , this->lexer->token ) ;
		
		parserGetToken(this);
		
	}
	else
	{
		$syntaxError;
	}
	return n;
}

// id : [] type ;

static 
node_t* parserDeclT2Array( pparser_t this , node_t* left )
{
	node_t* vArray = astMakeNodeBlock( this->ast ) ;
	node_t* exprNode  = NULL ;

	size_t rowSave = this->lexer->row_start;
	size_t colSave = this->lexer->col_start;

	while ( this->lexer->sym == sym_pq0 )
	{
		parserGetToken(this);
		
		exprNode = parserExpr(this);
		
		astPushNodeBlock( this->ast , vArray , exprNode ) ;
		
		$MATCH(  sym_pq1 , L']' ) ;
	}

	if ( vArray->block.next.size ) // è presente vettore array
	{
		// crea un nodo post fisso ed inserisce operatore post fisso [ con riferimento al blocco array
		node_t* node = astMakeNodePostfix( this->ast , this->lexer , left ) ;
		
		node->postfix.sym  	 = sym_pq0 ; 			// operatore post fisso [
		node->token 		 = gcWcsDup(L"[");  
		node->postfix.array  = vArray ;
		node->row		 	 = rowSave ;
		node->col			 = colSave -1 ;

		return node ;
	}
	return left ;
}

node_t* parserDeclT2( pparser_t this , wchar_t* id )
{
	node_t *n=NULL;
	
	node_t *t2_array=parserDeclT2Array(this,n) ;
	// array
	node_t* t2_type=parserDeclT1(this,id);
	// id
	// type
	n = astMakeDeclT2( this->ast , this->lexer , t2_array, t2_type ) ;
	
	return n;
}


/*

// declarations

declType :

	declTypeT1 :

		t1	=
				integer
				real
				char
				byte
				?
				id

	declTypeT2
				
		t2	=
				[ expr ]*	    of	t1

	declTypeT3
				
		t3	=
				( declType,* )	ret	t1

	declTypeT4

		t4	=
				*	to	t1
				*	to	t2
				*	to	t3

	declTypeT5

		t5	=
				[ expr ]*		of	t4
				( declType,* )	ret	t4
				*	            to	t4

	declStructUnion:

		t6	=
		{[
			declType*
		}]		
				
				
	declType  :   
						id  =   declTypeT1

	declArrayType   :   
						id  =   [] declTypeT1
						id  =   [] declTypeT4
	declSubType     :   
						id  =   () declTypeT1
						id  =   () declTypeT4                    

	declPointerToType     :   
						id  =   *  declTypeT1
	declPointerToArray    :                     
						id  =   *  declTypeT2
	declPointerToSub     : 
						id  =   *  declTypeT3
	declPointerToPointer                    
						id  =   *  declTypeT4
						

*/

node_t* parserDecl( pparser_t this )
{
    if ( this->lexer->sym == sym_end ) return NULL ;

    node_t *n=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDecl\n" );


	if ( this->lexer->sym == sym_id )	//	id
	{
		wchar_t* idSave = gcWcsDup(this->lexer->token);
		
		parserGetToken(this);			//	::

		if ( this->lexer->sym == sym_scope )	//	parser decl
		{		

			parserGetToken(this);			//	=
			
			switch (  this->lexer->sym )
			{
				case sym_qm :	// declTypeT1	id : type				
				case sym_id :
					n=parserDeclT1(this,idSave);
				break;

				case sym_pq0 :	// declTypet2	id : [] type
					n=parserDeclT2(this,idSave)	;		
				break;

				case sym_p0 :	// declTypet3	() type
					fwprintf(stderr,L" ( "); exit(-1);
				break;

				case sym_mul :	// declTypet4	 *
					fwprintf(stderr,L" * "); exit(-1);
				break;
																
				default:
				
					$syntaxError;
					
				break;
			}
		}		
	}

 

 return n ;
}


/**/




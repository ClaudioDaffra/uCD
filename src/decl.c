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

node_t* parserDeclT2( pparser_t this , wchar_t* id )
{
	node_t *n=NULL;

	//$MATCH(sym_pq0,L'[');
	// array
	node_t *t2_array=parserPostFixArray(this,n) ;  

	// id
	// type	
	node_t* t2_type=parserDeclT1(this,id);

	// make [] type node
	n = astMakeDeclT2( this->ast , this->lexer , t2_array, t2_type ) ;
	n->declT2.id = gcWcsDup(id);
	n->token = gcWcsDup(id);
	
	return n;
}

// id : () type ;

node_t* parserDeclT3( pparser_t this , wchar_t* id )
{
	node_t *n=NULL;
	
	// param
	$MATCH(sym_p0,L'(');  
	$MATCH(sym_p1,L')'); 
	
	// id
	// type	
	node_t* t3_type=parserDeclT1(this,id);

	// make [] type node
	n = astMakeDeclT3( this->ast , this->lexer , t3_type ) ;
	n->declT3.id = gcWcsDup(id);
	n->token = gcWcsDup(id);
			
	return n;
}

// id : * type ;

node_t* parserDeclT4( pparser_t this , wchar_t* id )
{
	node_t *n=NULL;

	if ( this->lexer->sym == sym_mul )
	{
		node_t* t4 = astMakeDeclT4( this->ast , this->lexer , id , NULL ) ;
		parserGetToken(this);

		node_t* t4_type=NULL;

		if ( this->lexer->sym == sym_p0 ) // * () T
		{
			t4_type=parserDeclT3(this,gcWcsDup(L"*")); 
			t4->declT4.sym = sym_p0;
			//t4->declT4.id = gcWcsDup(id);			
		}
		if ( this->lexer->sym == sym_pq0 ) // * [] T
		{
			t4_type=parserDeclT2(this,gcWcsDup(L"*")); 
			t4->declT4.sym = sym_pq0;
			//t4->declT4.id = gcWcsDup(id);
		}
		if ( t4_type == NULL )  // * T
		{
			t4_type=parserDeclT1(this,gcWcsDup(L"*"));
			t4->declT4.sym = sym_ptr ;
			//t4->declT4.id = gcWcsDup(id);
		}
	
		t4->declT4.type = t4_type; 
		
		return t4 ;
	}
	
	return n;
}

/*

// declarations

declType :

	declTypeT1 :

		t1	::
				integer
				real
				char
				byte
				?
				id

	declTypeT2 :
				
		t2	::
				[ expr ]*	    of	t1

	declTypeT3
				
		t3	::
				()				ret	t1

	declTypeT4

		t4	=
				*	to	t1
				*	to	t2
				*	to	t3

	declTypeT5

		t5	=
				[ expr ]*		of	t4
				()				ret	t4
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
			parserGetToken(this);			//	type [ ( *
			
			switch (  this->lexer->sym )
			{
				case sym_qm :	// declTypeT1	 id :: type				
				case sym_id :
					n=parserDeclT1(this,idSave);
				break;

				case sym_pq0 :	// declTypeT2	 id :: [] type
					n=parserDeclT2(this,idSave)	;		
				break;

				case sym_p0 :	// declTypeT3	 id :: () type
					n=parserDeclT3(this,idSave)	;
				break;

				case sym_mul :	// declTypeT4	 id :: * type
					n=parserDeclT4(this,idSave)	;
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




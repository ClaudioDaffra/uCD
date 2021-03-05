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

	// array
	node_t *t2_array=parserPostFixArray(this,n) ;  

	// id
	// type	
	node_t* t2_type=parserDeclT1(this,id);
	if ( t2_type == NULL )	return NULL ; // se errore da t1


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
	if ( t3_type == NULL )	return NULL ; // se errore da t1
	
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
			if ( t4_type == NULL ) return NULL ; // error from t3			
		}
		if ( this->lexer->sym == sym_pq0 ) // * [] T
		{
			t4_type=parserDeclT2(this,gcWcsDup(L"*")); 
			t4->declT4.sym = sym_pq0;
			if ( t4_type == NULL ) return NULL ; // error from t3
		}
		if ( t4_type == NULL )  // * T
		{
			t4_type=parserDeclT1(this,gcWcsDup(L"*"));
			t4->declT4.sym = sym_ptr ;
			if ( t4_type == NULL ) return NULL ; // error from t3
		}
	
		if ( t4_type == NULL ) $syntaxError ;
		
		t4->declT4.type = t4_type; 
		
		return t4 ;
	}
	
	return n;
}

// id :: { ... } ;

node_t* parserDeclType( pparser_t this , wchar_t* id )
{
	node_t *n=NULL;
	node_t *decl=NULL;	
	node_t *fields=astMakeNodeBlock(this->ast);
	
	if ( this->lexer->sym == sym_pg0 )
	{
		$MATCH(sym_pg0,L'{');

		do {
			
			decl=parserDecl(this);
			
			if ( decl!=NULL ) astPushNodeBlock( this->ast , fields , decl ) ;
			
			if ( this->lexer->sym == sym_pv ) parserGetToken(this); // skip
		
		} while ( this->lexer->sym 	!= sym_pg1
			 &&   decl 				!= NULL
			 &&	  !kError
		) ;
		
		$MATCH(sym_pg1,L'}');
		
		n=astMakeDeclType( this->ast , this->lexer , id , fields ) ;		
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

	declTypeT3 :
				
		t3	::
				()				ret	t1

	declTypeT4 :

		t4	=
				*	to	t1
				*	to	t2
				*	to	t3

	declStructUnion:

		t6	=
		{[
			declType*
		}]		
				
				
	declVarType  :   
						id  :   declTypeT1

	declArrayType   :   
						id  :   [] declTypeT1
						id  :   [] declTypeT4
	declSubType     :   
						id  :   () declTypeT1
						id  :   () declTypeT4                    

	declPointerToType     :   
						id  :   *  declTypeT1
	declPointerToArray    :                     
						id  :   *  declTypeT2
	declPointerToSub     : 
						id  :   *  declTypeT3
	declPointerToPointer                    
						id  :   *  typedef
						
	declType			id :  { } 
*/

node_t* parserDecl( pparser_t this )
{
    if ( this->lexer->sym == sym_end ) return NULL ;

    node_t *n=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDecl\n" );


	if ( this->lexer->sym == sym_kw_readOnly 
	||	 this->lexer->sym == sym_kw_readWrite 
	||	 this->lexer->sym == sym_kw_type 		)	
	{

		eQualifier_t qual = qualNull ;
		if ( this->lexer->sym == sym_kw_readOnly  ) qual=qualReadOnly  ;
		if ( this->lexer->sym == sym_kw_readWrite ) qual=qualReadWrite ;
			
		parserGetToken(this);
		
		if ( this->lexer->sym == sym_id )	//	id
		{
			wchar_t* idSave = gcWcsDup(this->lexer->token);

			parserGetToken(this);			//	::

			if ( this->lexer->sym == sym_dp )	//	parser decl
			{		
				parserGetToken(this);			//	type [ ( * {
				
				switch (  this->lexer->sym )
				{
					case sym_qm :	// declTypeT1	 	id :: type			simple typle				
					case sym_id :
						n=parserDeclT1(this,idSave);
						n->declT1.qualifier=qual;
					break;
		
					case sym_pq0 :	// declTypeT2		id :: [] type		array type
						n=parserDeclT2(this,idSave)	;
						n->declT2.qualifier=qual;
					break;

					case sym_p0 :	// declTypeT3	 	id :: () type		function type
						n=parserDeclT3(this,idSave)	;
						n->declT3.qualifier=qual;
					break;

					case sym_mul :	// declTypeT4	 	id :: * type		pointer type
						n=parserDeclT4(this,idSave)	;
						n->declT4.qualifier=qual;
					break;

					case sym_pg0 :	// declType	 		id :: { ... }		struct type
						n=parserDeclType(this,idSave)	;
					break;
																					
					default:
					
						$syntaxError;

					break;
				}
			}
					
		}
		
		// put qualifier
	}
	
 return n ;
}



/**/



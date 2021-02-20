#include "../lib/cxx.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

/*

// declarations

	declType :

		t1	=
				integer
				real
				char
				byte
				?
				id
				
		t2	=
				[ expr ]+	of	t1
				
		t3	=
				( declType,* )	ret	t1

		t4	=
				*	to	t1
				*	to	t2
				*	to	t3

		t5	=
				[ expr ]+		of	t4
				( declType,* )	ret	t4
				*				to	t4

	declStructUnion:

		t6	=
		{[
			declType*
		}]		
	
*/

// integer , real , char , byte , id , ?

node_t* parserDeclT1( pparser_t this )
{
	node_t *n=NULL;
	
	if ( this->lexer->sym == sym_id 
	|| 	 this->lexer->sym == sym_qm  )
	{
		n = astMakeDeclT1( this->ast , this->lexer , this->lexer->token ) ;
		
		parserGetToken(this);
		
	}
	return n;
}

node_t* parserDecl( pparser_t this )
{
    if ( this->lexer->sym == sym_end ) return NULL ;

    node_t *n=NULL;

    if ( this->fDebug ) fwprintf ( this->pFileOutputParser , L"parserDecl\n" );

    n = parserDeclT1(this);

 return n ;
}


/**/




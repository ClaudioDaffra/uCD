#include "../lib/cxx.h"
#include "global.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "ast.h"

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
				
				
	declSimpleType  :   
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



